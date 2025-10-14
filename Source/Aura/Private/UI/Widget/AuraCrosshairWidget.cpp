// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraCrosshairWidget.h"
#include "Components/Image.h"


void UAuraCrosshairWidget::SetCrosshairColorFromHealth(float HealthPercent)
{
    FLinearColor NewColor = FLinearColor::White; // Default

    UE_LOG(LogTemp, Warning, TEXT("Crosshair Health Percent: %f"), HealthPercent);

    if (HealthPercent >= 0.0f) // Valid health percentage
    {
        if (HealthPercent > 0.7f)
            NewColor = FLinearColor::Green;
        else if (HealthPercent > 0.4f)
            NewColor = FLinearColor::Yellow;
        else
            NewColor = FLinearColor::Red;
    }
    else
    {
        // No enemy targeted - use default white
        NewColor = FLinearColor::White;
    }

    SetCrosshairColor(NewColor);
}

void UAuraCrosshairWidget::SetCrosshairColor(FLinearColor NewColor)
{
    if (CrosshairImage)
    { 
        if (NewColor == FLinearColor::Green)
        {
            CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 3.0);

        }
        else if (NewColor == FLinearColor::Yellow)
        {
            CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 2.0);
        }
        else if (NewColor == FLinearColor::Red)
        {
            CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 1.0);

        }

        CrosshairImage->SetColorAndOpacity(NewColor);
        UE_LOG(LogTemp, Warning, TEXT("Crosshair color set to: R=%.2f, G=%.2f, B=%.2f"),
            NewColor.R, NewColor.G, NewColor.B);
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CrosshairImage is null in SetCrosshairColor"));
    }
}
