//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отрисовки 2D-изображений как растрового изображения

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "RasterRender.h"

//======================================================================================//
//                                 class RasterRender                                   //
//======================================================================================//

RasterRender::RasterRender()
{
}

RasterRender::~RasterRender()
{
}

template<void (__stdcall* t)( GLenum, GLint )> class GLStateSaver
{
	GLint	m_OldValue;
	GLenum	m_Name;
public:
	GLStateSaver( GLenum Name ):
		m_Name(Name)
	{
		glGetIntegerv( Name, &m_OldValue );
	}
	~GLStateSaver()
	{
		t( m_Name, m_OldValue );
	}
	void operator () ( GLint  NewValue)
	{
		t( m_Name, NewValue );
	}
};

bool RasterRender::Draw( const IDVRGraphicsWrapper::Rect& DrawArea, int bitmapWidth, int bitmapHeight, GLenum format, const void* data )
{
	const float c_Bottom = 1.0f;
	float x		= DrawArea.x0;
	float y		= DrawArea.y0;
	float width	= DrawArea.x1 - DrawArea.x0;
	float height	= DrawArea.y1 - DrawArea.y0;

	int ww, wh;
	GetViewport( ww, wh );

	float OriginX = x;
	float OriginY = y + height;

	int OriginXInt = -static_cast<int>( OriginX * ww );
	int OriginYInt = -static_cast<int>( (c_Bottom - OriginY) * wh );

	GLStateSaver<&glPixelStorei> stLength( GL_UNPACK_ROW_LENGTH );
	GLStateSaver<&glPixelStorei> stAlignment( GL_UNPACK_ALIGNMENT );
	stAlignment( 1 );

	int TrueWidth	= bitmapWidth;
	int TrueHeight	= bitmapHeight;
	if( OriginXInt > 0)
	{
		const BYTE* pByteData = reinterpret_cast<const BYTE*>( data );
	//	pByteData += OriginXInt * BytePerPixelFromFormat(format);
		data = pByteData;
		OriginX = 0;
		stLength( bitmapWidth ) ;
		int BitmapOffset = bitmapWidth * OriginXInt/ (width * ww);
		TrueWidth = bitmapWidth - BitmapOffset;
	}

	if( OriginYInt > 0 )
	{
		int BitmapOffset = bitmapHeight * OriginYInt/ (height * wh);
		const BYTE* pByteData = reinterpret_cast<const BYTE*>( data );
		if( bitmapHeight > OriginYInt )
		{
			pByteData += bitmapWidth * BytePerPixelFromFormat(format) * BitmapOffset;
		}
		data = pByteData;
		OriginY = c_Bottom;
		
		TrueHeight = bitmapHeight - BitmapOffset;
	}

	glPixelZoom( (ww * width )/bitmapWidth, (wh * height )/bitmapHeight );
	glRasterPos2f( OriginX, OriginY );
	glDrawPixels( TrueWidth, TrueHeight, format , GL_UNSIGNED_BYTE, data );
	glPixelZoom(1.0f, 1.0f);

//	glPixelStorei( GL_UNPACK_ROW_LENGTH )
	return true;
}