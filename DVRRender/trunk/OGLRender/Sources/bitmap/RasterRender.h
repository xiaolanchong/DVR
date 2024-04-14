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
#ifndef _RASTER_RENDER_1068827063921796_
#define _RASTER_RENDER_1068827063921796_

#include "IBitmapRender.h"

//======================================================================================//
//                                 class RasterRender                                   //
//======================================================================================//

//! \brief Отрисовки 2D-изображений как растрового изображения
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class RasterRender : public IBitmapRender
{
public:
	RasterRender();
	virtual ~RasterRender();

	virtual bool Draw( const IDVRGraphicsWrapper::Rect& DrawArea, int bitmapWidth, int bitmapHeight, GLenum format, const void* data ) ;
};

#endif // _RASTER_RENDER_1068827063921796_