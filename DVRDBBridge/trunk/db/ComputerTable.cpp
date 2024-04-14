//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Set/get database computer properties

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ComputerTable.h"

using namespace odbc;

//======================================================================================//
//                                 class ComputerTable                                  //
//======================================================================================//

ComputerTable::ComputerTable(const std::string& sConnectString):
	m_con ( DriverManager::getConnection ( sConnectString ) )
{	
	
}

ComputerTable::ComputerTable(boost::shared_ptr<odbc::Connection>	con):
	m_con(  con )
{

}

ComputerTable::~ComputerTable()
{
}

void	ComputerTable::GetValue( LPCTSTR szName, DWORD& dwValue) 
{
	QueryRes_t ResSet = GetValue( szName, false );
	dwValue = (DWORD)ResSet.second->getInt(1);
}

void	ComputerTable::GetValue( LPCTSTR szName, std::string& sValue )
{
	QueryRes_t ResSet = GetValue( szName, false );
	sValue = ResSet.second->getString(1);
}

void	ComputerTable::GetValue( LPCTSTR szName, std::vector<BYTE>& binValue )
{
	QueryRes_t ResSet = GetValue( szName, false );
	const odbc::Bytes b = ResSet.second->getBytes(1);
	if( b.getData() )
		binValue.assign( b.getData(), b.getData() + b.getSize()  );
	else
		binValue.clear();
}

void	ComputerTable::SetValue(  LPCTSTR szName, DWORD dwValue)
{
	std::string sIP = GetOwnIPAddress();
	std::string sQuery = (boost::format("UPDATE Computers SET %s=%d "
		"WHERE ComputerIP='%s'"	) % szName % dwValue % sIP).str();

	UpdateValue( m_con.get(), sQuery );
}

void	ComputerTable::SetValue(  LPCTSTR szName, LPCTSTR sValue )
{
	std::string sIP = GetOwnIPAddress();
	std::string sQuery = (boost::format("UPDATE Computers SET %s='%s' "
		"WHERE ComputerIP='%s'"	) % szName % sValue % sIP).str();

	UpdateValue( m_con.get(), sQuery );
}

void	ComputerTable::SetValue( LPCTSTR szName, const std::vector<BYTE>& binValue )
{
	QueryRes_t ResSet = GetValue( szName, true );
	std::istrstream z( (const char*)&binValue[0], (int)binValue.size() );
	ResSet.second->updateBinaryStream( 1, &z, (int) binValue.size() );
	ResSet.second->updateRow();
}

std::string ComputerTable::GetOwnIPAddress() 
try
{
	return ::GetOwnIPAddress();
}
catch ( std::exception& ex) 
{
	throw ComputerTableException( ex.what() );
};
