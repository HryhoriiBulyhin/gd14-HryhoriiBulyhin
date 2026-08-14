#include "pch.h"
#include "WeaponType.h"

bool StringToWeaponType(const std::string& id, WeaponType& weaponType)
{
    if (id == "Unarmed")
    {
        weaponType = WeaponType::Unarmed;
        return true;
    }
    if (id == "Bat")
    {
        weaponType = WeaponType::Bat;
        return true;
    }
    if (id == "Knife")
    {
        weaponType = WeaponType::Knife;
        return true;
    }
    if (id == "Case")
    {
        weaponType = WeaponType::Case;
        return true;
    }
    return false;
}

const char* WeaponTypeToString(WeaponType type)
{
    switch (type)
    {
    case WeaponType::Unarmed:
    {
        return "Unarmed";
    }
    case WeaponType::Knife:
    {
        return "Knife";
    }
    case WeaponType::Case:
    {
        return "Case";
    }
    }
    return "";
}
