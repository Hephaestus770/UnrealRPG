#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataForwardTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetDataForwardTraceDelegate, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * Third-person targeting task:
 *  - Gets a forward ray from the camera (ignores world occluders)
 *  - Optionally snaps to pawns along that ray
 *  - Then traces from muzzle to that aim point to get the final hit
 */
UCLASS()
class AURA_API UTargetDataForwardTrace : public UAbilityTask
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
        meta = (DisplayName = "Target Data Forward Trace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
    static UTargetDataForwardTrace* CreateTargetDataForwardTrace(UGameplayAbility* OwningAbility,
        float MaxRange = 8000.f,
        float CameraNearOffset = 30.f,
        bool  bPreferPawnAlongSight = true);

    virtual void Activate() override;

    UPROPERTY(BlueprintAssignable)
    FTargetDataForwardTraceDelegate ValidData;

protected:
    void PerformForwardTrace();
    void SendTargetDataToServer(const FHitResult& FinalHit);
    void OnTargetDataReplicationCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

    // Config
    float TraceDistance = 8000.f;
    float CamNearOffset = 30.f;
    bool  bPreferPawnOnSight = true;
};
