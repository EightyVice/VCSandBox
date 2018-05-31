#include "pch.h"
#include "CCustomConsole.h"

CModelManager			*gModelManager = NULL;
CVehicleManager			*gVehicleManager = NULL;
CPedManager				*gPedManager = NULL;
CPlayerPedManager		*gPlayerPedManager = NULL;
CPopulationManager		*gPopulationManager = NULL;
CWeaponManager			*gWeaponManager = NULL;

void DoPatches()
{
	//patches dont need an instance yet, maybe later when we will use toggleable patches
	gPlayerPedManager = new CPlayerPedManager();
	new CGamePatches();

	gModelManager = new CModelManager();
	gVehicleManager = new CVehicleManager();
	gPedManager = new CPedManager();
	gPopulationManager = new CPopulationManager();
	gWeaponManager = new CWeaponManager();

	plugin::Events::drawingEvent += []
	{
		CCustomConsole::Render();
	};
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DoPatches();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

