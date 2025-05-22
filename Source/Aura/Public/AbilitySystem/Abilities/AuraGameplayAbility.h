// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FText GetDescription(int32 Level);
	virtual FText GetNextLevelDescription(int32 Level);
	static FText GetLockedDescription(int32 Level);

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;


protected:

	// This will get only static value of ManaCost, not suitable for things like Get %10 of current mana or other attribute values
	float GetManaCost(float InLevel = 1.f) const;
	// Same as GetManaCost
	float GetCooldown(float InLevel = 1.f) const;
};
