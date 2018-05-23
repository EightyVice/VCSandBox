#include "main.h"


#define MAX_PEDS_IN_STREAM_ZONE			70
#define MAX_VEHICLES_IN_STREAM_ZONE		15
#define STREAM_DISTANCE					150

int pedModelIds[] = { 7, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 43, 44, 45, 46,
47, 48, 49, 50, 51, 52, 57, 58, 59, 60, 61, 62, 66, 67, 68, 70, 71, 72, 73, 78, 79, 80, 81, 82, 83, 84, 94, 95, 96, 97, 98, 99, 100, 101,
102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 120, 121, 122, 123, 124, 125, 126, 127, 128, 132,
133, 134, 135, 136, 137, 142, 143, 144, 146, 147, 153, 154, 155, 156, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 170, 171,
173, 174, 175, 176, 177, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 200, 202, 203, 204, 206, 209, 210, 212, 213, 217, 220,
221, 222, 223, 227, 228, 229, 230, 234, 235, 236, 239, 240, 241, 242, 247, 248, 249, 250, 252, 253, 254, 255, 258, 259, 260, 261, 262,
9, 10, 11, 12, 13, 31, 38, 39, 40, 41, 53, 54, 55, 56, 63, 64, 69, 75, 76, 77, 85, 87, 88, 89, 90, 91, 92, 93, 129, 130, 131, 138, 139,
140, 141, 145, 148, 150, 151, 152, 157, 169, 172, 178, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 201, 205, 207, 211, 214, 215,
216, 218, 219, 224, 225, 226, 231, 232, 233, 237, 238, 243, 244, 245, 246, 251, 256, 257, 263 };

int vehModelIds[] = { 400, 401, 402, 404, 405, 410, 411, 412, 413, 414, 415, 418, 419, 420, 421, 422, 424, 426, 429, 436, 438, 439, 445, 451, 458, 461, 462, 463,
466, 467, 468, 471, 474, 475, 477, 478, 479, 480, 482, 489, 491, 492, 496, 500, 505, 506, 507, 516, 517, 518, 521, 522, 526, 527, 529, 533, 534, 535, 536, 540, 541, 542,
542, 543, 545, 546, 547, 549, 550, 551, 554, 555, 558, 559, 560, 561, 562, 565, 566, 567, 568, 575, 576, 579, 580, 581, 585, 586, 587,
589, 600, 602, 603 };

bool canManagePopulation = false;
bool gameReady = false;
int startTime = 0;

int spawned = 0;
CPed* testPed = nullptr;

CPopulationManager::CPopulationManager()
{
	Events::gameProcessEvent += []
	{
		/*if (testPed)
		{
		CWorld::Remove(testPed);
		operator_delete<CPed>(testPed);
		testPed = nullptr;
		}
		int model = pedModelIds[rand() % 250];
		printf("Spawning ped: %d with model %d\n", spawned, model);
		testPed = gPedManager->Create(model, FindPlayerPed(0)->GetPosition()+CVector(2.0, 0.0, 0.0), true);
		testPed->SetOrientation(0.0, 0.0, 0.0);*
		spawned++;*/
		gPopulationManager->Update();
	};
}


