// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
/**
 * AuraGameplayTags
 * 
 * Singleton containing native Gameplay Tags
 */

struct FAuraGameplayTags
{
public:

	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	// Primary Attribute Tags
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Dexterity;

	// Secondary Attribute Tags
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_StaminaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_MaxStamina;

	// Input Tags
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;


	// Damage Tags
	FGameplayTag Damage;


private:

	static FAuraGameplayTags GameplayTags;


};
