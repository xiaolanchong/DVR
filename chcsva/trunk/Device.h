// Device.h: interface for the CDevice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SmpGrbHlp.h"

// Forward declaration
struct IUnknown;
struct IPin;
struct IAMCrossbar;
struct IAMStreamConfig;
struct IAMVideoProcAmp;
struct IAMAnalogVideoDecoder;
struct IAMDroppedFrames;
struct IBaseFilter;
struct IMediaControl;
struct IMediaEventEx;
struct IMediaSeeking;
struct IGraphBuilder;
struct ICaptureGraphBuilder2;

//class PinRouter;
//////////////////////////////////////////////////////////////////////
// CDevice

//
//	WDM Video Capture Filter
//
//	Filter Interfaces:
//		+IAMAnalogVideoDecoder
//		-IAMCameraControl
//		+IAMDroppedFrames
//		-IAMExtDevice
//		-IAMExtTransport
//		-IAMFilterMiscFlags
//		-IAMTimecodeReader
//		-IAMVideoCompression
//		-IAMVideoControl
//		+IAMVideoProcAmp
//		+IAMStreamConfig
//		+IAMCrossbar

class CDevice : public CSampleGrabberHelper
{
public:
	CDevice();
	virtual ~CDevice();
	
	bool InitDevice(LPCTSTR stMoniker,
		long lWidth, long lHeight, long lFrmRate,
		long lInput, long lVStandard, bool bBWMode);
	
	bool StartDevice();
	void StopDevice();

	bool IsDeviceValid();
	bool IsDeviceStarted(long msTimeout = 0);

	long IsSignalLocked();

	bool SetEventHWND(HWND hWnd, long lMsg);
	void HandleDeviceEvents();

	long GetNumDropped();
	long GetNumNotDropped();

	virtual long GetDeviceID() { return -1; };

public:
	// Static helper functions
	static HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	static void RemoveGraphFromRot(DWORD pdwRegister);

	static IPin* GetPin(IBaseFilter* pFilter, int iNum, bool input);

protected:
	// Ask for accept stream format
	virtual bool OnImageFormat(BITMAPINFOHEADER *pbihImg);

	// Called in context of SampleGrabber filter thread
	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);

	void DropFrame();
	void CloseInterfaces();
	
	bool CreateBaseInterfaces();
	bool CreateFilter(IBaseFilter **ppFilter, LPCTSTR stFilterMoniker);
	bool CreateSourceBaseInterfaces(IBaseFilter *pSource);

	bool RouteToPin(long lPin, bool bStopBefore = true);
	bool RouteToSignaledPin(long lDesirePin = -1);	// Search for signaled
	bool SetTVFormat(long lVStandard = 16);			// AnalogVideo_PAL_B
	bool SetCaptureFormat(long lWidth, long lHeight, long lFrmRate = 0);
	bool SetVideoProc(bool bBWMode);

	bool PrepareStartCapture(IBaseFilter *pSource);

	//! определить карту захвата
	//! \return да - используется Techwell
	static bool	IsTechwellCaptureCard(LPCTSTR stMoniker);

private:
//	std::auto_ptr<PinRouter>	m_pPinRouter;
	void NewStopDevice();
	void NewStartDevice();

	CComPtr<IAMCrossbar>			m_pXbar;
	CComPtr<IAMStreamConfig>		m_pVSC;
	CComPtr<IAMVideoProcAmp>		m_pVPA;
	CComPtr<IAMDroppedFrames >		m_pVDrp;
	CComPtr<IAMAnalogVideoDecoder>	m_pVdoDec;

	CComPtr<IMediaControl> m_pMC;
	CComPtr<IMediaEventEx> m_pME;
	CComPtr<IMediaSeeking> m_pMS;
	CComPtr<IGraphBuilder> m_pGraph;
	CComPtr<ICaptureGraphBuilder2> m_pCapture;

	CComPtr<IBaseFilter> m_pCaptureFilter;

	bool m_bValid;
	bool m_bHasStreamControl;

	long m_lDroppedNotBase;
	long m_lDroppedBase;

	DWORD m_dwGraphRegister;

	// No copies do not implement
	CDevice(const CDevice &rhs);
	CDevice &operator=(const CDevice &rhs);
};

//! \brief класс переключения пинов в отдельном потоке
//! \version 1.0
//! \date 07-27-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!

/*
class PinRouter : Elvees::CThread
{
	//! идент-р утсройства
	std::tstring			m_sDeviceUID;
	CComPtr<IAMCrossbar>	m_pXbar;
	CComPtr<IMediaControl>	m_pMC;
	
	virtual int Run();

	bool	RouteToPinInAnotherThread(long PinNumber);
	void	StartDevice();
	void	StopDevice();
	bool	IsDeviceStarted();

	Elvees::CManualResetEvent	m_ExitEvent;
	Elvees::CAutoResetEvent		m_SwitchEvent;

	long							m_PinNumber;

	template<typename T>	void TryActionWithMediaControl( T func, const TCHAR* szFunctionName )
	{
		HRESULT hr;
		int AttemptNumber = 0;
		if( (m_pMC->*func)() != S_OK )
		{
			do 
			{
				Sleep(5);
				++AttemptNumber;
				hr = (m_pMC->*func)();
			} 
			while( hr != S_OK || AttemptNumber < 10 );
			if(hr != S_OK)
			{
				Elvees::OutputF(Elvees::TError, TEXT("IMediaControl::%s exceeded attempt number, hr=0x%X"), szFunctionName, hr);
			}
		}
	}
public:
	PinRouter( 
		CComPtr<IAMCrossbar>	pXbar, 
		CComPtr<IMediaControl> pMC, 
		const std::tstring& sDeviceUID );

	void	Start();
	void	Stop();
	void	RouteToPin( long PinNumber );
};
*/