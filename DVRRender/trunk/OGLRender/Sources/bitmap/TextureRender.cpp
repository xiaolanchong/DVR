//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отрисовки 2D-изображений в качестве текстуры полигона

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "TextureRender.h"


//! \brief интерфейс для привязки параметров текстуры размеров не степени 2
//! \version 1.0
//! \date 06-14-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class INonPowerTwoTextureBinder
{
	const GLenum	c_TargetTexture;

	GLenum	GetTargetTexture() const 
	{
		return c_TargetTexture;
	}
protected:
	INonPowerTwoTextureBinder( GLenum TargetTexture ) : 
		 c_TargetTexture(TargetTexture)
		 {
			 glEnable(GetTargetTexture());
		 }

		 struct TextureParam
		 {
			 int bitmapWidth;
			 int bitmapHeight; 
			 GLenum format; 
			 const void* data;

			 TextureParam( 
				 int _bitmapWidth,
				 int _bitmapHeight, 
				 GLenum _format, 
				 const void* _data
				 ):
			 bitmapWidth(_bitmapWidth),
				 bitmapHeight(_bitmapHeight),
				 format(_format), 
				 data(_data)
			 {
			 }
		 };

		 virtual const TextureParam&	ProcessTextureData( const TextureParam& param ) = 0;
public:
	virtual ~INonPowerTwoTextureBinder() 
	{
		glDisable( GetTargetTexture() );
	}

	void Bind( int TextureID, int bitmapWidth, int bitmapHeight, GLenum format, const void* data ) 
	{
		glBindTexture( GetTargetTexture(), TextureID );
#if 0
		static bool init = false;
		if( !init )
		{
			glTexImage2D( GetTargetTexture(), 0, bytesPerPixel, bitmapWidth,bitmapHeight,
				0, format, GL_UNSIGNED_BYTE, (GLvoid*)data );
			init = true;
		}
		else
		{
			glTexSubImage2D( GetTargetTexture(), 0, 0, 0, bitmapWidth, bitmapHeight,
				format, GL_UNSIGNED_BYTE, (GLvoid*)data );
		}
#else
		//HACK : bpp
		const TextureParam& tp = ProcessTextureData( TextureParam(bitmapWidth, bitmapHeight, format, data) );
		glTexImage2D( GetTargetTexture(), 0, 3/*HACK*/, tp.bitmapWidth, tp.bitmapHeight,
			0, tp.format, GL_UNSIGNED_BYTE, (GLvoid*)tp.data 
			);
#endif

		//Set bitmap rendering quality
		glTexParameteri(GetTargetTexture(), GL_TEXTURE_MIN_FILTER,GL_LINEAR );
		glTexParameteri(GetTargetTexture(), GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	}
};

//! \brief использует расширение GL_TEXTURE_RECTANGLE_ARB
//! \version 1.0
//! \date 06-14-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class	HardwareBinder : public INonPowerTwoTextureBinder
{
	static const GLenum OGL_NONRECT_EXTENSION = GL_TEXTURE_RECTANGLE_ARB;
public:
	HardwareBinder(  ) : INonPowerTwoTextureBinder( OGL_NONRECT_EXTENSION )  
	{

	}

	const TextureParam& ProcessTextureData( const TextureParam& tp )
	{
		return tp;
	}
};

//! \brief использует gluScaleImage (или другую ф-ю для ресайза), очень медленно!
//! \version 1.0
//! \date 06-14-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class	SoftwareBinder : public INonPowerTwoTextureBinder
{
	static std::vector<BYTE>	m_OuputData;	
	TextureParam		m_OuputInfo;
public:
	SoftwareBinder( ) : INonPowerTwoTextureBinder( GL_TEXTURE_2D ),
		m_OuputInfo( 256, 256, GL_BGR_EXT, 0)
	{
		m_OuputData.resize( m_OuputInfo.bitmapHeight * m_OuputInfo.bitmapWidth * 3, 0 );
		memset( &m_OuputData[0], 255, 256 * 64 * 3 );
		m_OuputInfo.data = &m_OuputData[0];
	}

	const TextureParam& ProcessTextureData( const TextureParam& input )
	{
		_ASSERTE(input.format == GL_BGR_EXT);
		int res = gluScaleImage(	
			input.format, input.bitmapWidth, input.bitmapHeight, GL_UNSIGNED_BYTE, input.data,
			m_OuputInfo.bitmapWidth, m_OuputInfo.bitmapHeight, GL_UNSIGNED_BYTE, &m_OuputData[0] );
		_ASSERTE(0 == res);
		return m_OuputInfo;
	}

};

std::vector<BYTE>	SoftwareBinder::m_OuputData;

//======================================================================================//
//                                 class TextureRender                                  //
//======================================================================================//

TextureRender::TextureRender()
{
	glGenTextures(1, &mTexture);
}

TextureRender::~TextureRender()
{
	glDeleteTextures( 1, &mTexture );
}

 bool TextureRender::Draw( const IDVRGraphicsWrapper::Rect& DrawArea, int bitmapWidth, int bitmapHeight, GLenum format, const void* data )
 {
	 float x		= DrawArea.x0;
	 float y		= DrawArea.y0;
	 float width	= DrawArea.x1 - DrawArea.x0;
	 float height	= DrawArea.y1 - DrawArea.y0;

	 std::auto_ptr<INonPowerTwoTextureBinder>	_Binder( new HardwareBinder/*SoftwareBinder*/() );
	 _Binder->Bind( mTexture, bitmapWidth, bitmapHeight, format, data  );

	 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	 glMatrixMode( GL_MODELVIEW );
	 glPushMatrix();
	 glTranslatef( x, y, 0.0f );

	 glColor3f( 1.0f, 1.0f, 1.0f );
/*	// only if software
	 bitmapWidth  = 1;
	 bitmapHeight = 1;
*/
	 //Draw textured quad
	 glBegin(GL_QUADS);
	 glTexCoord2f ( 0.0f, bitmapHeight );		glVertex3f( 0.0f, 0.0f, 0.0f );
	 glTexCoord2f ( bitmapWidth, bitmapHeight );	glVertex3f( width, 0.0f, 0.0f );
	 glTexCoord2f ( bitmapWidth, 0.0f );			glVertex3f( width, height, 0.0f );
	 glTexCoord2f ( 0.0f, 0.0f );				glVertex3f( 0.0f, height, 0.0f );
	 glEnd();

	 glPopMatrix();

	 return true;
 }