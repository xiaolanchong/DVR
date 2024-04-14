//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Common mode frame

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   14.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_FRAME_4597487007811864_
#define _CAMERA_FRAME_4597487007811864_

#include "CameraCanvas.h"
#include "layout/LayoutCameraManager.h"
#include "LayoutKeeper.h"

class IDVRStreamRender;
class RenderEventCallback;

//======================================================================================//
//                                  class CameraFrame                                   //
//======================================================================================//

//! \brief Common mode frame
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class CameraFrame : public wxPanel
{
	LayoutKeeper	m_LayoutKeeper;

	virtual void PreFullScreen(bool bSetFullScreen) 
	{
	}
	wxPanel*	m_pRenderContainerWnd;
public:
	CameraFrame(const std::vector<int>& AvailableCameras);
	virtual ~CameraFrame();

	virtual void					Init(  );
	virtual RenderEventCallback*	InitRender( IDVRRenderManager* pMainRender )	= 0;
	virtual void					UninitRender()									= 0;
	virtual void					OnActivate(bool bActive)
	{
	}
protected:	// data 
	wxToolBar*					m_pViewToolBar;

	LayoutCameraManager	m_LayoutMgr;
protected:	// overriding
	virtual wxSizer*	GetControlPanelSizer()	= 0;
	virtual void		Invalidate();
public:
	wxWindow*			GetRenderContainerWindow() { return m_pRenderContainerWnd; }
	void				SetRenderWindow( wxWindow* pRenderWindow , bool bSet);

protected:	// handlers

	void	OnViewMode( wxCommandEvent& ev );
	void	OnStretchMode( wxCommandEvent& ev );
	void	OnFullScreen( wxCommandEvent& ev );

	DECLARE_EVENT_TABLE(); 
};

#endif // _CAMERA_FRAME_4597487007811864_