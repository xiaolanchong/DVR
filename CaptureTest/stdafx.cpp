// stdafx.cpp : source file that includes just the standard includes
// CaptureTest.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "chcs.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


DWORD_PTR        procOutputData = 0;
CHCS::OutputProc procOutput     = NULL; 

#if 1
#define WRITELOG
#endif

#ifdef WRITELOG
#include <memory>
#include <iostream>
#include <fstream>

#ifdef _UNICODE
typedef std::wfstream _tfstream;
#else
typedef std::fstream _tfstream;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// ElveesTools Output function
/////////////////////////////////////////////////////////////////////////////

namespace Elvees
{

#ifdef WRITELOG
	static _tfstream s_debugOut;
#endif

	void Output(TOutput type, LPCTSTR stText)
	{
#if 1
		TCHAR stHeader[24];
		SYSTEMTIME stLocalTime;

		GetLocalTime(&stLocalTime);

		wsprintf(stHeader,
			TEXT("%02hd:%02hd:%02hd [%04ld] %c "),
			stLocalTime.wHour,
			stLocalTime.wMinute,
			stLocalTime.wSecond,
			GetCurrentThreadId(),
			(type == TTrace)    ? TEXT('D') :
			(type == TWarning)  ? TEXT('W') :
			(type == TInfo)     ? TEXT('I') :
			(type == TError)    ? TEXT('E') :
			(type == TCritical) ? TEXT('C') : TEXT('U'));

		_tprintf(TEXT("%s%s\n"), stHeader, stText);
#else
		OutputDebugString(stText);
		OutputDebugString(TEXT("\r\n"));
#endif

#ifdef WRITELOG
		if(!s_debugOut.is_open())
		{
			s_debugOut.open("chcsva.log", std::ios_base::out | std::ios_base::app);

			s_debugOut << TEXT("****************New Log*****************") << std::endl;
		}

		s_debugOut << stHeader << stText << std::endl;
#endif

		if(!procOutput)
			return;

		int nMode = 
			(type == TInfo) ? CHCS_INFO :
			(type == TTrace) ? CHCS_TRACE :
			(type == TError) ? CHCS_ERROR : 
			(type == TWarning) ? CHCS_WARNING :
			(type == TCritical) ? CHCS_CRITICAL : CHCS_INFO;

		__try
		{
#ifndef _UNICODE
			WCHAR szMsg[1024];
			MultiByteToWideChar(CP_ACP, 0, stText, lstrlen(stText) + 1,
				szMsg, countof(szMsg));

			procOutput(procOutputData, nMode, szMsg);
#else
			procOutput(procOutputData, nMode, stText);
#endif
		}
		__except(1)
		{
			OutputDebugString(TEXT("Unexpected exception in DebugOutput\n"));
		}
	}

} // End of namespace Elvees
