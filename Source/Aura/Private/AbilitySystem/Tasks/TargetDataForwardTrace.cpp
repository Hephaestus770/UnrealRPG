#include "AbilitySystem/Tasks/TargetDataForwardTrace.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

static FORCEINLINE FVector GetMuzzleLocation(AActor* Avatar)
{
    if (!Avatar) return FVector::ZeroVector;
/*
    if (USkeletalMeshComponent* Skel = Avatar->FindComponentByClass<USkeletalMeshComponent>())
    {
        // Change "Muzzle" to your socket name
        if (Skel->DoesSocketExist(TEXT("Muzzle")))
        {
            return Skel->GetSocketLocation(TEXT("Muzzle"));
        }
        return Skel->GetComponentLocation();
    }
*/
    return Avatar->GetActorLocation();
}

UTargetDataForwardTrace* UTargetDataForwardTrace::CreateTargetDataForwardTrace(UGameplayAbility* OwningAbility,
    float MaxRange,
    float CameraNearOffset,
    bool bPreferPawnAlongSight)
{
    UTargetDataForwardTrace* Task = NewAbilityTask<UTargetDataForwardTrace>(OwningAbility);
    Task->TraceDistance = MaxRange;
    Task->CamNearOffset = CameraNearOffset;
    Task->bPreferPawnOnSight = bPreferPawnAlongSight;
    return Task;
}

void UTargetDataForwardTrace::Activate()
{
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

    if (bIsLocallyControlled)
    {
        PerformForwardTrace();
    }
    else
    {
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

        AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
            .AddUObject(this, &UTargetDataForwardTrace::OnTargetDataReplicationCallback);

        if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UTargetDataForwardTrace::PerformForwardTrace()
{
    if (!Ability) return;

    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!PC || !Avatar) return;

    // Use the shared utility function
    FHitResult Hit = UAuraAbilitySystemLibrary::ComputeForwardTrace(Avatar, TraceDistance, CamNearOffset, bPreferPawnOnSight);

    // Send to server (your existing logic)
    SendTargetDataToServer(Hit);
}

void UTargetDataForwardTrace::SendTargetDataToServer(const FHitResult& FinalHit)
{
    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit(FinalHit);
    DataHandle.Add(Data);

    const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

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

    EndTask(); // <-- add this to stop multiple stacked listeners

}

void UTargetDataForwardTrace::OnTargetDataReplicationCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
    EndTask(); // <-- add here too

}
