//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ODBC Database implementation

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.12.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "VideoConfigDBImpl.h"
#include "DBMode.h"
#include <strstream>

//======================================================================================//
//                               class VideoConfigDBImpl                                //
//======================================================================================//

using namespace odbc;
using namespace DBBridge;

VideoConfigDBImpl::VideoConfigDBImpl()
try
	: ComputerTable ( DBMode().GetConnectionString() )
{
/*	DBMode db;
	std::string sConnStr  = db.GetConnectionString();	
	if( sConnStr.empty() ) 
	{
		throw InitFailed("Can't query the 'DbConnectionString' value");
	}
	m_con = boost::shared_ptr<Connection>( DriverManager::getConnection ( sConnStr ) );*/
}
catch (std::exception &ex) 
{
	throw IVideoConfig::InitFailed( ex.what() );
};

VideoConfigDBImpl::VideoConfigDBImpl(boost::shared_ptr<odbc::Connection> con):
	ComputerTable(con)
{
}

VideoConfigDBImpl::~VideoConfigDBImpl()
{
}

void	VideoConfigDBImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) 
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	QueryRes_t ResSet = GetValue( szName, false );
	dwValue = (DWORD)ResSet.second->getInt(1);*/
	ComputerTable::GetValue( szName, dwValue );  
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	VideoConfigDBImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue )
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	QueryRes_t ResSet = GetValue( szName, false );
	sValue = ResSet.second->getString(1);*/
	ComputerTable::GetValue( szName, sValue ); 
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	VideoConfigDBImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue )
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	QueryRes_t ResSet = GetValue( szName, false );
	const odbc::Bytes b = ResSet.second->getBytes(1);
	if( b.getData() )
		binValue.assign( b.getData(), b.getData() + b.getSize()  );
	else
		binValue.clear();*/
	ComputerTable::GetValue( szName, binValue );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	VideoConfigDBImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue)
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	std::tstring sIP = GetOwnIPAddress();
	std::string sQuery = (boost::format("UPDATE Computers SET %s=%d "
										"WHERE ComputerIP='%s'"	) % szName % dwValue % sIP).str();

	UpdateValue( m_con.get(), sQuery );*/
	ComputerTable::SetValue( szName, dwValue );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	VideoConfigDBImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue )
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	std::tstring sIP = GetOwnIPAddress();
	std::string sQuery = (boost::format("UPDATE Computers SET %s='%s' "
										"WHERE ComputerIP='%s'"	) % szName % sValue % sIP).str();

	UpdateValue( m_con.get(), sQuery );*/
	ComputerTable::SetValue( szName, sValue );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};


void	VideoConfigDBImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue )
try
{
	UNREFERENCED_PARAMETER(szPath);
/*	QueryRes_t ResSet = GetValue( szName, true );
	std::istrstream z( (const char*)&binValue[0], (int)binValue.size() );
	ResSet.second->updateBinaryStream( 1, &z, (int) binValue.size() );
	ResSet.second->updateRow();*/
	ComputerTable::SetValue( szName, binValue );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};


