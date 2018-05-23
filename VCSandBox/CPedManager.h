#pragma once

class CPedManager : public CEntityManager
{
public:
	CPedManager();
	~CPedManager();

	CPed *Create(int modelid, const CVector& position, bool wander = false);
};