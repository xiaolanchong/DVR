//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ðåàëèçàöèÿ èíòåðôåéñà

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "InstallerIml.h"
#include "TempVideoConfig.h"
#include "..\resource.h"

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif



void ReleaseManager( CHCS::IStreamManager* p)
{
	p->Release();
}

boost::shared_ptr<CHCS::IStreamManager>	GetManager( 
	const VideoLibary& VideoDynamicLibrary, 
	DWORD InitFlags,
	CHCS::IVideoSystemConfig* pVideoConfig
	)
{
	CHCS::INITCHCS_EX in;
	
	const DWORD c_nFlags = InitFlags;

	in.dwMode		= c_nFlags;
	in.dwSize		= sizeof(CHCS::INITCHCS_EX);
	in.dwUserData	= 0;
	in.procOutput	= 0;
	in.m_pVideoSystemConfig = pVideoConfig;

	CHCS::IStreamManager* pManager = VideoDynamicLibrary.InitStreamManagerEx(&in);
	if( pManager )
	{
		return		
			boost::shared_ptr< CHCS::IStreamManager >( pManager, &ReleaseManager ) ;
	}
	else
	{
		return	boost::shared_ptr< CHCS::IStreamManager >();
	}
}

void	LoadCodecStateFromResource( UINT nResourceID, std::vector<BYTE> & Content)
{
	extern HMODULE g_hDLLHandler;
	HRSRC hResource = FindResource( g_hDLLHandler, MAKEINTRESOURCE(nResourceID), _T("CODEC_STATE") );
	HGLOBAL hMem = LoadResource( g_hDLLHandler, hResource );
	DWORD		DataSize	=	SizeofResource( g_hDLLHandler, hResource );
	const BYTE* pData		=	(const BYTE*)LockResource( hMem );
	Content.assign( pData, pData + DataSize );
	UnlockResource( hMem );
}

bool	SetDefaultVideoSettings( CHCS::IVideoSystemConfigEx* pVideoConfig )
try
{
	std::vector<BYTE> CodecStateData;
	LoadCodecStateFromResource( IDR_CODEC_STATE_DEFAULT, CodecStateData );


	CHCS::IVideoSystemConfig::CodecSettings cs;
	cs.m_CodecFCC			= 0x44495658;
	cs.m_lVideoFPM			= 25 * 60;
	cs.m_lCodecDataRate		= 0;
	cs.m_lVideoMaxSize		= 5 * 60;
	cs.m_lCodecKeyRate		= 0;
	cs.m_lCodecQuality		= 10000;
	cs.m_CodecStateData		= CodecStateData;

	pVideoConfig->SetCodecSettings( cs );

	return true;
}
catch ( std::exception&  ) 
{
	return false;
};

std::map<std::tstring, UUID>	GetLocalMonikersInDatabase( CHCS::IVideoSystemConfigEx* pVideoConfig )
{
	std::map<std::tstring, UUID> DeviceMap;

	std::vector<CHCS::IVideoSystemConfig::DeviceSettings> ds;
	pVideoConfig->GetLocalDeviceSettings( ds );
	for ( size_t i = 0; i < ds.size(); ++i )
	{
		DeviceMap.insert( std::make_pair( ds[i].stMoniker, ds[i].streamID ) );
	}

	return DeviceMap;
}


//======================================================================================//
//                                  class InstallerIml                                  //
//======================================================================================//

InstallerIml::InstallerIml():
	m_hrCoinitResult( CoInitialize(0) ) ,
	m_bUseBridgeOption( true ),
	m_bUseBridge( true )
{
	m_pVideoDynamicLibrary = std::auto_ptr<VideoLibary>( new VideoLibary); 
	m_pBridgeDynamicLibrary = std::auto_ptr<BridgeLibrary> ( new BridgeLibrary ); 
}

InstallerIml::~InstallerIml()
{
	m_wndStream.ReleaseStream();
	m_pCurrentStream.reset();
	m_pManager.reset();
	m_pVideoDynamicLibrary.reset();
	m_pBridgeDynamicLibrary.reset();

	if(  m_hrCoinitResult == S_OK ) 
	{
		CoUninitialize();
	}
	if( m_wndStream )
	{
		m_wndStream.UnsubclassWindow();
	}
}
/*
bool z(const std::string& )
{
	return true;
}*/

bool InstallerIml::Register(const char* ServerIPAddress)
try
{
	USES_CONVERSION;
	m_bUseBridge = m_bUseBridgeOption;
	m_ServerIPAddress = A2CW( ServerIPAddress );
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );

	if( !pVideoConfig.get() )
	{
		return false;
	}

	if( !SetDefaultVideoSettings( pVideoConfig.get() ) )
	{
		return false;
	}
	return true;
}
catch ( std::exception&  ) 
{
	return false;
};

