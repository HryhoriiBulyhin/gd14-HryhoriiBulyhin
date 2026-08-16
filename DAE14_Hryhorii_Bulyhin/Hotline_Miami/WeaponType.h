#pragma once
#include <string>

enum class WeaponType
{
	Unarmed = 0,
	Bat,
	Knife,
	Case,
	Count
};
bool StringToWeaponType(const std::string& id, WeaponType& weaponType);
const char* WeaponTypeToString(WeaponType type);
