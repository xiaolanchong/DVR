//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main notebook window (tabs)for wxWidget application

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   09.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MainNotebook.h"
#include "../mod_realtime/VideoFrame.h"
#include "../mod_archive/ArchiveSplitter.h"
//#include "../common/wxCustomBitmap.h"
#include "../mod_log/LogFrame.h"
#include "../server/ServerHandler.h"
#include "../common/RenderEventHandler.h"
#include "../common/DBHandler.h"
#include "../common/CameraNaming.h"

//======================================================================================//
//                                  class MainNotebook                                  //
//======================================================================================//

MainNotebook::MainNotebook( 
						   wxWindow* pParentWnd, 
						   DVREngine::IEngineSettingsReader* pParamReader,
						   DVREngine::IEngineSettingsWriter* pParamWriter ) 
	//m_pImageList ( new wxImageList( 24, 24) ),
	//
	:m_pSharedWindow(0)
{
	Init( pParentWnd, pParamReader, pParamWriter );

	pParentWnd->SetClientSize( 800, 600 );
}

MainNotebook::~MainNotebook()
{
//	m_pVideoFrame->SetRenderWindow( m_pMainRender->GetCanvas() );
	m_pVideoFrame->OnActivate(false);
	m_pArchiveFrame->OnActivate(false);

	m_pVideoFrame->UninitRender();
	m_pArchiveFrame->UninitRender();
	m_RenderHandler = std::auto_ptr<RenderEventHandler>(  );
	m_pMainRender.reset();
}

boost::shared_ptr<Elvees::IMessage>		MainNotebook::GetServerMessageStream()
{
	return m_pLogFrame->CreateMessageStream( "Server" );
}
boost::shared_ptr<Elvees::IMessage>		MainNotebook::GetVideoMessageStream()
{
	return m_pLogFrame->CreateMessageStream( "Video" );
}
boost::shared_ptr<Elvees::IMessage>		MainNotebook::GetClientMessageStream()
{
	return m_pLogFrame->CreateMessageStream( "Client" );
}
boost::shared_ptr<Elvees::IMessage>		MainNotebook::GetBackupMessageStream()
{
	return m_pLogFrame->CreateMessageStream( "Backup" );
}



void MainNotebook::Init( 
						wxWindow* pParentWnd, 
						DVREngine::IEngineSettingsReader* pParamReader,
						DVREngine::IEngineSettingsWriter* pParamWriter
						)
{
	//! считаем из БД, но может быть исключение
	//! вынести вверх (в app)
	DBHandler db;

	std::vector<int> AvailableIds;
	try
	{
		db.GetCameraArr(AvailableIds);
	}
	catch ( std::exception& ) 
	{
	}
	m_pNaming.reset( new CameraNaming );

	m_pVideoFrame		= new VideoFrame(AvailableIds, pParamReader, pParamWriter );
	m_pArchiveFrame		= new ArchiveSplitter();
	m_pLogFrame			= new LogFrame(pParentWnd);



	bool res;
	res = wxXmlResource::Get()->LoadPanel( m_pVideoFrame, pParentWnd, wxT("Tab_Video"));
	_ASSERTE(res);
	
	res = wxXmlResource::Get()->LoadPanel( m_pArchiveFrame, pParentWnd, wxT("Tab_Archive"));
	_ASSERTE(res);

	m_pVideoFrame->Show(true);
	m_pArchiveFrame->Show(false);
	m_pLogFrame->Show(false);

	static_cast<CameraFrame*>(m_pVideoFrame)->Init();	
	m_pArchiveFrame->Init(AvailableIds);



//#define RENDER_STUB

	boost::shared_ptr<CHCS::IVideoSystemConfig>	zpVideoConfig( DBBridge::CreateClientVideoSystemConfig() ); 

	m_pMainRender	= std::auto_ptr<IDVRRenderManager>( 
#ifdef RENDER_STUB
		new DCRender( m_pVideoFrame->GetRenderContainerWindow() )
#else
		CreateRenderManager( 
							 zpVideoConfig,
							 m_pLogFrame->CreateMessageStream( "Video[Client]" ),
							 m_pVideoFrame->GetRenderContainerWindow() ) 
#endif
		);
	m_pSharedWindow = 
#ifdef RENDER_STUB
		reinterpret_cast<wxWindow*>(m_pMainRender->GetCanvas());
#else
		m_pMainRender->GetWindow();
#endif
	m_RenderHandler = std::auto_ptr<RenderEventHandler>( new RenderEventHandler( m_pSharedWindow ) );

	RenderEventCallback* pCallback = 0;
	pCallback = m_pVideoFrame->InitRender( m_pMainRender.get() );
	m_RenderHandler->AddHander( pCallback );
	m_pVideoFrame->SetRenderWindow( m_pSharedWindow, true );

	pCallback = m_pArchiveFrame->InitRender( m_pMainRender.get() );
	m_RenderHandler->AddHander( pCallback );

	m_RenderHandler->SetActive(0);

//	m_pMainRender->SetRenderMode( IDVRRenderManager::RM_StreamRender );
	IDVRStreamRender* pRender = m_pMainRender->GetStreamRender();
	pRender->MakeActive();

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// create text ctrl with minimal size 100x60
	topsizer->Add(
		m_pVideoFrame,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		0 );         // set border width to 10

	topsizer->Add(
		m_pArchiveFrame,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		0 );         // set border width to 10

	topsizer->Add(
		m_pLogFrame,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		0 );         // set border width to 10

	pParentWnd->SetSizer( topsizer );      // use the sizer for layout

	topsizer->SetSizeHints( pParentWnd );   // set size hints to honour minimum size

	pParentWnd->Layout();
	pParentWnd->Refresh();

	m_ChildWindows.push_back( std::make_pair( Page_Video,	m_pVideoFrame ) );
	m_ChildWindows.push_back( std::make_pair( Page_Archive, m_pArchiveFrame ) );
	m_ChildWindows.push_back( std::make_pair( Page_Log,		m_pLogFrame ) );

	m_pVideoFrame->SetMenuHandler( m_pNaming.get() );
//	m_pArchiveFrame->SetMenuHandler( m_pNaming.get() );
}

