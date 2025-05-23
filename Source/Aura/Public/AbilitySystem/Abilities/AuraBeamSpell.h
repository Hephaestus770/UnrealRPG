// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamtTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintCallable)
	void RemoveOnDeathBindingFromPrimaryTarget();

	UFUNCTION(BlueprintCallable)
	void RemoveOnDeathBindingFromAdditionalTarget(const TArray<AActor*>& AdditionalTarget);

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Beam")
	FName TraceStartSocketName;

	/** Radius around hit for additional targets, scalable per ability level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	FScalableFloat Radius;

	/** Maximum number of shock targets, scalable per ability level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	FScalableFloat MaxTargets;

};
