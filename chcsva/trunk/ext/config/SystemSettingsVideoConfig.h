//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация через вызов CHCS::IVideoSystemConfig напрямую

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SYSTEM_SETTINGS_VIDEO_CONFIG_1483681618862953_
#define _SYSTEM_SETTINGS_VIDEO_CONFIG_1483681618862953_

#include "SystemSettings.h"
#include "../IVideoSystemConfig.h"

//======================================================================================//
//                           class SystemSettingsVideoConfig                            //
//======================================================================================//

//! \brief реализация через вызов CHCS::IVideoSystemConfig напрямую
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   13.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class SystemSettingsVideoConfig : public SystemSettings
{
	CHCS::IVideoSystemConfig* m_pVideoConfig;

	StorageSystemSettings			m_StorageSettings;
	std::vector<DeviceSettings>		m_LocalDevices;
	std::vector<CameraSettings>		m_Cameras;

	bool							m_bInitSuccess;
public:
	SystemSettingsVideoConfig(CHCS::IVideoSystemConfig* pVideoConfig);
	virtual ~SystemSettingsVideoConfig();

	virtual bool LoadStorageSettings( StorageSystemSettings& param );

	virtual void SaveStorageSettings( LPCTSTR stBasePath )
	{

	}

	virtual bool LoadLocalDeviceSettings( std::vector<DeviceSettings>& param);

//	virtual bool RegisterLocalDevice( const UUID& streamID, LPCTSTR stMoniker ); 

	virtual bool LoadCameraSettings( bool bShowDialogOnFailed, std::vector<CameraSettings>& param ); 

	virtual ULONG	GetIPAddressOfStreamServer( long StreamID );

	void InvalidateConfigInterface()
	{
		m_pVideoConfig = NULL;
	}
};

#endif // _SYSTEM_SETTINGS_VIDEO_CONFIG_1483681618862953_