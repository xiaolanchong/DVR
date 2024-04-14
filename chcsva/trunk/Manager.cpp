// Manager.cpp: implementation of the CManager class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "Manager.h"

#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include "resource.h"

#include "StreamDevice.h"
#include "StreamArchive.h"

//#pragma TODO("Uncomment EUGENE_ADD_ON")
#define EUGENE_ADD_ON

#ifdef EUGENE_ADD_ON
#include "ext/DBRegWrapper.h"
#include "ext/DatabaseWrapper.h"
#else
#include "DBAccessors.h"
#endif

// new 
#include "ext/localvideo/LocalClientStream.h"
#include "ext/localvideo/LocalTier.h"

#include "ext/manager/SystemSettings.h"
#include "ext/manager/ManagerWindow.h"
#include "ext/manager/CaptureServer.h"
#include "ext/manager/StreamList.h"

#include "ext/archive/IArchiveStreamFactory.h"
#include "ext/ImageGraph/CompressFilter.h"
#include "ext/ImageGraph/ArchiveWriterFilter.h"
#include "ext/ImageGraph/ServerStream.h"


static std::tstring UIDtoString( const UUID& uid )
{
	UUID MutableUID = uid;
	UuidString stUID;
	UuidToString( &MutableUID, &stUID );
	std::tstring s( (const TCHAR*)stUID );
	RpcStringFree( &stUID );
	return s;
}

#define MAX_NET_CLIENTS		100

CManager::CManager(DWORD dwMode)
	: m_dwMode(dwMode)
	, m_shutdownEvent(false)
	, m_startedEvent(false)
	, xvidcore(TEXT("xvidcore.dll"))
	, xvidvfw(TEXT("xvidvfw.dll"))
	, m_pCaptureServer(new CaptureServer)

{
	// Default parameters
	m_hDeviceOwner = NULL;

	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	lstrcpy(m_stBasePath, TEXT(""));
	lstrcpy(m_stDBInitString, TEXT(""));

	m_bUpdateSettings = false;
}

CManager::~CManager()
{
	if(m_hDeviceOwner)
		::CloseHandle(m_hDeviceOwner);
}

//////////////////////////////////////////////////////////////////////////
//

bool CManager::LoadSettings()
{
	StorageSystemSettings param;

	SystemSettings ss;
	bool res = ss.LoadStorageSettings( param );

	if(! res )
	{
		return false;
	}

	lstrcpyn( m_stBasePath,		param.m_stBasePath.c_str(), MAX_PATH);	// Archive base path
	lstrcpyn( m_stDBInitString, param.m_stDBInitString.c_str(), 4096);	// DB Initialization string

	// Archive settings

	m_CodecFCC			= param.m_CodecFCC;				// Codec FOURCC

	m_lVideoFPM			= param.m_lVideoFPM;				// Video Frame per minute
	m_lVideoMaxSize		= param.m_lVideoMaxSize;			// Video max size in seconds

	m_lCodecQuality		= param.m_lCodecQuality;			//
	m_lCodecKeyRate		= param.m_lCodecKeyRate;			// Forced KeyRate
	m_lCodecDataRate	= param.m_lCodecDataRate;			// Forced DataRate
	m_CodecStateData	= param.m_CodecStateData;

	return true;
}

void CManager::SaveSettings()
{
	if(!m_bUpdateSettings)
		return;

	SystemSettings ss;
	ss.SaveStorageSettings( m_stBasePath, m_stDBInitString );
}

bool CManager::PrepareArchive(bool bUseDefaultOnFailed /*= false*/)
{
	::SetLastError(0);

	if(!::CreateDirectory(m_stBasePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if(!bUseDefaultOnFailed)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive base directory"));
			return false;
		}

		//  Create directory in dll directory
		::GetModuleFileName(_Module.GetModuleInstance(), m_stBasePath, countof(m_stBasePath));

		int i=lstrlen(m_stBasePath);
		for(; m_stBasePath[i] != '\\'; i--);
		
		m_stBasePath[i+1] = 0;
		lstrcat(m_stBasePath, TEXT("Archive"));

		::SetLastError(0);

		if(!::CreateDirectory(m_stBasePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive directory"));
			return false;
		}

		m_bUpdateSettings = true;
	}

	return true;
}

bool CManager::InitRegisteredDevices()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Create registered streams"));

	SystemSettings ss;
	std::vector<DeviceSettings> CamParams;
	bool res = ss.LoadDeviceSettings( CamParams );

	if( !res )
	{
		return false;
	}
	
	return m_pCaptureServer->CreateDevices( CamParams );
}

