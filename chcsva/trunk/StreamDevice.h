// StreamDevice.h: interface for the CStreamDevice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Device.h"
#include "Stream.h"
#include "FrmBuffer.h"
#include "ext/ImageGraph/FramePin.h"
#include "ext/FrameNoSignalFiller.h"

class  CDeviceStream;
struct IAVIStream;
class	ArchiveWriter;

/*
class FrameNoSignalFiller
{
	std::vector< BYTE >	m_NoSignalImage;
//	unsigned int		m_ImageWidth;
//	unsigned int		m_ImageHeight;
public:
	FrameNoSignalFiller( unsigned int ImageWidth, unsigned int ImageHeight );
	void	Fill( CFrmBuffer* pFrmBuffer) const;
};*/

//#define DISABLE_FILTERS
//#define ENABLE_RESIZE

//#define JUST_FOR_FPS_TEST
//////////////////////////////////////////////////////////////////////
// CStreamDevice
//////////////////////////////////////////////////////////////////////

class CDevicePin :		public CFrmBuffer::Allocator, 
						public  IOutputFramePin,
						boost::noncopyable
{
#ifdef JUST_FOR_FPS_TEST
	unsigned int m_PrevTime;
	unsigned int m_NonEmptyFrameCounter;
	unsigned int m_TotalFrameCounter;
#endif
	long m_lPin;
	CDeviceStream* m_pDevice;

	bool m_bEnabled;

	UUID	m_StreamUID;
	std::auto_ptr<FrameNoSignalFiller>	m_pFrameFiller;
public:
	explicit CDevicePin(const UUID& streamUID, CDeviceStream* pDevice, long lPin);
	virtual ~CDevicePin();

	bool IsEnabled() { return m_bEnabled; }

	virtual long GetDevicePin() const { return m_lPin; }
	
//	virtual bool IsValid();
//	virtual bool IsDeviceString(LPCTSTR stDevice);

	virtual bool IsStarted();

//	virtual bool Start();
//	virtual void Stop();

	virtual void Delete();

	UUID	GetUID() const;

	std::tstring	GetMoniker() const;

private:
	// Allocator source ID request
	virtual long AllocatorSourceID();
public:
	// Accept stream format
	virtual bool OnImageFormat(BITMAPINFOHEADER* pbiImg);
	// Called in context of SampleGrabber filter thread

	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);
private:
	//////////////////////////////////////////////////////////////////////////
	// Compress

	bool IsFrameNeeded(CHCS::IFrame* pFrame);
	void CompressFrame(CHCS::IFrame* pFrame);


	void	FillEmptyFrame( CFrmBuffer* pFrame );
	//////////////////////////////////////////////////////////////////////////
	// Normalize image

#ifdef ENABLE_RESIZE
	BITMAPINFOHEADER m_bihOut;
	BYTE  *m_pFrameOut;
#endif

private:
	DWORD m_dwLastUpdated;
	Elvees::CManualResetEvent m_newImage;


	Elvees::CCriticalSection m_lastFrameSect;
};

//////////////////////////////////////////////////////////////////////
// CDeviceStream
//////////////////////////////////////////////////////////////////////

class CDeviceStream :	public CDevice,
						boost::noncopyable
{
private:
	explicit CDeviceStream( const UUID& streamUID, LPCTSTR stMoniker );
public:
	virtual ~CDeviceStream();

public:
	// Create device
	static CDeviceStream* CreateDevice(
		const UUID& streamID,			// Unique stream ID
		LPCTSTR stMoniker,		// Device moniker
		long lWidth = 0,		// Width
		long lHeight = 0,		// Height
		long lFrmRate = 0,		// Frame rate
		long lInput = 0,		// Input
		long lVStandard = 0,	// Video Standart (see DX documentation)
		bool bBWMode = false);	// Force Black-White video

	void Delete() { 
	}

	int GetPinCount() const { return 4; }
	boost::shared_ptr<CDevicePin> GetPin(int i)
	{
		switch(i)
		{
		case 0:
			return m_pin1;
		case 1:
			return m_pin2;
		case 2:
			return m_pin3;
		case 3:
			return m_pin4;
		}
		return boost::shared_ptr<CDevicePin>();
	}

	bool StartDevice(LPCTSTR stMoniker)
	{
		m_bUseTechwellDriverHack = IsTechwellCaptureCard(stMoniker);
		return CDevice::StartDevice();
	}

	void StopDevice();

	std::tstring GetMoniker() const
	{
		return m_stMoniker;
	}

	UUID		GetStreamUID() const
	{
		return m_StreamUID;
	}

protected:
	int m_curpin;
	boost::shared_ptr<CDevicePin> m_pin1;
	boost::shared_ptr<CDevicePin> m_pin2;
	boost::shared_ptr<CDevicePin> m_pin3;
	boost::shared_ptr<CDevicePin> m_pin4;

private:
	// Accept stream format
	virtual bool OnImageFormat(BITMAPINFOHEADER* pbiImg);
	// Called in context of SampleGrabber filter thread
	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);
	// Get output device id	
	virtual long GetDeviceID() { return -1; };

	//! исползовать хак в драйверах техвела? - первым пикселем передается номер пина
	bool	m_bUseTechwellDriverHack;

	std::tstring m_stMoniker;
	UUID		 m_StreamUID;

	// No copies do not implement
	CDeviceStream(const CDeviceStream &rhs);
	CDeviceStream &operator=(const CDeviceStream &rhs);
};

typedef Elvees::TNodeList<CDeviceStream> CDeviceList;