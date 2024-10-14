// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TargetDataUnderMouse.h"


UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	FHitResult HitResult;
	
	Ability->GetCurrentActorInfo()->PlayerController.Get()->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!HitResult.bBlockingHit) return;

	ValidData.Broadcast(HitResult.Location);


}
