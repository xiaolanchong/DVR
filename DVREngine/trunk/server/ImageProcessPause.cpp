//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получить задержку в мс между обработкой изображений в потоке камеры

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   15.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ImageProcessPause.h"
/*
//======================================================================================//
//                               class ImageProcessPause                                //
//======================================================================================//

ImageProcessPause::ImageProcessPause()
{
}

ImageProcessPause::~ImageProcessPause()
{
}*/

unsigned int	GetApproxFunctionValue( size_t MaxCameraOnDevice, size_t DeviceNumber )
{
	_ASSERTE( MaxCameraOnDevice );
	_ASSERTE( DeviceNumber );
	if( !MaxCameraOnDevice  || ! DeviceNumber)
	{
		return 0;
	}
	--MaxCameraOnDevice;
	--DeviceNumber;
	const size_t c_MaxDevices		  = 16;
	const size_t c_MaxCamerasOnDevice = 4;
	const int FpsTable[c_MaxCamerasOnDevice][c_MaxDevices] =
	{
		{  -1, -1, -1,  -1, 10, 10, 10, 10, 4, 4, 4, 4, 2, 2, 2, 2 },
		{  10, 10, 10,  10, 4,   4,  4,  4, 2, 2, 2, 2, 2, 2, 2, 2 },
		{  4,  4,   4,   4,  2,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2 },
		{  2,  2,   2,   2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2 }
	};

	if( MaxCameraOnDevice > c_MaxCamerasOnDevice || 
		DeviceNumber > c_MaxDevices )
	{
		return 1000/2 - 100;
	}
	else if( FpsTable[ MaxCameraOnDevice ][ DeviceNumber ] < 0) 
	{
		return 0;
	}
	else 
	{
		return 1000/FpsTable[ MaxCameraOnDevice ][ DeviceNumber ] - 30;
	}
}

unsigned int	GetImageProcessPause()
try
{
	std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig( DBBridge::CreateVideoConfig() );

	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

	std::vector<int> CamIDArrFromOneDevice;

	const size_t DeviceNumber = AllDevices.size();
	size_t MaxCamerasOnDevice = 0;
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig ( pVideoConfig->GetDevice( AllDevices[idxDevice].c_str() ) );
		pDeviceConfig->GetCameras( CamIDArrFromOneDevice );

		MaxCamerasOnDevice = std::max( MaxCamerasOnDevice, CamIDArrFromOneDevice.size() );
	}

/*	if( MaxCameraOnDevice > 2 )
	{
		return 1000/2;
	}
	else */
	return GetApproxFunctionValue( MaxCamerasOnDevice, DeviceNumber );
}
catch( std::exception& )
{
	return 0;
};