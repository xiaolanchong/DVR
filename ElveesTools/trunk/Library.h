// Library.h: interface for the CLibrary class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_LIBRARY_INCLUDED_
#define ELVEESTOOLS_LIBRARY_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CLibrary
//////////////////////////////////////////////////////////////////////

class CLibrary
{
public:
	explicit CLibrary(LPCTSTR lpstrLibrary);
	virtual ~CLibrary();

	FARPROC GetProcAddress(LPCTSTR lpProcName);

private:
	HMODULE m_hModule;

	// No copies do not implement
	CLibrary(const CLibrary &rhs);
	CLibrary &operator=(const CLibrary &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_LIBRARY_INCLUDED_
