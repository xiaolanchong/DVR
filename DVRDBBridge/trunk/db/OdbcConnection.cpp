//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� c �� �� ���������� ODBC

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "OdbcConnection.h"
#include "DBMode.h"

//======================================================================================//
//                                 class OdbcConnection                                 //
//======================================================================================//

OdbcConnection::OdbcConnection(const char* szConnectionString)
{
	std::string sConnStr;
	if( NULL == szConnectionString || NULL == *szConnectionString )
	{
		DBMode db;
		sConnStr = db.GetConnectionString();	
	}
	else
	{
		sConnStr = szConnectionString;
	}
	if( sConnStr.empty() ) 
	{
		throw std::runtime_error("Can't query the 'DbConnectionString' value");
	}
	m_DBConnection = boost::shared_ptr<odbc::Connection>( odbc::DriverManager::getConnection ( sConnStr ) );
}
OdbcConnection::~OdbcConnection()
{
}