#include "pch.h"
CObject *CObjectManager::CreateObject(unsigned int model, CVector pos) {
	if (gModelManager->LoadModel(model))
	{
		//Special thanks for Crspy
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