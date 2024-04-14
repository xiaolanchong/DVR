//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: classes for chcsva in-place configuration

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   15.09.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "TempVideoConfig.h"

// סמחהאול ןמ 4 ךאלונû םא ךאזהמו ףסענמיסעגמ
const int c_MaxCameraNumberForOneDevice = 4;

std::vector<std::tstring> Devices;
int nCameraIDCounter = 1;

using CHCS::IVideoSystemConfig;

void	GenerateDeviceSettings( const std::tstring& sMoniker, const UUID& DeviceUID, IVideoSystemConfig::DeviceSettings& ds  )
{


	ds.streamID		= DeviceUID;
	ds.stMoniker	= sMoniker;
	// default settings
	ds.lWidth		= 352;
	ds.lHeight		= 288;
	ds.bBWMode		= false;
	ds.lVStandard	= 16;
	ds.lFrmRate		= 25;
}


void	GenerateCamerasForDevice(
				 const UUID& DeviceUID, 
				 std::vector<CHCS::IVideoSystemConfig::CameraSettings>& CamSettingsForDevice,
				 int& CameraID
				 )
{
	CamSettingsForDevice.clear();

	for ( int PinNumber = 0; PinNumber < c_MaxCameraNumberForOneDevice; ++PinNumber )
	{
		UUID NewCameraUID = DeviceUID;
		BYTE v = NewCameraUID.Data4[7];
		v &= 0xF0;
		v |=  static_cast<BYTE>(PinNumber) & 0xF;
		NewCameraUID.Data4[7] = v;

		CHCS::IVideoSystemConfig::CameraSettings cs;

		cs.m_lCameraID	= CameraID ++;
		cs.m_DeviceUID	= DeviceUID;
		cs.m_lDevicePin	= PinNumber;
		//don't care
		cs.m_stCameraIP = _T("");

		CamSettingsForDevice.push_back( cs );
	}
}

LocalVideoSystemConfig::LocalVideoSystemConfig(  ):
	m_CameraID(1)
{
}

CHCS::IVideoSystemConfig::DeviceSettings LocalVideoSystemConfig::AddDevice( const std::tstring& sMoniker, const UUID& DeviceUID )
{
	
	std::vector<CHCS::IVideoSystemConfig::CameraSettings>	CamSettingsForDevice;
	CHCS::IVideoSystemConfig::CameraSettings cs;
	CHCS::IVideoSystemConfig::DeviceSettings ds;

	GenerateDeviceSettings( sMoniker, DeviceUID, ds );
	GenerateCamerasForDevice( DeviceUID, CamSettingsForDevice, m_CameraID );
	m_LocalCameraSettings.insert( m_LocalCameraSettings.end(), CamSettingsForDevice.begin(), CamSettingsForDevice.end() );

	m_LocalDeviceSettings.push_back( ds );
	return ds;
}

void		 LocalVideoSystemConfig::GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params )
{
	params = m_LocalDeviceSettings;
}

void		 LocalVideoSystemConfig::GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params)
{
	params	= m_LocalCameraSettings;
}

std::tstring LocalVideoSystemConfig::GetArchivePath()
{
	// not implemented
	return std::tstring();
}

void		 LocalVideoSystemConfig::SetArchivePath(const std::tstring& sArchivePath)
{
	// not implemented
}


void		 LocalVideoSystemConfig::GetCodecSettings(IVideoSystemConfig::CodecSettings& params)
{
	// not implemented
}

void		 LocalVideoSystemConfig::SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) 
{
	// not implemented
}