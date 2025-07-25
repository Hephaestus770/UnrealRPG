// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const FGameplayTag& /*SlotTag*/, const FGameplayTag& /*PrevSlotTag*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /*AbilityTag*/, bool /*bActivate*/);

class ULoadScreenSaveGame;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();

	//Delegates
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;

	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);
	void AddCharacterAbilites(const TArray<TSubclassOf<UGameplayAbility>>& StarupAbilities);
	void AddCharacterPassiveAbilites(const TArray<TSubclassOf<UGameplayAbility>>& StarupPassiveAbilities);

	bool bStartupAbilitiesGiven = false; // Purpose of this: First abilities will be given then delegate bindings in overlay widget will happen


	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);


	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	void UpdateAbilityStatuses(int32 Level);

	// Because ServerEquip is a server function, PassiveNiagaraComponent delegate will not work on clients. 
	// To clients being able to see the vfx from passive abities
	UFUNCTION(NetMulticast, Unreliable) // Unrealiable because it is just a vfx
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	/*Server*/
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);
	/*end Server*/

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlot);

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FText& OutDescription, FText& OutNextLevelDescription);

	static void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesOfSlot(const FGameplayTag& SlotTag);
	//static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& SlotTag);

protected:

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);


};
