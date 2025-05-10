// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura/Public/AuraGameplayTags.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "DrawDebugHelpers.h"


void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!TargetActor) return;

	// Get params
	FDamageEffectParams Params = MakeDamageEffectParamsFromClassDefaults(TargetActor);
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// Calculate direction from attacker to target
	FVector Direction = (TargetActor->GetActorLocation() - AvatarActor->GetActorLocation()).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = AvatarActor->GetActorForwardVector().GetSafeNormal(); // Zero Vector Edge Case
	}

	// Calculate death impulse
	FVector DeathImpulse = Direction * Params.DeathImpulseMagnitude;

	Params.DeathImpulse = DeathImpulse;

	const bool bKnockback = FMath::RandRange(1, 100) < Params.KnockbackChance;
	if (bKnockback)
	{
		// Create a rotation with a 45-degree upward pitch to mimic projectile knockback
		FRotator KnockbackRotation = Direction.Rotation();
		KnockbackRotation.Pitch = 45.f; // Tilt the direction upward, remove this for just pushback. Value Can Be Configurable!!!

		// Calculate knockback force using the rotated direction
		const FVector KnockbackDirection = KnockbackRotation.Vector();
		const FVector KnockbackForce = KnockbackDirection * Params.KnockbackForceMagnitude;
		Params.KnockbackForce = KnockbackForce;
	}

	// Apply the damage effect
	UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor)
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	// We may or may not have a valid Target Actor but GetAbilitySystemComponent library function already does a nullptr check so we don't need to
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance.GetValueAtLevel(GetAbilityLevel());
	Params.DebuffDamage = DebuffDamage.GetValueAtLevel(GetAbilityLevel());
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;

	return Params;

}

