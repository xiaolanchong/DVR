/*
*	DVR 
*	Copyright(C) 2006 Elvees
*	All rights reserved.
*
*	$Filename: VideoClient.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-03-18
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Application class used for dvrRender testing
*
*
*/

#include "stdafx.h"
#include "../DVRRender.hpp"
#include "../OGLRender/Sources/OGLWrapper.hpp"
#include "VideoClient.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_APP( VideoClientApp )

//////////////////////////////////////////////////////////////////////////
bool VideoClientApp::OnInit()
{
	VideoClientFrame* frame = new VideoClientFrame( (wxWindow*)NULL, "VideoClient", wxDefaultPosition, wxSize(640,480) );
	frame->Show( true );
	return true;
}



//////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(VideoClientFrame, wxFrame)
	EVT_SIZE( VideoClientFrame::OnSize )
END_EVENT_TABLE()
//////////////////////////////////////////////////////////////////////////
VideoClientFrame::VideoClientFrame(wxWindow *parent, const wxString& title,
								   const wxPoint& pos, const wxSize& size, 
								   long style /* = wxDEFAULT_FRAME_STYLE */):
										wxFrame(parent, wxID_ANY, title, pos, size, style)
{
	typedef IDVRRenderManager* (*funcCreateRenderManager)( wxWindow* );

#ifdef _DEBUG
	mLibrary = boost::shared_ptr<wxDynamicLibrary>( new wxDynamicLibrary( "OGLRenderd.dll"  ) );
#else
	mLibrary = boost::shared_ptr<wxDynamicLibrary>( new wxDynamicLibrary( "OGLRender.dll"  ) );
#endif
	funcCreateRenderManager createRenderFunc = (funcCreateRenderManager)mLibrary->GetSymbol( "CreateRenderManager" );
	mRenderManager = boost::shared_ptr<IDVRRenderManager>( (createRenderFunc)( this ) );

	mStreamRender = mRenderManager->GetStreamRender();
	mArchiveRender = mRenderManager->GetArchiveRender();

	mStreamCamera00 = boost::shared_ptr<IDVRStreamRender::ICamera>( mStreamRender->GetCamera(0) );

}

VideoClientFrame::~VideoClientFrame()

{
    mRenderManager.reset();
}

void VideoClientFrame::OnSize( wxSizeEvent& event )
{
	if( mRenderManager )
	{
		wxRect rect = event.GetRect();

		wxWindow* window = (wxWindow*)mRenderManager->GetWindow();
		window->SetSize( wxRect(0,0,640,480) );
	}
}


void VideoClientFrame::StreamRenderHandler()
{
			
}

void VideoClientFrame::ArchiveRenderHandler()
{

}