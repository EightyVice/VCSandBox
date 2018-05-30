#include "main.h"

#include "CPathFind.h"

CPedManager::CPedManager() {}

CPed *CPedManager::Create(int modelid, const CVector& position, bool wander)
{
	if (gModelManager->LoadModel(modelid))
	{
		CPed* ped = new CCivilianPed(CPopulation::IsFemale(modelid) ? PEDTYPE_CIVFEMALE : PEDTYPE_CIVMALE, modelid);
		ped->m_nPedFlags.bRespondsToThreats = false;
		ped->m_nPedFlags.b50 = false;
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