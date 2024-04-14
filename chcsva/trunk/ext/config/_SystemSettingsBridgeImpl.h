//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация через вызов DVRDBBridge напрямую

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SYSTEM_SETTINGS_BRIDGE_IMPL_6022779111741343_
#define _SYSTEM_SETTINGS_BRIDGE_IMPL_6022779111741343_

#include "SystemSettings.h"

//======================================================================================//
//                            class SystemSettingsBridgeImpl                            //
//======================================================================================//

//! \brief реализация через вызов DVRDBBridge напрямую
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   13.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class SystemSettingsBridgeImpl : public SystemSettings
{
public:
	SystemSettingsBridgeImpl();
	virtual ~SystemSettingsBridgeImpl();
private:
	virtual bool LoadStorageSettings( StorageSystemSettings& param );

	virtual void SaveStorageSettings( LPCTSTR stBasePath, LPCTSTR stDBInitString);

	virtual bool LoadLocalDeviceSettings( std::vector<DeviceSettings>& param);

	/*virtual*/ bool RegisterLocalDevice( const UUID& streamID, LPCTSTR stMoniker ); 

	virtual bool LoadCameraSettings( bool bShowDialogOnFailed, std::vector<CameraSettings>& param ); 

	virtual ULONG	GetIPAddressOfStreamServer( LPCTSTR stDBInitString, long StreamID );
};

#endif // _SYSTEM_SETTINGS_BRIDGE_IMPL_6022779111741343_