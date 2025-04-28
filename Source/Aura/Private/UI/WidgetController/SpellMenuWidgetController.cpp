// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			FText Description;
			FText NextLevelDescription;
			GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}

		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	// ability equipped binded here!
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
	{
		SpellPointsChanged.Broadcast(Points);
		CurrentSpellPoints = Points;

		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		FText Description;
		FText NextLevelDescription;
		GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	});
}

void USpellMenuWidgetController::SelectAbility(UAuraUserWidget* AbilityButton)
{
	AbilitySelected.Broadcast(AbilityButton);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		const FGameplayTag SelectedAbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityTypeTag);
		bWaitForEquipSelection = false;
	}

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTagExact(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	FText Description;
	FText NextLevelDescription;
	GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitForEquipSelection)
	{
		const FGameplayTag SelectedAbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityTypeTag);
		bWaitForEquipSelection = false;
	}

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

	SpellGlobeSelectedDelegate.Broadcast(false, false, FText(), FText());
	AbilitySelected.Broadcast(nullptr);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	WaitForEquipDelegate.Broadcast(AbilityTypeTag);
	bWaitForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitForEquipSelection) return;
	// Check selected ability against the slot's ability type so don't equip active spell to a passive slot or vice verse
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag)
{
	bWaitForEquipSelection = false;

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	// Clear and broadcast previous ability info
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlotTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	//Broadcast empty info if PrevSlot is a valid slot. Only if equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	// Set and broadcast new ability info
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(Info);

	//Stop EquippedSpellRow animations
	StopWaitForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	// Default state of buttons are disabled
	bShouldEnableSpellPointsButton = false;
	bShouldEnableSpellPointsButton = false;

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableEquipButton = false;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}

	//else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	// We don't need to check or do anything if ability status is locked default state is disabled already
}
