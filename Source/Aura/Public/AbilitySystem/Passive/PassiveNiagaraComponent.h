// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	
	UPassiveNiagaraComponent();

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Abilities.Passive"))
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;

	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC);

};
