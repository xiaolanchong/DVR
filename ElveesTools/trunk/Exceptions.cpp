// Exception.cpp: implementation of the CException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Exceptions.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// GetExceptionDescription
//////////////////////////////////////////////////////////////////////

LPCTSTR __cdecl GetExceptionDescription(DWORD dwExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	dwCode;
		LPCTSTR	stName;
	};

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, TEXT("a Control-C")},
		{0x40010008, TEXT("a Control-Break")},
		{0x80000002, TEXT("a Datatype Misalignment")},
		{0x80000003, TEXT("a Breakpoint")},
		{0xc0000005, TEXT("an Access Violation")},
		{0xc0000006, TEXT("an In Page Error")},
		{0xc0000017, TEXT("a No Memory")},
		{0xc000001d, TEXT("an Illegal Instruction")},
		{0xc0000025, TEXT("a Noncontinuable Exception")},
		{0xc0000026, TEXT("an Invalid Disposition")},
		{0xc000008c, TEXT("a Array Bounds Exceeded")},
		{0xc000008d, TEXT("a Float Denormal Operand")},
		{0xc000008e, TEXT("a Float Divide by Zero")},
		{0xc000008f, TEXT("a Float Inexact Result")},
		{0xc0000090, TEXT("a Float Invalid Operation")},
		{0xc0000091, TEXT("a Float Overflow")},
		{0xc0000092, TEXT("a Float Stack Check")},
		{0xc0000093, TEXT("a Float Underflow")},
		{0xc0000094, TEXT("an Integer Divide by Zero")},
		{0xc0000095, TEXT("an Integer Overflow")},
		{0xc0000096, TEXT("a Privileged Instruction")},
		{0xc00000fD, TEXT("a Stack Overflow")},
		{0xc0000142, TEXT("a DLL Initialization Failed")},
		{0xe06d7363, TEXT("a Microsoft C++ Exception")},
	};

	for(int i = 0; i < countof(ExceptionMap); i++)
		if(dwExceptionCode == ExceptionMap[i].dwCode)
			return ExceptionMap[i].stName;

	return TEXT("an Unknown exception type");
}

//////////////////////////////////////////////////////////////////////
// CException
//////////////////////////////////////////////////////////////////////

CException::CException(LPCTSTR lpWhere, LPCTSTR lpMessage)
{
	if(lpWhere)
		lstrcpy(m_stWhere, lpWhere);
	else
		lstrcpy(m_stWhere, TEXT("Enexpected"));

	if(lpMessage)
		lstrcpy(m_stMessage, lpMessage);
	else
		lstrcpy(m_stMessage, TEXT("Unknown error."));
}

CException::~CException()
{
}

LPCTSTR CException::GetWhere() const
{
	return const_cast<LPCTSTR>(m_stWhere);
}

LPCTSTR CException::GetMessage() const
{
	return const_cast<LPCTSTR>(m_stMessage);
}

void CException::Report(HWND hWnd /* = NULL */) const
{
	::MessageBox(hWnd, m_stMessage, m_stWhere, MB_ICONSTOP);
}

//////////////////////////////////////////////////////////////////////
// CWin32Exception
//////////////////////////////////////////////////////////////////////

CWin32Exception::CWin32Exception(LPCTSTR lpWhere, DWORD dwError)
	: CException(lpWhere, NULL)
	, m_dwError(dwError)
{
	DWORD dwRes;
	TCHAR stError[MAX_ERRORMSG_LEN];

	// Try to format English text first
	dwRes = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		0, dwError, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL),
		stError, countof(stError) - 1, NULL);

	if(!dwRes && GetLastError() == ERROR_RESOURCE_LANG_NOT_FOUND)
	{
		dwRes = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
			0, dwError, 0, stError, countof(stError) - 1, NULL);
	}

	if(!dwRes)
	{
		wsprintf(stError, TEXT("FormatMessage failed (Error: %ld)"), GetLastError());
	}
	else
	{
		for(int i=0; i<lstrlen(stError); i++)
		{
			if(stError[i] == TEXT('\r'))
				stError[i] = TEXT(' ');

			if(stError[i] == TEXT('\n'))
				stError[i] = TEXT(' ');
		}
	}

	wsprintf(m_stMessage, TEXT("(Error: %ld) %s"), dwError, stError);
}

DWORD CWin32Exception::GetError() const
{
	return m_dwError;
}

//////////////////////////////////////////////////////////////////////
// CStructuredException
//////////////////////////////////////////////////////////////////////

bool CStructuredException::m_bMiniDump = false;

void __cdecl CStructuredException::EnableMiniDump(bool bEnable)
{
	m_bMiniDump = bEnable;
}

void __cdecl CStructuredException::MapSEtoCE()
{
	_set_se_translator(TranslateSEtoCE);
}

