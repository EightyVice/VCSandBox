#include "main.h"

#include "CPathFind.h"

CPedManager::CPedManager() {}

CPed *CPedManager::Create(int modelid, const CVector& position, bool wander)
{
	if (gModelManager->LoadModel(modelid))
	{
		CPed* ped = new CCivilianPed(CPopulation::IsFemale(modelid) ? PEDTYPE_CIVFEMALE : PEDTYPE_CIVMALE, modelid);
		ped->Teleport(position);
		ped->m_placement.SetOrientation(0.0f, 0.0f, 0.0f);

		CWorld::Add(ped);
		if (wander)
		{
			ped->m_dwObjective = eObjective::OBJECTIVE_WANDER;
		}
		return ped;
	}
	return nullptr;
}