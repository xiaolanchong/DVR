//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - не инициализирован

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   22.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_STATE_IDLE_8660266675766672_
#define _ARCHIVE_STATE_IDLE_8660266675766672_

#include "ArchiveState.h"

//======================================================================================//
//                                class ArchiveStateIdle                                //
//======================================================================================//

//! \brief Состояние архива - не инициализирован
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveStateIdle : public ArchiveState
{
public:
	ArchiveStateIdle(
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames
		);
	virtual ~ArchiveStateIdle();

	virtual void	Seek( time_t NewArchivePosition )
	{

	}
	virtual void	OnUpdate()
	{
	}

	virtual void	PreDrawFrame(int nCameraID)
	{

	}

	virtual time_t GetArchivePosition()
	{
		return 0;
	}
};

#endif // _ARCHIVE_STATE_IDLE_8660266675766672_