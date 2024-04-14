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

#include "stdafx.h"
#include "ArchiveStatePause.h"

//======================================================================================//
//                               class ArchiveStatePause                                //
//======================================================================================//

ArchiveStatePause::ArchiveStatePause(
									 boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
									 StreamMap_t& Streams, 
									 FrameMap_t& Frames,
									 std::pair<time_t, time_t> WorkInterval,
									 time_t				PauseTime
									 ):
	ArchiveState(  StreamManager, Streams, Frames, WorkInterval, "play"  ),
	m_PauseTime(PauseTime)
{
}

ArchiveStatePause::~ArchiveStatePause()
{
}

void	ArchiveStatePause::Seek( time_t NewArchivePosition ) 
{
	m_PauseTime = NewArchivePosition;
}

void	ArchiveStatePause::OnUpdate()
{
}

time_t	ArchiveStatePause::GetArchivePosition()
{
	return m_PauseTime;
}