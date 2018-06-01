#pragma once
#include <Windows.h>
#include <string>
#include <deque>
#include "plugin.h"

#define MAX_CHARACTERS 100
#define MAX_BUFFER_SIZE 10

class CCustomConsole
{
public:
	static bool m_bIsActive;
	static bool m_bIsInitialized;
	static std::string m_TypedString;
	static std::string m_LastString;
	static std::deque<std::string> m_Buffer;
	static void Render();
	static void RegisterOneChar(char c);
	static void RegisterOneLine(std::string str);
};

struct SRect
{
	union
	{
		struct
		{
			CVector2D m_tl;
			CVector2D m_br;
		};
		struct
		{
			float m_fLeft;          // x1
			float m_fBottom;        // y1
			float m_fRight;         // x2
			float m_fTop;           // y2
		};
		struct
		{
			float m_fX1;          // x1
			float m_fY1;        // y1
			float m_fX2;         // x2
			float m_fY2;           // y2
		};
	};

	SRect() {}

	CRect &GetRect()
	{
		return *reinterpret_cast<CRect*>(this);
	}
};
VALIDATE_SIZE(SRect, 0x10);