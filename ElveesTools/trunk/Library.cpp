// Library.cpp: implementation of the CLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Library.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CLibrary
//////////////////////////////////////////////////////////////////////

CLibrary::CLibrary(LPCTSTR lpstrLibrary) : m_hModule(NULL)
{
	if(lpstrLibrary)
	{
		m_hModule = ::LoadLibrary(lpstrLibrary);

		if(!m_hModule)
		{
			OutputF(TWarning, TEXT("Cannot load \"%s\" library."),
				lpstrLibrary);
		}
	}
}

CLibrary::~CLibrary()
{
	if(m_hModule)
		::FreeLibrary(m_hModule);
}

FARPROC CLibrary::GetProcAddress(LPCTSTR lpProcName)
{
	FARPROC fProc = NULL;

	if(m_hModule)
	{
	#ifdef _UNICODE
		char stProcName[128];

		WideCharToMultiByte(CP_ACP, 0, lpProcName, (int)wcslen(lpProcName) + 1,
			stProcName, sizeof(stProcName), NULL, NULL);

		fProc = ::GetProcAddress(m_hModule, stProcName);
	#else
		fProc = ::GetProcAddress(m_hModule, lpProcName);
	#endif
	}

	return fProc;
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
