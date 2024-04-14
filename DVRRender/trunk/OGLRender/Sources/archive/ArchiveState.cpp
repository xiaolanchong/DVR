//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - проигр-е, пауза, остановка

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ArchiveState.h"
#include "chcsva.h"
#include "../../../DVRRender.hpp"
#include "../../../IDVRGraphicsWrapper.h"
#include "../OGLWrapper.hpp"
#include "../DllMain.hpp"

//======================================================================================//
//                                  class ArchiveState                                  //
//======================================================================================//

ArchiveState::ArchiveState(
			boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
			StreamMap_t& Streams, 
			FrameMap_t& Frames,
			std::pair<time_t, time_t> WorkInterval,
			const char* szName):
	m_StreamManager(StreamManager),
	m_Frames(Frames),
	m_Streams(Streams),
	m_StateName( szName ) ,
	m_WorkInterval(WorkInterval)
{
}

ArchiveState::~ArchiveState()
{

}

void	ArchiveState::AddCameras( const std::vector<int>& CameraIndexArr ) 
{
	time_t nCurrentTime = GetArchivePosition();
	for ( size_t i = 0; i < CameraIndexArr.size(); ++i )
	{
		if( m_Streams.find( CameraIndexArr[i] ) == m_Streams.end() )
		{
			CHCS::IStream* pStream;
			m_StreamManager->StartArchiveStream( &pStream, CameraIndexArr[i], nCurrentTime );
			_ASSERTE( pStream );
			m_Streams.insert( std::make_pair( CameraIndexArr[i], Stream_t(  pStream , boost::mem_fn( &CHCS::IStream::Release )) ) );
		}
	}
}

void	ArchiveState::RemoveCameras( const std::vector<int>& CameraIndexArr )
{
	for ( size_t i = 0; i < CameraIndexArr.size(); ++i )
	{
		m_Streams.erase( CameraIndexArr[i] );
	}
}

bool	ArchiveState::DrawFrame(  int CameraIndex, float x, float y, float width, float height
								/*  std::vector<unsigned char>& UserFrameData  */) 
{
	PreDrawFrame(CameraIndex);
	FrameMap_t::iterator it = m_Frames.find( CameraIndex );

	IDVRGraphicsWrapper* pRender = dllGetGraphicsWrapper();
	_ASSERTE(pRender);

	if( it != m_Frames.end() )
	{
		CHCS::IFrame *pFrame = it->second.get();
		if( pFrame )
		{
			long BmpHdrSize = pFrame->GetFormat( 0 );
			if( BmpHdrSize <= 0 )
			{
				return false;
			}

			std::vector<BYTE> BmpHdrBuffer(BmpHdrSize, 0);
			
			pFrame->GetFormat( reinterpret_cast<BITMAPINFO*>(&BmpHdrBuffer[0]) );
			const BITMAPINFO& bi = *reinterpret_cast<BITMAPINFO*>(&BmpHdrBuffer[0]);

			pRender->DrawBitmap( 
				IDVRGraphicsWrapper::Rect( x, y, x + width, y + height ),
				bi.bmiHeader.biWidth,
				bi.bmiHeader.biHeight,
				bi.bmiHeader.biBitCount,
				pFrame->GetBytes()
				);


			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
/*		std::auto_ptr<IDVRGraphicsWrapper::IBrush>	pBrush( pRender->CreateBrush( wxColour( 192, 128, 0 ) ) );
		std::auto_ptr<IDVRGraphicsWrapper::IFont>	pFont( pRender->CreateFont( "zzz" ) );
		pRender->FillRect(pBrush.get(), IDVRGraphicsWrapper::Rect( x ,y ,x + width, y + height ));
		std::string sText = (boost::format ("%d : %s") % CameraIndex % m_StateName).str();
		pRender->DrawText( pFont.get(), sText, IDVRGraphicsWrapper::Rect( x ,y ,x + width, y + height ), 
			IDVRGraphicsWrapper::TA_Center, wxColour( 0, 0, 0 ) );
		return true;*/
		return false;
	}
}

bool	ArchiveState::GetFrameData(int CameraIndex, std::vector<unsigned char>& UserFrameData )
{
	FrameMap_t::iterator it = m_Frames.find( CameraIndex );

	if( it != m_Frames.end() )
	{
		CHCS::IFrame *pFrame = it->second.get();
		if( !pFrame )
		{
			return false;
		}

		size_t DataSize = 0;
		bool res = pFrame->GetCustomData( 0, DataSize );
		if( res && DataSize)
		{
			UserFrameData.resize( DataSize );
			bool res = pFrame->GetCustomData( &UserFrameData[0], DataSize );
			_ASSERTE(res && UserFrameData.size() == DataSize );
			return true;
		}
		else
		{
			UserFrameData.clear();
			return false;
		}
	}
	else
	{
		return false;
	}
}

void	DumpTimes( const std::vector<time_t>& TimeArr )
{
	std::string sWholeLine;
	for ( size_t i =0; i < TimeArr.size(); ++i )
	{
		char Buf[128];
		tm t;
		localtime_s(  &t, &TimeArr[i] );
		strftime( Buf, 128, "[%d.%m.%y %H:%M:%S] " , &t );
		sWholeLine += Buf;
		//sWholeLine += ",";
	}
	sWholeLine += "\n";
	OutputDebugStringA( sWholeLine.c_str() );
}

boost::optional<time_t> ArchiveState::GetDefaultArchivePosition()
{
/*	if( m_Frames.size() == 0 )
	{
		return m_WorkInterval.first;
	}*/
	std::vector<INT64> FrameTimes;
	for ( FrameMap_t::iterator it = m_Frames.begin(); it != m_Frames.end(); ++it )
	{
		if( it->second.get() )
		{
			time_t StreamTime = it->second->GetTime() ;
			if( StreamTime )
			{
				FrameTimes.push_back( StreamTime );
			}
		}
	}
	//_ASSERTE( std::find( FrameTimes.begin(), FrameTimes.end(), 0 ) == FrameTimes.end() );
	if( !FrameTimes.empty() )
	{
		INT64 AverageStreamTime = std::accumulate( FrameTimes.begin(), FrameTimes.end(), INT64(0) );
		AverageStreamTime /= FrameTimes.size();
//		DumpTimes( FrameTimes );
//		_ASSERTE( AverageStreamTime > 	1158132580 - 60 * 60 * 24 * 5	 );
		return boost::optional<time_t>(AverageStreamTime);
	}
	else
	{
		return boost::optional<time_t>();
	}
}

boost::optional<time_t> ArchiveState::GetCameraArchivePosition(int CameraID)
{
	FrameMap_t::iterator it = m_Frames.begin();
	
	if( it != m_Frames.end() )
	{
		return it->second->GetTime();
	}
	else
	{
		return boost::optional<time_t>();
	}
}