//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ CHCS::IVideoSystemConfig ÷åðåç DBBridge::IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "VideoSystemConfigBridgeImpl.h"
#include "../Common/CommonUtility.h"
#include "../Common/OwnIPAddress.h"
#include "../interface/IDBInstaller.h"

using CHCS::IVideoSystemConfig;

namespace Local
{

std::auto_ptr<DBBridge::IVideoConfig> GetAnyConfigInt()
{
	std::vector<std::tstring > AllIPArr;
	DBBridge::GetAllIPAddresses( AllIPArr );
	if(  !AllIPArr.empty() )
	{
		return std::auto_ptr<DBBridge::IVideoConfig> ( DBBridge::CreateVideoConfig( AllIPArr[0].c_str() ) );
	}
	else
	{
		return std::auto_ptr<DBBridge::IVideoConfig> ();
	}
}

std::auto_ptr<DBBridge::IVideoConfig> GetConfigInt()
{
	std::vector<std::tstring >	AllIPArr, DBAndLocalIPArr;
	DBBridge::GetAllIPAddresses( AllIPArr );

	std::vector<std::tstring >	LocalIPArr = GetOwnIPAddress();

	if( LocalIPArr.empty() )
	{
	//	return std::auto_ptr<DBBridge::IVideoConfig> ( );
		throw  std::runtime_error("Local IP address list is empty");
	}
	else if( AllIPArr.empty() )
	{
		std::tstring IpAddr = LocalIPArr[0];
		return std::auto_ptr<DBBridge::IVideoConfig> ( DBBridge::CreateVideoConfig( IpAddr.c_str() ) );
	}
	else
	{
		std::sort( AllIPArr.begin(), AllIPArr.end() );
		std::sort( LocalIPArr.begin(), LocalIPArr.end() );
		std::set_intersection( 
			AllIPArr.begin(), AllIPArr.end(),
			LocalIPArr.begin(), LocalIPArr.end(),
			std::back_inserter( DBAndLocalIPArr )
			);
		if( DBAndLocalIPArr.empty() )
		{
			throw  std::runtime_error("No local IP are in the database");
		}

		std::tstring IpAddr =
			(  DBAndLocalIPArr.empty() ) ? LocalIPArr[0] : DBAndLocalIPArr[0];
		return std::auto_ptr<DBBridge::IVideoConfig> ( DBBridge::CreateVideoConfig( IpAddr.c_str() ) );
	}
}

std::auto_ptr<DBBridge::IVideoConfig> GetConfigInt(LPCTSTR ServerIPAddress)
{
	return std::auto_ptr<DBBridge::IVideoConfig> ( DBBridge::CreateVideoConfig( ServerIPAddress ) );
}

}

//======================================================================================//
//                          class VideoSystemConfigBridgeImpl                           //
//======================================================================================//

VideoSystemConfigBridgeImpl::VideoSystemConfigBridgeImpl( LPCWSTR ServerIPAddress)
{
	USES_CONVERSION;
	m_ServerIPAddress = ( W2CT( ServerIPAddress ) ) ;
}

VideoSystemConfigBridgeImpl::VideoSystemConfigBridgeImpl( bool  OnlyForLocalIP ): 
	m_OnlyLocalIP( OnlyForLocalIP ) 
{

}

VideoSystemConfigBridgeImpl::~VideoSystemConfigBridgeImpl()
{
}

void		 VideoSystemConfigBridgeImpl::GetCodecSettings(IVideoSystemConfig::CodecSettings& params)
try
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();

	if( !pVideoConfig.get() )
	{
		throw std::runtime_error("Failed to get DBBridge::IVideoConfig interface");
	}

	DWORD Value;

	pVideoConfig->GetValue( _T("Storage"), _T("Codec_DataRate"),	Value	);
	params.m_lCodecDataRate = Value;
	pVideoConfig->GetValue( _T("Storage"), _T("Codec_FCC"),			Value  );
	params.m_CodecFCC = Value;
	pVideoConfig->GetValue( _T("Storage"), _T("Codec_KeyRate"),		Value	);
	params.m_lCodecKeyRate = Value;
	pVideoConfig->GetValue( _T("Storage"), _T("Codec_Quality"),		Value		);
	params.m_lCodecQuality = Value;
	pVideoConfig->GetValue( _T("Storage"), _T("Video_FMP"),			Value		);
	params.m_lVideoFPM = Value;
	pVideoConfig->GetValue( _T("Storage"), _T("Video_MaxSize"),		Value		);
	params.m_lVideoMaxSize = Value;

	std::vector<BYTE> CodecStateData;
	pVideoConfig->GetValue( _T("Storage"), _T("Codec_State"),	params.m_CodecStateData );
}
catch( std::exception& ex )
{
	throw std::runtime_error(ex.what());
};

