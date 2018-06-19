#pragma once
#include "pch.h"

class CMissionManager {
public:
    void MountMainSCM();
    void StartMission(unsigned int iMissionID);
    void FailCurrentMission();
};