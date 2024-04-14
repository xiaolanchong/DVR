//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ”правление архивом по группе камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "../../../DVRRender.hpp"
#include "../../../IDVRGraphicsWrapper.h"
#include "ArchiveRender.h"


#include "../DllMain.hpp"

#include "../OGLWrapper.hpp"
#include "../OGLRenderManager.hpp"

#include "ArchiveStateIdle.h"
#include "ArchiveStateStop.h"
#include "ArchiveStatePlay.h"
#include "ArchiveStatePause.h"

//======================================================================================//
//                                 class ArchiveRender                                  //
//======================================================================================//

ArchiveRender::ArchiveRender( boost::shared_ptr<CHCS::IStreamManager> streamManager ):
	m_StreamManager( streamManager )
{
	m_State = std::auto_ptr<ArchiveState>(
		new ArchiveStateIdle( m_StreamManager, m_Streams, m_Frames )
		);
}

ArchiveRender::~ArchiveRender()
{
}

void	ArchiveRender::AddCameras( const std::vector<int>& CameraIndexArr )
{
}

void	ArchiveRender::RemoveCameras( const std::vector<int>& CameraIndexArr )
{
}

bool	ArchiveRender::DrawFrame( int CameraIndex, float x, float y, float width, float height
								 /*, std::vector<unsigned char>& UserFrameData*/   )
{
	return m_State->DrawFrame( CameraIndex, x, y, width, height );
}

bool	ArchiveRender::GetFrameUserData( int CameraID, std::vector<unsigned char>& UserFrameData  )
{
	return m_State->GetFrameData( CameraID, UserFrameData );
}

void	ArchiveRender::AcquireArchive( time_t nStartTime, time_t nEndTime, const std::vector<int>& CameraIndexArr )
{
	m_WorkTimeInterval = std::make_pair( nStartTime, nEndTime );
	m_State = std::auto_ptr<ArchiveState>( 
		new ArchiveStatePlay( m_StreamManager, m_Streams, m_Frames, m_WorkTimeInterval, CameraIndexArr ) );
}

void	ArchiveRender::ReleaseArchive( )
{
	m_Frames.clear();
	m_Streams.clear();
	m_State = std::auto_ptr<ArchiveState>(
		new ArchiveStateIdle( m_StreamManager, m_Streams, m_Frames )
		);
}

void	ArchiveRender::Play()
{
	time_t StartTime = m_State->GetArchivePosition();
	m_State = std::auto_ptr<ArchiveState>(
		new ArchiveStatePlay( m_StreamManager, m_Streams, m_Frames, m_WorkTimeInterval, StartTime )
		);
}

void	ArchiveRender::Pause()
{
	time_t StartTime = m_State->GetArchivePosition();
	m_State = std::auto_ptr<ArchiveState>(
		new ArchiveStatePause( m_StreamManager, m_Streams, m_Frames, m_WorkTimeInterval, StartTime )
		);
}

void	ArchiveRender::Stop()
{
	m_State = std::auto_ptr<ArchiveState>(
		new ArchiveStateStop( m_StreamManager, m_Streams, m_Frames, m_WorkTimeInterval )
		);
}

void	ArchiveRender::Seek( time_t NewArchivePosition )
{
	m_State->Seek( NewArchivePosition );
}

time_t	ArchiveRender::GetArchivePosition()
{
	return m_State->GetArchivePosition( );
}

boost::optional<time_t>	ArchiveRender::GetCameraArchivePosition(int CameraID) 
{
	return  m_State->GetCameraArchivePosition( CameraID);
}

void ArchiveRender::BindCallback( boost::function0<void> fn )
{
	m_DrawCallbackFn = fn;
}

void ArchiveRender::UnbindCallback()
{
	m_DrawCallbackFn = 0;
}

void ArchiveRender::OnRenderUpdate()
{
	if( m_DrawCallbackFn )
	{
		m_DrawCallbackFn();
	}	
}

void	ArchiveRender::OnCameraUpdate()
{
	m_State->OnUpdate( );
}

void ArchiveRender::MakeActive()
{
	OGLRenderManager* rm = dllGetRenderManager();
	rm->SetActiveRender( OGLRenderManager::RM_ARCHIVE_RENDER );
}