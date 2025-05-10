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

	// Calculate direction from attacker to target
	FVector Direction = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();

	// Calculate death impulse
	FVector DeathImpulse = Direction * Params.DeathImpulseMagnitude;

	// Debug visualization of Death Impulse
	/*
	if (GEngine && GEngine->GetWorldFromContextObject(GetAvatarActorFromActorInfo(), EGetWorldErrorMode::LogAndReturnNull))
	{
		FVector StartLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		FVector EndLocation = StartLocation + DeathImpulse;

		// Draw a debug arrow to represent the Death Impulse
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.f, 0, 2.f);
	}
	*/

	// Update damage effect params with the calculated death impulse
	Params.DeathImpulse = DeathImpulse;

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

	return Params;

}

