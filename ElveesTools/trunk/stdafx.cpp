// stdafx.cpp : source file that includes just the standard includes
// ElveesTools.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#define VARIABLES \
	"_MSC_VER = "         TODOMAKESTRING(TODOSTRINGIZE, _MSC_VER) \
	", WINVER = "         TODOMAKESTRING(TODOSTRINGIZE, WINVER) \
	", _WIN32_WINNT = "   TODOMAKESTRING(TODOSTRINGIZE, _WIN32_WINNT) \
	", _WIN32_WINDOWS = " TODOMAKESTRING(TODOSTRINGIZE, _WIN32_WINDOWS) \
	", _WIN32_IE = "      TODOMAKESTRING(TODOSTRINGIZE, _WIN32_IE)

// Check common variables
#pragma TODO(VARIABLES)