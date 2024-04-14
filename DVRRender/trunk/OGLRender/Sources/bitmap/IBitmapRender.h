//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс отрисовки 2D-изображений

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_BITMAP_RENDER_9246983276475708_
#define _I_BITMAP_RENDER_9246983276475708_

#include "../../../IDVRGraphicsWrapper.h"

inline void GetViewport( int& width, int& height )
{
	//Retrieve old value
	int values[4];
	glGetIntegerv( GL_VIEWPORT, &values[0] );

	width = values[2];
	height = values[3];
}

inline unsigned char BytePerPixelFromFormat( GLenum format )
{
	switch(format)
	{
	case GL_ALPHA:
	case GL_LUMINANCE8:
		return 1;
	case GL_RGB:
	case GL_BGR_EXT:
		return 3;
	case GL_RGBA:
	case GL_BGRA_EXT:
		return 4;
	default:
		return 0;
	}
}

//======================================================================================//
//                                 class IBitmapRender                                  //
//======================================================================================//

//! \brief Интерфейс отрисовки 2D-изображений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class IBitmapRender
{
public:
	//IBitmapRender();
	virtual ~IBitmapRender() {};

	virtual bool Draw( const IDVRGraphicsWrapper::Rect& DrawArea, int bitmapWidth, int bitmapHeight, GLenum format, const void* data ) = 0;
	
	static boost::shared_ptr<IBitmapRender>	CreateBitmapRender();
};

#endif // _I_BITMAP_RENDER_9246983276475708_