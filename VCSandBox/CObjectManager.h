/*
    Grand Theft CO-OP: Vice City
    -----------------------------
    FILE: CObjectManager.cpp
    DESCRIPTION: Manages the objects entities in game
    AUTHOR(S): Crspy

    License: GPL v3
    Copyrights (c) 2017-2017 GTC Team
*/
#pragma once
#include "pch.h"

class CObjectManager {
public:
	CObject* CreateObject(unsigned int model, CVector pos);
	
};
