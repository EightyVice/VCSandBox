/*
    Ingame-Console 
    ----------------
    FILE: CCustomConsole.cpp
    DESCRIPTION: Implements Ingame-Console
    AUTHOR(S): Statham

    License: GPL v3
    Copyrights (c) 2017-2017 Statham
*/
#include "pch.h"
#include "CCustomConsole.h"
#include "CWorld.h"
#include "CFont.h"
#include "CSprite2D.h"

#undef SetPropA

bool CCustomConsole::m_bIsActive = false;
bool CCustomConsole::m_bIsInitialized = false;
std::string CCustomConsole::m_TypedString;
std::string CCustomConsole::m_LastString;
std::deque<std::string> CCustomConsole::m_Buffer;

wchar_t g2String[512];
void AsciiToUnicode(const char* pStr)
{
	Call<0x552500, const char *, wchar_t *>(pStr, g2String);
}

bool IsKeyPressed(WORD key)
{
	return CallMethodAndReturn<bool, 0x617430, void*, WORD>((void *)0x9B491C, key);
}

class CKeyHook
{
public:
	// Is F1-F12 key pressed.
	inline static bool IsFunctionKeyPressed(unsigned short nKey)
	{
		return IsKeyPressed((nKey - 112) + 1001);
	}

	// Is standar key pressed.
	inline static bool IsStandardKeyPressed(unsigned short nKey)
	{
		return IsKeyPressed(nKey);
	}
};

