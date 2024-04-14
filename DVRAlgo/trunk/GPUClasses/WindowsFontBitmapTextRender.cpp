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
/*
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nFontHeight;
	tcsncpy_s(logFont.lfFaceName, _countof(logFont.lfFaceName), sFontName.c_str(), _TRUNCATE);

	m_hTextFont = CreatePointFontIndirect(&logFont, pDC);*/
	// make the system font the device context's selected font 
	//SelectObject (m_hMemDC, GetStockObject (SYSTEM_FONT)); 
	m_hTextFont = CreateFontA(
		nFontHeight,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		sFontName.c_str());                 // lpszFacename
	_ASSERTE(m_hTextFont);

	SelectObject (m_hMemDC, m_hTextFont); 

	// create the bitmap display lists 
	// we're making images of glyphs 0 thru 255 
	// the display list numbering starts at 1000, an arbitrary choice 
	wglUseFontBitmaps (m_hMemDC, 0, 256, c_ListName); 
}

WindowsFontBitmapTextRender::~WindowsFontBitmapTextRender()
{
	DeleteObject( m_hTextFont ); 
	DeleteObject( m_hMemBitmap );
	DeleteDC( m_hMemDC );
}

SIZE	WindowsFontBitmapTextRender::GetTextExtent( const char* szText )
{
	SIZE sizeText;
	GetTextExtentPoint32A( m_hMemDC, szText, static_cast<int>( strlen(szText) ), &sizeText);
	return sizeText;
}

bool	WindowsFontBitmapTextRender::DrawText(  
			float x, float y, 
			float width, float height, 
			const char* szText, 
			const RGBQUAD& clText
 )
{
	glColor4ub( clText.rgbRed, clText.rgbGreen, clText.rgbBlue, clText.rgbReserved );

	// начало в левой нижней точке
	glRasterPos2f( x, y );

	// display a string: 
	// indicate start of glyph display lists 
	glListBase (c_ListName); 
	// now draw the characters in a string 
	glCallLists ( static_cast<int>(strlen(szText)), GL_UNSIGNED_BYTE, szText );

	return true;
}