UUID ConvertToUUID( const std::tstring& sUUID)
{
	TCHAR Buf[256];
	lstrcpyn( Buf, sUUID.c_str(), 256);  

	UUID DeviceUID = {0};
	UuidFromString( (RPC_CSTR)Buf, &DeviceUID);
	return DeviceUID;
}

void		VideoSystemConfigBridgeImpl:: GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params )
try
{
	params.clear();
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt( );

	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

//	params.resize( AllDevices.size() );
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		IVideoSystemConfig::DeviceSettings ds ;

		std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig ( pVideoConfig->GetDevice( AllDevices[idxDevice].c_str() ) );
		std::tstring sUID = AllDevices[idxDevice];
	
		ds.streamID		= ConvertToUUID( sUID ) ;
		std::tstring stMoniker ;
		pDeviceConfig->GetValue(_T("Device_DisplayName"), stMoniker);
		ds.stMoniker	= stMoniker;

		DWORD Value;
		pDeviceConfig->GetValue( _T("Device_Width"),	Value	);
		ds.lWidth		= Value;
		pDeviceConfig->GetValue( _T("Device_Height"),	Value	);
		ds.lHeight		= Value;
		pDeviceConfig->GetValue( _T("Device_BWMode"),	Value	);
		ds.bBWMode		= (Value != 0);
		pDeviceConfig->GetValue( _T("Device_Standard"),	Value	);
		ds.lVStandard	= Value;
		pDeviceConfig->GetValue( _T("Device_FrameRate"),	Value	);
		ds.lFrmRate		= Value;

		params.push_back( ds ); 
	}
}
catch( std::exception& ex )
{
	params.clear();
	throw std::runtime_error(ex.what());
};

class AddSettingsToArray
{
	std::vector<IVideoSystemConfig::CameraSettings>& m_Arr;
	std::set<int>	m_CameraIDSet;
public:
	AddSettingsToArray( std::vector<IVideoSystemConfig::CameraSettings>& Arr ) : m_Arr(Arr){}

	void operator() ( DBBridge::ICameraConfig* pCam, DBBridge::IDeviceConfig* pDevice )
	{
		int CameraID = pCam->GetCameraID();
		if( m_CameraIDSet.find( CameraID ) == m_CameraIDSet.end() )
		{

			IVideoSystemConfig::CameraSettings cs;
			cs.m_DeviceUID	= ConvertToUUID( pDevice->GetDeviceUID() ) ;
			cs.m_lCameraID	= CameraID;
			cs.m_stCameraIP = pDevice->GetIPAddress();
			cs.m_lDevicePin = pCam->GetCameraPin();

			m_Arr.push_back(cs);
			m_CameraIDSet.insert( CameraID );
		}
		else
		{
			// already exists
		}
	}
};

void		 VideoSystemConfigBridgeImpl::GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params)
try
{
	params.clear();
	std::vector<std::tstring > AllIPArr;
	DBBridge::GetAllIPAddresses( AllIPArr );
	AddSettingsToArray Pred(params);
	for ( size_t i =0; i < AllIPArr.size(); ++i )
	{
		std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig( DBBridge::CreateVideoConfig(  AllIPArr[i].c_str() ) );

		IterateCameras( pVideoConfig.get(), Pred  );
	}
}
catch( std::exception& ex )
{
	params.clear();
	throw std::runtime_error(ex.what());
};

std::string GetFullDatabaseName( const std::string& sDirName )
{
	char FullPathName[ MAX_PATH ];
	if( NULL != PathCombineA( FullPathName, sDirName.c_str(), "DVR.db" ) )
	{
		return std::string( FullPathName );
	}
	else
	{
		throw std::runtime_error("Failed to get full database name");
	}
}

void		 VideoSystemConfigBridgeImpl::SetArchivePath(const std::tstring& sArchivePath)
try
{
	boost::shared_ptr<DBBridge::IDBInstaller> pInstall( DBBridge::CreateDBInstaller() );
	// for file database installation
	std::string DatabaseFile = GetFullDatabaseName( sArchivePath ) ;
	pInstall->Install( DatabaseFile.c_str() );

	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();
	pVideoConfig->SetValue( _T(""), _T("ArchivePath"), sArchivePath.c_str() );
}
catch( std::exception& ex )
{
	throw std::runtime_error(ex.what());
};