bool	RegisterDevice( CHCS::IVideoSystemConfigEx* pVideoConfig, const CHCS::IVideoSystemConfig::DeviceSettings& LocalDevice )
try
{
	if( !pVideoConfig )
	{
		return false;
	}

	pVideoConfig->CreateDevice( LocalDevice );

	return true;
}
catch ( std::exception& ) 
{
	return false;
};


//////////////////////////////////////////////////////////////////////////


bool InstallerIml::Unregister()
try
{
	m_bUseBridge = m_bUseBridgeOption;
	DeleteAllDevicesAndCameras();
	return true;
}
catch ( std::exception& )  
{
	return false;
};

bool InstallerIml::GetAvalableStreams( CAUUID* pAvailableStreams, CAUUID* pInstalledStreams )
try
{
	if( !pAvailableStreams )
	{
		return false;
	}
	memset( pAvailableStreams, 0, sizeof( CAUUID ) );

	if( pInstalledStreams )
	{
		memset( pInstalledStreams, 0, sizeof( CAUUID ) );
	}

	LocalRegister LocalRegistrar;

	bool res = m_pVideoDynamicLibrary->RegisterDevices( & LocalRegistrar );
	if(!res)
	{
		return false;
	}

	LocalVideoSystemConfig	LocalVideoConfig;
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );

	std::map<std::tstring, UUID> DeviceMap = GetLocalMonikersInDatabase(pVideoConfig.get());

	const std::vector<std::tstring>& Monikers = LocalRegistrar.GetDeviceMonikers();
	for ( size_t i =0; i < Monikers.size(); ++i )
	{
		std::map<std::tstring, UUID>::const_iterator it = DeviceMap.find( Monikers[i] );
		if( it != DeviceMap.end() )
		{
			LocalVideoConfig.AddDevice( it->first, it->second );
		}
		else
		{
			CHCS::IVideoSystemConfig::DeviceSettings ds = LocalVideoConfig.AddDevice( Monikers[i] );
			RegisterDevice( pVideoConfig.get(), ds );
		}
	}
	

	std::vector<CHCS::IVideoSystemConfig::DeviceSettings> LocalDeviceSettings;
	LocalVideoConfig.GetLocalDeviceSettings( LocalDeviceSettings );
	

	m_pManager = GetManager( *m_pVideoDynamicLibrary.get(), CHCS_MODE_DEVICE|CHCS_MODE_DATABASE|CHCS_MODE_GUI, &LocalVideoConfig);

	if(!m_pManager.get())
	{
		return false;
	}

	std::vector<CHCS::IVideoSystemConfig::CameraSettings> CamSettings; 
	LocalVideoConfig.GetCameraSettings(CamSettings);

	for ( size_t idxCamera = 0; idxCamera < CamSettings.size(); ++idxCamera )
	{
		int nCameraID		=	CamSettings[idxCamera].m_lCameraID;
		int PinNumber		=	CamSettings[idxCamera].m_lDevicePin;
		UUID DeviceUID		=   CamSettings[idxCamera].m_DeviceUID;
		UUID NewCameraUID	=	DeviceUID;
		BYTE v = NewCameraUID.Data4[7];
		v &= 0xF0;
		v |=  static_cast<BYTE>(PinNumber) & 0xF;
		NewCameraUID.Data4[7] = v;

		//hack
		m_CameraArr.push_back( boost::make_tuple( 
								NewCameraUID,
								UuidToString( DeviceUID ).c_str(),
								nCameraID,
								PinNumber
								) 
			);
	}

	pAvailableStreams->cElems = static_cast<ULONG>( m_CameraArr.size() );
	pAvailableStreams->pElems = (UUID*)CoTaskMemAlloc( sizeof(UUID) *  pAvailableStreams->cElems );

	for ( size_t i = 0; i < m_CameraArr.size(); ++i )
	{
		pAvailableStreams->pElems[i] = boost::get<0>( m_CameraArr[i] );
	}
	
	return true;
}
catch ( std::exception& ) 
{
	return false;
};

class FindUUID : public std::unary_function<bool, const CameraInfo_t&>
{
	UUID	m_uid;
public:
	FindUUID( const UUID& uid ):
		m_uid(uid)
	{
	}

	bool operator()( const CameraInfo_t& ci ) const
	{
		return IsEqualGUID( m_uid, boost::get<0>(ci) ) == TRUE;
	}
};

