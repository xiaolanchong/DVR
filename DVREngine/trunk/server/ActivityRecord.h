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
#ifndef _ACTIVITY_RECORD_9061380059493652_
#define _ACTIVITY_RECORD_9061380059493652_

#include "AlarmInfo.h"

//======================================================================================//
//                                 class ActivityRecord                                 //
//======================================================================================//

//! \brief Занесение периодов рамок в БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ActivityRecord
{
	boost::shared_ptr<DBBridge::IDBServer>	m_pDBServer;
	std::map<int, std::pair< time_t, time_t > >	m_CameraActivity;

	void ProcessFramesForCamera( int CameraID ); 
	void DumpActivityPeriodForCamera( int CameraID, time_t StartTime, time_t EndTime );

	void DumpActivityPeriod( bool bDumpAll );

	const int c_nTimeOutInSecond ;
public:
	ActivityRecord( boost::shared_ptr<DBBridge::IDBServer> pDBServer, int nTimeOutInSecond = 5 );
	virtual ~ActivityRecord();

	void	ProcessFrames( const Frames_t& AlarmFrames);
};

#endif // _ACTIVITY_RECORD_9061380059493652_