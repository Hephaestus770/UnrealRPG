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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FText GetDescription(int32 Level);
	virtual FText GetNextLevelDescription(int32 Level);
	static FText GetLockedDescription(int32 Level);

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, 
		const FGameplayTagContainer* TargetTags = nullptr, 
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

public:
	/** ==========================
		*  CONFIGURABLE ABILITY DATA
		*  ========================== */

		// Ability-specific mana cost (can be different per ability)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cost")
	FScalableFloat  ManaCost = 1.f;

	// Ability-specific cooldown time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cooldown")
	FScalableFloat CooldownTime = 1.f;

	// Each ability has a unique cooldown tag (ex: Cooldown.Fire.Fireball)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cooldown")
	FGameplayTag CooldownTag;

	/** ==========================
	 *  SHARED GAMEPLAY EFFECTS
	 *  ========================== */

	 // Reusable GameplayEffect asset for costs (set up in editor once)
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cost")
	//TSubclassOf<class UGameplayEffect> CostGameplayEffect;

	// Reusable GameplayEffect asset for cooldowns (set up in editor once)
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cooldown")
	//TSubclassOf<class UGameplayEffect> CooldownGameplayEffect;

	/** ==========================
   *  GAS OVERRIDES
   *  ========================== */

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

private:
	// Temporary container for merged cooldown tags
	UPROPERTY(Transient)
	mutable FGameplayTagContainer TempCooldownTags;


protected:

	// This will get only static value of ManaCost, not suitable for things like Get %10 of current mana or other attribute values
	float GetManaCost(float InLevel = 1.f) const;
	// Same as GetManaCost
	float GetCooldown(float InLevel = 1.f) const;
};
