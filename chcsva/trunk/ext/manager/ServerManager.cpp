//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: √лавный класс реализации интерфейса IStreamManager дл€ сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "ServerManager.h"

// new 
#include "../localvideo/LocalClientStream.h"
#include "../localvideo/LocalTier.h"

#include "../config/SystemSettings.h"
#include "CaptureServer.h"

#include "../ImageGraph/CompressFilter.h"
#include "../ImageGraph/ArchiveWriterFilter.h"
#include "../ImageGraph/CommonProcessFilter.h"
#include "../ImageGraph/ServerStream.h"

#include "../remote/video/RemoteVideoProviderServer.h"
#include "../remote/archive/RemoteArchiveProvider.h"
#include "../remote/ipc/NetUtil.h"



//======================================================================================//
//                                 class ServerManager                                  //
//======================================================================================//

ServerManager::ServerManager( boost::shared_ptr<SystemSettings> pVideoConfig, DWORD WorkingMode):
	BaseManager( pVideoConfig, true, WorkingMode ),
	m_pCaptureServer(new CaptureServer)
{
	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	lstrcpy(m_stBasePath, TEXT(""));

	m_bUpdateSettings = false;
}

ServerManager::~ServerManager()
{

}

void ServerManager::InitializeWork() 
{
	// Load settings from registry
	LoadSettings();

	// Start stream clients
	if( CreateNetServer() )
	{
		//		StartStreamClients();
		//		StartArchiveClients();
	}

	//////////////////////////////////////////////////////////////////////////
	// Start local device or map if it essential

	if(!m_Sentry.IsAlreadyRunning())
	{
		Elvees::Output(Elvees::TInfo, TEXT("Starting devices"));
	}
	else
	{
		Elvees::Output(Elvees::TWarning, TEXT("Starting devices twice"));
	}

	// Start stream server
	if( CreateNetServer() )
	{
		//			StartStreamServer();
		//			StartArchiveServer();
	}
#ifndef ARCHIVE_ONLY
	CreateVideoProvider();
	InitRegisteredDevices();

	if( UseOnlyDatabaseStreams() )
	{
		ReadStreamsFromDB(false);
	}
	else
	{
		// FIXME w/o database mode just start all pins
		//		PrepareStreams(true);
	}
#endif
	CreateArchiveProvider();
	SaveSettings();
}

void ServerManager::UninitializeWork()
{
#ifndef ARCHIVE_ONLY
	// first delete all devices, because a device can call a stream callback procedure
	
	// удалить фильтры в обратном пор€дке
	m_pCaptureServer->DeleteAllDevices();
	m_StreamList.Clear();
	m_IntermediateFilters.clear();
	DestroyVideoProvider();
	//! а здесь буферы кадров, удал€ютс€ последним
	m_pCaptureServer->DeleteAllPins();

	// flush after capture has been finished
	GetLocalTier()->FlushFrames();
#endif
	DestroyArchiveProvider();

	m_pCaptureServer.reset();
	m_VideoProvider.reset();
	m_pArchiveProvider.reset();
}


//////////////////////////////////////////////////////////////////////////
//

bool ServerManager::LoadSettings()
{
	StorageSystemSettings param;

	SystemSettings& ss = GetSystemSettings();
	bool res = ss.LoadStorageSettings( param );

	if(! res )
	{
		return false;
	}

	lstrcpyn( m_stBasePath,		param.m_stBasePath.c_str(), MAX_PATH);	// Archive base path

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

void ServerManager::SaveSettings()
{
	if(!m_bUpdateSettings)
		return;

	SystemSettings& ss = GetSystemSettings();
	ss.SaveStorageSettings( m_stBasePath );
}

//////////////////////////////////////////////////////////////////////////


bool ServerManager::InitRegisteredDevices()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Create registered streams"));

	SystemSettings& ss = GetSystemSettings();
	std::vector<DeviceSettings> DeviceParams;
	bool res = ss.LoadLocalDeviceSettings( DeviceParams );

	if( !res )
	{
		return false;
	}

	return m_pCaptureServer->CreateDevices( DeviceParams );
}

class  DevicePred
{
	std::tstring m_sDeviceID;
public:
	DevicePred( const std::tstring& sDeviceID ) : m_sDeviceID(sDeviceID){}

