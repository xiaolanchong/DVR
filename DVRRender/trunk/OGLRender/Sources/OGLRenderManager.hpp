/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLRenderManager.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-03-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: OpenGL-implementation of the DVRRenderManager
*
*
*/

#ifndef __OGLRENDERMANAGER_HPP__
#define __OGLRENDERMANAGER_HPP__

class OGLStreamRender;
class ArchiveRender;
class OGLGraphicsWrapper;

class OGLRenderManager : public wxGLCanvas, public IDVRRenderManager
{
public:
	enum RenderMode
	{
		RM_STREAM_RENDER = 0,
		RM_ARCHIVE_RENDER
	};

	boost::shared_ptr<Elvees::IMessage>	m_pDebugOutput;

	//! отладочная процедура для CHCSVA (передается при инициализации и выводит в m_Msg)
	static void OutputDebugProc( DWORD_PTR, int, LPCWSTR );
public:
	//Default render refresh value
	static const int StreamTimerID = 1005;
	static const int StreamTimerRefresh = 50;		//Stream Rendering ops

	static const int ArchiveTimerID = 1006;			
	static const int ArchiveTimerRefresh = 50;		//Archive rendering ops

	static const int CameraRefreshTimerID = 1007;
	static const int CameraRefreshTimer = 500;		//Cameras releasing ops
public:
	OGLRenderManager(	boost::shared_ptr<CHCS::IVideoSystemConfig>		pVideoSystemConfig,
						boost::shared_ptr<Elvees::IMessage> pOutputDebug,
						wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "OGLRenderManager");
	virtual ~OGLRenderManager();
//public:
private:
	void OnEraseBackground( wxEraseEvent& WXUNUSED(event) );
	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnTimer( wxTimerEvent& event );
	void OnStreamRenderTimer( wxTimerEvent& event  );
	void OnArchiveRenderTimer( wxTimerEvent& event );
	void OnCameraRefreshTimer( wxTimerEvent& event );

	//! нарисовать на контекст OpenGL
	void OnGLDraw();
public:
	/*
	*	Returns stream render instance
	*/
	IDVRStreamRender* GetStreamRender() const;

	/*
	*	Returns archive render instance	
	*/
	IDVRArchiveRender* GetArchiveRender() const;

	/*
	*	Returns graphics wrapper used for rendering
	*/
	IDVRGraphicsWrapper* GetWrapper();

	/*
	*	Get parent wxWindow
	*/
	wxWindow* GetWindow();
public:
	/*
	*	Sets\Gets active render
	*/
	void SetActiveRender( RenderMode rm );
	RenderMode GetActiveRender() const;

	/*
	*	Sets\Gets refresh period for the specified render mode
	*/
	void SetRefreshPeriod( RenderMode m, int period );
	int GetRefreshPeriod( RenderMode m ) const;
private:
	wxTimer mStreamTimer;
	wxTimer mArchiveTimer;
	wxTimer mCamerasRefreshTimer;
private:
	wxDynamicLibrary mLibCHCSVA; //CHCSVA library
	boost::shared_ptr<OGLGraphicsWrapper> mWrapper;	//OpenGL wrapper pointer
	boost::shared_ptr<OGLStreamRender> mStreamRender;	//Stream render
	boost::shared_ptr<ArchiveRender> mArchiveRender;	//Archive render
	boost::shared_ptr<CHCS::IStreamManager> mStreamManager;

private:
	boost::function1<void, IDVRStreamRender*> mStreamCallbackFunc;	//Callback function
	boost::function1<void, IDVRArchiveRender*> mArchiveCallbackFunc;	//Callback function
	
	DECLARE_EVENT_TABLE()
private:
	RenderMode mRenderMode;
};



#endif //__OGLRENDERMANAGER_HPP__
