// Utils.h
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_UTILS_INCLUDED_
#define ELVEESTOOLS_UTILS_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include <wctype.h>

//////////////////////////////////////////////////////////////////////////
// Debugging defines...
//////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_ONLY
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else
#define DEBUG_ONLY(x)
#endif
#endif

#ifndef countof
#define countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

//////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////////
// Assistant functions
//////////////////////////////////////////////////////////////////////////

bool __cdecl IsFileExists(LPCTSTR stFile);

bool __cdecl HexToString(const BYTE* pBuffer, int nBytes, LPTSTR lpDest, int cchDest);
bool __cdecl StringToHex(LPCTSTR lpSource, BYTE *pBuffer, int nBytes);

LPCTSTR __cdecl GetLastErrorMessage(DWORD dwError);
LPCTSTR __cdecl GetCurrentDirectory();
LPCTSTR __cdecl GetDateTimeStamp();

LPCTSTR __cdecl DumpData(const BYTE* pData,
		int dataLength,
		LPTSTR stOut, long cbOut,
		int lineLength = 0);

LPCTSTR __cdecl GetComputerName();
LPCTSTR __cdecl GetUserName();

bool __cdecl GetUsedMemorySize(DWORD dwProcessId, DWORD* pdwWorkingSet, DWORD* pdwVirtualBytes);

// CRT replace
LPTSTR __cdecl lstrrchr(LPCTSTR string, int ch);
LPTSTR __cdecl GetFilePart(LPCTSTR source);

//////////////////////////////////////////////////////////////////////////
// Output functions
//////////////////////////////////////////////////////////////////////////

typedef enum
{
	TTrace,
	TWarning,
	TInfo,
	TError,
	TCritical
}
TOutput;

// Only prototype
extern void __cdecl Output(TOutput type, LPCTSTR stText);

void __cdecl OutputF(TOutput type, LPCTSTR stFormat, ...);

} // End of namespace Elvees

#endif // ELVEESTOOLS_UTILS_INCLUDED_
