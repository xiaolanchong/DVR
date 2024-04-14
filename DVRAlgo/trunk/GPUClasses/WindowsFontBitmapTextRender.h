//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Использует wglUseFontBitmaps для отображения текста

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   24.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _WINDOWS_FONT_BITMAP_TEXT_RENDER_4214658625771893_
#define _WINDOWS_FONT_BITMAP_TEXT_RENDER_4214658625771893_

#include "ITextRender.h"

//======================================================================================//
//                          class WindowsFontBitmapTextRender                           //
//======================================================================================//

//! \brief Использует wglUseFontBitmaps для отображения текста
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class WindowsFontBitmapTextRender : public ITextRender
{
	HDC				m_hMemDC;
	HBITMAP		m_hMemBitmap;
	HFONT			m_hTextFont;

	static const int	c_ListName = 1000;
public:
	WindowsFontBitmapTextRender(const std::string& sFontName, size_t nFontHeight);
	virtual ~WindowsFontBitmapTextRender();

	virtual SIZE	GetTextExtent( const char* szText ) ; 

	virtual bool	DrawText(  float x, float y, float width, float height, const char* szText, const RGBQUAD& clText );
};

#endif // _WINDOWS_FONT_BITMAP_TEXT_RENDER_4214658625771893_