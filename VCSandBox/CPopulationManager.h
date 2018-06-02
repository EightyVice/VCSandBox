#pragma once

class CPopulationManager
{
public:
	CPopulationManager();
	

	void Update();
	CVector GetPedCreationCoords(CVector pos, bool vehicle);

	std::list<CPed*> peds;
	std::list<CVehicle*> vehicles;
};