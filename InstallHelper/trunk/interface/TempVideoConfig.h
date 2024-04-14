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
#ifndef _TEMP_VIDEO_CONFIG_7739020434536093_
#define _TEMP_VIDEO_CONFIG_7739020434536093_



class LocalRegister : public	CHCS::IDeviceRegistrar
{
	std::vector<std::tstring>	m_DeviceMonikers;

	virtual void __stdcall	 RegisterLocalDevice( LPCWSTR szMoniker, LPCWSTR szFriendlyName )
	{
		m_DeviceMonikers.push_back( static_cast<LPCTSTR>( CW2CT( szMoniker ) ) );
	}
public:
	const std::vector<std::tstring>&	GetDeviceMonikers()
	{
		return m_DeviceMonikers;
	}
};

inline UUID GenerateDeviceUID()
{
	UUID NewDeviceUID;
	UuidCreate( &NewDeviceUID );
	return NewDeviceUID;
};

class	LocalVideoSystemConfig : public CHCS::IVideoSystemConfig
{
	std::vector<CHCS::IVideoSystemConfig::DeviceSettings>	m_LocalDeviceSettings;
	std::vector<CHCS::IVideoSystemConfig::CameraSettings>	m_LocalCameraSettings;

	int m_CameraID ;
public:
	LocalVideoSystemConfig( );

	CHCS::IVideoSystemConfig::DeviceSettings				AddDevice( const std::tstring& sMoniker, const UUID& DeviceUID = GenerateDeviceUID() );

	virtual void		 GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params );
	virtual void		 GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params);
private:
	virtual std::tstring GetArchivePath() ;
	virtual void		 SetArchivePath(const std::tstring& sArchivePath)	;

	virtual void		 GetCodecSettings(IVideoSystemConfig::CodecSettings& params) ;
	virtual void		 SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) ;
public:
};

#endif // _TEMP_VIDEO_CONFIG_7739020434536093_