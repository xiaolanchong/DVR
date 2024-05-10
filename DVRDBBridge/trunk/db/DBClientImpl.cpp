//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация IDBClient

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DBClientImpl.h"
#include "../interface/IVideoConfig.h"
#include "../DVRDBBridge.h"
#include "../common/CommonUtility.h"
#include "../../../DBVideoConfig/trunk/testDB/MainFunction.h"

using namespace DBBridge;

//======================================================================================//
//                                  class DBClientImpl                                  //
//======================================================================================//

DBClientImpl::DBClientImpl(const std::tstring& sConnection):
	m_sConnectionString( sConnection ) 
{

}

DBClientImpl::~DBClientImpl()
{
}

void DBClientImpl::GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr)
try
{
	CameraIDArr.clear();
#if 1 // test code
	CameraIDArr.emplace_back( 1, "cam1");
	CameraIDArr.emplace_back( 4, "cam4");
	CameraIDArr.emplace_back( 5, "cam5");
	CameraIDArr.emplace_back( 12, "cam12");
	return;
#else

	std::vector<std::tstring> AllComputersInDatabase;
	GetAllIPAdress( m_sConnectionString, AllComputersInDatabase );

	std::vector<std::pair<int, std::string> > CameraIDArrOnOneComputer;
	for ( size_t i = 0; i < AllComputersInDatabase.size(); ++i )
	{
		boost::shared_ptr<IVideoConfig> pVideoCfg ( DBCreateVideoConfig( m_sConnectionString, AllComputersInDatabase[i] ) );
		if( pVideoCfg.get() )
		{
			GetAllCamerasWithName( pVideoCfg.get(), CameraIDArrOnOneComputer );
			CameraIDArr.insert( CameraIDArr.end(), CameraIDArrOnOneComputer.begin(), CameraIDArrOnOneComputer.end()  );
		}
	}
#endif
}
catch ( IVideoConfig::VideoConfigException& ex ) 
{
	CameraIDArr.clear();
	throw DBClientException( ex.what() );
}
catch ( std::runtime_error& ex ) 
{
	CameraIDArr.clear();
	throw DBClientException( ex.what() );
};

void DBClientImpl::GetFrames( time_t StartTime, time_t EndTime, std::vector<DBBridge::Frame>& FrameArr )
try
{
	FrameArr.clear();
	CFrame DBFrameWriter( m_sConnectionString ) ;

	SYSTEMTIME stStart, stEnd;
	FromCRTTimeToSystemTime( StartTime,	stStart );
	FromCRTTimeToSystemTime( EndTime,	stEnd   );

	std::vector<FRAMELINE> Frames;
	DBFrameWriter.GetFrameLine( stStart, stEnd, Frames  );
	for ( size_t i =0; i < Frames.size(); ++i )
	{
		int CameraIDAsDWord = static_cast<int>( Frames[i].dwCameraID );

		// любая рамка, сейчас не используется, важен период
		FrameArr.push_back( DBBridge::Frame( CameraIDAsDWord, 
											 FromSystemTimeToCRTTime( Frames[i].TimeBegin ),
											 FromSystemTimeToCRTTime( Frames[i].TimeEnd ),
											 DBBridge::Frame::Rect(0, 0, 0, 0)
											) 
							);
	}
}
catch ( std::exception& /*ex*/ ) 
{
	FrameArr.clear();
};

void DBClientImpl::SetCameraDescription( int CameraID, const std::string& sNewCameraDesc )
{
	// not implemented at the moment, later mb
	// get IVideoConfig, find camera with this id, and set the description
	UNREFERENCED_PARAMETER(CameraID);
	UNREFERENCED_PARAMETER(sNewCameraDesc);
}