	bool operator () (const CameraSettings& cs) const
	{
		return cs.m_stCameraUID == m_sDeviceID;
	}

};

size_t GetCameraNumberOnSameDevice( const std::vector<CameraSettings>& CamSettingsArr, const std::tstring& sDeviceID )
{
	return std::count_if( CamSettingsArr.begin(), CamSettingsArr.end(), DevicePred( sDeviceID ) );
}

unsigned int GetFPSForCamera( size_t CameraNumberOnSameDevice )
{
	const unsigned int c_MinFPS = 2;
	const unsigned int FPSArr[] = { 25, 12, 4, c_MinFPS };
	if( CameraNumberOnSameDevice == 0 )
	{
		// error
		_ASSERTE(false);
		return 0;
	}
	else if( CameraNumberOnSameDevice > sizeof( FPSArr )/sizeof(FPSArr[0]) )
	{
		return c_MinFPS;
	}
	else
	{
		return	FPSArr[ CameraNumberOnSameDevice - 1 ];
	}
}

bool ServerManager::ReadStreamsFromDB(bool bShowDialogOnFailed)
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
			Elvees::OutputF(Elvees::TInfo, TEXT("     %hS"),
				inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[nAddr]));

			nAddr++;
		}
	}

	SystemSettings& ss = GetSystemSettings();
	std::vector<CameraSettings> CamParams;
	bool res = ss.LoadCameraSettings( bShowDialogOnFailed, CamParams );

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

	USES_CONVERSION;
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


		IOutputFramePin* pOutputPin = m_pCaptureServer->FindOuputPin( camUID, m_lDevicePin );
		if( pOutputPin  )
		{
			const size_t CamNumber = GetCameraNumberOnSameDevice( CamParams, CamParams[i].m_stCameraUID );
			// must be at least one
			_ASSERTE(CamNumber);
			const unsigned int FPS = GetFPSForCamera( CamNumber );
			AttachToCaptureFilter( pOutputPin, m_lCameraID, m_lDevicePin, camUID, FPS );
			RequiredPins.push_back( std::make_pair( camUID, m_lDevicePin ) );
			Elvees::OutputF(Elvees::TInfo, TEXT(" %c%ld %s (%ld) \"%s\""),
				TEXT('~'),
				m_lCameraID,	m_stCameraIP, m_lDevicePin, m_stCameraUID);
		}
		else if( !SystemSettings::IsLocalIPAddress( inet_addr( T2CA( m_stCameraIP ) ) ) )
		{
			// remote
			Elvees::OutputF(Elvees::TInfo, TEXT(" %c%ld %s (%ld) \"%s\""),
				TEXT('*'),
				m_lCameraID,	m_stCameraIP, m_lDevicePin, m_stCameraUID);
		}
		else
		{
			// it is local but error occuried 
			Elvees::OutputF(Elvees::TError, TEXT(" %c%ld %s (%ld) \"%s\" failed to launch"),
				TEXT('~'),
				m_lCameraID,	m_stCameraIP, m_lDevicePin, m_stCameraUID);
		}
	}

	// если произошла ошибка при прив€зывании StreamID к пину, то удалить этот пин
	m_pCaptureServer->SetUsedPins( RequiredPins );

	return true;
}

//////////////////////////////////////////////////////////////////////

IntermediateFilter*		ServerManager::CreateServerStream(
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

	return pServerStreamForConsumer;
}

unsigned int FPStoFPM( unsigned int fps )
{
	return fps * 60;
}

IntermediateFilter*						ServerManager::CreateCompressFilter( long StreamID, unsigned int CompressFPS  )
{
	EncoderParam CompressParam;
	CompressParam.m_CodecFCC		= m_CodecFCC;
	CompressParam.m_lVideoFPM		= FPStoFPM(CompressFPS); // fps -> fpm
	CompressParam.m_lCodecQuality	= m_lCodecQuality;
	CompressParam.m_lCodecDataRate	= m_lCodecDataRate;
	CompressParam.m_lCodecKeyRate	= m_lCodecKeyRate;
	CompressParam.m_CodecStateData	= m_CodecStateData;

	CompressFilter* pCompressFilter = new CompressFilter( StreamID, CompressParam, 0 );
	m_IntermediateFilters.push_back( boost::shared_ptr<IInputFramePin>(pCompressFilter) );
	return pCompressFilter;
}

