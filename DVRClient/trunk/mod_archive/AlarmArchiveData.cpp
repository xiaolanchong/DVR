//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Хранит по камерам произошедшие события для архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "AlarmArchiveData.h"

//======================================================================================//
//                                 class AlarmRectData                                  //
//======================================================================================//

AlarmRectData::AlarmRectData( )
{
}

AlarmRectData::AlarmRectData(const Frames_t& AllAlarmData ):
	m_AllAlarms( AllAlarmData ) 
{
}

AlarmRectData::~AlarmRectData()
{
}

void AlarmRectData::GetData( 
		int CameraID, 
		time_t  ArchiveTime, 
		Frames_t& AlarmRectForTimeAndCamera 
		) const
{
	AlarmRectForTimeAndCamera.clear();
	for ( size_t i =0; i < m_AllAlarms.size(); ++i )
	{
		if( m_AllAlarms[i].GetCameraID()	== CameraID		&&
			m_AllAlarms[i].GetStartTime()	<= ArchiveTime	&&
			m_AllAlarms[i].GetEndTime()		>= ArchiveTime )
		{
			AlarmRectForTimeAndCamera.push_back( m_AllAlarms[i] );
		}
	}
#if 0
	// test
	static float x = 0.1f;
	AlarmRectForTimeAndCamera.push_back( DBBridge::Frame( 1, 0, 1234455, DBBridge::Frame::Rect( x, x, 0.4f, 0.5f) ) );
	x += 0.001f;
#endif
}