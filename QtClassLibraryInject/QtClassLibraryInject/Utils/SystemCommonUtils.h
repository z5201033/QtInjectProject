#pragma once

#include <windows.h>

class SystemCommonUtils
{
	static DWORD m_mainThreadId;
public:
	static bool getMainThreadIdFromProcessId(DWORD dwPId, DWORD& dwTId);
	static void initCurrentMainThreadId(DWORD* dwTId = nullptr);
	static bool getCurrentMainThreadId(DWORD& dwTId);

};

