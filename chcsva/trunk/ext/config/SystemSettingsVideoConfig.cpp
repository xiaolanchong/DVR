//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ ÷åðåç âûçîâ CHCS::IVideoSystemConfig íàïðÿìóþ

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "SystemSettingsVideoConfig.h"

using CHCS::IVideoSystemConfig;

void	DumpCodecSettings( const IVideoSystemConfig::CodecSettings& cs )
{
	Elvees::OutputF( Elvees::TInfo, _T("Compressor settings: fourcc=%x fps=%ld MaxSize=%ld Quality=%ld KeyRate=%ld DataRate=%ld Size=%u"), 
		DWORD(cs.m_CodecFCC	),
		LONG(cs.m_lVideoFPM/60	),
		LONG(cs.m_lVideoMaxSize	),		
		LONG(cs.m_lCodecQuality	),		
		LONG(cs.m_lCodecKeyRate	),			
		LONG(cs.m_lCodecDataRate ),			
		DWORD(cs.m_CodecStateData.size()
		)	
	);
}

//======================================================================================//
//                           class SystemSettingsVideoConfig                            //
//======================================================================================//

SystemSettingsVideoConfig::SystemSettingsVideoConfig(CHCS::IVideoSystemConfig* pVideoConfig)
try
:
	m_pVideoConfig(pVideoConfig),
	m_bInitSuccess(false)
{
	if( !m_pVideoConfig ) 
	{
		return;
	}

	IVideoSystemConfig::CodecSettings codec;
	m_pVideoConfig->GetCodecSettings(codec);

	try
	{
		m_StorageSettings.m_stBasePath		=	m_pVideoConfig->GetArchivePath();
	}
	catch (std::exception& )
	{
		 m_StorageSettings.m_stBasePath.clear();
	}
	m_StorageSettings.m_CodecFCC		= codec.m_CodecFCC;
	m_StorageSettings.m_lVideoFPM		= codec.m_lVideoFPM;
	m_StorageSettings.m_lVideoMaxSize	= codec.m_lVideoMaxSize;			
	m_StorageSettings.m_lCodecQuality	= codec.m_lCodecQuality;			
	m_StorageSettings.m_lCodecKeyRate	= codec.m_lCodecKeyRate;			
	m_StorageSettings.m_lCodecDataRate	= codec.m_lCodecDataRate;			
	m_StorageSettings.m_CodecStateData	= codec.m_CodecStateData;

/* test,	m_StorageSettings.m_CodecStateData.clear(); */
	DumpCodecSettings( codec );

	std::vector<IVideoSystemConfig::DeviceSettings>	ds;
	m_pVideoConfig->GetLocalDeviceSettings( ds );
	
	m_LocalDevices.resize( ds.size() );
	for ( size_t i = 0; i < ds.size(); ++i )
	{
		const IVideoSystemConfig::DeviceSettings& src = ds[i];
		DeviceSettings&	dst = m_LocalDevices[i];

		dst.streamID	= src.streamID;				//!< Unique stream ID
		dst.stMoniker	= src.stMoniker;		//!< Device moniker - certaing string which uniquelly 
		dst.lWidth		= src.lWidth;		//! Width - pixels
		dst.lHeight		= src.lHeight;		//! Height - pixels
		dst.lFrmRate	= src.lFrmRate;		//! Frame rate - fps
		dst.lVStandard	= src.lVStandard;	//! Video Standart (see DX documentation)
		dst.bBWMode		= src.bBWMode;		//!< true - blackwhite, false - colour video
	}

	std::vector<IVideoSystemConfig::CameraSettings>	cs;
	m_pVideoConfig->GetCameraSettings(cs);

	m_Cameras.resize( cs.size() );

	for ( size_t i = 0; i < cs.size(); ++i )
	{
		const IVideoSystemConfig::CameraSettings& src = cs[i];
		CameraSettings&	dst = m_Cameras[i];

		dst.m_lCameraID		=	src.m_lCameraID;

		UUID MutableUID = src.m_DeviceUID;
		UuidString stUID;
		UuidToString( &MutableUID, &stUID );
		std::tstring s( (const TCHAR*)stUID );
		RpcStringFree( &stUID );

		dst.m_stCameraUID		=	s;
		dst.m_stCameraIP	=	src.m_stCameraIP;
		dst.m_lDevicePin	=	src.m_lDevicePin;
	}

	m_bInitSuccess = true;
}
catch ( std::exception& ) 
{
	m_bInitSuccess = false;
	throw;
};


SystemSettingsVideoConfig::~SystemSettingsVideoConfig()
{
}

bool SystemSettingsVideoConfig::LoadStorageSettings( StorageSystemSettings& param )
{
	param = m_StorageSettings;
	return m_bInitSuccess;	
}

bool SystemSettingsVideoConfig::LoadLocalDeviceSettings( std::vector<DeviceSettings>& param)
{
	param = m_LocalDevices;
	return m_bInitSuccess;
}


bool SystemSettingsVideoConfig::LoadCameraSettings( bool bShowDialogOnFailed, std::vector<CameraSettings>& param )
{
	param = m_Cameras;
	return m_bInitSuccess;
}

class FindCamera : public std::unary_function<bool, const CameraSettings& >
{
	const long	m_StreamID;
public:
	bool operator() ( const CameraSettings& cs ) const
	{
		return m_StreamID == cs.m_lCameraID;
	}
	FindCamera( long StreamID ) : 
		m_StreamID(StreamID)
	{

	}
};

ULONG	SystemSettingsVideoConfig::GetIPAddressOfStreamServer( long StreamID )
{
	std::vector<CameraSettings>::const_iterator it = std::find_if( 
		m_Cameras.begin(), m_Cameras.end(), FindCamera( StreamID ) 
		);
	if(  it != m_Cameras.end() )
	{
		USES_CONVERSION;
		return inet_addr( CT2CA( it->m_stCameraIP.c_str() ) );
	}
	else
	{
		return INADDR_NONE;
	}
}