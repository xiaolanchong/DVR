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

#include "stdafx.h"

#include "ArchiveStateStop.h"
#include "ArchiveStatePlay.h"
#include "ArchiveStatePause.h"

//======================================================================================//
//                                class ArchiveStateStop                                //
//======================================================================================//

ArchiveStateStop::ArchiveStateStop(
								boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
								StreamMap_t& Streams, 
								FrameMap_t& Frames,
								std::pair<time_t, time_t> WorkInterval
							):
	ArchiveState( StreamManager, Streams, Frames, WorkInterval, "stop" ) 
{
	m_Frames.clear();
}

ArchiveStateStop::~ArchiveStateStop()
{

}

void	ArchiveStateStop::OnUpdate()
{
	// do nothing
}