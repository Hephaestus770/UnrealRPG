// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"
UMMC_MaxMana::UMMC_MaxMana()
{
	FocusDef.AttributeToCapture = UAuraAttributeSet::GetFocusAttribute();
	FocusDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	FocusDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(FocusDef);

}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* Targettags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = Targettags;

	float Focus = 0.f;
	GetCapturedAttributeMagnitude(FocusDef, Spec, EvaluationParameters, Focus);
	Focus = FMath::Max<float>(Focus, 0.f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 30 + 2.5f * Focus + 3.f * PlayerLevel;

}
