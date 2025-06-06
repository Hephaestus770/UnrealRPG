// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    if (bIsLocallyControlled)
    {
        SendMouseCursorData();
    }
    else
    {
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
        AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddLambda(
        [this, SpecHandle, ActivationPredictionKey](const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
        {
            AbilitySystemComponent->ConsumeClientReplicatedTargetData(SpecHandle, ActivationPredictionKey);
            if (ShouldBroadcastAbilityTaskDelegates())
            {
                ValidData.Broadcast(DataHandle);
            }
        });

        const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        if (!bCalledDelegate)
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());
    FHitResult CursorHitResult;

    if (APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
    {
        PC->GetHitResultUnderCursor(ECC_Target, false, CursorHitResult);
    }

    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHitResult;
    DataHandle.Add(Data);

    FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        ActivationPredictionKey,
        DataHandle,
        FGameplayTag(),
        ActivationPredictionKey);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}

void UTargetDataUnderMouse::OnTargetDataReplicationCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey()); // Tells target data has been received, dont keep it stored

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
