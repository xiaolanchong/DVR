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

#include "stdafx.h"
#include "ArchiveStateIdle.h"

//======================================================================================//
//                                class ArchiveStateIdle                                //
//======================================================================================//

ArchiveStateIdle::ArchiveStateIdle(
								   boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
								   StreamMap_t& Streams, 
								   FrameMap_t& Frames
								   ):
	ArchiveState( StreamManager, Streams, Frames, std::make_pair(0, 0), "idle" ) 
{
//	m_Streams.clear();
//	m_Frames.clear();
}

ArchiveStateIdle::~ArchiveStateIdle()
{
}