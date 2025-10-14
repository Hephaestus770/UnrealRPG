// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraGameplayTags.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"


FText UAuraGameplayAbility::GetDescription(int32 Level)
{
    FFormatNamedArguments Args;
    Args.Add("Name", FText::FromString(TEXT("Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum")));
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(FText::FromString("<Default>{Name}, </><Level>{Level}</>"), Args);
}

FText UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
    FFormatNamedArguments Args;
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(
        FText::FromString("<Default>Next Level: </><Level>{Level}</>\n<Default>Causes much more damage</>"),
        Args
    );

}

FText UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    FFormatNamedArguments Args;
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(FText::FromString("<Default>Spell Locked Until Level: {Level}</>"), Args);

}

bool UAuraGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    // Call parent class to check default conditions (including CostGameplayEffect)
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        UE_LOG(LogTemp, Warning, TEXT("UAuraGameplayAbility::CanActivateAbility: Parent check failed for %s"), *GetName());
        return false;
    }

    // Check if character is in air and ability is blocked
    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Character && Character->GetCharacterMovement()->IsFalling() && GetAssetTags().HasTag(FAuraGameplayTags::Get().Abilities_NotUsable_OnAir))
    {
        UE_LOG(LogTemp, Warning, TEXT("UAuraGameplayAbility::CanActivateAbility: %s blocked due to OnAir tag"), *GetName());
        return false;
    }

    // Explicitly check mana
    
    if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
        const UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(ASC->GetAttributeSet(UAuraAttributeSet::StaticClass()));
        if (!AttributeSet)
        {
            UE_LOG(LogTemp, Error, TEXT("UAuraGameplayAbility::CanActivateAbility: AttributeSet is null for %s"), *GetName());
            return false;
        }

        float CurrentMana = AttributeSet->GetMana();
        float RequiredMana = ManaCost.GetValueAtLevel(GetAbilityLevel());
        UE_LOG(LogTemp, Log, TEXT("UAuraGameplayAbility::CanActivateAbility: %s - CurrentMana=%.2f, RequiredMana=%.2f"), *GetName(), CurrentMana, RequiredMana);

        if (CurrentMana < RequiredMana)
        {
            UE_LOG(LogTemp, Warning, TEXT("UAuraGameplayAbility::CanActivateAbility: Cannot activate %s due to insufficient mana"), *GetName());
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAuraGameplayAbility::CanActivateAbility: Invalid ActorInfo or ASC for %s"), *GetName());
        return false;
    }
    
    return true;
}

void UAuraGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (!CostGameplayEffectClass || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
        return;

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC)
        return;

    // Build the outgoing GameplayEffectSpec
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGameplayEffectClass, GetAbilityLevel());
    if (!SpecHandle.IsValid())
        return;

    // Get the current mana cost for this ability level
    float CurrentManaCost = ManaCost.GetValueAtLevel(GetAbilityLevel());

    // Subtract mana (negative because GE adds modifiers to attributes)
    SpecHandle.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(FName("Data.Cost.Mana")),
        -CurrentManaCost
    );

    // Apply the cost effect to the owner to actually spend mana
    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

void UAuraGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (!CooldownGameplayEffectClass || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return;
    }

    // Create a spec from our shared Cooldown GE
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());

    if (SpecHandle.IsValid())
    {
        // Inject cooldown tag dynamically
        SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);

        // Inject cooldown duration dynamically
        SpecHandle.Data->SetSetByCallerMagnitude(
            FGameplayTag::RequestGameplayTag(FName("Data.Cooldown")),
            CooldownTime.GetValueAtLevel(GetAbilityLevel())
        );

        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
    }
    else
    {
        Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
    }
}

const FGameplayTagContainer* UAuraGameplayAbility::GetCooldownTags() const
{
    TempCooldownTags.Reset();

    const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
    if (ParentTags)
    {
        TempCooldownTags.AppendTags(*ParentTags);
    }

    if (CooldownTag.IsValid())
    {
        TempCooldownTags.AddTag(CooldownTag);
    }

    return &TempCooldownTags;
}

// This will get only static value of ManaCost, not suitable for things like Get %10 of current mana or other attribute values
float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
   /*
    float ManaCostVar = 0.f;
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (FGameplayModifierInfo ModInfo : CostEffect->Modifiers)
        {
            if (ModInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                ModInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCostVar);
                break;
            }
        }
    }
   */
    return ManaCost.GetValueAtLevel(InLevel);
}

// Same as GetManaCost this is return value from this static as well
float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
    /*
    float Cooldown = 0.f;
    if (UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    */
    return CooldownTime.GetValueAtLevel(InLevel);
    
}
