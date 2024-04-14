//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Фильтр декомпрессора потока сжатых изображений
//	+клиентский поток

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "DecompressFilter.h"

//======================================================================================//
//                                class DecompressFilter                                //
//======================================================================================//

DecompressFilter::DecompressFilter(long StreamID):
	m_StreamID(StreamID),
	CFrmBuffer::Allocator(5)
{
	m_pFrameLast = 0;
	m_LastFrameTime = 0;
}


DecompressFilter::~DecompressFilter()
{
	if(m_pFrameLast)
		m_pFrameLast->Release();
}

long DecompressFilter::AddRef()
{
	// no external reference control
	return 1;
}

long DecompressFilter::Release()
{
	// no external reference control
	return 1;
}

bool DecompressFilter::Start()
{
	return true;
}

void DecompressFilter::Stop() 
{

}

bool DecompressFilter::IsStarted()
{
	// always ready to decompress
	return true;
}

long DecompressFilter::SeekTo(INT64 timePos)
{
	return 0;
}

void	DecompressFilter::PushImage( CHCS::IFrame* pFrame, bool, size_t )
{
	// not reentrant ( XVID, allocation & others )
	Elvees::CCriticalSection::Owner lock(m_ReentranceSection);
	_ASSERTE( m_LastFrameTime < pFrame->GetRelativeTime() ) ;
	if( pFrame->GetSourceID() != m_StreamID )
	{
		// ошибка - пакет для другого потока
		_ASSERTE(false);
		return;
	}
	m_LastFrameTime = pFrame->GetRelativeTime();
	BITMAPINFO biFormat;
	long nFormatSize = pFrame->GetFormat(&biFormat);
	const void* pData = pFrame->GetBytes();
	_ASSERTE( !nFormatSize  );
//	_ASSERTE( biFormat.bmiHeader.biCompression == c_InputFourCC );

	//! инициализируем декодер, этого нельзя сделать в конструкторе - мы не знаем формата
	//! надеемя также на постоянный формат, иное было бы странно
	//! выходной формат RGB24 с сохранением ширины
	if( !m_Decoder.IsDecompressionInited() )
	{
		BITMAPINFO biOutput = { 0 };
		BITMAPINFOHEADER& bihOutput = biOutput.bmiHeader;

		bihOutput.biSize		= sizeof(BITMAPINFOHEADER);
		bihOutput.biBitCount	= 24;
		bihOutput.biPlanes		= 1;
		bihOutput.biCompression = BI_RGB;
		bihOutput.biWidth		= biFormat.bmiHeader.biWidth;
		bihOutput.biHeight		= biFormat.bmiHeader.biHeight;
		bihOutput.biSizeImage	= DIBSIZE(bihOutput  );

		bool res = m_Decoder.InitDecompressor(&biFormat, &biOutput);
		_ASSERTE(res);

		if(!res)
		{
			return;
		}

		res = SetFrameAllocatorFormat(&biOutput);
		_ASSERTE(res);

		if(!res)
		{
			return;
		}
	}
	void* pOutputData = m_Decoder.DecompressData( pData, biFormat.bmiHeader.biSizeImage, unsigned(0) );
	_ASSERTE(pOutputData);

	if(!pOutputData)
	{
		return;
	}

	CFrmBuffer* pNewFrame = AllocateFrameBuffer();
	_ASSERTE(pNewFrame);
	pNewFrame->SetFrameTime( 0, pFrame->GetTime() );

	BITMAPINFO biOutput = { 0 };
	m_Decoder.GetOutFormat(&biOutput);
	pNewFrame->SetFrameData( reinterpret_cast<BYTE*>( pOutputData ), biOutput.bmiHeader.biSizeImage );

	std::vector<unsigned char> Buf;
	GetFrameUserData( pFrame, Buf );
	pNewFrame->SetCustomData(  Buf.size() ? &Buf[0] : 0, Buf.size() );

	SetLastFrame(pNewFrame);
}

void DecompressFilter::SetLastFrame(CHCS::IFrame* pFrame)
{
	if(!pFrame)
		return;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
		m_pFrameLast->Release();

	m_pFrameLast = pFrame;
}

bool DecompressFilter::GetLastFrame(CHCS::IFrame** ppFrame)
{
	if(!ppFrame)
		return false;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
	{
		m_pFrameLast->AddRef();
		*ppFrame = m_pFrameLast;
		return true;
	}

	return false;
}