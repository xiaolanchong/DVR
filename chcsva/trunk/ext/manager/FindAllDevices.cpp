//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: 

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "CaptureServer.h"
#include "../IVideoSystemConfig.h"

namespace CHCS
{

using std::min;

CHCSVA_API bool		__stdcall RegisterDevices( CHCS::IDeviceRegistrar* pRegistrar )
{
	if(  !pRegistrar )
	{
		return false;
	}
	USES_CONVERSION;

	HRESULT hr = CoInitialize(NULL);
	CaptureServer CapServer;

	std::vector< std::pair<std::tstring, std::tstring > > MonikersAndFriendlyNamesForAllDevices;
	CapServer.GetAllDevicesInSystem( MonikersAndFriendlyNamesForAllDevices );

	const size_t TrueSize =  MonikersAndFriendlyNamesForAllDevices.size();

	for ( size_t i  = 0; i < TrueSize; ++i )
	{
		const std::tstring & stFriendlyName = MonikersAndFriendlyNamesForAllDevices[i].second;
		const std::tstring & stMoniker		= MonikersAndFriendlyNamesForAllDevices[i].first;

		pRegistrar->RegisterLocalDevice( CT2CW( stMoniker.c_str() ), CT2CW( stFriendlyName.c_str() ) );
	}

	if( S_OK == hr )
	{
		CoUninitialize();
	}

	return true;
}

}