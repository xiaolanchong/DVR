//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация CHCS::IVideoSystemConfig через DBBridge::IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_SYSTEM_CONFIG_BRIDGE_IMPL_5070308918255213_
#define _VIDEO_SYSTEM_CONFIG_BRIDGE_IMPL_5070308918255213_

//======================================================================================//
//                          class VideoSystemConfigBridgeImpl                           //
//======================================================================================//

//! \brief реализация CHCS::IVideoSystemConfig через DBBridge::IVideoConfig
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   13.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class VideoSystemConfigBridgeImpl : public CHCS::IVideoSystemConfigEx
{
	bool			m_OnlyLocalIP;
	std::tstring	m_ServerIPAddress;
public:
	VideoSystemConfigBridgeImpl( LPCWSTR  ServerIPAddress );
	VideoSystemConfigBridgeImpl( bool  OnlyForLocalIP );
	virtual ~VideoSystemConfigBridgeImpl();

private:
	virtual void		 GetCodecSettings(IVideoSystemConfig::CodecSettings& params) ;
	virtual void		 SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) ;

	virtual void		 GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params ) ;
	virtual void		 GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params) ;

	virtual std::tstring GetArchivePath() ;
	virtual void		 SetArchivePath(const std::tstring& sArchivePath)	;

	virtual void	DeleteAllDevicesAndCameras() ;
	virtual void	CreateDevice( const IVideoSystemConfig::DeviceSettings& ds ) ; 
	virtual LONG	CreateCamera( const UUID& DeviceUID, unsigned int PinNumber ) ;
	virtual void	DeleteCamera( LONG CameraID ); 

	std::auto_ptr<DBBridge::IVideoConfig> GetConfigInt() const;

};

#endif // _VIDEO_SYSTEM_CONFIG_BRIDGE_IMPL_5070308918255213_