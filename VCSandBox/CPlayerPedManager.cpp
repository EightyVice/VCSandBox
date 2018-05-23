#include "main.h"

CPad			localPlayerKeys;
CMatrix			cameraAimMatrix;
eCamMode		localPlayerCameraMode;

char(__thiscall* original_CPlayerPed__ProcessControl)(CPlayerPed*);
char __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed * This, DWORD _EDX)
{
	if (This != FindPlayerPed())
	{
		int currentPlayerID = gPlayerPedManager->GetPlayerPedSlot(This);
		//printf("pcontrol: %d\n", currentPlayerID);
		if (currentPlayerID == -1)return 0;

		// set player to focus
		CPlayerPed * localPlayer = CWorld::Players[0].m_pPed;
		CWorld::Players[0].m_pPed = gPlayerPedManager->players[currentPlayerID];
		
		// save local player keys
		localPlayerKeys = *CPad::GetPad(0);

		// set remote player's keys
		*CPad::GetPad(0) = *new CPad();

		// save the internal cammode.
		localPlayerCameraMode = (eCamMode)TheCamera.Cams[TheCamera.ActiveCam].Mode;

		// onfoot mouse looking mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = 0;
		// save local player's aim
		cameraAimMatrix = *(CMatrix*)&TheCamera.Cams[TheCamera.ActiveCam].Front;

		// set remote player's aim
		*(CMatrix*)&TheCamera.Cams[TheCamera.ActiveCam].Front = cameraAimMatrix;//set remote here

		// call the internal CPlayerPed[]::Process
		original_CPlayerPed__ProcessControl(This);

		// restore local player keys
		*CPad::GetPad(0) = localPlayerKeys;

		CWorld::Players[0].m_pPed = localPlayer;

		// restore the camera mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = localPlayerCameraMode;

		//restore local player's aim
		*(CMatrix*)&TheCamera.Cams[TheCamera.ActiveCam].Front = cameraAimMatrix;
		return 0;
	}
	return original_CPlayerPed__ProcessControl(This);
}

CPlayerPedManager::CPlayerPedManager() 
{
	for (int i = 0; i < 500; i++)
	{
		this->playerSlots[i] = false;
	}
	this->playerSlots[0] = true; //0 is always local player
	//this->playerSlots[1] = true; //1 is always the second player, coop leftover 

	original_CPlayerPed__ProcessControl = (char(__thiscall*)(CPlayerPed*))DetourFunction((PBYTE)0x60EA90, (PBYTE)CPlayerPed__ProcessControl_Hook);
}

CPlayerPed *CPlayerPedManager::Create(int modelid, const CVector& position)
{
	gModelManager->LoadModel(modelid);
	int playerid = this->GetFreePlayerSlot(); 

	CPlayerPed* player = operator_new<CPlayerPed>();

	player->m_nPedFlags = FindPlayerPed()->m_nPedFlags;
	player->m_dwPedType = ePedType::PEDTYPE_PLAYER2;
	printf("[CPlayerPedManager]Create - %d\n", playerid);
	CWorld::Add(player);
	player->SetModelIndex(modelid);
	player->m_placement.SetOrientation(0.0f, 0.0f, 0.0f);
	player->Teleport(position);
	this->playerSlots[playerid] = true;
	this->players[playerid] = player;
	return player;
}

int CPlayerPedManager::GetFreePlayerSlot()
{
	for (int i = 1; i < 500; i++)
	{
		if (!this->playerSlots[i])return i;
	}
	return -1;
}

int CPlayerPedManager::GetPlayerPedSlot(CPlayerPed * player)
{
	for (int i = 0; i < 500; i++)
	{
		if (this->players[i] == player)return i;
	}
	return -1;
}