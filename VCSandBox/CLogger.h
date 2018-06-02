#pragma once
#include <Windows.h>
#include <string>
#include <fstream>      // std::ofstream

class CLogger
{
public:
	CLogger(std::string acFileName) :
		pLogStream(new std::ofstream(acFileName)),
		m_fileName(acFileName)
	{
	}

	~CLogger()
	{
		delete pLogStream;
	}

	template <typename ...Args>
	void log(const std::string& format, Args && ...args)
	{
		auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
		std::string output(size + 1, '\0');
		std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);

		if (pLogStream)
		{
			if (!pLogStream->is_open())
				pLogStream->open(m_fileName, std::fstream::out | std::fstream::app);

			SYSTEMTIME	t;
			GetLocalTime(&t);

			static char szFormat[1024];
			sprintf(szFormat, "%02d/%02d/%04d %02d:%02d:%02d - ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond);

			*pLogStream << szFormat << output << "\n";
			pLogStream->close();
		}
	}

private:
	std::ofstream* pLogStream;
	std::string m_fileName;
};

extern CLogger* gLogger;