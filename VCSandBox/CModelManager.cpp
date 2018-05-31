#include "pch.h"

CModelManager::CModelManager() {
	printf("[CMODELMANAGER] Started.\n");
}

bool CModelManager::IsModelLoaded(int modelid)
{
	return CStreaming::ms_aInfoForModel[modelid].m_nLoadState == LOADSTATE_LOADED;
}

bool CModelManager::LoadModel(int modelid)
{
	bool loaded = this->IsModelLoaded(modelid);
	if (!loaded)
	{
		unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelid].m_nFlags;
		CStreaming::RequestModel(modelid, 0);
		CStreaming::LoadAllRequestedModels(false);

		loaded = this->IsModelLoaded(modelid);
		if (loaded)
		{
			if (!(oldFlags & 1))
			{
				CStreaming::SetModelIsDeletable(modelid);
			}
		}
	}
	return loaded;
}