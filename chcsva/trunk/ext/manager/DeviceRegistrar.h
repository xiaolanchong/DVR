//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Убнаружение всех устройств захвата и регистрация их в системе
//	поскольку, как правило, сначала надо сконфигурировать устройство захвата
//	(редко когда используются все пины), эта часть вынесена из ServerManager/CManager.
//	Т.е. необходим отдельный запуск InitStreamManager

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DEVICE_REGISTRAR_5447491258109570_
#define _DEVICE_REGISTRAR_5447491258109570_

class SystemSettings;

//======================================================================================//
//                                class DeviceRegistrar                                 //
//======================================================================================//

//! \brief Убнаружение всех устройств захвата и регистрация их в системе
//	поскольку, как правило, сначала надо сконфигурировать устройство захвата
//	(редко когда используются все пины), эта часть вынесена из ServerManager/CManager.
//	Т.е. необходим отдельный запуск InitStreamManager
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   01.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class DeviceRegistrar
{
	bool RegisterDevice(SystemSettings& ss, const UUID& streamID, LPCTSTR stMoniker);
public:
	DeviceRegistrar();
	virtual ~DeviceRegistrar();

	void FindAndInstallDevices(SystemSettings& ss);
};

#endif // _DEVICE_REGISTRAR_5447491258109570_