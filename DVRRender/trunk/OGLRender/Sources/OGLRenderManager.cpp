/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLRenderManager.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-03-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "StdAfx.h"
#include "chcsva.h"
#include "../../IDVRGraphicsWrapper.h"
#include "../../DVRRender.hpp"
#include "OGLWrapper.hpp"
#include "OGLStreamRender.hpp"
//#include "OGLArchiveRender.hpp"
#include "archive/ArchiveRender.h"
#include "OGLRenderManager.hpp"


//////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(OGLRenderManager, wxGLCanvas)
	EVT_SIZE( OGLRenderManager::OnSize )
	EVT_PAINT( OGLRenderManager::OnPaint )
	EVT_TIMER( StreamTimerID, OGLRenderManager::OnStreamRenderTimer )
	EVT_TIMER( ArchiveTimerID, OGLRenderManager::OnArchiveRenderTimer )
	EVT_ERASE_BACKGROUND( OGLRenderManager::OnEraseBackground)
END_EVENT_TABLE()
//////////////////////////////////////////////////////////////////////////
OGLRenderManager::OGLRenderManager(
									boost::shared_ptr<CHCS::IVideoSystemConfig>		pVideoSystemConfig,
								   boost::shared_ptr<Elvees::IMessage> pDebugOutput,
								   wxWindow *parent, 
								   const wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ):
	wxGLCanvas( parent, id, pos, size, style|wxWS_EX_PROCESS_IDLE, name),
	mWrapper( new OGLGraphicsWrapper() ),
	mRenderMode( RM_STREAM_RENDER ),
	m_pDebugOutput( pDebugOutput )
{
#ifdef WIN32
	if( !mLibCHCSVA.Load( TEXT("chcsva.dll") ) )
		return;
#endif

	//Load library
	CHCS::funcInitStreamManagerEx mInitStreamManagerFunc = 
		(CHCS::funcInitStreamManagerEx)mLibCHCSVA.GetSymbol( "InitStreamManagerEx" );

	//Initialize CHCSVA
	const DWORD c_VideoMode = CHCS_MODE_CLIENT
#ifndef _DEBUG
		& ~CHCS_MODE_GUI
#endif
		;
	CHCS::INITCHCS_EX initCHCS;
	initCHCS.dwReserved = 0;
	initCHCS.dwUserData = reinterpret_cast<DWORD_PTR>(pDebugOutput.get());
	initCHCS.dwSize = sizeof(initCHCS);
	initCHCS.dwMode = c_VideoMode;
	initCHCS.procOutput = &OGLRenderManager::OutputDebugProc;	
	initCHCS.m_pVideoSystemConfig = pVideoSystemConfig.get();

	mStreamManager = boost::shared_ptr<CHCS::IStreamManager>( mInitStreamManagerFunc( &initCHCS ) );
#if 0
	size_t StreamNumber ;
	mStreamManager->GetAvailableStreamIDs( 0, &StreamNumber );
	std::vector<long> z( StreamNumber );
	mStreamManager->GetAvailableStreamIDs( &z[0], &StreamNumber );
#endif
	mArchiveRender = boost::shared_ptr<ArchiveRender>( new ArchiveRender( mStreamManager ) );
	mStreamRender = boost::shared_ptr<OGLStreamRender>( new OGLStreamRender( mStreamManager ) );

	//Set timer owners
	mStreamTimer.SetOwner( this, StreamTimerID );
	mArchiveTimer.SetOwner( this, ArchiveTimerID );
	mCamerasRefreshTimer.SetOwner( this, CameraRefreshTimer );

	//Start timers
	mStreamTimer.Start( StreamTimerRefresh );
	mArchiveTimer.Start( ArchiveTimerRefresh );
	mCamerasRefreshTimer.Start( CameraRefreshTimer );
}

OGLRenderManager::~OGLRenderManager()
{
}

void OGLRenderManager::OutputDebugProc( DWORD_PTR pUserData, int nType, LPCWSTR szText)
{
	//! вызывается из chcsva, просто преобразуем в ANSI и положим в поток
	_ASSERTE( pUserData );
	_ASSERTE( szText );
	std::vector<char> sBuf;
	size_t nSize = wcstombs(0, szText, 0 );
	sBuf.resize( nSize + 1, 0 );
	wcstombs( &sBuf[0], szText, sBuf.size() );
	Elvees::IMessage* pDebug = reinterpret_cast<Elvees::IMessage*>(pUserData);
	pDebug->Print( nType, &sBuf[0] );
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderManager::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
	/* Do nothing, to avoid flashing on MSW */
}

