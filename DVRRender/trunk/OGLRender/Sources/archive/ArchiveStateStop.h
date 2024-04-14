//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - остановка

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   22.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_STATE_STOP_7390296967609743_
#define _ARCHIVE_STATE_STOP_7390296967609743_

#include "ArchiveState.h"

//======================================================================================//
//                                class ArchiveStateStop                                //
//======================================================================================//

//! \brief Состояние архива - остановка
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveStateStop : public ArchiveState
{
public:
	ArchiveStateStop( 
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames,
		std::pair<time_t, time_t> WorkInterval
		);
	virtual ~ArchiveStateStop();

	virtual void							OnUpdate();

	virtual void	Seek( time_t NewArchivePosition )
	{
		// can't seek in this state
	}

	virtual void	PreDrawFrame(int nCameraID) 
	{
	}

	virtual time_t GetArchivePosition() 
	{
		// in this state always in the begining
		return m_WorkInterval.first;
	}
};

#endif // _ARCHIVE_STATE_STOP_7390296967609743_