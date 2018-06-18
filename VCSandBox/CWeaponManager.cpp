/*
    Grand Theft CO-OP: Vice City
    -----------------------------
    FILE: CWeaponManager.cpp
    DESCRIPTION: Manages the weapons in game.
    AUTHOR(S): Vektor

    License: GPL v3
    Copyrights (c) 2017-2017 GTC Team
*/
#include "pch.h"

CWeaponManager::CWeaponManager() {};

void CWeaponManager::GiveWeapon(CPed* ped, eWeaponType weapon, unsigned int ammo, bool armed)
{
	CWeaponInfo * winfo = new CWeaponInfo();
	winfo = CWeaponInfo::GetWeaponInfo(weapon);
	if (winfo->m_nModelId >= 0)gModelManager->LoadModel(winfo->m_nModelId);
	if (winfo->m_nModel2Id >= 0)gModelManager->LoadModel(winfo->m_nModel2Id);
	FindPlayerPed()->GiveWeapon(weapon, ammo, armed);
}