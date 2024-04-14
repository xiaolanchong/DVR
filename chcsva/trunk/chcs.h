// chcs.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#ifndef ELVEES_CHCS_H__INCLUDED_
#define ELVEES_CHCS_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER < 0x51E) // MS.NET 2003
#error Too old compiler! Use new one...
#endif

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER					// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501			// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT			// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS			// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410	// Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE				// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600		// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define STRICT
#define _CRT_SECURE_NO_DEPRECATE		// MSVS 8.0 Madness 
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

/////////////////////////////////////////////////////////////////////////////
// Windows Header Files:

#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <time.h>
#include <math.h>

#include <commctrl.h>
#include <shellapi.h>

#include <vfw.h>
#include <dbghelp.h>

//STL
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <algorithm>
#include <functional>
#include <memory>
#include <fstream>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

/////////////////////////////////////////////////////////////////////////////
// ATL

#define _ATL_FREE_THREADED
#include <atlbase.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>

#if _ATL_VER < 0x710
#error You have to use ATL ver 7.1 or above...
#endif

#include <oledb.h>		// ATL OLEDB Consumer Templates header.
#include <oledberr.h>	// ATL OLEDB Error Codes
#include <atldbcli.h>	// ATL consumer code for OLEDB

#if _ATL_VER < 0x710
#error You have to use ATL ver 7.1 or above...
#endif

/////////////////////////////////////////////////////////////////////////////
// External includes

// Debug version use trunk versions
#include "../../../../CommonLibs/HEAD/ElveesTools/trunk/ElveesTools.h"
#include "../../../../CommonLibs/HEAD/ElveesTools/trunk/VidDecoder.h"
#include "../../../../CommonLibs/HEAD/ElveesTools/trunk/VidEncoder.h"
#include "ext/common/ExceptionHandler.h"

//old dependence on DVRDBBridge
//#include "../../DVRDBBridge/trunk/DVRDBBridge.h"
//#include "../../DVRDBBridge/trunk/interface/IVideoConfig.h"

#ifndef _DEBUG
#pragma TODO("Set release directory headers")
#endif

/////////////////////////////////////////////////////////////////////////////
// Dll interfaces

#include "chcsva.h"
//#include "Manager.h"

/////////////////////////////////////////////////////////////////////////////
// Macroses

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

#ifdef _UNICODE
#define tformat wformat
#else
#define tformat format
#endif

#ifdef UNICODE
typedef	RPC_WSTR UuidString;
#else
typedef	RPC_CSTR UuidString;
#endif

#define SAFE_RELEASE(x) \
	if(x)\
	{\
		(x)->Release();\
		(x) = NULL;\
	}

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))
#define countof(x) (sizeof(x)/sizeof((x)[0]))

bool StringToUuid(LPTSTR StringUuid, UUID* Uuid);

void	GetFrameUserData( CHCS::IFrame* pFrame, std::vector<unsigned char>& UserFrameData );

#endif // ELVEES_CHCS_H__INCLUDED_
