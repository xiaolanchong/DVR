//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Смешанная имплементация - реестр + sqlite БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   21.04.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "MixClientImpl.h"
#include "../win/RegClientImpl.h"
#include "../common/CommonUtility.h"
#include "SqliteWrapper.h"

//======================================================================================//
//                                 class MixClientImpl                                  //
//======================================================================================//

MixClientImpl::MixClientImpl(const std::string& sDBFileName):
	m_sDBFileName(sDBFileName)
{
}

MixClientImpl::~MixClientImpl()
{
}

void MixClientImpl::GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr)
{
	RegClientImpl RegVideoCfg;
	RegVideoCfg.GetCameraArr( CameraIDArr );
}

void MixClientImpl::SetCameraDescription( int CameraID, const std::tstring& sNewCameraDesc ) 
{
	RegClientImpl RegVideoCfg;
	RegVideoCfg.SetCameraDescription( CameraID, sNewCameraDesc );
}

time_t ConvertToTime( const std::string& sTime)
{
	boost::posix_time::ptime PosixTime	( boost::posix_time::time_from_string( sTime ) );
	tm TimeStruct = to_tm(PosixTime);
	return mktime( &TimeStruct );
}

void MixClientImpl::GetFrames( time_t StartTime, time_t EndTime, std::vector<DBBridge::Frame>& FrameArr )
try
{
#if 0
	StartTime	= 0;
	EndTime		= 100000000;
	throw std::runtime_error("Blah-blah");
#endif
	FrameArr.clear();
	SQLiteDatabase db( m_sDBFileName.c_str() );

	boost::format fmtQuery( 
		"SELECT CameraID, TimeBegin, TimeEnd "
		"FROM	Frames "
		"WHERE	(TimeBegin BETWEEN '%1%' AND '%2%') OR (TimeEnd BETWEEN '%1%' AND '%2%')"
		);

	std::string sQuery = (fmtQuery % GetTimeForSql(StartTime) % GetTimeForSql( EndTime )).str();

	SQLiteSelectStatement FrameQuery( db, sQuery.c_str() );

	size_t RecordNumber = 0;
	while( FrameQuery.FetchData() )
	{
		int			nCameraID	= FrameQuery.GetInt(0);
		std::string	sTimeBegin	= FrameQuery.GetText(1);
		std::string	sTimeEnd	= FrameQuery.GetText(2);
		
		boost::posix_time::ptime tEnd	( boost::posix_time::time_from_string( sTimeEnd ) );

		FrameArr.push_back( DBBridge::Frame( nCameraID, ConvertToTime( sTimeBegin ), ConvertToTime( sTimeEnd ),
											 DBBridge::Frame::Rect ( 0, 0, 0, 0 ) ) 
							);

		++RecordNumber;
	}
	int z = 0;
}
catch( std::exception& ex )
{
	throw DBBridge::IDBClient::DBClientException(ex.what());
};