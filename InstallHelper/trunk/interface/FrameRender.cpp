//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс отображения сжатых кадров от IFrame в контекст

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   31.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "FrameRender.h"

//======================================================================================//
//                                  class FrameRender                                   //
//======================================================================================//

FrameRender::FrameRender()
{
}

FrameRender::~FrameRender()
{
}

bool	FrameRender::InitDecoder(CHCS::IFrame* pFrame)
{
	memset( &m_biInput, 0, sizeof(BITMAPINFO ) );
	memset( &m_biOutput, 0, sizeof(BITMAPINFO ) );
	pFrame->GetFormat(&m_biInput);
	

//	BITMAPINFO biOutput = { 0 };
	BITMAPINFOHEADER& bihOutput = m_biOutput.bmiHeader;

	bihOutput.biSize		= sizeof(BITMAPINFOHEADER);
	bihOutput.biBitCount	= 24;
	bihOutput.biPlanes		= 1;
	bihOutput.biCompression = BI_RGB;
	bihOutput.biWidth		= m_biInput.bmiHeader.biWidth;
	bihOutput.biHeight		= m_biInput.bmiHeader.biHeight;
	bihOutput.biSizeImage	= DIBSIZE(bihOutput  );

	bool res = m_Decoder.InitDecompressor(&m_biInput, &m_biOutput);
	_ASSERTE(res);

	return res;
}

namespace Elvees
{
	enum TOutput;
	void __cdecl Output(TOutput type, LPCTSTR stText)
	{
	}
}

bool	FrameRender::DrawFrame( HDC hDC, const RECT& rcDrawArea, CHCS::IFrame* pFrame ) 
{
	BITMAPINFO biInput;
	pFrame->GetFormat(&biInput);
	if( (	
			!m_Decoder.IsDecompressionInited()	||
			!memcmp( &m_biInput, &biInput, sizeof(BITMAPINFO) ) 
		)						&& 
		!InitDecoder(pFrame) )
	{
		return false;
	}
	const void* pFrameData = pFrame->GetBytes();

	const void *pData = m_Decoder.DecompressData( pFrameData, m_biInput.bmiHeader.biSizeImage, true ); 
	/*
	HDC h = GetDC( GetDesktopWindow() );
	StretchDIBits(	
		hDC, 
		rcDrawArea.left, rcDrawArea.top, rcDrawArea.right - rcDrawArea.left, rcDrawArea.bottom - rcDrawArea.top,
		0, 0, m_biOutput.bmiHeader.biWidth,  m_biOutput.bmiHeader.biHeight,
		pData, &m_biOutput, DIB_RGB_COLORS, SRCCOPY
		)  ;
	ReleaseDC( GetDesktopWindow(), h );*/
	SetStretchBltMode(hDC, HALFTONE);

	return m_biOutput.bmiHeader.biHeight ==
				StretchDIBits(	
					hDC, 
					rcDrawArea.left, rcDrawArea.top, rcDrawArea.right - rcDrawArea.left, rcDrawArea.bottom - rcDrawArea.top,
					0, 0, m_biOutput.bmiHeader.biWidth,  m_biOutput.bmiHeader.biHeight,
					pData, &m_biOutput, DIB_RGB_COLORS, SRCCOPY
					)  ;
}