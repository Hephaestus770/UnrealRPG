// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraCrosshairWidget.h"
#include "Components/Image.h"


void UAuraCrosshairWidget::SetCrosshairColorFromHealth(float HealthPercent)
{
    if (CrosshairImage)
    {
        CrosshairImage->SetColorAndOpacity(FLinearColor::White); // Default
        CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 3.0);

        if (HealthPercent >= 0.0f) // Valid health percentage
        {
            if (HealthPercent > 0.7f)
            {
                CrosshairImage->SetColorAndOpacity(FLinearColor::Green);
                CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 3.0);

            }

            else if (HealthPercent > 0.4f)
            {
                CrosshairImage->SetColorAndOpacity(FLinearColor::Yellow);
                CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 2.0);

            }
            else
            {
                CrosshairImage->SetColorAndOpacity(FLinearColor::Red);
                CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 1.0);

            }
        }
        else
        {
            // No enemy targeted - use default white
            CrosshairImage->SetColorAndOpacity(FLinearColor::White); // Default
            CrosshairImage->GetDynamicMaterial()->SetScalarParameterValue("Thickness", 3.0);

        }
    }

}