void CPopulationManager::Update()
{
	if (!gameReady)
	{
		if (FindPlayerPed())
		{
			gameReady = true;

			//startTime = CTimer::m_snTimeInMilliseconds;
		}
	}
	else
	{
		if (!canManagePopulation)
		{
			if (CTimer::m_snTimeInMilliseconds - startTime > 5000)
			{
				canManagePopulation = true;
			}
		}
	}

	if (canManagePopulation)
	{
		CPed * player = FindPlayerPed();
		CVector position = player->GetPosition();
		int pedsSpawned = this->peds.size();
		int vehiclesSpawned = this->vehicles.size();
		int pedsNeeded = MAX_PEDS_IN_STREAM_ZONE - pedsSpawned;
		int vehiclesNeeded = MAX_VEHICLES_IN_STREAM_ZONE - vehiclesSpawned;

		if (pedsNeeded > 0)
		{
			for (int i = 0; i < pedsNeeded; i++)
			{
				if (vehiclesNeeded > 0)
				{
					CVector spawnPos = this->GetPedCreationCoords(position, true);
					CEntity ** entities = nullptr;
					short out;
					CWorld::FindObjectsInRange(spawnPos, 30.0, false, &out, 1, entities, false, true, false, false, false);
					short out2;
					CWorld::FindObjectsInRange(spawnPos, 10.0, false, &out2, 1, entities, false, false, true, false, false);
					out += out2;

					int modelID = pedModelIds[rand() % 249];

					printf("spawned ped with model %d pool: %d/%d\n", modelID, CPools::ms_pPedPool->GetNoOfUsedSpaces(), CPools::ms_pPedPool->GetNoOfFreeSpaces());

					CPed* ped = gPedManager->Create(modelID, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 0.6f), true);
					if (!ped)return;

					if (out == 0)
					{
						int vehModel = vehModelIds[rand() % 93];
						printf("vehmodel: %d\n", vehModel);
						CVehicle* veh = gVehicleManager->Create(vehModel, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 1.0f));

						/* causes crashes
						CNodeAddress node1, node2;
						CCarCtrl::FindNodesThisCarIsNearestTo(veh, node1, node2);
						CPathNode * pathNode = ThePaths.GetPathNode(node1);
						CVector toPos = pathNode->GetNodeCoors();
						*/

						this->vehicles.push_back(veh);
						vehiclesNeeded--;
						veh->SetDriver(ped);
						ped->m_pVehicle = veh;
						CCarCtrl::JoinCarWithRoadSystem(veh);
						ped->SetEnterCar(veh, 0);
						//CCarEnterExit::SetPedInCarDirect(ped, veh, 0, true);
					}
					else
					{
						spawnPos = this->GetPedCreationCoords(spawnPos);
						ped->Teleport(spawnPos);
					}

					this->peds.push_back(ped);
				}
				else
				{
					CVector spawnPos = this->GetPedCreationCoords(position);
					CEntity ** entities = nullptr;
					short out;
					CWorld::FindObjectsInRange(spawnPos, 10.0, false, &out, 1, entities, false, false, true, false, false);
					if (out > 0)continue;
					//printf("%d: %.2f %.2f %.2f \n", i, spawnPos.x, spawnPos.y, spawnPos.z);
					int modelID = pedModelIds[rand() % 249];
					printf("spawned ped with model %d pool: %d/%d\n", modelID, CPools::ms_pPedPool->GetNoOfUsedSpaces(), CPools::ms_pPedPool->GetNoOfFreeSpaces());
					CPed* ped = gPedManager->Create(modelID, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 0.6f), true);
					if (!ped)return;
					peds.push_back(ped);
				}

				//spawned++;
			}
		}

		for (auto i = this->peds.begin(); i != this->peds.end();)
		{
			CPed * ped = *i;
			CVector pedPos = ped->GetPosition();
			if (DistanceBetweenPoints(position, pedPos) > STREAM_DISTANCE)
			{
				printf("Delete one ped\n");

				CWorld::Remove(ped);
				operator_delete<CPed>(ped);
				ped = nullptr;
				i = this->peds.erase(i);
			}
			else
			{
				++i;
			}
		}
		for (auto i = this->vehicles.begin(); i != this->vehicles.end();)
		{
			CVehicle * veh = *i;
			CVector pedPos = veh->GetPosition();
			if (DistanceBetweenPoints(position, pedPos) > STREAM_DISTANCE)
			{
				printf("Delete one veh\n");
				if (veh->m_pDriver)
				{
					CPed * ped = veh->m_pDriver;
					this->peds.remove(ped);
					CWorld::Remove(ped);
					operator_delete<CPed>(ped);
					ped = nullptr;
				}
				CWorld::Remove(veh);
				operator_delete<CVehicle>(veh);
				veh = nullptr;
				i = this->vehicles.erase(i);
			}
			else
			{
				++i;
			}
		}
		//printf("Peds: %d  needed: %d\n", pedsSpawned, pedsNeeded);
	}
}

CVector CPopulationManager::GetPedCreationCoords(CVector pos, bool vehicle)
{
	int xAddition = Random(0, STREAM_DISTANCE) - STREAM_DISTANCE / 2;
	int yAddition = Random(0, STREAM_DISTANCE) - STREAM_DISTANCE / 2;

	CVector out;
	out = pos;
	out.x += xAddition;
	out.y += yAddition;

	CNodeAddress pathLink;
	CNodeAddress * node = nullptr;
	//CNodeAddress * FindNodeClosestToCoors(CPathFind *g_pathfind, CNodeAddress *pathLink, float X, float Y, float Z, int nodeType, float maxDistance, __int16 unk2, int unk3, __int16 unk4, __int16 bBoatsOnly, int unk6)
	if (!vehicle)node = plugin::CallMethodAndReturn<CNodeAddress*, 0x437150, CPathFind*, CNodeAddress*, CVector, int, float, INT16, int, INT16, INT16, int>(&ThePaths, &pathLink, out, 1, 999999.0, 1, 0, 0, 0, 0);
	else node = plugin::CallMethodAndReturn<CNodeAddress*, 0x437150, CPathFind*, CNodeAddress*, CVector, int, float, INT16, int, INT16, INT16, int>(&ThePaths, &pathLink, out, 0, 999999.0, 1, 0, 0, 0, 0);
	CPathNode * pathNode = (CPathNode*)node;
	CVector nodepos = { (float)pathNode->m_wPosX, (float)pathNode->m_wPosY, (float)pathNode->m_wPosZ };
	return nodepos;
}