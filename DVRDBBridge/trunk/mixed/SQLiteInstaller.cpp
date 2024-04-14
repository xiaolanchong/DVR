//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ñîçäàíèå ÁÄ SQLite 3.3

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "SQLiteInstaller.h"
#include "SqliteWrapper.h"

//======================================================================================//
//                                class SQLiteInstaller                                 //
//======================================================================================//

SQLiteInstaller::SQLiteInstaller()
{
}

SQLiteInstaller::~SQLiteInstaller()
{
}

const TCHAR* c_szMainPath = _T("Software\\ElVEES\\DVR");

void	SQLiteInstaller::Install( const std::string & sDBConnectionString )
try
{
	CRegKey regKey;
	if( ERROR_SUCCESS != regKey.Create( HKEY_LOCAL_MACHINE, c_szMainPath ) )
	{
		throw DBBridge::IDBInstaller::DBInstallerException( "Failed to create registry key" );
	}
	
	if( sDBConnectionString.empty() )
	{
		//! íå ñîçäàâàòü ÁÄ 
		return;
	}

	regKey.SetStringValue( "DBConnectionString", sDBConnectionString.c_str() );

	SQLiteDatabase db( sDBConnectionString.c_str() );

	const char * szQuery =
	"CREATE TABLE Frames "
	"( "
	"   FrameID		INTEGER  PRIMARY KEY AUTOINCREMENT "
	",  CameraID	INT "
	",  TimeBegin	TIMESTAMP "
	",  TimeEnd		TIMESTAMP "
#if 0
	",  LeftBound	FLOAT " 
	",  TopBound	FLOAT "
	",  RightBound	FLOAT "
	",  BottomBound	FLOAT "
#endif
	") "
	;

	SQLiteUpdateStatement st(db, szQuery);
	st.Update();
}
catch( std::exception& ex )
{
//	throw DBBridge::IDBInstaller::DBInstallerException( ex.what() );
};

void	SQLiteInstaller::Uninstall()
{
	if( SHDeleteKey( HKEY_LOCAL_MACHINE,  c_szMainPath ) != ERROR_SUCCESS )
	{
		throw DBBridge::IDBInstaller::DBInstallerException( "Failed to delete registry key" );
	}
}