void MainNotebook::SetServerHandler( ServerHandler* pHandler)
{
	m_pServerHandler = pHandler;
}

void	MainNotebook::OnTimer()
{  
	Frames_t ca;
	if( m_pServerHandler )
	{
		m_pServerHandler->GetCurrentFrames(ca);
	}
	m_pVideoFrame->SetFrames( ca );
}
/*
BEGIN_EVENT_TABLE(MainNotebook, wxNotebook)
	EVT_TIMER(TIMER_ID,				 MainNotebook::OnTimer)
END_EVENT_TABLE()
*/


void	MainNotebook::OnPageChanged(Mode ev)
{
	CameraFrame* pNewParent = 0;
	CameraFrame* pOldParent	= 0;
	IDVRRender* pRender = 0;
	switch( ev ) 
	{
	case Page_Video:
		pRender = m_pMainRender->GetStreamRender();
		pNewParent	= m_pVideoFrame;
		pOldParent	= m_pArchiveFrame->GetCameraFrame();
		m_RenderHandler->SetActive(Page_Video);
		break;
	case Page_Archive:
		pRender = m_pMainRender->GetArchiveRender();
		pNewParent	= m_pArchiveFrame->GetCameraFrame();
		pOldParent	= m_pVideoFrame;
		m_RenderHandler->SetActive(Page_Archive);
		break;
	default:

		;
	}
	m_pVideoFrame->OnActivate(false);
	m_pArchiveFrame->OnActivate(false);
	if(m_pSharedWindow && pNewParent && pOldParent) 
	{
		wxWindow* pRenderParent = pNewParent->GetRenderContainerWindow();
		pOldParent->SetRenderWindow(m_pSharedWindow, false);
		pRender->MakeActive();
		m_pSharedWindow->Reparent( pRenderParent );
		pNewParent->SetRenderWindow( m_pSharedWindow, true );
		pNewParent->OnActivate(true);
	}
}

void	MainNotebook::SetActiveWindow(  Mode WindowMode )
{
	for ( size_t i = 0; i < m_ChildWindows.size(); ++i )
	{
		if( m_ChildWindows[i].first == WindowMode )
		{
			m_ChildWindows[i].second->Show( true );
		}
		else
		{
			m_ChildWindows[i].second->Show( false );
		}
	}

	// poor
	m_pArchiveFrame->GetParent()->Layout();
	m_pArchiveFrame->GetParent()->Refresh();

	OnPageChanged( WindowMode );
}