void OGLRenderManager::OnPaint( wxPaintEvent& event )
{
	/* must always be here */
	wxPaintDC dc(this);

	// ничего не делаем, отрисовываем по таймеру
}

void OGLRenderManager::OnGLDraw()
{
	//Set this context as current for OpenGL operations
	SetCurrent();

#if defined _DEBUG  && 0
	mWrapper->Clear( wxColour(0,0,255) );
	mWrapper->DrawLine( 0.0f, 0.0f, 1.0f, 1.0f );
	mWrapper->DrawLine( 1.0f, 0.0f, 0.0f, 1.0f );
	mWrapper->DrawText( "Hello World!!!", 0.5,0.5,1.0,0.5, IDVRGraphicsWrapper::TA_Center );
#endif

	if( mRenderMode == RM_STREAM_RENDER )
	{
		//Call callback function if exist 
		if( !mStreamCallbackFunc.empty() )
			mStreamCallbackFunc( mStreamRender.get() );		
	}
	else if( mRenderMode == RM_ARCHIVE_RENDER )
	{
		//Call callback function if exist 
		if( !mArchiveCallbackFunc.empty() )
			mArchiveCallbackFunc( mArchiveRender.get() );
	}

	//Swap buffers
	SwapBuffers();
}

void OGLRenderManager::OnStreamRenderTimer( wxTimerEvent& event )
{
	if( mRenderMode == RM_STREAM_RENDER )
	{
		//Call render update method
		mStreamRender->OnRenderUpdate();

		//Repaint invocation
		//Refresh( false );
		OnGLDraw();
	}
}

void OGLRenderManager::OnArchiveRenderTimer( wxTimerEvent& event )
{
	if( mRenderMode == RM_ARCHIVE_RENDER )
	{
		//Call render update method
		mArchiveRender->OnRenderUpdate();

		//Repaint invocation
		//Refresh( false );
		OnGLDraw();
	}
}

void OGLRenderManager::OnCameraRefreshTimer( wxTimerEvent& event )
{
	mStreamRender->OnCameraUpdate();
	mArchiveRender->OnCameraUpdate();
}

void OGLRenderManager::OnSize( wxSizeEvent& event )
{
	// this is also necessary to update 
	//the context on some platforms
	wxGLCanvas::OnSize(event);

	//Get new client size
	int width, height;
	GetClientSize(&width, &height);

	//If OpenGL context present
	if ( GetContext() )
	{
		//Resize viewport
		mWrapper->SetViewport( width, height );
	}
}

//////////////////////////////////////////////////////////////////////////
IDVRStreamRender* OGLRenderManager::GetStreamRender() const
{
	return static_cast<IDVRStreamRender*>( mStreamRender.get() );	
}

IDVRArchiveRender* OGLRenderManager::GetArchiveRender() const
{
	return static_cast<IDVRArchiveRender*>( mArchiveRender.get() );
}

IDVRGraphicsWrapper* OGLRenderManager::GetWrapper()
{
	return mWrapper.get();
}

wxWindow* OGLRenderManager::GetWindow()
{
	return dynamic_cast<wxWindow*>(this);
}

void OGLRenderManager::SetActiveRender( RenderMode rm )
{
	mRenderMode = rm;	
}

OGLRenderManager::RenderMode OGLRenderManager::GetActiveRender() const
{
	return mRenderMode;
}

void OGLRenderManager::SetRefreshPeriod( OGLRenderManager::RenderMode m, int period )
{
	switch( m )
	{
	case OGLRenderManager::RM_STREAM_RENDER:
		mStreamTimer.Start( period );
		break;
	case OGLRenderManager::RM_ARCHIVE_RENDER:
		mArchiveTimer.Start( period );
		break;
	}
}

int OGLRenderManager::GetRefreshPeriod( OGLRenderManager::RenderMode m ) const
{
	int period = -1;
	switch( m )
	{
	case OGLRenderManager::RM_STREAM_RENDER:
		period = mStreamTimer.GetInterval();
		break;
	case OGLRenderManager::RM_ARCHIVE_RENDER:
		period = mArchiveTimer.GetInterval();
		break;
	}
	return period;
}

//////////////////////////////////////////////////////////////////////////
