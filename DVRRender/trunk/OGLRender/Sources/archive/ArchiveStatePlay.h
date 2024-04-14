//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - проигрывание

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   22.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_STATE_PLAY_1344754592539950_
#define _ARCHIVE_STATE_PLAY_1344754592539950_

#include "ArchiveState.h"
/*
class TimeRecorder
{
	std::pair<time_t, time_t>	m_WorkPeriod;
	time_t						m_L
};*/

//======================================================================================//
//                                class ArchiveStatePlay                                //
//======================================================================================//

//! \brief Состояние архива - проигрывание
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveStatePlay : public ArchiveState
{
	time_t			m_LastArchiveTime;
public:
	ArchiveStatePlay(
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames,
		std::pair<time_t, time_t> WorkInterval,
		time_t				StartTime
		);

	ArchiveStatePlay(
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames,
		std::pair<time_t, time_t> WorkInterval,
		const std::vector<int>&		CameraIDArr
		);

	virtual ~ArchiveStatePlay();

	virtual void	Seek( time_t NewArchivePosition ) ;
	virtual void	OnUpdate() ;
	virtual void	PreDrawFrame(int nCameraID) ;

	virtual time_t GetArchivePosition() ;
};

#endif // _ARCHIVE_STATE_PLAY_1344754592539950_