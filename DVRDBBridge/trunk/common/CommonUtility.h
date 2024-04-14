//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Общие функции режимов работы (реестр, БД)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   20.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _COMMON_UTILITY_1226352356869072_
#define _COMMON_UTILITY_1226352356869072_

#include "../interface/IVideoConfig.h"

void	FromCRTTimeToSystemTime( time_t CRTTime, SYSTEMTIME& st );
time_t  FromSystemTimeToCRTTime( const SYSTEMTIME& st );

void	GetServerCameraList(DBBridge::IVideoConfig* pVideoConfig, 
							std::vector<int>& CamLocalArr, 
							std::vector<int>& CamAllArr  );

//! форматирует время в строку для SQL
//! \param nTime время
//! \return строковое представление '%Y-%m-%d %H:%M:%S'
std::string GetTimeForSql( time_t nTime);

template<typename T> void	IterateCameras( DBBridge::IVideoConfig* pVideoConfig, T &func )
{
	_ASSERTE(pVideoConfig);
	if( !pVideoConfig )
	{
		return;
	}

	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

	std::vector<int> CamIDArrFromOneDevice;
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		CamIDArrFromOneDevice.clear();

		std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig ( pVideoConfig->GetDevice( AllDevices[idxDevice].c_str() ) );
		pDeviceConfig->GetCameras( CamIDArrFromOneDevice );
		//CamIDArr.insert( CamIDArr.end(), CamIDArrFromOneDevice.begin(), CamIDArrFromOneDevice.end() );
		for ( size_t idxCamera = 0; idxCamera < CamIDArrFromOneDevice .size(); ++idxCamera )
		{
			try
			{
				std::auto_ptr<DBBridge::ICameraConfig> pCam ( pDeviceConfig->GetCamera( CamIDArrFromOneDevice[idxCamera] ) );
				func( pCam.get(), pDeviceConfig.get() );
			}
			catch ( std::exception& ) 
			{
			}
		}
	}
}

namespace DBBridge
{

	//! получить все идентификаторы камер на данном компьютере
	//! \param pVideoConfig интерфейс конфигурации
	//! \param CamIDArr		выходной массив идентификаторов
	void	GetAllCameras( IVideoConfig* pVideoConfig, std::vector<int>& CamIDArr );

	void	GetAllCamerasWithName( IVideoConfig* pVideoConfig, std::vector<std::pair<int, std::string> >& CamIDArr );

	//ICameraConfig* GetCamera(IVideoConfig* pVideoConfig, int CameraID);

}

#endif // _COMMON_UTILITY_1226352356869072_