void CManager::FindAndInstallDevices()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Searching for new devices"));

	std::vector< std::pair<std::tstring, std::tstring > > MonikersAndFriendlyNamesForAllDevices;
	m_pCaptureServer->GetAllDevicesInSystem( MonikersAndFriendlyNamesForAllDevices );

	for ( size_t i  = 0; i < MonikersAndFriendlyNamesForAllDevices.size(); ++i )
	{
		const std::tstring & stFriendlyName = MonikersAndFriendlyNamesForAllDevices[i].second;
		const std::tstring & stMoniker		= MonikersAndFriendlyNamesForAllDevices[i].first;
		if( !m_pCaptureServer->IsDeviceAlreadyCreated( stMoniker.c_str() ) )
		{
			
			Elvees::OutputF(Elvees::TInfo, TEXT("New device \"%s\" found"), stFriendlyName.c_str() );

			UUID streamID = GUID_NULL;
			::CoCreateGuid(&streamID);
			RegisterDevice(streamID, stMoniker.c_str());
		}
	}
}

bool CManager::RegisterDevice(const UUID& streamID, LPCTSTR stMoniker)
{
	SystemSettings ss;
	return ss.SaveDeviceSettings( streamID, stMoniker );
}

bool CManager::ReadStreamsFromDB(bool bShowDialogOnFailed)
{
	Elvees::Output(Elvees::TInfo, TEXT("> Read stream properties from database"));

	//////////////////////////////////////////////////////////////////////////
	//

	PHOSTENT pHost = gethostbyname(NULL);

	if(pHost)
	{
		Elvees::Output(Elvees::TInfo, TEXT("Local address(es):"));

		int nAddr = 0;

		while(pHost->h_addr_list[nAddr])
		{
			Elvees::OutputF(Elvees::TInfo, TEXT("\t%hS"),
				inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[nAddr]));

			nAddr++;
		}
	}

	SystemSettings ss;
	std::vector<CameraLiteSettings> CamParams;
	bool res = ss.LoadCameraSettingsFromDB( 
			m_stDBInitString, sizeof(m_stDBInitString), 
			bShowDialogOnFailed, CamParams );

	if( !res )
	{
		return false;
	}

	std::vector< std::pair<UUID, long> >	RequiredPins;

	for ( size_t i = 0; i < CamParams.size(); ++i  )
	{
		long	lDevicePin	= CamParams[i].m_lDevicePin;		
		TCHAR stCameraUID[1024];
		lstrcpyn( stCameraUID, CamParams[i].m_stCameraUID.c_str(), 1024 );

		UUID camUID;
		if(!StringToUuid(stCameraUID, &camUID))
		{
			continue;
		}
		RequiredPins.push_back( std::make_pair( camUID, lDevicePin ) );
	}

	m_pCaptureServer->SetUsedPins( RequiredPins );

	RequiredPins.clear();

	//std::set<long>	AttachedStreamSet;
	for ( size_t i = 0; i < CamParams.size(); ++i  )
	{
		long	m_lCameraID		= CamParams[i].m_lCameraID;
		long	m_lDevicePin	= CamParams[i].m_lDevicePin;
		LPCTSTR m_stCameraIP	= CamParams[i].m_stCameraIP.c_str();
		TCHAR m_stCameraUID[1024];
		lstrcpyn( m_stCameraUID, CamParams[i].m_stCameraUID.c_str(), 1024 );

		UUID camUID;
		if(!StringToUuid(m_stCameraUID, &camUID))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Invalid stream UID = \"%s\""), m_stCameraUID);
			break;
		}

		if( IsStreamAlreadyAttached(m_lCameraID) )
		{
			Elvees::OutputF(Elvees::TError, TEXT("Stream %ld is already attached, ignore"), m_lCameraID);
			continue;
		}

		bool bLocal = false;

			IOutputFramePin* pOutputPin = m_pCaptureServer->FindOuputPin( camUID, m_lDevicePin );
			if( pOutputPin  )
			{
				bLocal = true;
				AttachToCaptureFilter( pOutputPin, m_lCameraID, m_lDevicePin, camUID );
				RequiredPins.push_back( std::make_pair( camUID, m_lDevicePin ) );
			}

		if(!bLocal && (m_dwMode & CHCS_MODE_NETCLIENT) )
		{
			CStream *
			pStream = LocalClientStream::CreateStream(m_lCameraID, camUID);
			if(pStream)
			{
				pStream->SetID(m_lCameraID);
				StreamList::StreamInfo info;
				info.m_bIsLocal		= false;
				info.m_DevicePin	= m_lDevicePin;
				info.m_pStream		= boost::shared_ptr<CStream>( pStream );
				info.m_stMoniker	= m_stCameraUID;
				info.m_StreamID		= m_lCameraID;
				info.m_StreamUID	= camUID;
				m_StreamList.Push(info );
				RequiredPins.push_back( std::make_pair( camUID, m_lDevicePin ) );
			}
		}

		Elvees::OutputF(Elvees::TInfo, TEXT(" %c%ld %s (%ld) \"%s\""),
			bLocal ? TEXT('~') : TEXT('*'), m_lCameraID,
			m_stCameraIP, m_lDevicePin, m_stCameraUID);
	}

	// если произошла ошибка при привязывании StreamID к пину, то удалить этот пин
	m_pCaptureServer->SetUsedPins( RequiredPins );

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CHCS::IStream* CManager::StreamByID(long lStreamID)
{
	return m_StreamList.StreamByID( lStreamID );
}

