/*
*	DVR 
*	Copyright(C) 2006 Elvees
*	All rights reserved.
*
*	$Filename: VideoClient.h
*	$Author:   Yanakov Leonid
*	$Date:     2006-03-18
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Application class used for dvrRender testing
*
*
*/
#ifndef __VIDEOCLIENT_H__
#define __VIDEOCLIENT_H__
//////////////////////////////////////////////////////////////////////////

class wxDynamicLibrary;
class IDVRRenderManager;
class IDVRStreamRender;
class IDVRArchiveRender;
class IDVRStreamRender::ICamera;




// Define a new application type
class VideoClientApp: public wxApp
{
public:
	bool OnInit();
};
DECLARE_APP( VideoClientApp )

//////////////////////////////////////////////////////////////////////////
//Define a main frame window
class VideoClientFrame: public wxFrame
{
public:
	VideoClientFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
		const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	virtual ~VideoClientFrame();
public:
	void StreamRenderHandler();
	void ArchiveRenderHandler();
protected:
	void OnSize( wxSizeEvent& event );
	DECLARE_EVENT_TABLE()
private:
	boost::shared_ptr<IDVRRenderManager> mRenderManager;
	boost::shared_ptr<wxDynamicLibrary> mLibrary;
	boost::shared_ptr<IDVRStreamRender::ICamera> mStreamCamera00;
	IDVRStreamRender* mStreamRender;
	IDVRArchiveRender* mArchiveRender;
};

#endif //__VIDEOCLIENT_H__
