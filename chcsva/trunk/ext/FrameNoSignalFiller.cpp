//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заполнитель кадров картинкой, если нет сигнала на входе платы захвата

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "FrameNoSignalFiller.h"
#include "../Resource.h"
//======================================================================================//
//                              class FrameNoSignalFiller                               //
//======================================================================================//

FrameNoSignalFiller::FrameNoSignalFiller( unsigned int ImageWidth, unsigned int ImageHeight ):
	m_ImageWidth(ImageWidth),
	m_ImageHeight(ImageHeight)
{
	CreateContext( ImageWidth, ImageHeight );
	ClearImage( m_CacheContext,  ImageWidth, ImageHeight );
	DrawNoSignalIcon( m_CacheContext,  ImageWidth, ImageHeight ); 

	std::vector<BYTE> TempImage;
	CopyBitmapToMemory( TempImage );
	ConvertRGBtoYUY2( TempImage, m_ImageData, ImageWidth, ImageHeight );
}

FrameNoSignalFiller::~FrameNoSignalFiller()
{
	DeleteObject( m_CacheBmp );
	DeleteObject( m_CacheContext );
}

void	FrameNoSignalFiller::CreateContext(unsigned int ImageWidth, unsigned int ImageHeight)
{

	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC	 = GetDC( hDesktopWnd );
	m_CacheContext  = CreateCompatibleDC( hDesktopDC );
	_ASSERT( m_CacheContext );

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof( BITMAPINFO ) );
	CreateHeader( bi.bmiHeader, ImageWidth, ImageHeight );

	m_CacheBmp = CreateDIBSection( hDesktopDC, &bi, DIB_RGB_COLORS, NULL, NULL, 0 );
	SelectObject( m_CacheContext, m_CacheBmp );
	_ASSERT( m_CacheBmp );

	ReleaseDC( hDesktopWnd, hDesktopDC );
}

void FrameNoSignalFiller::CreateHeader( BITMAPINFOHEADER& bih, unsigned int Width, unsigned int Height )
{
	ZeroMemory(&bih, sizeof( BITMAPINFOHEADER ) );

	bih.biBitCount	= 24;
	bih.biPlanes	= 1;
	bih.biWidth		= Width;
	bih.biHeight	= Height;
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biCompression = BI_RGB;
	bih.biSizeImage	= DIBSIZE(bih);
}

void	FrameNoSignalFiller::ClearImage( HDC hDC, unsigned int Width, unsigned int Height )
{
	RECT rc = 
	{
		0, 0, Width, Height
	};
	//! fast filling, from CDC::FillSolidRect
	SetBkColor( hDC, RGB(0, 0, 128)  ); 
	ExtTextOut( hDC, 0, 0, ETO_OPAQUE,  &rc, 0, 0, 0 );

}

void FrameNoSignalFiller::DrawNoSignalIcon( HDC hDC, unsigned int ImageWidth, unsigned int ImageHeight)
{
	HINSTANCE hThisModule = _Module.GetResourceInstance();
	const int DesiredWidth	= 32;
	const int DesiredHeight = 32;
	HICON hIcon = (HICON)::LoadImage( hThisModule, MAKEINTRESOURCE(IDI_NOSIGNAL), IMAGE_ICON, DesiredWidth, DesiredHeight, 0 ); 
	if( hIcon )
	{
		::DrawIcon( hDC, ImageWidth - DesiredWidth - 5, 5, hIcon );
	}
}

void		FrameNoSignalFiller::CopyBitmapToMemory(std::vector<BYTE>& ImageData) const
{	
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof( BITMAPINFO ) );
	CreateHeader( bi.bmiHeader, m_ImageWidth, m_ImageHeight);

	ImageData.resize( bi.bmiHeader.biSizeImage );

	int res = GetDIBits( m_CacheContext, m_CacheBmp, 0, m_ImageHeight, &ImageData[0], &bi, DIB_RGB_COLORS );

	_ASSERT(res);
}

struct yuv
{
	BYTE y, u, v;
};

yuv RGBtoYUV( BYTE R, BYTE G, BYTE B )
{
	double Y =  (0.257 * R) + (0.504 * G) + (0.098 * B) + 16;
	double V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128;
	double U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128;

	yuv out;
	out.y = static_cast<BYTE>(Y);
	out.u = static_cast<BYTE>(U);
	out.v = static_cast<BYTE>(V);

	return out;
}

void	FrameNoSignalFiller::ConvertRGBtoYUY2( 
						const std::vector<BYTE>& SrcImage, std::vector<BYTE>& DstImage, 
						unsigned int ImageWidth, unsigned int ImageHeight )
{
	if( SrcImage.empty() )
	{
		return;
	}

	BITMAPINFOHEADER bihInput = {0};
	BITMAPINFOHEADER bihOutput = {0};
	CreateHeader( bihInput, ImageWidth, ImageHeight );

	bihOutput.biBitCount	= 16;
	bihOutput.biPlanes		= 1;
	bihOutput.biWidth		= ImageWidth;
	bihOutput.biHeight		= ImageHeight;
	bihOutput.biSize		= sizeof(BITMAPINFOHEADER);
	bihOutput.biCompression = mmioFOURCC( 'Y', 'U', 'Y', '2' ) ;
	bihOutput.biSizeImage	= DIBSIZE(bihOutput);

	unsigned InPitch	= DIBWIDTHBYTES(bihInput);
	unsigned OutPitch	= DIBWIDTHBYTES(bihOutput);

	DstImage.resize( bihOutput.biSizeImage , 0x80);

	for ( size_t y = 0; y < ImageHeight; ++y )
	{
		const BYTE* pSrcLine = &SrcImage[0] + InPitch * (ImageHeight - 1 - y);
		BYTE*		pDstLine = &DstImage[0] + OutPitch * y;
		bool bUseU = true;
		for ( size_t x = 0 ; x < ImageWidth; ++x , bUseU = !bUseU )
		{
			yuv px = RGBtoYUV( pSrcLine[3* x + 2], pSrcLine[3* x + 1], pSrcLine[3* x + 0] );
			BYTE Chroma = bUseU ? px.u : px.v;
			pDstLine[ 2*x + 0 ] = px.y;
			pDstLine[ 2*x + 1 ] = Chroma;
		}
	}
	
}