CHCS::IStream* CManager::StreamByUID(UUID& StreamUID, long lPin)
{
	return m_StreamList.StreamByUID( StreamUID, lPin );
}


bool CManager::GetStreams( std::vector<long>& StreamIDs )
{
	return m_StreamList.GetStreams( StreamIDs );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int CManager::Run()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Manager started"));

	CoInitialize(NULL);

	// Load settings from registry
	LoadSettings();

	// Check and initialize archive
	if(m_dwMode & CHCS_MODE_STORE)
		PrepareArchive(true);

	// Start stream clients
	if(m_dwMode & CHCS_MODE_NETCLIENT)
	{
//		StartStreamClients();
//		StartArchiveClients();
	}

	//////////////////////////////////////////////////////////////////////////
	// Create windows

	std::auto_ptr<ManagerWindow> pManagerWindow;

	if(m_dwMode & CHCS_MODE_GUI)
	{
		////
		bool bClientMode = (m_dwMode & CHCS_MODE_NETCLIENT) != 0;
		pManagerWindow = std::auto_ptr<ManagerWindow>( new ManagerWindow( !bClientMode ) );
	}

	//////////////////////////////////////////////////////////////////////////
	// Start local device or map if it essential

	if(m_dwMode & CHCS_MODE_DEVICE)
	{
		m_hDeviceOwner = ::CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\CHCSDeviceOwner"));

		if(m_hDeviceOwner && GetLastError() != ERROR_ALREADY_EXISTS)
			Elvees::Output(Elvees::TInfo, TEXT("Starting devices"));
		else
			Elvees::Output(Elvees::TWarning, TEXT("Starting devices twice"));

		// Start stream server
		if(m_dwMode & CHCS_MODE_NETSERVER)
		{
//			StartStreamServer();
//			StartArchiveServer();
		}

		InitRegisteredDevices();

		if(m_dwMode & CHCS_MODE_DEVICENEW)
			FindAndInstallDevices();
	}

	if(m_dwMode & CHCS_MODE_DATABASE)
	{
		ReadStreamsFromDB(false);
//		PrepareStreams(false);
	}
	else
	{
		// FIXME w/o database mode just start all pins
//		PrepareStreams(true);
	}

	SaveSettings();
	UpdateStreamListInWindow( pManagerWindow.get() );

	m_startedEvent.Set();

	//////////////////////////////////////////////////////////////////////////
	// Main loop
	
	MSG msg;
	DWORD dwRet;
	HANDLE hShutdown = m_shutdownEvent.GetEvent();

	while(1)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hShutdown, FALSE, INFINITE, QS_ALLINPUT);

		if(dwRet == WAIT_OBJECT_0 + 1)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
				//	m_shutdownEvent.Set();
					break;
				}

				if( !pManagerWindow.get() || !pManagerWindow->IsDialogMessage( &msg )  )
				{
					TranslateMessage(&msg); 
					DispatchMessage(&msg);
				}
			}
		}
		else if(dwRet == WAIT_OBJECT_0) // Shutdown
		{
			pManagerWindow.reset();
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	// first delete all devices, because a device can call a stream callback procedure

	m_pCaptureServer->DeleteAllDevices();
	m_IntermediateFilters.clear();
	m_StreamList.Clear();
	m_pCaptureServer->DeleteAllPins();

	// flush after capture has been finished
	GetLocalTier()->FlushFrames();
	
//	StopStreamServer();
//	StopStreamClients();
//	StopArchiveServer();
//	StopArchiveClients();

	CoUninitialize();

	Elvees::Output(Elvees::TInfo, TEXT("> Manager stopped"));
	m_startedEvent.Reset();

	return 0;
}

