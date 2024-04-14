//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Занесение периодов рамок в БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ActivityRecord.h"

//======================================================================================//
//                                 class ActivityRecord                                 //
//======================================================================================//

ActivityRecord::ActivityRecord( boost::shared_ptr<DBBridge::IDBServer> pDBServer, int nTimeOutInSecond ):
	m_pDBServer(pDBServer),
	c_nTimeOutInSecond( nTimeOutInSecond )
{
}

ActivityRecord::~ActivityRecord()
{
	DumpActivityPeriod( true );
}

void	ActivityRecord::ProcessFrames( const Frames_t& AlarmFrames)
{
	std::set<int> CameraIDSet;
	for ( size_t i =0; i < AlarmFrames.size(); ++i )
	{
		CameraIDSet.insert( AlarmFrames[i].GetCameraID() );
	}
	std::for_each( CameraIDSet.begin(), CameraIDSet.end(), 
		boost::bind( &ActivityRecord::ProcessFramesForCamera, this, _1 ) );

	DumpActivityPeriod( false );
}

bool IsForceDumpPeriodForLongTimeActivity( time_t StartTime, time_t EndTime )
{
	const int c_MaxActivityPeriodInSec = 5 * 60;
	return EndTime - StartTime > c_MaxActivityPeriodInSec;
}

void ActivityRecord::DumpActivityPeriod( bool bForceDumpAll )
{
	std::vector<int> ErasedCameraIDSet;

	const time_t Now = time(0);
	std::map<int, std::pair< time_t, time_t > >::iterator it = m_CameraActivity.begin();
	for ( ; it != m_CameraActivity.end();  ++it )
	{
		if( bForceDumpAll										||  
			Now - it->second.second > c_nTimeOutInSecond		|| 
			IsForceDumpPeriodForLongTimeActivity( it->second.first, it->second.second ) 
			)
		{
			DumpActivityPeriodForCamera( it->first, it->second.first, it->second.second );
			ErasedCameraIDSet.push_back( it->first );
		}
	}

	for ( size_t i =0; i < ErasedCameraIDSet.size(); ++i )
	{
		m_CameraActivity.erase( ErasedCameraIDSet[i] );
	}
}

void ActivityRecord::ProcessFramesForCamera( int CameraID )
{
	std::map<int, std::pair< time_t, time_t > >::iterator it = m_CameraActivity.find(CameraID);
	const time_t Now = time(0);
	if ( it == m_CameraActivity.end()  )
	{
		m_CameraActivity.insert( std::make_pair( CameraID, std::make_pair( Now, Now ) ) );
	}
	else
	{
		it->second.second = Now;
	}
}

void ActivityRecord::DumpActivityPeriodForCamera( int CameraID, time_t StartTime, time_t EndTime )
{
#if 1
	tm tmStart, tmEnd;
	localtime_s( &tmStart, &StartTime );
	localtime_s( &tmEnd,   &EndTime );
	char szBegin[ 64 ], szEnd[64];
	strftime( szBegin, 64, "%H:%M:%S", &tmStart );
	strftime( szEnd,   64, "%H:%M:%S", &tmEnd   );
	boost::format fmt( "\n\n***********[ActivityRecord]camera%d = %s - %s\n\n" );
	std::string s = (fmt % CameraID % szBegin % szEnd).str();
	OutputDebugStringA( s.c_str() );
#endif

#if 1
	try
	{
	Frames_t frm;
	frm.push_back( DBBridge::Frame( CameraID, StartTime, EndTime, DBBridge::Frame::Rect(0, 0, 0, 0) ) );
	m_pDBServer->AddFrames( frm );
	}
	catch( std::exception& )
	{

	}
#endif
}