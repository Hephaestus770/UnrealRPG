// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include <Interaction/CombatInterface.h>

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	// FIXED BUG: If HomingTarget died before the projectile hit him, projectile will become non homing projectile so it can continue to move.
	if (HasAuthority() && ProjectileMovement->HomingTargetComponent.IsValid())
	{
		ICombatInterface* HomingTargetCombatInterface = Cast<ICombatInterface>(ProjectileMovement->HomingTargetComponent->GetOwner());
		if (HomingTargetCombatInterface)
		{
			HomingTargetCombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &AAuraProjectile::OnHomingTargetDeath);
		}
	}

	if (!HasAuthority())
	{
		AActor* ThisOwner = GetOwner();
	}
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent(), NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true);

}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	bDidHit = true;
}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if (!bDidHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
									  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* ThisOwner = GetOwner();

	// Ignore the Owner.
	if (OtherActor == ThisOwner)
		return;
	// Prevent friendly fire.
	if (!UAuraAbilitySystemLibrary::IsNotFriend(ThisOwner, OtherActor))
		return;
	// Ensure projectile overlaps once
	if (!bDidHit) 
		OnHit();

	if (HasAuthority())
	{
		// NOTE: DamageEffectParams should be valid only on the server (we set it there, but also don't replicate it).
		check(DamageEffectParams.SourceAbilitySystemComponent);
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)) // does overlaaping component has ASC?
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f; // Tilt the direction upward, remove this for just pushback

				const FVector KnockbackDirection = Rotation.Vector();
				const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
				DamageEffectParams.KnockbackForce = KnockbackForce;
			}

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bDidHit = true;
	}
}

void AAuraProjectile::OnHomingTargetDeath(AActor* DeadActor)
{
	ProjectileMovement->bIsHomingProjectile = false;
}