void CStructuredException::DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	MINIDUMP_EXCEPTION_INFORMATION eInfo;

	if(excpInfo == NULL)
	{
		// Generate exception to get proper context in dump
		__try 
		{
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
			EXCEPTION_EXECUTE_HANDLER) 
		{
			OutputDebugString(TEXT("CStructuredException: DumpMiniDump - Invalid context\n"));
		}
	} 
	else
	{
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
		//	MiniDumpNormal,
			MiniDumpWithIndirectlyReferencedMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

int __cdecl CStructuredException::DumpException(PEXCEPTION_POINTERS excpInfo)
{
	if(m_bMiniDump)
	{
		HANDLE hMiniDumpFile;
		TCHAR szFileName[MAX_PATH];

		// Create minidump filename based on time and module name
		
		time_t ltime;
		struct tm *tmTime;
		TCHAR szModule[MAX_PATH];
		TCHAR szApplication[MAX_PATH];
		LPTSTR stUnknown = TEXT("Unknown");
		LPTSTR pszModule = stUnknown;
		LPTSTR pszApplication = stUnknown;
		LPTSTR lastperiod;
		MEMORY_BASIC_INFORMATION memInfo;

		// Application
		if(GetModuleFileName(NULL, szApplication, countof(szApplication)))
			pszApplication = GetFilePart(szApplication);
		
		// Bad module
		if(VirtualQuery((LPCVOID)(DWORD_PTR)excpInfo->ContextRecord->Eip, &memInfo, sizeof(memInfo)) &&
			GetModuleFileName((HINSTANCE)memInfo.AllocationBase, szModule, countof(szModule)))
			pszModule = GetFilePart(szModule);
		
		// Extract the file name portion and remove it's file extension
		lastperiod = lstrrchr(pszApplication, TEXT('.'));
		if(lastperiod) lastperiod[0] = 0;

		lastperiod = lstrrchr(pszModule, TEXT('.'));
		if(lastperiod) lastperiod[0] = 0;
	
		time(&ltime);
		tmTime = localtime(&ltime);
		
		wsprintf(szFileName, TEXT("%s_%s_%02d-%02d-%04d_%02d-%02d-%02d.dmp"),
			pszApplication, pszModule, 
			tmTime->tm_mday, (1 + tmTime->tm_mon), (1900 + tmTime->tm_year),
			tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec);
		
		hMiniDumpFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
		
		if(hMiniDumpFile != INVALID_HANDLE_VALUE)
		{
			DumpMiniDump(hMiniDumpFile, excpInfo);
			FlushFileBuffers(hMiniDumpFile);

			LARGE_INTEGER size;
			size.LowPart = GetFileSize(hMiniDumpFile, (LPDWORD)&size.HighPart); 

			if(size.LowPart == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) 
			{
				Output(TWarning, TEXT("DumpException: fail to get minidump file size"));
				size.QuadPart = 1;
			}

			CloseHandle(hMiniDumpFile);
		
			if(size.QuadPart == 0)
			{
				Output(TWarning, TEXT("DumpException: Writing minidump failed"));

				// Delete wrong dump file...
				DeleteFile(szFileName);
			}
		}
		else
		{
			OutputF(TWarning, TEXT("DumpException: fail to open file \"%s\" for minidump err=%ld"),
				szFileName, GetLastError());
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void __cdecl CStructuredException::TranslateSEtoCE(UINT /*dwEC*/, PEXCEPTION_POINTERS excpInfo)
{
	DumpException(excpInfo);
	throw CStructuredException(excpInfo);
}

CStructuredException::CStructuredException(PEXCEPTION_POINTERS excpInfo)
	: CException(NULL, NULL)
{
	m_er      = *excpInfo->ExceptionRecord;
	m_context = *excpInfo->ContextRecord;

	DWORD dwModuleBase = 0;
	MEMORY_BASIC_INFORMATION memInfo;
	
	TCHAR szModule[MAX_PATH];
	TCHAR szApplication[MAX_PATH];
	
	LPTSTR stUnknown = TEXT("Unknown");
	LPTSTR pszModule = stUnknown;
	LPTSTR pszApplication = stUnknown;
	LPTSTR lastperiod;

	if(GetModuleFileName(NULL, szApplication, countof(szApplication)))
		pszApplication = GetFilePart(szApplication);
	
	if(VirtualQuery((LPCVOID)(DWORD_PTR)excpInfo->ContextRecord->Eip, &memInfo, sizeof(memInfo)) &&
		GetModuleFileName((HINSTANCE)memInfo.AllocationBase, szModule, countof(szModule)))
	{
		pszModule = GetFilePart(szModule);
		dwModuleBase = (DWORD)(DWORD_PTR)memInfo.AllocationBase;
	}
	
	// Extract the file name portion and remove it's file extension

	lastperiod = lstrrchr(pszApplication, TEXT('.'));
	if(lastperiod) lastperiod[0] = 0;

	lastperiod = lstrrchr(pszModule, TEXT('.'));
	if(lastperiod) lastperiod[0] = 0;

	wsprintf(m_stMessage,
		TEXT("%s caused %s(0x%08X) at %04x:%08X in module %s!%08lX"),
		pszApplication,
		GetExceptionDescription(m_er.ExceptionCode), m_er.ExceptionCode,
		m_context.SegCs, m_context.Eip,
		pszModule, m_context.Eip - dwModuleBase);

	wsprintf(m_stWhere,	TEXT("%s!%08lX"),
		pszModule, m_context.Eip - dwModuleBase);
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
