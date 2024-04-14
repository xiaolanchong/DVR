// Manager.h: interface for the CManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Stream.h"
#include "StreamDevice.h"
#include "StreamServerEvents.h"
#include "ext/manager/StreamList.h"

class CaptureServer;
class ManagerWindow;

//#define WM_DEVICE_EVENT			WM_APP+1

//#define DEVICEEVENTWND_CLASS  TEXT("DeviceEventClass")

//////////////////////////////////////////////////////////////////////
// CManager
//////////////////////////////////////////////////////////////////////

class CManager : boost::noncopyable,
	/*public CStreamServerEvents
	, */public Elvees::CThread
	, private Elvees::CUsesWinsock
{
public:
	explicit CManager(DWORD dwMode);
	virtual ~CManager();

	using Elvees::CThread::Start;

	bool IsStarted();
	void WaitStarted();

	void ShutDown();

	CHCS::IStream* StreamByID(long lStreamID);
	CHCS::IStream* StreamByUID(UUID& StreamUID, long lPin = 0);

	bool GetStreams( std::vector<long>& StreamIDs );


	bool CreateArchiveStream(CHCS::IStream** ppStream, long lSourceID, INT64 startPos);

	// Archive settings
	//

	bool IsArchiveEnabled();

	long    GetVideoRate();
	long    GetVideoMaxSize();
	LPCTSTR GetVideoBasePath();

private:
	bool LoadSettings();
	void SaveSettings();

	bool InitRegisteredDevices();
	void FindAndInstallDevices();

	// Check basepath folder (create if needed)
	bool PrepareArchive(bool bUseDefaultOnFailed = false);

	// Read Stream ID from database
	bool ReadStreamsFromDB(bool bShowDialogOnFailed = false);
	
	//
	//void PrepareStreams(bool bSetIDManually);


private:
	virtual int Run();

	bool RegisterDevice(const UUID& streamID, LPCTSTR stMoniker);

private:
	DWORD m_dwMode;

	// Settings
	//

	TCHAR m_stBasePath[MAX_PATH];	// Archive base path
	TCHAR m_stDBInitString[4096];	// DB Initialization string

	// Archive settings

	DWORD m_CodecFCC;				// Codec FOURCC

	long m_lVideoFPM;				// Video Frame per minute
	long m_lVideoMaxSize;			// Video max size in seconds

	long m_lCodecQuality;			//
	long m_lCodecKeyRate;			// Forced KeyRate
	long m_lCodecDataRate;			// Forced DataRate

	std::vector<BYTE>				m_CodecStateData;

	std::auto_ptr<CaptureServer>	m_pCaptureServer;
	// Network settings

	// Internal data
	//

	StreamList	m_StreamList;
	std::vector< boost::shared_ptr<IInputFramePin> > m_IntermediateFilters;

	bool	IsModeWriteArchive() const
	{
		return (CHCS_MODE_STORE & m_dwMode) != 0 ;
	}
	bool	IsModeNetServer() const
	{
		return (CHCS_MODE_NETSERVER & m_dwMode) != 0 ;
	}

	void	AttachToCaptureFilter( 
				IOutputFramePin* pCaptureFilter, 
				long StreamID,
				long  lDevicePin, 
				const UUID& camUID
				);
	
	bool IsStreamAlreadyAttached(long StreamID)
	{
		CHCS::IStream* p = StreamByID( StreamID );
		if( p )
		{
			p->Release();
			return true;
		}
		return false;
	}

	void	UpdateStreamListInWindow( ManagerWindow* pWindow);

	Elvees::CCriticalSection m_listSection;

	HANDLE m_hDeviceOwner;
	
	bool  m_bUpdateSettings;		// Update flag

	Elvees::CManualResetEvent m_startedEvent;
	Elvees::CManualResetEvent m_shutdownEvent;

private:
	Elvees::CLibrary xvidcore;
	Elvees::CLibrary xvidvfw;
};
