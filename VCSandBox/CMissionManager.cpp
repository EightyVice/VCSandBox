/*
    Grand Theft CO-OP: Vice City
    -----------------------------
    FILE: CMissionManager.cpp
    DESCRIPTION: Manages the missions in game.
    AUTHOR(S): LemonHaze
               Zurix

    License: GPL v3
    Copyrights (c) 2017-2017 GTC Team
*/
#include "pch.h"
void CMissionManager::MountMainSCM() {
    //The First Mission will be loaded
    
    // Load the SCM Script (restore)..
    MemCpy(0x4506D6, "\xE8\x55\xD9\x03\x00", 5);

    // Restore the CRunningScript::Process() hook..
    MemCpy((void*)0x450245, "\xE8\x26\xFB\xFF\xFF", 5);

    // Call CTheScripts::Init(void)
    Call(0x450330);
    // Call CTheScripts::StartNewScript(int)
    CRunningScript* rs = CallAndReturn<CRunningScript*, 0x450270, int>(0);
    
}
void CMissionManager::StartMission(unsigned int iMissionID) {
    /*
        The main.scm should be mounted before starting any mission
        You dont have to choose the first mission when its mounted because the first mission will start
        any way it will be failed, and went to the chosen mission
    */
    Command<Commands::FAIL_CURRENT_MISSION>();
    Command<Commands::LOAD_AND_LAUNCH_MISSION>(iMissionID);
}
void CMissionManager::FailCurrentMission() {
    Command<Commands::FAIL_CURRENT_MISSION>();
}