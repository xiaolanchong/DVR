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
#ifndef _TEXTURE_RENDER_4705578023757094_
#define _TEXTURE_RENDER_4705578023757094_

#include "IBitmapRender.h"

//======================================================================================//
//                                 class TextureRender                                  //
//======================================================================================//

//! \brief Отрисовки 2D-изображений в качестве текстуры полигона
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class TextureRender : public IBitmapRender
{
	GLuint mTexture;
public:
	TextureRender();
	virtual ~TextureRender();

	virtual bool Draw( const IDVRGraphicsWrapper::Rect& DrawArea, int bitmapWidth, int bitmapHeight, GLenum format, const void* data ) ;
};

#endif // _TEXTURE_RENDER_4705578023757094_