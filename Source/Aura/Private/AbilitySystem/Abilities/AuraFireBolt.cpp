// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FText UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>FIRE BOLT</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

				"<Default>Launches a bolt of fire, exploding on impact and dealing </>"
				"<Damage>{3}</>"
				"<Default> fire damage with a chance to burn</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(ScaledDamage)
		);
	}
	else
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>FIRE BOLT</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"


				"<Default>Launches {3} bolts of fire, exploding on impact and dealing </>"
				"<Damage>{4}</>"
				"<Default> fire damage with a chance to burn</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(FMath::Min(Level, MaxNumProjectiles)),
			FText::AsNumber(ScaledDamage)
		);
	}
}

FText UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FText::Format(
		FText::FromString(TEXT(
			"<Title>NEXT LEVEL:</>\n\n"

			"<Small>Level:</><Level>{0}</>\n"
			"<Small>ManaCost:</><Mana>{1}</>\n"
			"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

			"<Default>Launches {3} bolts of fire, exploding on impact and dealing </>"
			"<Damage>{4}</>"
			"<Default> fire damage with a chance to burn</>\n\n")),
		FText::AsNumber(Level),
		FText::AsNumber(ManaCost),
		FText::AsNumber(Cooldown),
		FText::AsNumber(FMath::Min(Level, MaxNumProjectiles)),
		FText::AsNumber(ScaledDamage)
	);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);
	const int32 NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());

	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles + 3);
	for (const FRotator& Rot: Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AActor* AvatarActor = GetAvatarActorFromActorInfo();

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			AvatarActor,
			Cast<APawn>(AvatarActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		// For Homing Projectiles
		if (bHomingProjectile)
		{
			Projectile->ProjectileMovement->bIsHomingProjectile = true;
			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);

			if (HomingTarget && HomingTarget->Implements<UCombatInterface>()) // HomingTarget is valid and implements the interface
			{
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				// HomingTargetSceneComponent will be garbage collected
				Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass()); 
				Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation); // Aim at target location
				Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
			}
				
		}

		Projectile->FinishSpawning(SpawnTransform);
	}

}
