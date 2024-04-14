//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получение режима работы с БД, строки соединения с БД, первоначальное создание БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DBMode.h"

#ifndef UNIX_RELEASE
static LPCTSTR c_szRegistryRootDir = _T("Software\\ElVEES\\DVR");
#endif


//======================================================================================//
//                                     class DBMode                                     //
//======================================================================================//

DBMode::DBMode()
{
}

DBMode::~DBMode()
{
}

DBMode::ConfigMode	DBMode::GetConfigMode()
{
/*	CRegKey key;
	DWORD res;
	res = key.Open( HKEY_LOCAL_MACHINE, c_szRegistryRootDir);
	if( res != ERROR_SUCCESS )
	{
		return cm_registry;
	}
	DWORD dwValue;
	res = key.QueryDWORDValue(_T("DBUsage"), dwValue);
	//return ( res != ERROR_SUCCESS || dwValue == 0) ? false : true; 
	if( ERROR_SUCCESS != res || dwValue == 0 )
	{
		return cm_registry;
	}
	else if( dwValue == 1 )
	{
		return cm_mixed;
	}
	else
	{
		return cm_database;
	}*/
	std::string sConString = GetConnectionString();
	if( !sConString.empty() && !PathFileExistsA( sConString.c_str() ) )
	{
		return cm_database;
	}
	else
	{
		return cm_mixed;
	}
}

std::string		DBMode::GetConnectionString()
{
#ifdef UNIX_RELEASE
#error "Get connection string"
#endif

	char buf[4096];
	CRegKey key;
	DWORD res;
	res = key.Open( HKEY_LOCAL_MACHINE, c_szRegistryRootDir);
	if( res != ERROR_SUCCESS )
	{
		return std::string();
	}
	ULONG nSize = 4096;
	res = key.QueryStringValue(_T("DbConnectionString"), buf, &nSize);
	if( res != ERROR_SUCCESS ) 
	{
		return std::string();
	}

	return std::string(buf);
}