bool CManager::IsStarted()
{
	return m_startedEvent.Wait(0);
}

void CManager::WaitStarted()
{
	HANDLE h[2];

	h[0] = m_startedEvent.GetEvent();
	h[1] = GetHandle(); // CThread;

//	WaitForMultipleObjects(2, h, FALSE, INFINITE);

	if(WaitForMultipleObjects(2, h, FALSE, 60000) == WAIT_TIMEOUT)
		Elvees::Output(Elvees::TWarning, TEXT("Manager starting too long time"));
}

void CManager::ShutDown()
{
	m_shutdownEvent.Set();
	Wait();
}

bool CManager::IsArchiveEnabled()
{
	return (m_dwMode & CHCS_MODE_STORE) ? true : false;
}

long CManager::GetVideoRate()
{
	return m_lVideoFPM;
}

long CManager::GetVideoMaxSize()
{
	return m_lVideoMaxSize;
}

LPCTSTR CManager::GetVideoBasePath()
{
	return m_stBasePath;
}

bool CManager::CreateArchiveStream(CHCS::IStream** ppStream, long lSourceID, INT64 startPos)
{
	HRESULT hr;
#ifdef EUGENE_ADD_ON
	CWrapDataSource dbsource;
	CWrapSession    dbsession;
	CWrapCameraInRoomCommand cmdCamera;
#else
	CDataSource dbsource;
	CSession    dbsession;
	CCommand<CAccessor<CAccCameraID> > cmdCamera;
#endif	

	// Open and read database
	ULONG sourceIP;
	

	bool bDone = false;
	if(
#if OLD
	!m_pArchiveClient || 
#endif
	!ppStream) 
		return false;

	do
	{	
		// Open database

	#ifdef _UNICODE
		hr = dbsource.OpenFromInitializationString(m_stDBInitString);
	#else
		WCHAR szDBInitString[4096];

		MultiByteToWideChar(CP_ACP, 0, m_stDBInitString, lstrlen(m_stDBInitString) + 1,
			szDBInitString, countof(szDBInitString));

		hr = dbsource.OpenFromInitializationString(szDBInitString);
	#endif
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
			break;
		}

		// Open session

		hr = dbsession.Open(dbsource);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database session hr=0x%08lX"), hr);
			break;
		}

		// Save all streams to list
		cmdCamera.m_lCameraID = lSourceID;

		hr = cmdCamera.Open(dbsession);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open table hr=0x%08lX"), hr);
			break;
		}

		if(cmdCamera.MoveFirst() == S_OK)
		{
		#ifndef _UNICODE
			sourceIP = inet_addr(cmdCamera.m_stCameraIP);
		#else
			char stSourceIP[56];

			WideCharToMultiByte(CP_ACP, 0, cmdCamera.m_stCameraIP,
				(int)wcslen(cmdCamera.m_stCameraIP) + 1,
				stSourceIP, sizeof(stSourceIP), NULL, NULL);

			sourceIP = inet_addr(stSourceIP);
		#endif

			if(INADDR_NONE == sourceIP)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Invalid stream IP = \"%s\""), cmdCamera.m_stCameraIP);
				break;
			}
		}
		CHCS::IStream* pStream = 
