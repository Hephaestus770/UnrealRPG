// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraGameplayTags.h"


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
    // BLOCK ABILITIES THAT HAVE "Abilities.NotUsable.OnAir" TAG WHILE IN AIR, THIS WAY NOT EVERY ABILITY IS BLOCKED. BLUEPRINTS DIDN'T WORKED!!!
    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Character && Character->GetCharacterMovement()->IsFalling() && AbilityTags.HasTag(FAuraGameplayTags::Get().Abilities_NotUsable_OnAir))
    {
        return false;
    }

    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

// This will get only static value of ManaCost, not suitable for things like Get %10 of current mana or other attribute values
float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
    float ManaCost = 0.f;
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (FGameplayModifierInfo ModInfo : CostEffect->Modifiers)
        {
            if (ModInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                ModInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
                break;
            }
        }
    }
    return ManaCost;
}

// Same as GetManaCost this is return value from this static as well
float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
    float Cooldown = 0.f;
    if (UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;
}
