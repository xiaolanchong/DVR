//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Посредник сервер-клиент при передачи кадров

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.04.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "LocalTier.h"
#include "../../FrmBuffer.h"

#include <boost/cstdint.hpp>
#include <boost/crc.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mem_fn.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>

const DWORD c_InputFourCC =  mmioFOURCC('Y', 'U', 'Y', '2' );

//! класс для переопределения AllocateFrameBuffer
struct StubAllocator : CFrmBuffer::Allocator 
{
	virtual long AllocatorSourceID() 
	{
		const long AnyNumberWhichYourCanImagine = 1000121;
		return AnyNumberWhichYourCanImagine;
	}

	StubAllocator() : CFrmBuffer::Allocator(3)
	{
	}

	virtual CFrmBuffer* AllocateFrameBuffer()
	{
		return CFrmBuffer::Allocator::AllocateFrameBuffer();
	}
};

//! класс декодирования для каждого потока
class StreamFrameStorage : boost::noncopyable
{
	//! декодер изображения YUY2->RGB24
	CVidDecoder						m_Decoder;
	//! текущий кадр в RGB24
	boost::shared_ptr<CHCS::IFrame>	m_pFrame;
	//! аллокатор
	StubAllocator					m_Allocator;
	//! синхронизация доступа к кадру
	Elvees::CCriticalSection		m_Sync;
public:
	StreamFrameStorage()
	{
		int a = 1;
	}

	~StreamFrameStorage()
	{
		// free frame before allocator call FlushFrames()
		m_pFrame.reset();
	}

	  //! создать новый декодированный кадр
	  //! \param pFrame исходный кадр в формате YUY2
	void	CreateFrame( CHCS::IFrame* pFrame )
	{
		_ASSERTE(pFrame);
		Elvees::CCriticalSection::Owner _lock(m_Sync);
		BITMAPINFO biFormat;
		long nFormatSize = pFrame->GetFormat(&biFormat);
		const void* pData = pFrame->GetBytes();
		_ASSERTE( !nFormatSize  );
		_ASSERTE( biFormat.bmiHeader.biCompression == c_InputFourCC );

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

			res = m_Allocator.SetFrameAllocatorFormat(&biOutput);
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

		CFrmBuffer* pNewFrame = m_Allocator.AllocateFrameBuffer();
		_ASSERTE(pNewFrame);
		pNewFrame->SetFrameTime( 0, pFrame->GetTime() );

		BITMAPINFO biOutput = { 0 };
		m_Decoder.GetOutFormat(&biOutput);
		pNewFrame->SetFrameData( reinterpret_cast<BYTE*>( pOutputData ), biOutput.bmiHeader.biSizeImage );

		m_pFrame = boost::shared_ptr<CHCS::IFrame>( pNewFrame, boost::mem_fn( &CHCS::IFrame::Release ) );
	}

	CHCS::IFrame* GetFrame() 
	{
		Elvees::CCriticalSection::Owner _lock(m_Sync);
		m_pFrame->AddRef();
		return m_pFrame.get();
	}
};

//======================================================================================//
//                                   class LocalTier                                    //
//======================================================================================//

//! реализация класса передачи
class LocalTier :	public ILocalTier
{
	//!для синхронизации доступа к словарю
	Elvees::CCriticalSection	m_Sync;
	typedef std::map<int, boost::shared_ptr<StreamFrameStorage> >	StreamMap_t;	
	//! для каждого потока здесь хранится информация
	StreamMap_t															m_StreamMap;
public:

	LocalTier();
private:
	void			SetFrame( CHCS::IFrame* pFrame );
	CHCS::IFrame*	GetFrame( int StreamID );
	void			FlushFrames();
};

LocalTier::LocalTier()
{
}

void	LocalTier::SetFrame( CHCS::IFrame* pFrame )
{
	if(!pFrame )
	{
		return;
	}

	const int nStreamID = pFrame->GetSourceID();

	boost::shared_ptr<StreamFrameStorage> si;
	{
	Elvees::CCriticalSection::Owner _lock( m_Sync );

	StreamMap_t::iterator it = m_StreamMap.find(nStreamID);
	if( it == m_StreamMap.end() )
	{
		StreamFrameStorage* p = new StreamFrameStorage();
		si = boost::shared_ptr<StreamFrameStorage>(p);
		m_StreamMap.insert( std::make_pair( nStreamID, si ) );
	}
	else
	{
		si = it->second;
	}
	}
	si->CreateFrame(pFrame);
}

CHCS::IFrame*	LocalTier::GetFrame( int nStreamID )
{
	Elvees::CCriticalSection::Owner _lock( m_Sync );

	StreamMap_t::iterator it = m_StreamMap.find(nStreamID);
	if( it == m_StreamMap.end() || !it->second.get() )
	{
		return 0;
	}

	return it->second->GetFrame();
	return 0;
}

void	LocalTier::FlushFrames()
{
	Elvees::CCriticalSection::Owner _lock( m_Sync );
	m_StreamMap.clear();
}

//////////////////////////////////////////////////////////////////////////

ILocalTier* GetLocalTier()
{
	static LocalTier lt;
	return &lt;
}