void CCustomConsole::Render()
{
	if((CKeyHook::IsFunctionKeyPressed(VK_F4) & 1) == true)
	{
		if(CCustomConsole::m_bIsActive == false)
			CCustomConsole::m_bIsActive = true;
		CWorld::Players[0].MakePlayerSafe(true);
		CCustomConsole::m_TypedString.clear();
	}
	else if((CKeyHook::IsFunctionKeyPressed(VK_F5) & 1) == true)
	{
		if(CCustomConsole::m_bIsActive == true)
			CCustomConsole::m_bIsActive = false;
		CWorld::Players[0].MakePlayerSafe(false);
	}
	if(CCustomConsole::m_bIsActive == true)
	{
		if(CCustomConsole::m_TypedString.size() < MAX_CHARACTERS)
		{		
			for(int i = 65; i < 91; ++i)	
				if(CKeyHook::IsStandardKeyPressed(i)) CCustomConsole::RegisterOneChar(tolower(i));
			for(int i = 48; i < 58; ++i)	
				if(CKeyHook::IsStandardKeyPressed(i)) CCustomConsole::RegisterOneChar(i);
			if(CKeyHook::IsStandardKeyPressed(32)) CCustomConsole::RegisterOneChar('_');
			if(CKeyHook::IsStandardKeyPressed(47)) CCustomConsole::RegisterOneChar('/');
		}

		if(CCustomConsole::m_TypedString.size() > 0)
		{
			if(CKeyHook::IsStandardKeyPressed(1042))	// key.back
			{
				CCustomConsole::m_TypedString.erase(CCustomConsole::m_TypedString.end() -1, CCustomConsole::m_TypedString.end());

				if(CCustomConsole::m_TypedString.size() == NULL)
				{
					if(CCustomConsole::m_bIsActive == true)
						CCustomConsole::m_bIsActive = false;
					CWorld::Players[0].MakePlayerSafe(false);
				}
			}

			if(CKeyHook::IsStandardKeyPressed(1045))	// key.enter
			{
				//if(!CCustomScripts::CallFunctionForCustomScriptsWithParams("OnConsoleCommand", (void *)CCustomConsole::m_TypedString.c_str(), eVariableType::VAR_TYPE_STR))
				CCustomConsole::RegisterOneLine("Unknown command");
				CCustomConsole::m_LastString.assign(CCustomConsole::m_TypedString);
				CCustomConsole::m_TypedString.clear();
			}
		}

		if(CCustomConsole::m_LastString.size() > 0)
		{
			if(CKeyHook::IsStandardKeyPressed(1043))	// key.tab
				CCustomConsole::m_TypedString.assign(CCustomConsole::m_LastString);
		}

		SRect m_Rect;

		m_Rect.m_fX1 = SCREEN_WIDTH_(0.0f);
		m_Rect.m_fY2 = SCREEN_HEIGHT_(0.0f);
		m_Rect.m_fX2 = SCREEN_WIDTH_(640.0f);
		m_Rect.m_fY1 = SCREEN_HEIGHT_(80.0f);
		CSprite2d::DrawRect(m_Rect.GetRect(), CRGBA(0, 0, 0, 150));

		m_Rect.m_fX1 = SCREEN_WIDTH_(0.0f);
		m_Rect.m_fY2 = SCREEN_HEIGHT_(69.0f);
		m_Rect.m_fX2 = SCREEN_WIDTH_(640.0f);
		m_Rect.m_fY1 = SCREEN_HEIGHT_(80.0f);
		CSprite2d::DrawRect(m_Rect.GetRect(), CRGBA(0, 255, 0, 150));

		m_Rect.m_fX1 = SCREEN_WIDTH_(0.0f + 0.3f);
		m_Rect.m_fY2 = SCREEN_HEIGHT_(69.0f + 0.3f);
		m_Rect.m_fX2 = SCREEN_WIDTH_(640.0f - 0.3f);
		m_Rect.m_fY1 = SCREEN_HEIGHT_(80.0f - 0.3f);
		CSprite2d::DrawRect(m_Rect.GetRect(), CRGBA(0, 0, 0, 150));

		if(CCustomConsole::m_TypedString.size() > 0)
		{
			CFont::SetBackgroundOff();
			CFont::SetBackGroundOnlyTextOff();
			CFont::SetJustifyOn();
			CFont::SetWrapx(SCREEN_WIDTH_(640.0f));
			CFont::SetPropOff();
			CFont::SetFontStyle(1);
			CFont::SetDropColor(CRGBA(0, 0, 0, 255));
			CFont::SetScale(SCREEN_WIDTH_(0.2f), SCREEN_HEIGHT_(0.42f));
			CFont::SetColor(CRGBA(255, 255, 255, 255));

			AsciiToUnicode(CCustomConsole::m_TypedString.c_str());
			CFont::PrintString(SCREEN_WIDTH_(5.3f), SCREEN_HEIGHT_(70.0f), g2String);
		}
	}

	for(int i = 0; i < CCustomConsole::m_Buffer.size(); i++)
	{
		CFont::SetBackgroundOff();
		CFont::SetBackGroundOnlyTextOff();
		CFont::SetJustifyOn();
		CFont::SetWrapx(SCREEN_WIDTH_(640.0f));
		CFont::SetPropOff();
		CFont::SetFontStyle(1);
		CFont::SetDropColor(CRGBA(0, 0, 0, 255));
		CFont::SetScale(SCREEN_WIDTH_(0.2f), SCREEN_HEIGHT_(0.42f));
		CFont::SetColor(CRGBA(255, 255, 255, 255));

		AsciiToUnicode(CCustomConsole::m_Buffer.at(i).c_str());
		CFont::PrintString(SCREEN_WIDTH_(5.3f), SCREEN_HEIGHT_((61.0f - (i * 9.0f))), g2String);
	}
}

void CCustomConsole::RegisterOneChar(char c)
{
	char szInsert[2] = {c, '\0'};
	CCustomConsole::m_TypedString.append(szInsert);
}

void CCustomConsole::RegisterOneLine(std::string str)
{
	if(str.size() > 0)
	{
		//AudioEngine.ReportMissionAudioEvent(1056, CVector(0.0, 0.0, 0.0));
		//AudioEngine.ReportCustomSound(eSoundTypes::SOUND_TYPE_HELP_BOX);

		CCustomConsole::m_Buffer.push_front(str);
		if(CCustomConsole::m_Buffer.size() == MAX_BUFFER_SIZE)
			CCustomConsole::m_Buffer.erase(CCustomConsole::m_Buffer.end()-1);
	}
}