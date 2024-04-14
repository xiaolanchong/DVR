// StreamDevice.cpp: implementation of the CStreamDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamDevice.h"
#include "NetProtocol.h"

const size_t c_nShrinkFactor = 2;

const bool	 c_bWriteArchive		= true;
/*
FrameNoSignalFiller::FrameNoSignalFiller(unsigned int ImageWidth, unsigned int ImageHeight)
{
	const unsigned int BytesInYUY2 = 2;
	const BYTE YValue =  32;
	const BYTE UValue = 112;
	const BYTE VValue = 192;
	m_NoSignalImage.resize( ImageWidth * ImageHeight * BytesInYUY2, YValue );
	for ( size_t i =0; i < m_NoSignalImage.size() && (i + 2 * BytesInYUY2 - 1) < m_NoSignalImage.size(); i += 2 * BytesInYUY2 )
	{
		m_NoSignalImage[i + 1] = UValue;
		m_NoSignalImage[i + 3] = VValue;
	}
}

void FrameNoSignalFiller::Fill(CFrmBuffer* pFrmBuffer ) const
{
	const std::vector<BYTE>& Image = m_
	pFrmBuffer->SetFrameData( &m_NoSignalImage[0], static_cast<unsigned>( m_NoSignalImage.size() ));
}
*/
//////////////////////////////////////////////////////////////////////
// CDeviceStream
//////////////////////////////////////////////////////////////////////

CDeviceStream* CDeviceStream::CreateDevice(
	const UUID& streamID,
	LPCTSTR stMoniker,
	long lWidth,
	long lHeight,
	long lFrmRate,
	long lInput,
	long lVStandard,
	bool bBWMode)
{
	CDeviceStream* pDevice = new CDeviceStream( streamID, stMoniker);

	if(pDevice)
	{
		pDevice->InitDevice(stMoniker, lWidth, lHeight, lFrmRate, lInput, lVStandard, bBWMode);
#if OLD
		// Store for new device detection
		lstrcpy(pDevice->m_pin1->m_stMoniker, stMoniker);
		lstrcpy(pDevice->m_pin2->m_stMoniker, stMoniker);
		lstrcpy(pDevice->m_pin3->m_stMoniker, stMoniker);
		lstrcpy(pDevice->m_pin4->m_stMoniker, stMoniker);
#endif
		pDevice->StartDevice(stMoniker);

	}
	else
	{
		Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
	}

	return pDevice;
}

CDeviceStream::CDeviceStream( const UUID& streamUID, LPCTSTR stMoniker)
	: m_pin1( new CDevicePin( streamUID, this, 0 ))
	, m_pin2( new CDevicePin( streamUID, this, 1) )
	, m_pin3( new CDevicePin( streamUID, this, 2) )
	, m_pin4( new CDevicePin( streamUID, this, 3) )
	, m_bUseTechwellDriverHack(false)
	, m_StreamUID(streamUID)
	, m_stMoniker(stMoniker)
{
	m_curpin = 0;
}

CDeviceStream::~CDeviceStream()
{
//	StopDevice();
}

bool CDeviceStream::OnImageFormat(BITMAPINFOHEADER* pbiImg)
{
	m_pin1->OnImageFormat(pbiImg);
	m_pin2->OnImageFormat(pbiImg);
	m_pin3->OnImageFormat(pbiImg);
	m_pin4->OnImageFormat(pbiImg);

	return true;
}

int ClampPinNumber(int pin)
{
	return (pin > 3) ?  0 : pin;
}

bool CheckDevice( const CDeviceStream* pThis )
{
	return	!IsBadReadPtr( pThis, sizeof( CDeviceStream ) )	;
}

bool CheckPin( const boost::shared_ptr<CDevicePin>& pPin, const CDeviceStream* pThis )
{
	return	CheckDevice( pThis )	&&
			pPin.get()											&& 
			!IsBadReadPtr( pPin.get(), sizeof( CDevicePin ) ) ;
}

