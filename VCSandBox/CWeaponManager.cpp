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