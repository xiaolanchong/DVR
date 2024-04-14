//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _D_V_R_RENDER_STUB_6908233503971558_
#define _D_V_R_RENDER_STUB_6908233503971558_

#include "IDVRRender.h"

//======================================================================================//
//                                 class DVRRenderStub                                  //
//======================================================================================//

//! \brief Not available
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class DVRRenderStub :	wxGLCanvas,
						public	IDVRRenderStub,
						IOGLRender,
						boost::noncopyable
{
public:
	DVRRenderStub( wxWindow* pWnd );
	virtual ~DVRRenderStub();


	typedef boost::tuple<unsigned char, unsigned char, unsigned char> Color_t;

	bool IsEnable() const { return m_bEnable; }
private:

//////////////////////////////////////////////////////////////////////////
	virtual void Bind( boost::function1<void, IOGLRender*> f );
	virtual void Unbind();
	virtual void SetCameras( const std::vector<int>& CameraIDArr );
	virtual wxWindow* GetWindow();
//////////////////////////////////////////////////////////////////////////
	/*
	*	Clears frame buffer with the specified color
	*/
	virtual void Clear( const Color& color );

	/*
	*	Draws text on the canvas at x,y
	*/
	virtual void DrawText( const std::string& text, float x, float y, const Color& color );

	/*
	*	Draws rectangle with specified color
	*/
	virtual void DrawRect( float x0, float y0, float x1, float y1, const Color& color );

	/*
	*	Draws rectangle and fill it with the specified color
	*/
	virtual void DrawFilledRect( float x0, float y0, float x1, float y1, const Color& fillColor );

	/*
	*	Draws and stretch\shrink ( if needed ) bitmap at the specified position 
	*/
	virtual void DrawBitmap( int nCameraID, float x, float y, float width, float height, bool highQuality  );

	/* 
	*	Draws an oval centered at (x,y)
	*	The oval is bound inside a rectangle with specified width and height
	*	segments represents the number of line segments used to draw the oval.
	*/
	virtual void DrawOval( float x, float y, float width, float height, const Color& color, int segments  ) {};


	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnTimer(wxTimerEvent& event);

	void			Draw();
	void			Render();
	void			InitGL();

	DECLARE_EVENT_TABLE();

	boost::function1<void, IOGLRender*>	m_Callback;

//	bool			m_init;

	int				m_nWindowWidth;
	int				m_nWindowHeight;
	bool			m_bEnable;


private:
	wxTimer m_timer;

};

#endif // _D_V_R_RENDER_STUB_6908233503971558_