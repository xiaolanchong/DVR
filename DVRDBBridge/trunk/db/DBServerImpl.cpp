//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ IDBServer

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DBServerImpl.h"
#include "../interface/IVideoConfig.h"
#include "../DVRDBBridge.h"
#include "../common/CommonUtility.h"
#include "DBMode.h"

using namespace DBBridge;
//using namespace odbc;

//======================================================================================//
//                                  class DBServerImpl                                  //
//======================================================================================//

DBServerImpl::DBServerImpl(const std::tstring& sConnectionString):
	m_sConnectionString( sConnectionString) 
{

};

DBServerImpl::~DBServerImpl()
{
}

void	DBServerImpl::GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr )
try
{
	boost::shared_ptr<IVideoConfig> pVideoCfg ( DBBridge::CreateVideoConfig() );
	GetServerCameraList( pVideoCfg.get(), CamLocalArr,  CamAllArr);
}
catch ( std::exception& ex ) 
{
	CamAllArr.clear();
	CamLocalArr.clear();
	throw IDBServer::DBServerException( ex.what() );
};

void	DBServerImpl::AddFrames( const std::vector<DBBridge::Frame>& frames  )
try
{
	CFrame DBFrameWriter( m_sConnectionString ) ;
	for ( size_t i =0; i < frames.size(); ++i )
	{
		SYSTEMTIME StartTime, EndTime;
		DWORD CameraIDAsDWord = static_cast<DWORD>( frames[i].GetCameraID() );
		FromCRTTimeToSystemTime( frames[i].GetStartTime(),	StartTime );
		FromCRTTimeToSystemTime( frames[i].GetEndTime(),	EndTime   );
		DBFrameWriter.InsertFrameLine( StartTime, EndTime, CameraIDAsDWord );
	}
}
catch (std::exception& ex)
{
	throw DBServerException( ex.what() );
};

void	DBServerImpl::DeleteInfoBeforeTime( time_t BoundTime )
try
{
	CFrame DBFrameWriter( m_sConnectionString ) ;
	SYSTEMTIME From, To;
	FromCRTTimeToSystemTime( 0 , From );
	FromCRTTimeToSystemTime( BoundTime , To);
	DBFrameWriter.DeleteFrameLine( From, To );
}
catch (std::exception& ex) 
{
	throw DBServerException( ex.what() );
};