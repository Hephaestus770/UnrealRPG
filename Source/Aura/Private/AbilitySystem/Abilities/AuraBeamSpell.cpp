// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Curves/RealCurve.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else if (OwnerCharacter) // No hit: compute fallback
	{
		// Use camera forward vector as fallback direction
		if (OwnerPlayerController)
		{
			FVector CamLoc;
			FRotator CamRot;
			OwnerPlayerController->GetPlayerViewPoint(CamLoc, CamRot);

			MouseHitLocation = OwnerCharacter->GetActorLocation() + CamRot.Vector() * MaxDistance;
		}
		else
		{
			// Fallback to character forward
			MouseHitLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * MaxDistance;
		}
		MouseHitActor = nullptr; // No real actor hit
	}

	// Rotate character to face MouseHitLocation
	if (OwnerCharacter)
	{
		FVector Dir = MouseHitLocation - OwnerCharacter->GetActorLocation();
		Dir.Z = 0; // Horizontal rotation only
		if (!Dir.IsNearlyZero())
		{
			OwnerCharacter->SetActorRotation(Dir.Rotation());
		}
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamtTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			// DEBUG MESSAGE FOR TO SET NAME OF THE TRACE SOCKET
			if (TraceStartSocketName.IsNone())
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1, 5.f, FColor::Red,
						FString::Printf(TEXT("TraceStartSocketName is not set on Blueprint %s"), *GetName())
					);
				}
				return;
			}
			const FVector SocketLocation = Weapon->GetSocketLocation(TraceStartSocketName); // Trace starting point
			TArray<AActor*>ActorsToIgnore; 
			ActorsToIgnore.Add(OwnerCharacter); // Ignore self
			FHitResult HitResult;
			APlayerController* PC = GetCurrentActorInfo()->PlayerController.Get();
			// Get camera view for direction
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);
			FVector CamForward = CamRot.Vector();

			//FVector TraceStart = SocketLocation;
			FVector TraceEnd = SocketLocation + (CamForward * MaxDistance); //BeamtTargetLocation; 

			
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter, 
				SocketLocation,
				TraceEnd,
				TraceRadius,
				TraceTypeQuery1, 
				false, 
				ActorsToIgnore,
				EDrawDebugTrace::ForDuration,
				HitResult,
				true,
				FLinearColor::Red,
				FLinearColor::Green,
				3.f);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}

			else
			{
				// If nothing hit, just go full distance forward
				MouseHitLocation = TraceEnd;
				MouseHitActor = nullptr;
			}

		}
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}


void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	if (!MouseHitActor)
	{
		return;
	}
	// Evaluate radius and max targets from scalable floats
	const float EvalRadius = Radius.GetValueAtLevel(GetAbilityLevel());
	const int32 EvalMaxTargets = FMath::RoundToInt(MaxTargets.GetValueAtLevel(GetAbilityLevel()));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*> OverlappingActors;

	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(), 
		OverlappingActors, 
		ActorsToIgnore, 
		EvalRadius, 
		MouseHitActor->GetActorLocation());

	// Remove any actors tagged "Player"
	OverlappingActors.RemoveAllSwap(
		[](AActor* A)
	{
		return A && A->ActorHasTag("Player");
	});

	UAuraAbilitySystemLibrary::GetClosestTargets(EvalMaxTargets - 1, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::RemoveOnDeathBindingFromPrimaryTarget()
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor);
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
	}
}

void UAuraBeamSpell::RemoveOnDeathBindingFromAdditionalTarget(const TArray<AActor*>& AdditionalTargets)
{
	for (AActor* Actor : AdditionalTargets)
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor);
		if (CombatInterface)
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
		}
	}
	
}
