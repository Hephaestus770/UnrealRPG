// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

FText UAuraGameplayAbility::GetDescription(int32 Level) const
{
    FFormatNamedArguments Args;
    Args.Add("Name", FText::FromString(TEXT("Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum")));
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(FText::FromString("<Default>{Name}, </><Level>{Level}</>"), Args);
}

FText UAuraGameplayAbility::GetNextLevelDescription(int32 Level) const
{
    FFormatNamedArguments Args;
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(
        FText::FromString("<Default>Next Level: </><Level>{Level}</>\n<Default>Causes much more damage</>"),
        Args
    );

}

FText UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    FFormatNamedArguments Args;
    Args.Add("Level", FText::AsNumber(Level));

    return FText::Format(FText::FromString("<Default>Spell Locked Until Level: {Level}</>"), Args);
}

