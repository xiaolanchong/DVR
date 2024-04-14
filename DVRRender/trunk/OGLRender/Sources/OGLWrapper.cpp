	/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLWrapper2D.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-16
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Simple OpenGL wrapper
*
*
*/
#include "StdAfx.h"
#include "../../DVRRender.hpp"
#include "../../IDVRGraphicsWrapper.h"
#include "DllMain.hpp"
#include "OGLWrapper.hpp"

#ifdef USE_GLUT
#include "text/GlutTextRender.h"
#endif

#include "text/OGLFontTextRender.h"
#include "text/WindowsFontBitmapTextRender.h"
#include "OGLRenderManager.hpp"

//#include <glaux.h>
//////////////////////////////////////////////////////////////////////////
//static GLuint gBitmapTexture;
static const float gPI = 3.14159265f;
static const float g2PI = 2*gPI;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OGLGraphicsWrapper::Brush::Brush():
	mColor( *wxBLACK )
{
}
OGLGraphicsWrapper::Brush::Brush( wxColor color ):
	mColor( color )
{
}

OGLGraphicsWrapper::Brush::~Brush()
{
}

void OGLGraphicsWrapper::Brush::SetColor(const wxColor& color )
{
	mColor = color;
}

wxColor OGLGraphicsWrapper::Brush::GetColor()
{
	return mColor;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OGLGraphicsWrapper::Pen::Pen():
	mColor( *wxBLACK ), mWidth( 1.0 )
{
}

OGLGraphicsWrapper::Pen::Pen( wxColor color, int width ):
	mColor( color ), mWidth( width )
{
}

OGLGraphicsWrapper::Pen::~Pen()
{
}

void OGLGraphicsWrapper::Pen::SetColor(const wxColor& color )
{
	mColor = color;
}

wxColor OGLGraphicsWrapper::Pen::GetColor() const
{
	return mColor;
}

void OGLGraphicsWrapper::Pen::SetWidth( float width )
{
	mWidth = width;
}

float OGLGraphicsWrapper::Pen::GetWidth() const
{
	return mWidth;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct GLSmoothMode
{
	GLSmoothMode( )
	{
		glEnable( GL_LINE_SMOOTH );
		glEnable( GL_BLEND  );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
	}

	~GLSmoothMode( )
	{
		glDisable( GL_BLEND  );
		glDisable( GL_LINE_SMOOTH );
	}
};

void OGLGraphicsWrapper::Clear( const wxColor& color )
{
	//Clear target buffer with the specified color
	const float max_uchar = std::numeric_limits<unsigned char>::max();
	glClearColor( color.Red()/max_uchar, color.Green()/max_uchar, color.Blue()/max_uchar, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );	
}

void OGLGraphicsWrapper::SetPen( IDVRGraphicsWrapper::IPen* pen )
{
	if( pen )
	{
		Pen*	_pen = 0;
#ifdef _DEBUG
		_pen = dynamic_cast<Pen*>( pen );
		assert( _pen );
#else
		_pen = static_cast<Pen*>( pen );
#endif
		Pen& mPen = *(_pen);
		
		wxColor color = mPen.GetColor();
		const float c = 1.0f / 255.0f;
		float red = c * (float)color.Red();
		float green = c * (float)color.Green();
		float blue = c * (float)color.Blue();

		glLineWidth( mPen.GetWidth() );
		glColor3f( red, green, blue );

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	}
	else
	{
	//	mPen = Pen();
	}
	
}

IDVRGraphicsWrapper::IBrush* OGLGraphicsWrapper::CreateBrush(const wxColor& color )
{
	return new Brush( color );
}

IDVRGraphicsWrapper::IPen* OGLGraphicsWrapper::CreatePen(const wxColor& color, int lineWidth )
{
	return new Pen( color, lineWidth );
}

IDVRGraphicsWrapper::IFont* OGLGraphicsWrapper::CreateFont( const std::string& filename )
{
	if( !m_pTextRender.get() )
	{
		m_pTextRender.reset( new 
			/*OGLFontTextRender */ 
			/*GlutTextRender*/
			WindowsFontBitmapTextRender("Tahoma", 8)
			);
	}
	return new Font( m_pTextRender );
}

//////////////////////////////////////////////////////////////////////////
bool OGLGraphicsWrapper::DrawBitmap( const Rect& DrawArea, int bitmapWidth, int bitmapHeight, int bpp, const void* data )
{
	assert( bitmapWidth > 0 && bitmapHeight> 0 && "Bitmap dimensions must be > 0" );

	if(!data)
	{
		return false;
	}

	float x			= DrawArea.x0;
	float y			= DrawArea.y0;
	float width		= DrawArea.x1 - DrawArea.x0;
	float height	= DrawArea.y1 - DrawArea.y0;

	//Determine bitmap bit-depth
	GLenum format;
	int bytesPerPixel;
	switch( bpp )
	{
	case 8:
		format = GL_LUMINANCE;
		bytesPerPixel = 1;
		break;
	case 16:
		format = GL_LUMINANCE_ALPHA;
		bytesPerPixel = 2;
		break;
	case 24:
		format = GL_BGR_EXT;
		bytesPerPixel = 3;
		break;
	case 32:
		format = GL_BGRA_EXT;
		bytesPerPixel = 4;
		break;
	default:
		return false;
	}

	if( !m_pBitmapRender.get() )
	{
		m_pBitmapRender = IBitmapRender::CreateBitmapRender();
	}

	m_pBitmapRender->Draw( DrawArea, bitmapWidth, bitmapHeight, format, data );

	return true;
}

wxSize OGLGraphicsWrapper::GetTextExtent( IFont* pFont, const std::string& text )
{
	OGLGraphicsWrapper::Font* pRealFont = dynamic_cast<OGLGraphicsWrapper::Font*>(pFont);
	if(!pRealFont) 
	{
		return wxSize(0, 0);
	}

	return pRealFont->GetTextExtent( text );
}

bool OGLGraphicsWrapper::DrawText( IFont* pFont, const std::string& text, const Rect& DrawArea, TextAlign_t align, const wxColour& clText )
{
	OGLGraphicsWrapper::Font* pRealFont = dynamic_cast<OGLGraphicsWrapper::Font*>(pFont);
	if(!pRealFont) 
	{
		return false;
	}

	float x0 = DrawArea.x0;
	float y0 = DrawArea.y0;
	float x1 = DrawArea.x1;
	float y1 = DrawArea.y1;

	wxSize	sizeText = pRealFont->GetTextExtent( text );

	int nViewportWidth, nViewportHeight;
	GetViewport( nViewportWidth, nViewportHeight );

	float x,y;
	float textWidth		= sizeText.GetX() / float(nViewportWidth);
	float textHeight	= sizeText.GetY() / float(nViewportHeight);

//	float xCenter = 0.5 * ( (x1 - x0) + textWidth );
//	float yCenter = 0.5 * ( (y1 - y0) + textHeight );

	switch( align )
	{
	case TA_West:
	case TA_NorthWest:
	case TA_SouthWest:
		x = x0;
		break;
	case TA_East:
	case TA_NorthEast:
	case TA_SouthEast:
		x = x1 - textWidth;
		break;
	case TA_Center:
	case TA_North:
	case TA_South:
		x = ( (x1 + x0) - textWidth ) / 2;
		break;
	default:
		_ASSERTE(false);
	}

	switch( align )
	{
	case TA_North:
	case TA_NorthWest:
	case TA_NorthEast:
		y = y0/* + textHeight*/;
		break;
	case TA_South:
	case TA_SouthWest:
	case TA_SouthEast:
		y = y1 - textHeight;
		break;
	case TA_Center:
	case TA_West:
	case TA_East:
		y = ( (y1 + y0) - textHeight ) /2;
		break;
	default:
		_ASSERTE(false);
	}

	pRealFont->DrawText( x, y, textWidth, textHeight, text, clText );
	return true;
}

bool OGLGraphicsWrapper::DrawLine( IPen* pPen, float x0, float y0, float x1, float y1 )
{
	SetPen( pPen );

	GLSmoothMode _smooth;
	//Draw Lines
	glBegin( GL_LINES );
	{
		glVertex2f( x0, y0 );
		glVertex2f( x1, y1 );
	}
	glEnd();

	return true;
}

bool OGLGraphicsWrapper::DrawRect( IPen* pPen, const Rect& DrawArea )
{
	float x0 = DrawArea.x0;
	float y0 = DrawArea.y0;
	float x1 = DrawArea.x1;
	float y1 = DrawArea.y1;

	SetPen( pPen );
	GLSmoothMode _smooth;
	//Draw Quads
	glBegin( GL_QUADS );
	{
		glVertex2f( x0, y0 );
		glVertex2f( x0, y1 );
		glVertex2f( x1, y1 );
		glVertex2f( x1, y0 );
	}
	glEnd();
	return true;
}

bool OGLGraphicsWrapper::FillRect( IBrush* pBrush, const Rect& DrawArea )
{
	OGLGraphicsWrapper::Brush* pRealBrush = dynamic_cast<OGLGraphicsWrapper::Brush*>(pBrush);
	if(!pRealBrush) 
	{
		return false;
	}

	float x0 = DrawArea.x0;
	float y0 = DrawArea.y0;
	float x1 = DrawArea.x1;
	float y1 = DrawArea.y1;

	GLSmoothMode _smooth;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	wxColour clFill = pRealBrush->GetColor();
	//Set color
	glColor3ub( clFill.Red(), clFill.Green(), clFill.Blue() );

	//Draw Quads
	glBegin( GL_QUADS );
	glVertex2f( x0, y0 );
	glVertex2f( x0, y1 );
	glVertex2f( x1, y1 );
	glVertex2f( x1, y0 );
	glEnd();
	return true;
}

//////////////////////////////////////////////////////////////////////////
void OGLGraphicsWrapper::SetViewport( int width, int height )
{
	//Check if viewport width or height is zero
	if( !width || !height )
		return;

	glEnable( GL_LINE_SMOOTH );

	//Setup viewport
	glViewport(0, 0, (GLint)width, (GLint)height);

	//Set projection matrix
	glMatrixMode( GL_PROJECTION );					
	glLoadIdentity();								
	glOrtho( 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f );

	//Set modelview matrix
	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();	
}

void OGLGraphicsWrapper::GetViewport( int& width, int& height )
{
	::GetViewport( width, height );
}

//////////////////////////////////////////////////////////////////////////
OGLGraphicsWrapper::OGLGraphicsWrapper()
{
}

OGLGraphicsWrapper::~OGLGraphicsWrapper()
{
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OGLGraphicsWrapper::TestGraphics()
{
	boost::shared_ptr<IPen> pen01( CreatePen( wxColor(255,0,0), 2 ) );
	boost::shared_ptr<IPen> pen02( CreatePen( wxColor(0,255,0), 5 ) );
	boost::shared_ptr<IPen> pen03( CreatePen( wxColor(0,0,0), 5 ) );
	boost::shared_ptr<IBrush> brush01( CreateBrush( wxColor( 255,255,0 ) ) );
#if 0
	Clear( wxColor( 0,0,255 ) );
	DrawLine( 0.0f, 0.0f, 1.0f, 1.0f );
	DrawLine( 0.0f, 1.0f, 1.0f, 0.0f );


	SetBrush( NULL );	
	SetPen( pen01.get() );
	DrawRect( 0.5f, 0.5f, 1.0f, 1.0f ); 

	SetBrush( brush01.get() );
	SetPen( pen02.get() );
	DrawRect( 0.0f, 0.0f, 0.5f, 0.5f );

	SetFont( NULL );
	SetPen( pen02.get() );
	DrawText( "Hello World!!!", 0.7f, 0.5f  );

#endif
}