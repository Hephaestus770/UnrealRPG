// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// Player Interface
	virtual void AddToXP_Implementation(int32 InXP) override;
	//virtual void LevelUp_Implementation() override;
	//virtual int32 GetXP_Implementation() const override;
	//virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	// end Player Interface

	// Combat Interface
	virtual int32 GetPlayerLevel_Implementation() override;
	// end Combat Interface

	//LevelUp VFX
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

protected:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

};
