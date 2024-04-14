//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - пауза

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   22.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_STATE_PAUSE_3834912152704320_
#define _ARCHIVE_STATE_PAUSE_3834912152704320_

#include "ArchiveState.h"

//======================================================================================//
//                               class ArchiveStatePause                                //
//======================================================================================//

//! \brief Состояние архива - пауза
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveStatePause : public ArchiveState
{
	time_t				m_PauseTime;
public:
	ArchiveStatePause(
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames,
		std::pair<time_t, time_t> WorkInterval,
		time_t				PauseTime
		);
	virtual ~ArchiveStatePause();

	virtual void	Seek( time_t NewArchivePosition ) ;
	virtual void	OnUpdate() ;
	virtual void	PreDrawFrame(int nCameraID) 
	{
	}

//	virtual std::auto_ptr<ArchiveState>	Play()	= 0;
//	virtual std::auto_ptr<ArchiveState>	Pause()	= 0;

	virtual time_t GetArchivePosition() ;
};

#endif // _ARCHIVE_STATE_PAUSE_3834912152704320_