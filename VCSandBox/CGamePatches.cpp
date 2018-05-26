#include "main.h"
#include "CGamePatches.h"
#include "CWeather.h"

CGamePatches::CGamePatches()
{
	printf("[CGAMEPATCHES] Processing...");
	this->MenuPatches();
	this->PopulationPatches();
	this->RunningScriptHook();
	this->GameLogicPatches();
	this->WantedPatches();
	this->CrashfixHooks();

	this->LimitPatches();
	printf("[CGAMEPATCHES] Main Patches Done");
}


static bool menuFirstProcessed = false;

void ProcessFrontEndMenu()
{
	if (!menuFirstProcessed)
	{
		// Start the game now - resume the timers
		injector::WriteMemory<UINT8>(0xB7CB49, 0);
		injector::WriteMemory<UINT8>(0xBA67A4, 0);
		injector::WriteMemory<INT32>(0xC8D4C0, 8); // gGameState
		menuFirstProcessed = true;
	}
	// Call the original function - Sets render states
	plugin::Call<0x573A60>();
}

void CGamePatches::MenuPatches()
{
	//Disable CMenuManage::Process
	MakeRet(0x49A01C);

	//Disable CMenuManager::DrawBackground
	MakeRet(0x4A212D);

	//Disable CMenuManager::DrawFrontEnd
	MakeRet(0x4A37A4);

	//Disable CMenuManager::LoadAllTextures
	MakeRet(0x4A3A13);

	//Disable menu after focus loss
	MakeRet(0x4A4FD0);
}


static bool scriptProcessed = false;
void Hook_CRunningScript__Process()
{
	if (!scriptProcessed)
	{
		CPools::ms_pPedPool->Clear();
		CPools::ms_pVehiclePool->Clear();


		// Change player model ID
		MemWrite<u8>(0x5384FA + 1, 1); //Not important if we set a new one after spawn

									   // Setup own ped on 0 game ID
		CPlayerPed::SetupPlayerPed(0);


		// Set player position
		FindPlayerPed()->m_placement.pos = { 531.629761f, 606.497253f, 10.901563f };

		// CStreaming::LoadScene
		CVector scenePosition = { 531.629761f, 606.497253f, 10.901563f };
		Call(0x40AF60, &scenePosition);



		CWorld::Players[0].m_bNeverGetsTired = true;





		CStreaming::RequestModel(269, 1);
		CStreaming::RequestModel(270, 1);
		CStreaming::RequestModel(275, 1);
		CStreaming::RequestModel(278, 1);
		CStreaming::RequestModel(284, 1);
		CStreaming::RequestModel(280, 1);
		CStreaming::RequestModel(286, 1);
		CStreaming::RequestModel(290, 1);
		CStreaming::RequestModel(294, 1);
		CStreaming::RequestModel(268, 1);
		CStreaming::RequestModel(270, 1);
		CStreaming::RequestModel(291, 1);
		CStreaming::RequestModel(275, 1);
		CStreaming::RequestModel(279, 1);
		CStreaming::RequestModel(283, 1);
		CStreaming::RequestModel(280, 1);
		CStreaming::RequestModel(286, 1);
		CStreaming::RequestModel(287, 1);
		CStreaming::RequestModel(259, 1);
		CStreaming::RequestModel(264, 1);
		CStreaming::RequestModel(272, 1);
		CStreaming::RequestModel(274, 1);
		CStreaming::RequestModel(277, 1);
		CStreaming::RequestModel(281, 1);
		CStreaming::RequestModel(276, 1);
		CStreaming::RequestModel(285, 1);
		CStreaming::RequestModel(288, 1);
		CStreaming::LoadAllRequestedModels(0);



		// First tick processed
		scriptProcessed = true;
	}
}



void CGamePatches::RunningScriptHook()
{
	// Don't load the SCM Script
	MakeShortJmp(0x4506D6, 0x45070E);

	injector::MakeCALL(0x450245, Hook_CRunningScript__Process);
	

}

void CGamePatches::GameLogicPatches()
{
	RsGlobal.appName = "ViceCitySandBox";

}

void CGamePatches::WantedPatches()
{
	// Disable Police Mavericks and News choppers at 3+ wanted stars
	// by making CWanted::NumOfHelisRequired always return 0
	MakeRet0(0x4D1DE0);
	// Disable CWanted::SetWantedLevel
	injector::MakeRET(0x4D1FA0);
}





void CGamePatches::CrashfixHooks()
{
	// Disable CRubbish::Init() (crashfix)
	MakeRet(0x568550);

}




void CGamePatches::PopulationPatches()
{
	// Disable CPopulation::AddToPopulation()
	//MakeRet(0x53BA80);

	// Disable CPopulation::AddPedsAtStartOfGame()
	//MakeRet(0x53E3E0);



	//Nop ped spawns
	MakeNop(0x53E5C6, 5); //3peds
	MakeNop(0x53E99B, 5);

	//disable cworld:remove in CPopulation::ManagePopulation
	MakeRet(0x53D690);

	//Nop removals in CPopulation::Remove
	MakeNop(0x53D896, 23);
	MakeNop(0x53D82F, 23);
	MakeNop(0x53D9E5, 19);
	MakeNop(0x53DBEC, 32);
	MakeNop(0x53E2E6, 12);

	//Disable CPopulation::Removeped
	MakeRet(0x53B160);


	// Disable CCarCtrl::GenerateRandomCars
	MakeRet(0x4292A0);

	// Disable CCarCtrl::GenerateOneRandomCar
	MakeRet(0x426DB0);
}

void CGamePatches::LimitPatches()
{
	/*


	*/
	//Pedpool inc
	MemWrite<s32>(0x4C02C8, 1000);
	//vehicle pool inc
	MemCpy((void*)0x4C02E4, "\x6A\x00\x68\x3E\x80\x00\x00", 7);//its 1000 i tested it
}