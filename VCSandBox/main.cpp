#include "pch.h"

CModelManager			*gModelManager = NULL;
CVehicleManager			*gVehicleManager = NULL;
CPedManager				*gPedManager = NULL;
CPlayerPedManager		*gPlayerPedManager = NULL;
CPopulationManager		*gPopulationManager = NULL;
CWeaponManager			*gWeaponManager = NULL;

class VCSandBox {
public:
    VCSandBox() {
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		//patches dont need an instance yet, maybe later when we will use toggleable patches
		gPlayerPedManager = new CPlayerPedManager();
		new CGamePatches();

		gModelManager = new CModelManager();
		gVehicleManager = new CVehicleManager();
		gPedManager = new CPedManager();
		gPopulationManager = new CPopulationManager();
		gWeaponManager = new CWeaponManager();
    }
} vCSandBox;