void	VideoConfigDBImpl::GetCameras( std::vector<int>& CamIDArr )
try
{
	CamIDArr.clear();
	std::tstring sIP = GetOwnIPAddress();
	std::string sQuery = (	"SELECT CameraID FROM Cameras INNER JOIN Computers "
							"ON Computers.ComputerID = Cameras.ComputerID "	);

	boost::shared_ptr<odbc::Statement> st ( m_con->createStatement (  ) );
	boost::shared_ptr<odbc::ResultSet> ResSet (st->executeQuery( sQuery ) );
	while ( ResSet->next() )
	{
		CamIDArr.push_back( ResSet->getInt(1) );
	}
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

DBBridge::ICameraConfig*	VideoConfigDBImpl::GetCamera( int nID  )
try
{
	std::string sQuery = ( (boost::format("SELECT CameraID FROM Cameras WHERE CameraID=%d") % nID).str()	);

	boost::shared_ptr<odbc::Statement> st ( m_con->createStatement (  ) );
	boost::shared_ptr<odbc::ResultSet> ResSet (st->executeQuery( sQuery ) );
	if ( !ResSet->next() )  throw InitFailed("No such camera");
	return new CameraConfigDBImpl( m_con, nID );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

DBBridge::ICameraConfig*	VideoConfigDBImpl::CreateCamera( LPCTSTR szUID )
try
{
	std::string sIP = GetOwnIPAddress();
	std::string sQuery = ( (boost::format("SELECT ComputerID FROM Computers WHERE ComputerIP='%s'") % sIP).str()	);
	boost::shared_ptr<odbc::Statement> st ( m_con->createStatement (  ) );
	boost::shared_ptr<odbc::ResultSet> ResSet (st->executeQuery( sQuery ) );
	if ( !ResSet->next() )  throw InitFailed("The computer is not in the 'Computers' table");
	int nComputerID = ResSet->getInt(1);
	ResSet.reset();

	// hack - get max CameraID and increment it - so we have an unique id =)
	sQuery = "SELECT MAX(CameraID) FROM Cameras";
	st		= boost::shared_ptr<odbc::Statement>( m_con->createStatement (  ) );
	ResSet	= boost::shared_ptr<odbc::ResultSet>(st->executeQuery( sQuery ) );
	if ( !ResSet->next() )  throw InitFailed("The computer is not in the 'Computers' table");
	int nCameraID = ResSet->getInt(1) + 1;
	ResSet.reset();

	sQuery = ( (boost::format("INSERT INTO Cameras ( CameraID, ComputerID, CameraUID) VALUES ( %d, %d, '%s')") 
					% nCameraID %  nComputerID % szUID).str()	);

	st  = boost::shared_ptr<odbc::Statement>( m_con->createStatement (  ResultSet::TYPE_SCROLL_SENSITIVE, ResultSet::CONCUR_UPDATABLE ) );

	int nCount = st->executeUpdate(sQuery);
	if(!nCount) throw InitFailed("Failed to delete the camera");
	m_con->commit();
	return new CameraConfigDBImpl( m_con, nCameraID );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void		VideoConfigDBImpl::DeleteCamera( int nID )
try
{
	std::string sQuery = ( (boost::format("DELETE FROM Cameras WHERE CameraID=%d") % nID).str()	);

	boost::shared_ptr<odbc::Statement> st ( m_con->createStatement ( ) );
	if( !st->executeUpdate( sQuery ) ) throw InitFailed("Failed to delete the camera");
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

//////////////////////////////////////////////////////////////////////////

CameraConfigDBImpl::CameraConfigDBImpl(  boost::shared_ptr<odbc::Connection> con, int nID ):
	m_con( con ),
	m_nID(nID)
{

}

void	CameraConfigDBImpl::GetValue( LPCTSTR szName, DWORD& dwValue) 
try
{
	VideoConfigDBImpl::QueryRes_t ResSet = GetValue( szName, true );
	dwValue = (DWORD)ResSet.second->getInt(1);
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	CameraConfigDBImpl::GetValue( LPCTSTR szName, std::tstring& sValue )
try
{
	VideoConfigDBImpl::QueryRes_t ResSet = GetValue( szName, true );
	sValue = ResSet.second->getString(1);
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	CameraConfigDBImpl::SetValue( LPCTSTR szName, DWORD dwValue)
try
{
	std::string sQuery = (boost::format("UPDATE Cameras SET %s=%d "
										"WHERE CameraID=%d "	) % szName % dwValue % m_nID).str();

	VideoConfigDBImpl::UpdateValue( m_con.get(), sQuery );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

void	CameraConfigDBImpl::SetValue( LPCTSTR szName, LPCTSTR sValue )
try
{
	std::string sQuery = (boost::format("UPDATE Cameras SET %s='%s' "
										"WHERE CameraID=%d "	) % szName % sValue % m_nID).str();

	VideoConfigDBImpl::UpdateValue( m_con.get(), sQuery );
}
catch (std::exception &ex) 
{
	throw IVideoConfig::NoSuchProperty( ex.what() );
};

std::tstring	CameraConfigDBImpl::GetCameraUID()
{
	std::tstring s;
	GetValue( "CameraUID", s );
	return s;
}