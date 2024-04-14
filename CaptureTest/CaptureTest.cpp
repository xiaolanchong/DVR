// CaptureTest.cpp : Defines the entry point for the console application.
//

#include "chcs.h"
#include <conio.h>

#include "../chcsva/trunk/ext/manager/SystemSettings.h"
#include "../chcsva/trunk/ext/manager/CaptureServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

	CaptureServer cs;

	std::vector< std::pair<std::tstring, std::tstring> >	MonikersAndNames;
	cs.GetAllDevicesInSystem( MonikersAndNames );

	std::vector<DeviceSettings> Devices;
	for ( size_t i = 0; i < MonikersAndNames.size(); ++i )
	{
		UUID StreamUID;
		TCHAR buf[255];
		UuidCreate( &StreamUID );
		UuidFromString( reinterpret_cast<UuidString>( buf ), &StreamUID );
			
		DeviceSettings ds;
		ds.stMoniker	= MonikersAndNames[i].first;
		ds.lFrmRate		= 2;
		ds.lWidth		= 352;
		ds.lHeight		= 288;
		ds.bBWMode		= false;
		ds.lVStandard	= 16;
		ds.stKeyName	= MonikersAndNames[i].second;
		ds.streamID		= StreamUID;

		Devices.push_back( ds );
	}

	cs.CreateDevices( Devices );

	_getch();

	cs.DeleteAllDevices();
	cs.DeleteAllPins();

	CoUninitialize();

	return 0;
}

