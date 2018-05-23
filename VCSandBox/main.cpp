#include "main.h"



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
        
    }
} vCSandBox;
