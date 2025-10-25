// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraCrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraCrosshairWidget : public UAuraUserWidget
{
	GENERATED_BODY()
	

public:
    // Function to get color based on health (Blueprint callable)
    UFUNCTION(BlueprintCallable, Category = "Crosshair")
    void SetCrosshairColorFromHealth(float HealthPercent);


protected:

    UPROPERTY(BlueprintReadOnly, Category = "Crosshair", meta = (BindWidget))
    class UImage* CrosshairImage;

};