void CDeviceStream::OnBufferCB(double fTime, BYTE *pData, long lLen)
{
	if( IsBadReadPtr( this, sizeof( CDeviceStream ) ) )
	{
		return;
	}

	if( m_bUseTechwellDriverHack) 
	{
		if ( pData[0] == m_curpin ) 
		{
			boost::shared_ptr<CDevicePin> pPin = GetPin(m_curpin);
			if( CheckPin(pPin, this) )
				pPin->OnBufferCB(fTime, pData, lLen);
		}
		else
		{
			return;
		}
	} 
	else 
	{
		boost::shared_ptr<CDevicePin> pPin = GetPin(m_curpin);
		if( CheckPin(pPin, this) )
			pPin->OnBufferCB(fTime, pData, lLen);
	}


	// Route to next enabled pin
	int pin = m_curpin + 1;

	while(CheckDevice(this) &&  pin != m_curpin)
	{
		pin = ClampPinNumber(pin);

		boost::shared_ptr<CDevicePin> pPin = GetPin(pin);
		if( CheckPin(pPin, this) && pPin->IsEnabled())
		{
			RouteToPin(pin, true);
			m_curpin = pin;
			break;
		}

		pin++;
		pin = ClampPinNumber(pin);
	}
}

void CDeviceStream::StopDevice()
{
	CDevice::StopDevice();
	CloseInterfaces();
}

//////////////////////////////////////////////////////////////////////
// CDevicePin
//////////////////////////////////////////////////////////////////////

CDevicePin::CDevicePin( const UUID& streamUID, CDeviceStream* pDevice, long lPin)
	: CFrmBuffer::Allocator(5)
	, m_pDevice(pDevice), 
	m_lPin(lPin),
	m_bEnabled(true)
	, m_newImage(false)
	//, m_StreamUID(streamUID)
{
	// Settings
#ifdef JUST_FOR_FPS_TEST
	m_PrevTime = timeGetTime();
	m_NonEmptyFrameCounter	= 0;
	m_TotalFrameCounter		= 0;
#endif


#ifdef ENABLE_RESIZE
	m_pFrameOut = NULL;
#endif
}

CDevicePin::~CDevicePin()
{

#ifdef ENABLE_RESIZE
	if(m_pFrameOut)
		free(m_pFrameOut);
#endif

}

void CDevicePin::Delete()
{
	m_bEnabled = false;
}

void	CDevicePin::FillEmptyFrame( CFrmBuffer* pFrame )
{
	if( !m_pFrameFiller.get() )
	{
		return;
	}

	const std::vector<BYTE>& Image = m_pFrameFiller->GetImage();
		pFrame->SetFrameData( &Image[0], static_cast<unsigned>( Image.size() ));
}

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////
/*
bool CStreamDevice::IsValid()
{
	return m_pDevice->IsDeviceValid();
}

bool CStreamDevice::IsDeviceString(LPCTSTR stDevice)
{
	return lstrcmpi(m_stMoniker, stDevice) == 0;
}*/

//////////////////////////////////////////////////////////////////////
// Allocator ID request
//////////////////////////////////////////////////////////////////////

long CDevicePin::AllocatorSourceID()
{
	return GetUID().Data1; // CStream::GetID()
}

//////////////////////////////////////////////////////////////////////
// Accept stream format from CDevice
//////////////////////////////////////////////////////////////////////

bool CDevicePin::OnImageFormat(BITMAPINFOHEADER* pbiImg)
{
	if(!pbiImg || IsBadReadPtr(pbiImg, sizeof(BITMAPINFOHEADER)))
		return false;

	// Capture only YUY2 images
	if(pbiImg->biCompression != mmioFOURCC('Y','U','Y','2'))
		return false;

#ifdef ENABLE_RESIZE	
	// Normalize/Resize image
	ZeroMemory(&m_bihOut, sizeof(m_bihOut));
	m_bihOut.biSize = sizeof(BITMAPINFOHEADER);
	m_bihOut.biPlanes = 1;
	m_bihOut.biCompression = mmioFOURCC('Y','V','1','2');
	m_bihOut.biBitCount = 12;
	m_bihOut.biWidth  = pbiImg->biWidth / c_nShrinkFactor;
	m_bihOut.biHeight = pbiImg->biHeight / c_nShrinkFactor;
	m_bihOut.biSizeImage = DIBSIZE(m_bihOut);

	if(m_pFrameOut)	free(m_pFrameOut);

	m_pFrameOut = (BYTE*)malloc(m_bihOut.biSizeImage);

	memset(m_pFrameOut, 0x80, m_bihOut.biSizeImage);
#endif
	m_pFrameFiller = std::auto_ptr<FrameNoSignalFiller>( 
		new FrameNoSignalFiller( pbiImg->biWidth, pbiImg->biHeight )  
		);

	// CFrmBuffer::Allocator
	return SetFrameAllocatorFormat(reinterpret_cast<BITMAPINFO*>(pbiImg));
}

