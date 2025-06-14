// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FText UElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FText::Format(
			FText::FromString(TEXT(
				"<Title>ELECTROCUTE</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"

				"<Default>Emits a beam of lightning, connecting with the target, repeatedly dealing </>"
				"<Damage>{3}</>"
				"<Default> lightning damage with a chance to stun</>\n\n")),
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
				"<Title>ELECTROCUTE</>\n\n"

				"<Small>Level:</><Level>{0}</>\n"
				"<Small>ManaCost:</><Mana>{1}</>\n"
				"<Small>Cooldown:</><Cooldown>{2}</>\n\n"


				"<Default>Emits a beam of lightning propagating to {3} additional targets nearby causing</>"
				"<Damage>{4}</>"
				"<Default> lightning damage with a chance to stun</>\n\n")),
			FText::AsNumber(Level),
			FText::AsNumber(ManaCost),
			FText::AsNumber(Cooldown),
			FText::AsNumber(MaxTargets.GetValueAtLevel(Level)),
			FText::AsNumber(ScaledDamage)
		);
	}
}

FText UElectrocute::GetNextLevelDescription(int32 Level)
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


			"<Default>Emits a beam of lightning propagating to {3} additional targets nearby causing</>"
			"<Damage>{4}</>"
			"<Default> lightning damage with a chance to stun</>\n\n")),
		FText::AsNumber(Level),
		FText::AsNumber(ManaCost),
		FText::AsNumber(Cooldown),
		FText::AsNumber(MaxTargets.GetValueAtLevel(Level)),
		FText::AsNumber(ScaledDamage)
	);
}
