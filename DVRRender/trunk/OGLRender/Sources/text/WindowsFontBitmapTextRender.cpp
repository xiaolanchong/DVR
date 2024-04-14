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

#include "stdafx.h"
#include "WindowsFontBitmapTextRender.h"

//======================================================================================//
//                          class WindowsFontBitmapTextRender                           //
//======================================================================================//

WindowsFontBitmapTextRender::WindowsFontBitmapTextRender(const std::string& sFontName, size_t nFontHeight)
{
	HWND	hDesktopWnd =	GetDesktopWindow();
	HDC		hDesktopDC	=	GetDC( hDesktopWnd ); 
	m_hMemBitmap = CreateCompatibleBitmap( hDesktopDC, 64, 64 );
	m_hMemDC	 = CreateCompatibleDC( hDesktopDC );
	_ASSERTE(m_hMemBitmap);
	_ASSERTE(m_hMemDC);
	ReleaseDC( hDesktopWnd, hDesktopDC );

//	HFONT hFont = CreateFont(
//		);

	// make the system font the device context's selected font 
	SelectObject (m_hMemDC, GetStockObject (SYSTEM_FONT)); 

	// create the bitmap display lists 
	// we're making images of glyphs 0 thru 255 
	// the display list numbering starts at 1000, an arbitrary choice 
	wglUseFontBitmaps (m_hMemDC, 0, 256, c_ListName); 
}

WindowsFontBitmapTextRender::~WindowsFontBitmapTextRender()
{
	DeleteDC( m_hMemDC );
	DeleteObject( m_hMemBitmap );
}

wxSize	WindowsFontBitmapTextRender::GetTextExtent( const char* szText )
{
	SIZE sizeText;
	GetTextExtentPoint32( m_hMemDC, szText, static_cast<int>( strlen(szText) ), &sizeText);
	return wxSize( sizeText.cx, sizeText.cy );
}

bool	WindowsFontBitmapTextRender::DrawText(  float x, float y, float width, float height, const char* szText, const wxColour& clText )
{
	glColor3ub( clText.Red(), clText.Green(), clText.Blue() );

	// начало в левой нижней точке
	glRasterPos2f( x, y + height );

	// display a string: 
	// indicate start of glyph display lists 
	glListBase (c_ListName); 
	// now draw the characters in a string 
	glCallLists ( static_cast<int>(strlen(szText)), GL_UNSIGNED_BYTE, szText );

	return true;
}