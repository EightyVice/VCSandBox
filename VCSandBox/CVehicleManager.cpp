#include "main.h"

CVehicleManager::CVehicleManager() {}

CVehicle *CVehicleManager::Create(int modelid, const CVector& position)
{
	if (gModelManager->LoadModel(modelid))
	{
		CVehicle *vehicle = nullptr;

		switch (this->GetVehicleType(modelid))
		{
		case VEHICLE_HELI:
			vehicle = new CHeli(modelid, 1);
			break;
		case VEHICLE_PLANE:
			vehicle = new CPlane(modelid, 1);
			break;
		case VEHICLE_BIKE:
			vehicle = new CBike(modelid, 1);
			//reinterpret_cast<CBike *>(vehicle)->VehicleDamage.m_nFlags |= 0x10;
			break;
		case VEHICLE_BOAT:
			vehicle = new CBoat(modelid, 1);
			break;
		default:
			vehicle = new CAutomobile(modelid, 1);
			break;
		}
		if (vehicle) 
		{
			vehicle->Teleport(position);
			vehicle->m_placement.SetOrientation(0.0f, 0.0f, 0.0f);
			vehicle->m_nLockStatus = CARLOCK_UNLOCKED;
			vehicle->m_nState = 4;
			CWorld::Add(vehicle);
			return vehicle;
		}
	}
	return nullptr;
}

eVehicleType CVehicleManager::GetVehicleType(int modelid)
{
	return (eVehicleType)reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelid])->m_nVehicleType;
}