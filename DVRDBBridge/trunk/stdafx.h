// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning ( disable : 4996) // localtime & gmtime are deprecated

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// TODO: reference additional headers your program requires here

//#define USE_LIBODBC
//#define USE_LIBSQLITE

//NOTE: 2006-009-19 libodbc++ используется для доступа к БД через ODBC
//		поскольку кроссплатформенность уже не требуется, а основная БД - MSSQL
//		то ее можно удалить
//		доступ к БД - интерфейс OLE DB

//NOTE: 2006-009-19 libsqlite используется для локального хранение БД на легковесном движке
//		однако эта конфигурация не требуется



#ifdef USE_LIBODBC
// odbc++
#include <odbc++/drivermanager.h>
#include <odbc++/connection.h>
#include <odbc++/preparedstatement.h>
#include <odbc++/resultset.h>

#endif

#ifndef UNIX_RELEASE
#include <atlbase.h>
#include <atldbcli.h>
#include <atltime.h>
#endif

//STL
#include <vector>
#include <string>
#include <strstream>
#include <iostream>
#include <utility>
#include <locale>
#include <functional>
#include <stdexcept>
#include <map>
#include <set>

//Boost
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#pragma warning ( push )
#pragma warning ( disable : 4701) // potentially uninitialized local variable 'result'
#include <boost/lexical_cast.hpp>
#pragma  warning (pop )
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//shared

#include "../../shared/Common/ExceptionTemplate.h"

#ifdef USE_LIBSQLITE
//sqlite
#include "../../../../3rd_party/sqlite_3_3_5/sqlite3.h"
#endif

#include "../../DBVideoConfig/trunk/testDB/CFrame.h"
#include "../../DBVideoConfig/trunk/testDB/CVideoConfig.h"

// chsva video configure interface
#include "../../chcsva/trunk/ext/IVideoSystemConfig.h"
#include "../../chcsva/trunk/ext/UuidUtility.h"

#define APPNAME _T("Database configuration")

//! TRACE на произвольное количество аргументов
//! \param szFormat строка, выводимая в trace
void TRACE(LPCTSTR szFormat, ...);

//Вывод информации об ощибке
//! \param hRes название полученой ошибки
std::tstring AtlTraceRelease(HRESULT hRes);

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if(p){ delete p; p = NULL; }}
#endif