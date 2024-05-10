// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef ELVEESTOOLS_STDAFX_H__INCLUDED_
#define ELVEESTOOLS_STDAFX_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#else
// Do error... Yet.
#error Too old compiler! Use new one...
#endif

// Forcedly use my settings
#undef WINVER
#undef _WIN32_IE
#undef _WIN32_WINNT
#undef _WIN32_WINDOWS

// Windows 2000
#define WINVER				0x0500
#define _WIN32_WINNT		0x0500
#define _WIN32_WINDOWS		0x0410
#define _WIN32_IE			0x0500

// Exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets. 
#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

/////////////////////////////////////////////////////////////////////////////
// Windows Header Files

#include <winsock2.h>
#include <tchar.h>
#include <vfw.h>
#include <winperf.h>
#include <assert.h>
#include <process.h>
#include <errno.h>
#include <dbghelp.h>
#include <eh.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "NodeList.h"
#include "UsesLib.h"

#include "Todo.h"
#include "Utils.h"

#define countof(x) (sizeof(x)/sizeof((x)[0]))

//
//	PVOID InterlockedExchangePointer(
//	  PVOID volatile* Target, PVOID Value);
//

#define InterlockedExchange_Pointer(Target, Value) \
    (PVOID)(LONG_PTR)InterlockedExchange((volatile LONG*)(LONG_PTR)(Target), (LONG)(LONG_PTR)(Value))

// Align memory allocation

#ifndef _aligned_malloc
#define _aligned_malloc(size, align) malloc(size)
#endif

#ifndef _aligned_free
#define _aligned_free(mem) free(mem)
#endif

#endif // ELVEESTOOLS_STDAFX_H__INCLUDED_