std::tstring VideoSystemConfigBridgeImpl::GetArchivePath()
try
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();
	std::tstring sArchivePath;
	pVideoConfig->GetValue( 0, _T("ArchivePath"), sArchivePath );
	return sArchivePath;
}
catch( std::exception& ex )
{
	throw std::runtime_error(ex.what());
};


void		 VideoSystemConfigBridgeImpl::SetCodecSettings(const IVideoSystemConfig::CodecSettings& params)
try
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();

	if( !pVideoConfig.get() )
	{
		throw std::runtime_error("Failed to get DBBridge::IVideoConfig interface");
	}

	pVideoConfig->SetValue( _T("Storage"), _T("Codec_DataRate"),	params.m_lCodecDataRate	);
	pVideoConfig->SetValue( _T("Storage"), _T("Codec_FCC"),			params.m_CodecFCC  );
	pVideoConfig->SetValue( _T("Storage"), _T("Codec_KeyRate"),		params.m_lCodecKeyRate	);
	pVideoConfig->SetValue( _T("Storage"), _T("Codec_Quality"),		params.m_lCodecQuality		);
	pVideoConfig->SetValue( _T("Storage"), _T("Video_FMP"),			params.m_lVideoFPM		);
	pVideoConfig->SetValue( _T("Storage"), _T("Video_MaxSize"),		params.m_lVideoMaxSize		);
	pVideoConfig->SetValue( _T("Storage"), _T("Codec_State"),		params.m_CodecStateData );
}
catch( std::exception& ex )
{
	throw std::runtime_error(ex.what());
};

void	VideoSystemConfigBridgeImpl::DeleteAllDevicesAndCameras()
try
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();
	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

	std::vector<int> CamIDArrFromOneDevice;
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		pVideoConfig->DeleteDevice(  AllDevices[idxDevice].c_str() );
	}

	boost::shared_ptr<DBBridge::IDBInstaller> pInstall( DBBridge::CreateDBInstaller() );
	pInstall->Uninstall();
}
catch ( DBBridge::IDBInstaller::DBInstallerException& )  
{
	
};

void	VideoSystemConfigBridgeImpl::CreateDevice( const IVideoSystemConfig::DeviceSettings& ds ) 
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();

	std::tstring sUID = UuidToString( ds.streamID );

	std::auto_ptr< DBBridge::IDeviceConfig > pDeviceConfig( 
		pVideoConfig->CreateDevice( sUID.c_str() )
		);

	pDeviceConfig->SetValue( _T("Device_DisplayName"), ds.stMoniker.c_str() );
	pDeviceConfig->SetValue( _T("Device_Width"),	ds.lWidth	);
	pDeviceConfig->SetValue( _T("Device_Height"),	ds.lHeight	);
	pDeviceConfig->SetValue( _T("Device_BWMode"),	ds.bBWMode	);
	pDeviceConfig->SetValue( _T("Device_Standard"),	ds.lVStandard	);
	pDeviceConfig->SetValue( _T("Device_FrameRate"),ds.lFrmRate	);
}

LONG		VideoSystemConfigBridgeImpl::CreateCamera( const UUID& DeviceUID, unsigned int PinNumber ) 
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig = GetConfigInt();

	std::tstring sUID = UuidToString( DeviceUID );
	std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig( pVideoConfig->GetDevice( sUID.c_str() ) );

	if( !pDeviceConfig.get() )
	{
		return -1;
	}

	std::auto_ptr<DBBridge::ICameraConfig> pCameraConfig( 
		pDeviceConfig->CreateCamera(  0, PinNumber ) 
		);

	return pCameraConfig->GetCameraID();
}

void	VideoSystemConfigBridgeImpl::DeleteCamera( LONG CameraID )
{
	UNREFERENCED_PARAMETER(CameraID);
}

std::auto_ptr<DBBridge::IVideoConfig> VideoSystemConfigBridgeImpl::GetConfigInt() const
{
	
	if( !m_ServerIPAddress.empty() )
	{
		return Local::GetConfigInt( m_ServerIPAddress.c_str() );
	}
	else if( m_OnlyLocalIP )
	{
		return Local::GetConfigInt();
	}
	else
	{
		return Local::GetAnyConfigInt();
	}
}