// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Interaction/CombatInterface.h>
#include <AuraGameplayTags.h>

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		const bool bStartupAbilitiesGiven = AuraASC->bStartupAbilitiesGiven;

		ActivateIfEquipped(AuraASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC) 
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);

				ActivateIfEquipped(AuraASC);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
	
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	const bool bStartupAbilitiesGiven = AuraASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (AuraASC->GetStatusTagFromAbilityTag(PassiveSpellTag) == FAuraGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
