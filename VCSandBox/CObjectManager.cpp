/*
	Grand Theft CO-OP: Vice City
    -----------------------------
    FILE: CObjectManager.cpp
    DESCRIPTION: Manages the objects entities in game
    AUTHOR(S): Crspy

    License: GPL v3
    Copyrights (c) 2017-2017 GTC Team
*/

#include "pch.h"
CObject *CObjectManager::CreateObject(unsigned int model, CVector pos) {
	if (gModelManager->LoadModel(model))
	{
		
		CObject* object = new CObject(model, false);
		object->m_nType = eObjectType::OBJECT_MISSION;
		object->m_placement.pos = pos;
		object->m_fAttachForce = 0.0f;
		object->m_placement.UpdateRW();
		object->UpdateRwFrame();
		CWorld::Add(object);

		return object;
		
	}
	return nullptr;
}
