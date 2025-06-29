// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <GameplayTagContainer.h>
#include "LoadScreenSaveGame.generated.h"
/*
USTRUCT(BlueprintType)
struct FActiveEffectSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString EffectName;

	UPROPERTY()
	float TimeRemaining;

	UPROPERTY()
	int32 StackCount;
};
*/
class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedAbility 
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel;

};

// Defined operator for AddUnique to work for our custom FSavedAbility struct!!1
inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;


	// PLAYER

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;
	
	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Dexterity = 0;

	UPROPERTY()
	float Intelligence = 0;


	// ABILITIES

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;




	/*
	UPROPERTY()
	TArray<FActiveEffectSaveData> ActiveEffects;
	*/
};
