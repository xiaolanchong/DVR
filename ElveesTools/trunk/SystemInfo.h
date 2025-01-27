// SystemInfo.h: interface for the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_SYSTEMINFO_INCLUDED_
#define ELVEESTOOLS_SYSTEMINFO_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CSystemInfo
//////////////////////////////////////////////////////////////////////

class CSystemInfo : public SYSTEM_INFO 
{
public:
	CSystemInfo()
	{
		::GetSystemInfo(this);
	}

	// Main function
	DWORD GetNumberOfProcessors()
	{
		return dwNumberOfProcessors;
	}
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#ifdef UNDEFINED
// for dump environment
void UnusedGlobalFunction() {}
#endif

#endif // ELVEESTOOLS_SYSTEMINFO_INCLUDED_