bool InstallerIml::AddStream		( const UUID* pStreamUID )
try
{
	if( !pStreamUID)
	{
		return false;
	}

	std::vector<CameraInfo_t>::const_iterator it = std::find_if( m_CameraArr.begin(), m_CameraArr.end(), FindUUID(*pStreamUID) );
	if( it == m_CameraArr.end() )
	{
		return false;
	}

	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );

	if( !pVideoConfig.get() )
	{
		return false;
	}
#if OLD
	std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig( pVideoConfig->GetDevice( boost::get<1>(*it) ) );

	if( !pDeviceConfig.get() )
	{
		return false;
	}

	std::auto_ptr<DBBridge::ICameraConfig> pCameraConfig( 
			pDeviceConfig->CreateCamera(  boost::get<2>(*it), boost::get<3>(*it) ) );

	return pCameraConfig.get() != NULL;
#else
	std::tstring sUID = boost::get<1>(*it) ;

	UUID DeviceUID = StringToUuid( sUID );
	const unsigned int PinNumber =  boost::get<3>(*it);
	pVideoConfig->CreateCamera( DeviceUID, PinNumber  );
#endif
	return true;
}
catch ( std::exception& ) 
{
	return false;
};

bool InstallerIml::RemoveStream	( const UUID* pStreamUID )
{
	return false;
}

bool InstallerIml::ShowStream		( const UUID* pStreamUID, HWND hWndForRender )
try
{
	if( !IsWindow( hWndForRender )	||  
		!m_pManager.get() )
	{
		return false;
	}

	if( !pStreamUID )
	{
		m_wndStream.ReleaseStream();
		return true;
	}

	std::vector<CameraInfo_t>::const_iterator it = std::find_if( m_CameraArr.begin(), m_CameraArr.end(), FindUUID(*pStreamUID) );
	if( it == m_CameraArr.end() )
	{
		return false;
	}

	if( m_wndStream )
	{
		if(  hWndForRender != m_wndStream )
		{
		m_wndStream.UnsubclassWindow();
		m_wndStream.SubclassWindow( hWndForRender );
		}
	}
	else
	{
		m_wndStream.SubclassWindow( hWndForRender );
	}

	SetTimer( m_wndStream, 0xdeadbeef, 300, NULL );

	CHCS::IStream* pStream;
	long nID = boost::get<2>(*it);
	m_pManager->GetStreamByID( &pStream, nID );

	if( !pStream )
	{
		return false;
	}

	WCHAR szUIDBuf[255];
	StringFromGUID2(* pStreamUID, szUIDBuf, 255 );
	m_wndStream.SetStream(	boost::shared_ptr<CHCS::IStream>( pStream, boost::mem_fn( &CHCS::IStream::Release ) ),
							szUIDBuf
						);

	return true;
}
catch (std::exception&) 
{
	return false;
}

std::string InstallerIml::GetArchivePath()
try
{
	std::string sArchivePath;
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );
#if OLD
	pConfig->GetValue( NULL, _T("ArchivePath"), sArchivePath );
	
#else
	sArchivePath = pVideoConfig->GetArchivePath();
#endif
	return sArchivePath;
}
catch ( std::exception& ) 
{
	return std::string();
}



bool		 InstallerIml::SetArchivePath( const std::string& sArchivePath )
try
{
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );
	pVideoConfig->SetArchivePath( sArchivePath );
	return true;
}
catch (std::exception&) 
{
	return false;
}

void	InstallerIml::DeleteAllDevicesAndCameras()
try
{
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> pVideoConfig( CreateVideoSystemConfig() );
#if OLD
	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

	std::vector<int> CamIDArrFromOneDevice;
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		pVideoConfig->DeleteDevice(  AllDevices[idxDevice].c_str() );
	}
#else
	pVideoConfig->DeleteAllDevicesAndCameras();
#endif
}
catch (std::exception&) 
{
}

boost::shared_ptr<CHCS::IVideoSystemConfigEx> InstallerIml::CreateVideoSystemConfig()
{
	CHCS::IVideoSystemConfigEx* pConfig = 0;
	if( m_bUseBridge && m_pBridgeDynamicLibrary.get() )
	{
		USES_CONVERSION;
		pConfig = m_pBridgeDynamicLibrary->CreateVideoSystemConfig( m_ServerIPAddress.c_str() );
	}
	else if( m_pVideoDynamicLibrary.get() )
	{
		pConfig = m_pVideoDynamicLibrary->CreateVideoSystemConfig(0);
	}
	return boost::shared_ptr<CHCS::IVideoSystemConfigEx>( pConfig );
}

//////////////////////////////////////////////////////////////////////////


