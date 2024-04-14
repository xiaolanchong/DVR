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
#include "stdafx.h"
#include "DVRRenderStub.h"
#include <process.h>

//======================================================================================//
//                                 class DVRRenderStub                                  //
//======================================================================================//

const int TIMER_ID = 0xfef;

DVRRenderStub::DVRRenderStub(wxWindow* pParentWnd ):
			wxGLCanvas(
				pParentWnd, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, 
				0
				#ifdef __WXMSW__ // only for windows
							|wxFULL_REPAINT_ON_RESIZE
				#endif
							, "GLWindow"),

			m_timer( this, TIMER_ID ),
			m_bEnable(true)
{
	m_timer.Start( 200 );
}

DVRRenderStub::~DVRRenderStub()
{

}

BEGIN_EVENT_TABLE(DVRRenderStub, wxGLCanvas)
	EVT_SIZE(DVRRenderStub::OnSize)
	EVT_PAINT(DVRRenderStub::OnPaint)
	EVT_ERASE_BACKGROUND(DVRRenderStub::OnEraseBackground)
	EVT_TIMER( TIMER_ID, DVRRenderStub::OnTimer )
END_EVENT_TABLE()

void DVRRenderStub::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);
	Render();
}

void DVRRenderStub::OnSize(wxSizeEvent& event)
{
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);

//	m_LayoutMgr.SetWindowSize( event.GetSize().GetX(), event.GetSize().GetY() );
}

void DVRRenderStub::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}

void DVRRenderStub::OnTimer(wxTimerEvent& event)
{
	Refresh();
}
//////////////////////////////////////////////////////////////////////////

void DrawText(const char* pz, float xf, float yf, float wf, float hf, int ww, int wh)
{
	int x = xf * ww;
	int y = (1.0f - yf) * wh;
	glColor3f( 1.0f, 0.0f, 0.0f );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, ww-1, 0, wh-1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	x += 3;
	y -= 12;
	glRasterPos2i( x, y );

	const char *p;

	for ( p = pz; *p; p++ )
	{
		if ( *p == '\n' )
		{
			y = y - 14;
			glRasterPos2i( x, y );
			continue;
		}
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, *p );
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void SetupMatrices()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef( 1.0f, -1.0f, 1.0 );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D( 0.0f, 1.0f, -1.0f, 0.0f );
	glDisable( GL_DEPTH_TEST );
}

void DVRRenderStub::Render()
{
#ifndef __WXMOTIF__
	if (!GetContext()) return;
#endif
	GLenum err;
	SetCurrent();
	err = glGetError();
	Draw();
	SwapBuffers();
	err = glGetError();
}

void	DVRRenderStub::Draw()
{
//	std::vector<IAreaLayout::Rect_t> ArrangeArr;
	wxSize s = GetClientSize();
//	GetLayout( m_ArrangeArr, s.GetWidth(), s.GetHeight() );
	glViewport(0, 0, (GLint) s.GetWidth(), (GLint) s.GetHeight());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Clear( Color(0.5f, 0.5f, 0.5f) );
	
	//	glDisable( GL_CULL_FACE );

/*	for( size_t i = 0; i < m_ArrangeArr.size(); ++i )
	{
		const Rect& rc = m_ArrangeArr[i].second;
		SetupMatrices();
		glBegin( GL_QUADS );
		float fColor = 1.0f*i/m_ArrangeArr.size();
		glColor3f( fColor, fColor, fColor );

		float	x = rc.x,
				y = rc.y,
				w = rc.w,
				h = rc.h;
		glVertex3f( x, y, 0.5f );
		glVertex3f( x + w, y, 0.5f );
		glVertex3f( x + w, y + h, 0.5f);
		glVertex3f( x, y +h, 0.5f ); 

		glEnd();
		std::string sText = (boost::format ( "%d" ) % m_ArrangeArr[i].first ).str();
		DrawText( sText.c_str(), x, y , w, h, s.GetWidth(),	s.GetHeight() );
	}*/

	m_Callback(this);
	glFlush();
}

void DVRRenderStub::Clear( const Color& color )
{
	glClearColor( color.red, color.green, color.blue, 1.0f );
}

void DVRRenderStub::DrawText( const std::string& text, float x, float y, const Color& color )
{

}

void DVRRenderStub::DrawRect( float x0, float y0, float x1, float y1, const Color& color )
{
}

void DVRRenderStub::DrawFilledRect( float x0, float y0, float x1, float y1, const Color& fillColor )
{
	SetupMatrices();
	glBegin( GL_QUADS );
	glColor3f( fillColor.red, fillColor.green, fillColor.blue );

	glVertex3f( x0, y0, 0.5f );
	glVertex3f( x1, y0, 0.5f );
	glVertex3f( x1, y1, 0.5f);
	glVertex3f( x0, y1, 0.5f ); 

	glEnd();
}

void DVRRenderStub::DrawBitmap( int nCameraID, float x, float y, float width, float height, bool highQuality  )
{
	UNREFERENCED_PARAMETER(highQuality);
	float fColor = nCameraID / 10.0f;
	DrawFilledRect( x, y, x + width, y + height, Color(fColor, fColor, fColor)  );
}

//////////////////////////////////////////////////////////////////////////


void	DVRRenderStub::Bind( boost::function1<void, IOGLRender*> f )
{
	m_Callback = f;
}

void	DVRRenderStub::Unbind()
{
//	m_Callback.reset();
}

void	DVRRenderStub::SetCameras( const std::vector<int>& CameraIDArr )
{

}

wxWindow* DVRRenderStub::GetWindow()
{
	return this;
}
