#include "SystemCommonUtils.h"

#include <tlhelp32.h>

DWORD SystemCommonUtils::m_mainThreadId = -1;

bool SystemCommonUtils::getMainThreadIdFromProcessId(DWORD dwPId, DWORD& dwTId)
{
	bool ret = false;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return ret;

	THREADENTRY32 te = { sizeof(THREADENTRY32) };
	if (Thread32First(hSnapshot, &te))
	{
		do
		{
			if (dwPId == te.th32OwnerProcessID)      // 认为找到的第一个该进程的线程为主线程
			{
				dwTId = te.th32ThreadID;
				ret = true;
				break;
			}
		} while (Thread32Next(hSnapshot, &te));
	}
	CloseHandle(hSnapshot); 

	return ret;
}

void SystemCommonUtils::initCurrentMainThreadId(DWORD* dwTId/* = nullptr*/)
{
	if (m_mainThreadId != -1)
		return;

	if (dwTId && *dwTId != 0 && *dwTId != -1)
	{
		m_mainThreadId = *dwTId;
		return;
	}

	getMainThreadIdFromProcessId(::GetCurrentProcessId(), m_mainThreadId);
}

bool SystemCommonUtils::getCurrentMainThreadId(DWORD& dwTId)
{
	initCurrentMainThreadId();

	if (m_mainThreadId == -1)
		return false;

	dwTId = m_mainThreadId;
	return true;
}