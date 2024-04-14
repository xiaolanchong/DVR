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

#include "chcs.h"
#include "DeviceRegistrar.h"
#include "../config/SystemSettings.h"
#include "CaptureServer.h"

//======================================================================================//
//                                class DeviceRegistrar                                 //
//======================================================================================//

DeviceRegistrar::DeviceRegistrar()
{
}

DeviceRegistrar::~DeviceRegistrar()
{
}

void DeviceRegistrar::FindAndInstallDevices(SystemSettings& ss)
{
	Elvees::Output(Elvees::TInfo, TEXT("> Searching for new devices"));
	CaptureServer CapServer;

	std::vector< std::pair<std::tstring, std::tstring > > MonikersAndFriendlyNamesForAllDevices;
	CapServer.GetAllDevicesInSystem( MonikersAndFriendlyNamesForAllDevices );

	for ( size_t i  = 0; i < MonikersAndFriendlyNamesForAllDevices.size(); ++i )
	{
		const std::tstring & stFriendlyName = MonikersAndFriendlyNamesForAllDevices[i].second;
		const std::tstring & stMoniker		= MonikersAndFriendlyNamesForAllDevices[i].first;

		Elvees::OutputF(Elvees::TInfo, TEXT("New device \"%s\" found"), stFriendlyName.c_str() );

		UUID streamID = GUID_NULL;
		::CoCreateGuid(&streamID);
		RegisterDevice(ss, streamID, stMoniker.c_str());
	}
}

bool DeviceRegistrar::RegisterDevice(SystemSettings& ss, const UUID& streamID, LPCTSTR stMoniker)
{
	return ss.RegisterLocalDevice( streamID, stMoniker );
}