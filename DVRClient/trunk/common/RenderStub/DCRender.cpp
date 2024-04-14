//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка для IDVRRenderManager - рисование на контексте окна

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   21.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DCRender.h"

DCWrapper2D::DCWrapper2D(wxDC* pDC, wxSize sizeContext) :
	m_pDC(pDC),
	m_sizeContext(sizeContext)
{
}

void DCWrapper2D::Clear( const OGLWrapper2D::Color& color )
{
	m_pDC->SetPen(wxNullPen);
	wxBrush br(ToColor(color));
	m_pDC->SetBrush( br );
	m_pDC->DrawRectangle( wxPoint(0, 0), m_sizeContext );
}

void DCWrapper2D::DrawText( const std::string& text, float x, float y, const OGLWrapper2D::Color& color,  OGLWrapper2D::Font font )
{
	m_pDC->DrawText( text.c_str(), ToPoint(x, y) );
}

void DCWrapper2D::DrawText( const std::string& text, float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, OGLWrapper2D::Font font)
{
	DrawText(text, x0, y0, color, font);
}

void DCWrapper2D::DrawRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth  )
{
	wxPen pen(ToColor(color), lineWidth  );
	wxBrush br( ToColor(color), wxTRANSPARENT );
	m_pDC->SetPen(pen);
	m_pDC->SetBrush(br );
	m_pDC->DrawRectangle( wxRect( ToPoint(x0, y0), ToPoint( x1, y1 ) ) );
}

void DCWrapper2D::DrawLine( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth )
{
}

void DCWrapper2D::DrawFilledRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& fillColor )
{
}

void DCWrapper2D::DrawBitmap( const OGLWrapper2D::Bitmap& bitmap, float x, float y, float width, float height, bool highQuality)
{
}

void DCWrapper2D::DrawOval( float x, float y, float width, float height, const OGLWrapper2D::Color& color, int segments , float lineWidth)
{
}

//////////////////////////////////////////////////////////////////////////

IDVRRender::Result DVRStreamRenderStub::DrawFrame(int cameraId, float x, float y, float width, float height, bool highQuality)
{
	m_pRender->DrawRect( x, y, x + width, y + height, OGLWrapper2D::Color(0.0f, 0.0f, 1.0f), 3);
	boost::format fmt("[%d]:stream");
	m_pRender->DrawText( (fmt % cameraId).str(), x, y, width, height, 
						OGLWrapper2D::Color(0.0f, 0.0f, 1.0f), OGLWrapper2D::FT_Default );
	return R_Success;
}

//////////////////////////////////////////////////////////////////////////

IDVRRender::Result DVRArchiveRenderStub::DrawFrame(int cameraId, float x, float y, float width, float height, bool highQuality)
{
	m_pRender->DrawRect( x, y, x + width, y + height, OGLWrapper2D::Color(0.0f, 1.0f, 0.0f), 3);
	boost::format fmt("[%d]:archive");
	m_pRender->DrawText( (fmt % cameraId).str(), x, y, width, height, 
						OGLWrapper2D::Color(0.0f, 0.0f, 1.0f), OGLWrapper2D::FT_Default );
	return R_Success;
}

//======================================================================================//
//                                    class DCRender                                    //
//======================================================================================//

DCRender::DCRender( wxWindow* pParentWnd) : 
	wxWindow( pParentWnd, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ),
	m_RenderMode( RM_StreamRender ),
	m_pRender( new DCWrapperBase2D ),
	m_StreamRenderStub( m_pRender ),
	m_ArchiveRenderStub( m_pRender )
{
	
}

DCRender::~DCRender()
{
}

IDVRRender* DCRender::GetRender( RenderMode mode ) const
{
	switch( mode )
	{
		case RM_StreamRender:	return &m_StreamRenderStub;
		case RM_ArchiveRender:	return &m_ArchiveRenderStub;
		default:				return 0;
	}
}

BEGIN_EVENT_TABLE(DCRender, wxWindow)
	EVT_PAINT( DCRender::OnPaint)
END_EVENT_TABLE()

void DCRender::OnPaint(wxPaintEvent& ev)
{
	wxPaintDC dc(this);
	wxSize s = GetClientSize();
	m_pRender = std::auto_ptr<DCWrapperBase2D>( new DCWrapper2D(&dc, s ) );
	switch( m_RenderMode ) 
	{
	case RM_StreamRender : if( !m_StreamFunc.empty() ) m_StreamFunc( &m_StreamRenderStub );
		break;
	case RM_ArchiveRender : if( !m_ArchiveFunc.empty() ) m_ArchiveFunc( &m_ArchiveRenderStub );
		break;
	default:;
	}
	m_pRender = std::auto_ptr<DCWrapperBase2D>( new DCWrapperBase2D );
}