#if OLD
		CStreamArchive::CreatStream(lSourceID, startPos);
#else 
		GetArchiveStreamFactory()->CreateArchiveStream( lSourceID, startPos, 0, sourceIP, GetVideoBasePath() );
#endif
		if(pStream)
		{
#if OLD
			if(!m_pArchiveClient->StartStream(pStream, sourceIP, 5006))
			{
				pStream->Release();
				break;
			}
#endif
			pStream->Start();
			pStream->SeekTo(startPos);

			*ppStream = pStream;
			bDone = true;
		}
	}
	while(false);

	return bDone;
}

void	CManager::AttachToCaptureFilter( 
						IOutputFramePin* pCaptureFilter, 
						long StreamID,
						long  lDevicePin, 
						const UUID &camUID
						)
{
	std::tstring stCameraUID = UIDtoString( camUID );
	ServerStream* pServerStreamForConsumer = new ServerStream( StreamID, NULL );
	pCaptureFilter->ConnectPin( pServerStreamForConsumer );

	StreamList::StreamInfo info;
	info.m_bIsLocal		= true;
	info.m_DevicePin	= lDevicePin;
	info.m_pStream		= boost::shared_ptr<CHCS::IStream>( pServerStreamForConsumer );
	info.m_stMoniker	= stCameraUID;
	info.m_StreamID		= StreamID;
	info.m_StreamUID	= camUID;

	m_StreamList.Push( info );

	if( !IsModeWriteArchive() )
	{
		return;
	}

	EncoderParam CompressParam;
	CompressParam.m_CodecFCC		= m_CodecFCC;
	CompressParam.m_lVideoFPM		= m_lVideoFPM;
	CompressParam.m_lCodecQuality	= m_lCodecQuality;
	CompressParam.m_lCodecDataRate	= m_lCodecDataRate;
	CompressParam.m_lCodecKeyRate	= m_lCodecKeyRate;
	CompressParam.m_CodecStateData	= m_CodecStateData;

	ArchiveWriteParam ArchiveParam;
	ArchiveParam.m_CodecFCC					= m_CodecFCC;
	ArchiveParam.m_FramesPerMinutes			= m_lVideoFPM;
	ArchiveParam.m_VideoBasePath			= GetVideoBasePath();
	ArchiveParam.m_VideoMaxSizeInSeconds	= m_lVideoMaxSize;


	ArchiveWriterFilter* pArchiveFilter = new ArchiveWriterFilter( StreamID, ArchiveParam );
	m_IntermediateFilters.push_back( boost::shared_ptr<IInputFramePin>(pArchiveFilter) );

	CompressFilter* pCompressFilter = new CompressFilter( StreamID, CompressParam, pArchiveFilter );
	pServerStreamForConsumer->ConnectPin ( pCompressFilter );
	m_IntermediateFilters.push_back( boost::shared_ptr<IInputFramePin>(pCompressFilter) );

}

void	CManager::UpdateStreamListInWindow( ManagerWindow* pWindow )
{
	const std::vector<StreamList::StreamInfo>& Arr = 	m_StreamList.GetStreamInfoArr();

	std::vector<StreamInfo > StreamArr;

	for( size_t i =0; i < Arr.size(); ++i )
	{
		StreamInfo info;
		info.m_bIsLocal		= Arr[i].m_bIsLocal;
		info.m_DevicePin	= Arr[i].m_DevicePin;
		info.m_pStream		= Arr[i].m_pStream.get();
		info.m_stMoniker	= Arr[i].m_stMoniker;
		info.m_StreamID		= Arr[i].m_StreamID;
		info.m_StreamUID	= Arr[i].m_StreamUID;

		StreamArr.push_back( info );
	}

	if( pWindow )
	{
		pWindow->SetStreams( StreamArr );
	}

}