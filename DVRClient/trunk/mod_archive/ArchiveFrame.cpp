//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main archive tab window

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   14.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ArchiveFrame.h"
#include "VideoSeeker.h"


//======================================================================================//
//                                  class ArchiveFrame                                  //
//======================================================================================//

ArchiveFrame::ArchiveFrame( 
							const std::vector<int>& AvailableCameras,
							boost::function1< void, time_t> fnOnTimeChanged):
	CameraFrame( AvailableCameras  ) ,
	m_fnOnTimeChanged( fnOnTimeChanged ) 
{

}

ArchiveFrame::~ArchiveFrame()
{
}

void ArchiveFrame::Init()
{
	CameraFrame::Init();
}

RenderEventCallback*	ArchiveFrame::InitRender( IDVRRenderManager* pMainRender )
{
	m_pCanvas = std::auto_ptr<ArchiveCanvas>(new ArchiveCanvas( m_LayoutMgr, pMainRender, m_fnOnTimeChanged ));
	return m_pCanvas.get();
}

void		ArchiveFrame::UninitRender()
{
	m_pCanvas.reset();
}

void	ArchiveFrame::OnTimeCallback( time_t NewTime )
{
#ifdef OLD
	m_pVideoSeeker->SetCurrentTime( NewTime );
	SetCurrentTimeText( NewTime );
#endif
	m_fnOnTimeChanged( NewTime );
}


wxSizer*	ArchiveFrame::GetControlPanelSizer()
{
	wxToolBar* pWnd = XRCCTRL( *this, "Stub_ToolBar_Archive1", wxToolBar );
	_ASSERTE(pWnd);
	return pWnd->GetContainingSizer();
}

BEGIN_EVENT_TABLE(ArchiveFrame, CameraFrame)

END_EVENT_TABLE()

#ifdef OLD 


#endif

void ArchiveFrame::PreFullScreen(bool bSetFullScreen) 
{
//	ShowPanel( !bSetFullScreen );
}

void	ArchiveFrame::OnActivate(bool bActive)
{
	if(!bActive)
	{
		m_pCanvas->ReleaseArchive(  );
#ifdef OLD
		InvalidateCurrentTime();
		m_State.reset( new ArchiveFrameIdle(*this) );
#endif
	}
}

#ifdef OLD

void	ArchiveFrame::OnSeekTimeChange()
{
	time_t CurrentTime = m_pVideoSeeker->GetCurrentTime();
	SetCurrentTimeText(CurrentTime);
	m_pCanvas->SeekArchive( CurrentTime );
}

void	ArchiveFrame::SetCurrentTimeText( time_t CurrentTime )
{
	_ASSERTE(m_pCurrentTimeText );
	tm pTime = *localtime( &CurrentTime );
	char buf[255];
	strftime( buf, 255, "%H:%M:%S %d.%m.%y",  &pTime);
	m_pCurrentTimeText->SetLabel( buf );
}

void	ArchiveFrame::InvalidateCurrentTime()
{
	_ASSERTE(m_pVideoSeeker);
	_ASSERTE(m_pCurrentTimeText );
	m_pVideoSeeker->InvalidatePeriod();
	m_pCurrentTimeText->SetLabel( wxT("") );
	Layout();
}



#endif

void	ArchiveFrame::AcquireArchive( time_t StartTime, time_t EndTime )
{
	m_pCanvas->StartArchive(StartTime, EndTime );
}

void	ArchiveFrame::ReleaseArchive()
{
	m_pCanvas->ReleaseArchive(  );
}

void ArchiveFrame::PlayArchive()
{
	m_pCanvas->PlayArchive();
}

void ArchiveFrame::PauseArchive()
{
	m_pCanvas->PauseArchive();
}

void ArchiveFrame::StopArchive()
{
	m_pCanvas->StopArchive();
}

void ArchiveFrame::SeekArchive(time_t NewArchiveTime)
{
	m_pCanvas->SeekArchive( NewArchiveTime );
}