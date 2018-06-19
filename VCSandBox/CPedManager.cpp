/*
    Grand Theft CO-OP: Vice City
    -----------------------------
    FILE: CPedManager.cpp
    DESCRIPTION: Manages the pedestrians in game.
    AUTHOR(S): Vektor
               Zurix
               Statham

    License: GPL v3
    Copyrights (c) 2017-2017 GTC Team
*/
#include "pch.h"
#include "CPathFind.h"

CPedManager::CPedManager() {
    gMissionManager->MountMainSCM();
    gMissionManager->StartMission(25);
}

CPed *CPedManager::Create(int modelid, const CVector& position, bool wander)
{
	if (gModelManager->LoadModel(modelid))
	{

		CPed* ped = new CCivilianPed(CPopulation::IsFemale(modelid) ? PEDTYPE_CIVFEMALE : PEDTYPE_CIVMALE, modelid);
		ped->m_nPedFlags.bRespondsToThreats = true;
		ped->m_nPedFlags.b50 = false;
		ped->m_nPedStatus = 1;
		ped->m_nPedFlags.bScriptPedIsPlayerAlly = false;
		ped->ClearAll();
		CWorld::Add(ped);

		ped->m_placement.pos = position;
		ped->ClearObjective();
		ped->SetMoveAnim();
		if (wander)
		{
			ped->m_dwObjective = eObjective::OBJECTIVE_WANDER;
		}
		return ped;
	}
	return nullptr;
		
}