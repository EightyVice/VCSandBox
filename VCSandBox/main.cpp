#include "pch.h"
#include "CCustomConsole.h"
#include "CLogger.h"

CModelManager			*gModelManager = NULL;
CVehicleManager			*gVehicleManager = NULL;
CPedManager				*gPedManager = NULL;
CPlayerPedManager		*gPlayerPedManager = NULL;
CPopulationManager		*gPopulationManager = NULL;
CWeaponManager			*gWeaponManager = NULL;

// fowrward declaration
int InstallExceptionCatcher(void(*OnException)(const char* log));

CLogger* gLogger = nullptr;

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

	// Install exception filter to log crashes
	InstallExceptionCatcher([](const char* buffer)
	{
		gLogger->log(buffer);
	});

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gLogger = new CLogger("VCSandBox.log");
		DoPatches();
		break;
	case DLL_PROCESS_DETACH:
		if (gLogger)
		{
			delete gLogger;
			gLogger = nullptr;
		}
		break;
	}
	return TRUE;
}

