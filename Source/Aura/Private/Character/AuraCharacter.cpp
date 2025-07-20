// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include <AuraGameplayTags.h>
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include <Game/AuraGameModeBase.h>
#include <Kismet/GameplayStatics.h>
#include "Game/LoadScreenSaveGame.h"
#include <AbilitySystem/AuraAttributeSet.h>
#include <AbilitySystem/AuraAbilitySystemLibrary.h>
#include <AbilitySystem/Data/AbilityInfo.h>






AAuraCharacter::AAuraCharacter()
{
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false; 

	CharacterClass = ECharacterClass::Elementalist; // Player Character is Elementalist Class
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitAbilityActorInfo();

	//Load Game from disk
	LoadProgress();

	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
	
}


void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode)
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{

			// Load Level,XP etc from disk
			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetXP(SaveData->XP);
				AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
				AuraPlayerState->bIsLoadingFromSave = true;
			}

			// Load in Abilities from disk
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				AuraASC->AddCharacterAbilitiesFromSaveData(SaveData);
				AuraASC->UpdateAbilityStatuses(SaveData->PlayerLevel); // I ADDED THIS !!!
			}
		
			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
			//ApplyEffectToSelf(DefaultSecondaryAttributes);
			//ApplyEffectToSelf(DefaultVitalAttributes);
		}
		
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddtoXP(InXP);
}
int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}
int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}
void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddtoAttributePoints(InAttributePoints);
}
void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	// When playing as Listen Server with 2 players, the magic circle is also shown on the Server at the world's zero location. 
	// Return early if character is not locally controlled
	if (!IsLocallyControlled()) return;

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;

	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	// When playing as Listen Server with 2 players, the magic circle is also shown on the Server at the world's zero location. 
	// Return early if character is not locally controlled
	if (!IsLocallyControlled()) return;

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCirle();
		AuraPlayerController->bShowMouseCursor = true;

	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode)
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		SaveData->PlayerStartTag = CheckpointTag;

		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveData->XP = AuraPlayerState->GetXP();
			SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
		}
		if (UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(GetAttributeSet()))
		{
			/*
			SaveData->Strength = AuraAS->GetStrength(); // These gets current values
			SaveData->Dexterity = AuraAS->GetDexterity();
			SaveData->Intelligence = AuraAS->GetIntelligence();
			*/
		
			SaveData->Strength = AuraAS->Strength.GetBaseValue(); // These gets base values
			SaveData->Dexterity = AuraAS->Dexterity.GetBaseValue();
			SaveData->Intelligence = AuraAS->Intelligence.GetBaseValue();
		

			SaveData->bFirstTimeLoadIn = false;

			// GameMode only lives on server so we can only save on server. GetAbilityInfo() function works on only servert too!
			if (!HasAuthority()) return; 

			UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
			FForEachAbility SaveAbilityDelegate;
			SaveData->SavedAbilities.Empty();
			SaveAbilityDelegate.BindLambda([this, AuraASC, &SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromSpec(AbilitySpec);
				UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				

				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilitySlot = AuraASC->GetInputTagFromAbilityTag(AbilityTag);
				SavedAbility.AbilityStatus = AuraASC->GetStatusTagFromAbilityTag(AbilityTag);
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilitType = Info.AbilityType;

				SaveData->SavedAbilities.AddUnique(SavedAbility);

			});

			AuraASC->ForEachAbility(SaveAbilityDelegate);

		}

		// TODO: Save active gameplay effects
	/*
	
		UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
		if (ASC)
		{
			SaveData->ActiveEffects.Empty(); // Ensure the array is empty before populating

			// Get the container of active effects
			const FActiveGameplayEffectsContainer& ActiveEffectsContainer = ASC->GetActiveGameplayEffects();

			// Iterate using a traditional for loop and iterators
			for (auto It = ActiveEffectsContainer.CreateConstIterator(); It; ++It)
			{
				const FActiveGameplayEffect& ActiveEffect = *It; // Dereference the iterator to get the FActiveGameplayEffect

				if (const UGameplayEffect* Effect = ActiveEffect.Spec.Def)
				{
					FActiveEffectSaveData EffectData;
					EffectData.EffectName = Effect->GetPathName();
					EffectData.TimeRemaining = ActiveEffect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
					EffectData.StackCount = ActiveEffect.Spec.StackCount;
					SaveData->ActiveEffects.Add(EffectData);
				}
			}
		}
	*/
		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]
	{
		AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (AuraGM)
		{
			AuraGM->PlayerDied(this);
		}
	});
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	
	
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
			// Cancel Abilities have Abilities Tag
			//const FGameplayTagContainer AbilitiesToCancelTags(GameplayTags.Abilities_Lightning_Electrocute);
			//const FGameplayTagContainer AbilitiesToIgnoreTags(GameplayTags.Abilities_Passive);

			//AuraASC->CancelAbilities(nullptr, &AbilitiesToIgnoreTags);
			
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();

		}

	}

}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}

}


void AAuraCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();

	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	//Stun Ability Register
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
	
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}

	}


	// Subscribe to OnLevelChangeDelegate to play the vfx
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](const int32 NewValue)
	{
		MulticastLevelUpParticles();
	}
	);
}

void AAuraCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	// NOTE: We need to call DisableInput() on the Actor we need to disable input for, i.e. this Pawn's Controller,
	// if we call it directly from this Pawn, it won't work.
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->DisableInput(PlayerController);
	}

	Super::MulticastHandleDeath_Implementation(DeathImpulse);
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			if (!AuraPlayerState->bIsLoadingFromSave)
			{
				LevelUpNiagaraComponent->Activate(true);
				UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation(), GetActorRotation());
			}
		}
	}
}