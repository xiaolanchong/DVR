//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка для видеоридера сервера

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   21.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerFakeReader.h"
#include "../PlatSleep.h"
//======================================================================================//
//                                class ServerFakeReader                                //
//======================================================================================//

ServerFakeReader::ServerFakeReader():
	m_nFrameNumber(0)
{
	//! создать заголовок
	memset( & m_BmpHdr, 0, sizeof(BITMAPINFOHEADER) );
	m_BmpHdr.biWidth		= 720;
	m_BmpHdr.biHeight		= 576;
	m_BmpHdr.biPlanes		= 1;
	m_BmpHdr.biSize			= sizeof(BITMAPINFOHEADER);
	m_BmpHdr.biBitCount		= 16;
	m_BmpHdr.biCompression	= alib::MakeFourCC('Y', 'U', 'Y', '2');
	//! не учитывем выравние, забивку
	m_BmpHdr.biSizeImage= m_BmpHdr.biWidth * m_BmpHdr.biHeight * m_BmpHdr.biBitCount/8;
	//! заполняем все 0x80
	m_ByteBuffer.resize( m_BmpHdr.biSizeImage, 0x80 );
}

ServerFakeReader::~ServerFakeReader()
{
}

const BITMAPINFOHEADER &	ServerFakeReader::GetHeader() const
{
	return m_BmpHdr;
}

bool	ServerFakeReader::Read( const ubyte *& pBytes, ulong & time )
{
	pBytes	= &m_ByteBuffer[0];
	time	= m_nFrameNumber++;

	//! не обязательно, эмулируем занятость
	PlatSleep(40); //25 fps
	return true;
}