IntermediateFilter*			ServerManager::CreateArchiveFilter( long StreamID, unsigned int CompressFPS )
{
	ArchiveWriteParam ArchiveParam;
	ArchiveParam.m_CodecFCC					= m_CodecFCC;
	ArchiveParam.m_FramesPerMinutes			= FPStoFPM(CompressFPS); // fps -> fpm
	ArchiveParam.m_VideoBasePath			= GetVideoBasePath();
	ArchiveParam.m_VideoMaxSizeInSeconds	= m_lVideoMaxSize;


	ArchiveWriterFilter* pArchiveFilter = new ArchiveWriterFilter( StreamID, ArchiveParam );
	m_IntermediateFilters.push_back( boost::shared_ptr<IInputFramePin>(pArchiveFilter) );

	return pArchiveFilter;
}

IntermediateFilter*			ServerManager::CreateCommonProcessFilter()
{
	boost::shared_ptr<CommonProcessFilter> pCommonProcessFilter ( new CommonProcessFilter() );
	m_IntermediateFilters.push_back( pCommonProcessFilter );
	return pCommonProcessFilter.get();
}

void	ServerManager::AttachToCaptureFilter( 
										IOutputFramePin* pCaptureFilter, 
										long StreamID,
										long  lDevicePin, 
										const UUID &camUID,
										unsigned int CompressFPS
										)
{
	IOutputFramePin*	pSourceFilter = CreateServerStream( pCaptureFilter, StreamID, lDevicePin, camUID );

	if( CreateNetServer()   )
	{
		IntermediateFilter * pCompress = CreateCompressFilter(StreamID, CompressFPS);
		pSourceFilter->ConnectPin( pCompress );

		if( IsArchiveEnabled() )
		{
			IntermediateFilter * pArchive  = CreateArchiveFilter(StreamID, CompressFPS);
			pCompress->ConnectPin( pArchive );
			pArchive->ConnectPin( m_VideoProvider.get() );
		}
		else
		{
			pCompress->ConnectPin( m_VideoProvider.get() );
		}
	}
	else if ( IsArchiveEnabled() )
	{
		IntermediateFilter * pCompress = CreateCompressFilter(StreamID, CompressFPS);
		IntermediateFilter * pArchive  = CreateArchiveFilter(StreamID, CompressFPS);

		pSourceFilter->ConnectPin( pCompress );
		pCompress->ConnectPin( pArchive );
	}
}

void	ServerManager::CreateVideoProvider()
try
{
	if( !CreateNetServer())
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("CHCS_MODE_NETSERVER disabled"));
		return;
	}

	m_VideoProvider = std::auto_ptr<RemoteVideoProvider>(
			new RemoteVideoProvider
			);
	m_VideoProvider->Start();
	Elvees::OutputF(Elvees::TInfo, 
		TEXT("RemoteVideoProvider created"));
}
catch ( NetException& ex ) 
{
	Elvees::OutputF(Elvees::TError, TEXT("Failed to create RemoteVideoProviderServer : %hs"), ex.what());
};

void	ServerManager::DestroyVideoProvider()
try
{
	if( m_VideoProvider.get() )
	{
		m_VideoProvider->Stop();
	}
	m_VideoProvider.reset();
}
catch (...) 
{

};

void	ServerManager::CreateArchiveProvider()
try
{
	if( !CreateNetArchive())
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("CHCS_MODE_NETARCHIVE disabled"));
		return;
	}

	m_pArchiveProvider = std::auto_ptr<RemoteArchiveProvider>( 
			new RemoteArchiveProvider( GetVideoBasePath() ) );
	m_pArchiveProvider->Start();

	Elvees::OutputF(Elvees::TInfo, TEXT("RemoteArchiveProvider created"));
}
catch ( NetException& ex ) 
{
	Elvees::OutputF(Elvees::TError, 
		TEXT("Failed to create RemoteArchiveProvider : %hs"), ex.what());
}

void	ServerManager::DestroyArchiveProvider()
try
{
	if( m_pArchiveProvider.get() )
	{
		m_pArchiveProvider->Stop();
	}
	m_pArchiveProvider.reset();
}
catch ( NetException&  ) 
{
}