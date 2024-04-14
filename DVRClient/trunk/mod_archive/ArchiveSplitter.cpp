//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Разделитель - основное окно режима архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   07.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ArchiveSplitter.h"
#include "ArchiveFrame.h"
#include "ArchiveControlPanel.h"
#include "../common/DBHandler.h"

BEGIN_EVENT_TABLE(SplitterEventHandler, wxEvtHandler)
	EVT_SPLITTER_DCLICK( XRCID("ID_SPLITTER"), SplitterEventHandler::OnDoubleClick)
END_EVENT_TABLE()

static SplitterEventHandler g_VetoDoubleClick;

//======================================================================================//
//                                class ArchiveSplitter                                 //
//======================================================================================//

ArchiveSplitter::ArchiveSplitter()
{
}

ArchiveSplitter::~ArchiveSplitter()
{
}

void					ArchiveSplitter::Init( const std::vector<int>& AvailableCameras )
{
	bool bRes;
	m_pSplitterWnd = XRCCTRL( *this, wxT("ID_SPLITTER"), wxSplitterWindow );	
	_ASSERTE(m_pSplitterWnd);

	m_pSplitterWnd->PushEventHandler(&g_VetoDoubleClick);

	m_pArchiveFrame = new ArchiveFrame( AvailableCameras, boost::bind( &ArchiveSplitter::OnTimeCallbackFromVideo, this, _1 ) );
	bRes = wxXmlResource::Get()->LoadPanel( m_pArchiveFrame, m_pSplitterWnd, wxT("Tab_Archive_Left") );
	_ASSERTE(bRes);

	m_pArchiveControl = new ArchiveControlPanel( this );
	bRes = wxXmlResource::Get()->LoadPanel( m_pArchiveControl, m_pSplitterWnd, wxT("Tab_Archive_Right") );
	//m_pArchiveControl = XRCCTRL( *m_pSplitterWnd, wxT("Tab_Archive_Right"), ArchiveControlPanel );
	_ASSERTE(m_pArchiveControl);

	m_pArchiveFrame->Init();
	m_pArchiveControl->Init();
	m_pArchiveControl->SetCameras( AvailableCameras );

	bRes = m_pSplitterWnd->SplitVertically( m_pArchiveFrame, m_pArchiveControl, -300 );
	_ASSERTE(bRes);

	m_pSplitterWnd->SetSashGravity( 1.0 );
	m_pSplitterWnd->SetMinimumPaneSize(200);
	//m_pSplitterWnd->SplitVertically()
}

RenderEventCallback*	ArchiveSplitter::InitRender( IDVRRenderManager* pMainRender )
{
	return m_pArchiveFrame->InitRender( pMainRender );
}

void					ArchiveSplitter::UninitRender()
{
	return m_pArchiveFrame->UninitRender( );
}

CameraFrame*		ArchiveSplitter::GetCameraFrame()
{
	return m_pArchiveFrame;
}

void			ArchiveSplitter::OnActivate(bool bActive)
{
	m_pArchiveFrame->OnActivate( bActive );
	m_pArchiveControl->ReleaseArchiveData();
}

void	ArchiveSplitter::OnTimeCallbackFromVideo( time_t NewArchiveTime )
{
	_ASSERTE( NewArchiveTime > 0 );
	m_pArchiveControl->SetCurrentArchiveTime( NewArchiveTime );
}

boost::shared_ptr<Frames_t> 	ArchiveSplitter::OnStartArchive( time_t StartTime, time_t EndTime )
{
	boost::shared_ptr<Frames_t> AlarmRects = GetAlarmDataForPeriod( StartTime, EndTime );
	m_pArchiveFrame->AcquireArchive(StartTime, EndTime ) ;
	return AlarmRects;
}

boost::shared_ptr<Frames_t>	ArchiveSplitter::GetAlarmDataForPeriod( time_t StartTime, time_t EndTime) const
try
{
	boost::shared_ptr<Frames_t>	AlarmRects( new Frames_t);
	DBHandler	db;
	db.GetAlarmData( StartTime, EndTime, *AlarmRects.get()  );
	return AlarmRects;
}
catch( std::exception& )
{
	// RESOURCES
	wxMessageBox( 
		_("Error occuried while loading data"), 
		_("Database error"), 
		wxOK|wxICON_ERROR
		);
	return boost::shared_ptr<Frames_t>( new Frames_t );
};

void		ArchiveSplitter::OnPlayArchive()
{
	m_pArchiveFrame->PlayArchive();
}

void		ArchiveSplitter::OnPauseArchive()
{
	m_pArchiveFrame->PauseArchive();
}

void		ArchiveSplitter::OnStopArchive()
{
	m_pArchiveFrame->StopArchive();
}

void	ArchiveSplitter::OnArchiveTimeChanged( time_t NewArchiveTime )
{
	m_pArchiveFrame->SeekArchive( NewArchiveTime );
}

void	ArchiveSplitter::SetMenuHandler( IMenuHandler* pMenuHandler)
{
	m_pArchiveFrame->SetMenuHandler( pMenuHandler );
}

void	ArchiveSplitter::GetCurrentSelectedCameras(std::vector<int>& CameraIDArr)
{
	m_pArchiveFrame->GetCurrentSelectedCameras( CameraIDArr );
}