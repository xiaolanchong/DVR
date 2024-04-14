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

#include "stdafx.h"
#include "chcsva.h"
#include "ArchiveStatePlay.h"

//======================================================================================//
//                                class ArchiveStatePlay                                //
//======================================================================================//

ArchiveStatePlay::ArchiveStatePlay(
								   boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
								   StreamMap_t& Streams, 
								   FrameMap_t& Frames,
								   std::pair<time_t, time_t> WorkInterval,
								   time_t				StartTime
								   ):
	ArchiveState(  StreamManager, Streams, Frames, WorkInterval, "play"  ),
	m_LastArchiveTime( WorkInterval.first )
{
	for ( StreamMap_t::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it )
	{
		it->second->SeekTo( StartTime );
	}
}

ArchiveStatePlay::ArchiveStatePlay(
	boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
	StreamMap_t& Streams, 
	FrameMap_t& Frames,
	std::pair<time_t, time_t> WorkInterval,
	const std::vector<int>&		CameraIDArr
	):
	ArchiveState(  StreamManager, Streams, Frames, WorkInterval, "play"  ),
	m_LastArchiveTime( WorkInterval.first )
{
	m_Frames.clear();
	m_Streams.clear();
	for ( size_t i = 0; i < CameraIDArr.size(); ++i )
	{
		CHCS::IStream * pStream;
		m_StreamManager->StartArchiveStream( &pStream, CameraIDArr[i], WorkInterval.first );
		_ASSERTE(pStream);
		if( pStream )
		{
			m_Streams[ CameraIDArr[i] ] = boost::shared_ptr<CHCS::IStream>( pStream, boost::mem_fn( &CHCS::IStream::Release ) );
		}
	}
}

ArchiveStatePlay::~ArchiveStatePlay()
{
}

void							ArchiveStatePlay::Seek( time_t NewArchivePosition ) 
{
	//! just seeking for all streams
	for ( StreamMap_t::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it )
	{
		it->second->SeekTo( NewArchivePosition );
	}
}

void							ArchiveStatePlay::OnUpdate() 
{
	//! синхронизация архива
	//! причины рассинхронизации - "дыры" в архива по каким-то камерам
	//time_t  GetArchivePosition();
}

void	ArchiveStatePlay::PreDrawFrame(int nCameraID) 
{
	StreamMap_t::iterator it = m_Streams.find( nCameraID );;
	if( it != m_Streams.end() )
	{
		CHCS::IFrame* pFrame = 0;
		const unsigned CaptureFrameTimeoutInMSec = 100;
		it->second->GetNextFrame( &pFrame, CaptureFrameTimeoutInMSec );
		if( pFrame )
		{
			m_Frames[nCameraID] = Frame_t( pFrame, boost::mem_fn( &CHCS::IFrame::Release ) ) ;
		}
	}
}

time_t						ArchiveStatePlay::GetArchivePosition() 
{
	boost::optional<time_t> DefTime = GetDefaultArchivePosition();
	if( DefTime.get_ptr() )
	{
		m_LastArchiveTime = DefTime.get();
	}
	_ASSERTE(m_LastArchiveTime);
	return m_LastArchiveTime;
}