//////////////////////////////////////////////////////////////////////
// Called in context of SampleGrabber filter thread
//////////////////////////////////////////////////////////////////////

void CDevicePin::OnBufferCB(double fTime, BYTE *pData, long lLen)
{
	// Called after destroy
	if(IsBadReadPtr(this, sizeof(CDevicePin)))
	{
		Elvees::Output(Elvees::TWarning, TEXT("CDevicePin::OnBufferCB() called after destroy"));
		return;
	}
#ifdef JUST_FOR_FPS_TEST
	++m_TotalFrameCounter;
#endif

	if( !lLen )
	{
		// empty frame 
//		Elvees::Output(Elvees::TWarning, TEXT("CDevicePin::OnBufferCB() empty frame"));
		return;
	}
	m_dwLastUpdated = timeGetTime();

	// Allocate frame buffer
	CFrmBuffer* pFrame = AllocateFrameBuffer();

	if(pFrame)
	{
		__time64_t ltime;
		_time64(&ltime);

		// Copy data to frame buffer...
		pFrame->SetSignalLocked(m_pDevice->IsSignalLocked());
		pFrame->SetFrameTime(m_dwLastUpdated, ltime);
		
		if( /*true */ m_pDevice->IsSignalLocked() > 0 )
		{
			// Copy frame buffer
			pFrame->SetFrameData(pData, lLen);
		}
		else if( m_pFrameFiller.get() )
		{
			FillEmptyFrame( pFrame );	
		}

		CompressFrame(pFrame);

		// если нижележащим фильтрам кадр не нужен, то просто удалим
		pFrame->Release();
	}
}

//////////////////////////////////////////////////////////////////////
// IStream
//////////////////////////////////////////////////////////////////////


bool CDevicePin::IsStarted()
{
	return m_pDevice->IsDeviceStarted(500);
}

//////////////////////////////////////////////////////////////////////////
// Video archiving
//////////////////////////////////////////////////////////////////////////

void CDevicePin::CompressFrame(CHCS::IFrame* pFrame)
{

#ifdef JUST_FOR_FPS_TEST
	const unsigned int c_Period = 60 * 1000;
	++m_NonEmptyFrameCounter;
	unsigned int Now = timeGetTime();
	if( (Now - m_PrevTime) >=  c_Period )
	{
		// time in ms
		double fNonEmptyFPS = m_NonEmptyFrameCounter * 1000.0 / ( Now - m_PrevTime ); 
		double fTotalFPS	= m_TotalFrameCounter * 1000.0 / ( Now - m_PrevTime ); 
		m_PrevTime = Now;
		m_NonEmptyFrameCounter = 0;
		m_TotalFrameCounter = 0;

		UuidString stUID;
		UUID z = GetUID();
		UuidToString( &z, &stUID );
		Elvees::OutputF( Elvees::TInfo, _T("[%s:%ld] tfps=%.1lf, nefps=%.1lf"), stUID, m_lPin, fTotalFPS, fNonEmptyFPS );
		RpcStringFree ( &stUID );
	}
#endif
	SendImage( pFrame, true, 0 );
}

UUID	CDevicePin::GetUID() const
{
	return m_pDevice->GetStreamUID();
}

std::tstring	CDevicePin::GetMoniker() const
{
	return m_pDevice->GetMoniker();
}