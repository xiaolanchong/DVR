// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <vector>
#include  <string>
#include <stdexcept>
#include <memory>
#include <stdlib.h>
#include <iostream>
#include <atldbcli.h>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>


// TODO: reference additional headers your program requires here

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if(p){ delete p; p = NULL; }}
#endif

#define APPNAME _T("testDB")

