// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"


FText UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>FIRE BOLT</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

				"<Default>Launches a bolt of fire, exploding on impact and dealing </>"
				"<Damage>{3}</>"
				"<Default> fire damage with a chance to burn</>\n\n")),
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
				"<Title>FIRE BOLT</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"


				"<Default>Launches {3} bolts of fire, exploding on impact and dealing </>"
				"<Damage>{4}</>"
				"<Default> fire damage with a chance to burn</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(FMath::Min(Level, NumProjectiles)),
			FText::AsNumber(ScaledDamage)
		);
	}
}

FText UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FText::Format(
		FText::FromString(TEXT(
			"<Title>NEXT LEVEL:</>\n\n"

			"<Small>Level:</><Level>{0}</>\n"
			"<Small>ManaCost:</><Mana>{1}</>\n"
			"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

			"<Default>Launches {3} bolts of fire, exploding on impact and dealing </>"
			"<Damage>{4}</>"
			"<Default> fire damage with a chance to burn</>\n\n")),
		FText::AsNumber(Level),
		FText::AsNumber(ManaCost),
		FText::AsNumber(Cooldown),
		FText::AsNumber(FMath::Min(Level, NumProjectiles)),
		FText::AsNumber(ScaledDamage)
	);
}
