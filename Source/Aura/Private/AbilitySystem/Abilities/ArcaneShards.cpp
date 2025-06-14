// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FText UArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>ICE CRYSTAL</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

				"<Default>Spawns a ice crystal on selected location, deals </>"
				"<Damage>{3}</>"
				"<Default> radial ice damage</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(ScaledDamage)
		);
	}
	else
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>ICE CRYSTAL</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"


				"<Default>Spawns a ice crystal on selected location and {3} more nearby, each dealing</>"
				"<Damage>{4}</>"
				"<Default> radial ice damage</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(FMath::Min(Level, MaxNumShards)),
			FText::AsNumber(ScaledDamage)
		);
	}
}

FText UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FText::Format(
		FText::FromString(TEXT(
			"<Title>NEXT LEVEL</>\n\n"

			"<Small>Level:</><Level>{0}</>\n"
			"<Small>ManaCost:</><Mana>{1}</>\n"
			"<Small>Cooldown:</><Cooldown>{2}</>\n\n"


			"<Default>Spawns a ice crystal on selected location and {3} more nearby, each dealing </>"
			"<Damage>{4}</>"
			"<Default> radial ice damage</>\n\n")),
		FText::AsNumber(Level),
		FText::AsNumber(ManaCost),
		FText::AsNumber(Cooldown),
		FText::AsNumber(FMath::Min(Level, MaxNumShards)),
		FText::AsNumber(ScaledDamage)
	);
}
