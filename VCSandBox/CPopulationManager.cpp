#include "pch.h"
#include "CCustomConsole.h"

#define MAX_PEDS_IN_STREAM_ZONE			70
#define MAX_VEHICLES_IN_STREAM_ZONE		15
#define STREAM_DISTANCE					150

int pedModelIds[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54 };

int vehModelIds[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,130,131,132,133,134,135,136,137
,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169 };


bool canManagePopulation = false;
bool gameReady = false;
int startTime = 0;

int spawned = 0;
CPed* testPed = nullptr;

template <typename ...Args>
void cprintf(const std::string& format, Args && ...args)
{
    auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);

    CCustomConsole::RegisterOneLine(output);
}
int handle;
CPopulationManager::CPopulationManager()
{

    Events::gameProcessEvent += []
    {
        if (KeyPressed(VK_TAB)) {

            gObjectManager->CreateObject(583, FindPlayerCoors());
        }
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
            if (CTimer::m_snTimeInMilliseconds - startTime > 1000)
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

        CPathNode* pNode = nullptr;

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

                    int modelID = 7;

                    cprintf("spawned ped with model %d pool: %d/%d", modelID, CPools::ms_pPedPool->GetNoOfUsedSpaces(), CPools::ms_pPedPool->GetNoOfFreeSpaces());

                    CPed* ped = gPedManager->Create(modelID, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 0.6f), true);
                    //gRadarManager->SetBlipToCoors(spawnPos);
                    if (!ped)return;

                    if (out == 0)
                    {
                        int vehModel = 232;
                        cprintf("vehmodel: %d", vehModel);
                        CVehicle* veh = gVehicleManager->Create(vehModel, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 1.0f));

                        this->vehicles.push_back(veh);
                        vehiclesNeeded--;
                        ((CAutomobile*)(veh))->PlaceOnRoadProperly();
                        CCarCtrl::JoinCarWithRoadSystem(veh);

                        // opcode 0x36A
                        ped->SetObjective(OBJECTIVE_ENTER_CAR_AS_DRIVER);

                        veh->SetUpDriver();
                        veh->m_nCreatedBy = eVehicleCreatedBy::PERMANENT_VEHICLE;
                    }
                    else
                    {
                        spawnPos = this->GetPedCreationCoords(spawnPos, false);
                        ped->m_placement.pos = spawnPos;
                    }

                    this->peds.push_back(ped);
                }
                else
                {
                    CVector spawnPos = this->GetPedCreationCoords(position, false);
                    CEntity ** entities = nullptr;
                    short out;
                    CWorld::FindObjectsInRange(spawnPos, 10.0, false, &out, 1, entities, false, false, true, false, false);
                    if (out > 0)continue;
                    //printf("%d: %.2f %.2f %.2f ", i, spawnPos.x, spawnPos.y, spawnPos.z);
                    int modelID = 7;
                    cprintf("spawned ped with model %d pool: %d/%d", modelID, CPools::ms_pPedPool->GetNoOfUsedSpaces(), CPools::ms_pPedPool->GetNoOfFreeSpaces());
                    CPed* ped = gPedManager->Create(modelID, CVector(spawnPos.x, spawnPos.y, spawnPos.z + 0.6f), true);
                    if (!ped)return;
                    peds.push_back(ped);
                }

                //spawned++;
            }
        }

        for (auto i = this->peds.begin(); i != this->peds.end();)
        {
            CPed * pPed = *i;
            CVector pedPos = pPed->GetPosition();
            if (DistanceBetweenPoints(position, pedPos) > STREAM_DISTANCE)
            {
                cprintf("Delete one ped");

                plugin::Call<0x45EB10, CPed*>(pPed);    // CTheScripts::CleanUpThisPed
                i = this->peds.erase(i);

            }
            else
            {
                ++i;
            }
        }

        for (auto i = this->vehicles.begin(); i != this->vehicles.end();)
        {
            CVehicle * pVeh = *i;
            CVector pVehPos = pVeh->GetPosition();
            if (DistanceBetweenPoints(position, pVehPos) > STREAM_DISTANCE)
            {
                cprintf("Delete one veh");
                if (pVeh->m_pDriver)
                {
                    CPed * pPed = pVeh->m_pDriver;
                    plugin::Call<0x45EB10, CPed*>(pPed);    // CTheScripts::CleanUpThisPed
                    this->peds.remove(pPed);
                }
                plugin::Call<0x45EAD0, CVehicle*>(pVeh);    // CTheScripts::CleanUpThisVehicle
                i = this->vehicles.erase(i);
            }
            else
            {
                ++i;
            }
        }
        //printf("Peds: %d  needed: %d", pedsSpawned, pedsNeeded);
    }
}

enum ePathType
{
    PATH_CAR = 0,
    PATH_PED = 1,
};

CVector GetPathNodeCoors(const CPathNode *pNode)
{
    return CVector(static_cast<float>(pNode->m_wPosX)*0.125f, static_cast<float>(pNode->m_wPosY)*0.125f, static_cast<float>(pNode->m_wPosZ)*0.125f);
}

CVector CPopulationManager::GetPedCreationCoords(CVector vPos, bool bVehicle)
{
    int xAddition = Random(0, STREAM_DISTANCE) - STREAM_DISTANCE / 2;
    int yAddition = Random(0, STREAM_DISTANCE) - STREAM_DISTANCE / 2;

    int iPath = ThePaths.FindNodeClosestToCoors(CVector(vPos.x + xAddition, vPos.y + yAddition, vPos.z), bVehicle ? PATH_CAR : PATH_PED, 9999.0f, 1, 0, 0, 0);
    //*pNode = ThePaths.nodes[iPath];
    return GetPathNodeCoors(&ThePaths.nodes[iPath]);
}
