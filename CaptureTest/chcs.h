// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

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
#include <stdio.h>
#include <tchar.h>
#include <time.h>

#include <windows.h>
#include <vfw.h>
#include <dbghelp.h>
#include <shlwapi.h>

#include <atlbase.h>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

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

#ifdef _UNICODE
#define tstring	wstring
#else
#define tstring string
#endif

#include "../../../CommonLibs/HEAD/ElveesTools/trunk/ElveesTools.h"
#include "../../../CommonLibs/HEAD/ElveesTools/trunk/VidDecoder.h"
#include "../../../CommonLibs/HEAD/ElveesTools/trunk/VidEncoder.h"

#include "../chcsva/trunk/chcsva.h"

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))
#define countof(x) (sizeof(x)/sizeof((x)[0]))

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

// TODO: reference additional headers your program requires here
