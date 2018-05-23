#pragma once
class CVehicleManager : public CEntityManager
{
public:
	CVehicleManager();	

	CVehicle *Create(int modelid, const CVector& position);
	eVehicleType GetVehicleType(int modelid);
};