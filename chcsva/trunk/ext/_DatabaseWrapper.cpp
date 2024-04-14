//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Обертка над БД - эмуляция взаимодействиея с OLE DB через IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "DatabaseWrapper.h"

void	GetAllCameraInfo(DBBridge::IVideoConfig* pVideoConfig, std::vector<CameraInfo_t>& Cameras)
{
	 if( !pVideoConfig)
	 {
		 return;
	 }

	 std::vector<std::tstring>	AllDevices;
	 pVideoConfig->GetDevices( AllDevices );

	 std::vector<int> CamIDArrFromOneDevice;
	 for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	 {
		 CamIDArrFromOneDevice.clear();

		 try
		 {
		 std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig ( pVideoConfig->GetDevice( AllDevices[idxDevice].c_str() ) );
		 pDeviceConfig->GetCameras( CamIDArrFromOneDevice );
		 
		 for ( size_t idxCamera = 0; idxCamera < CamIDArrFromOneDevice.size(); ++idxCamera )
		 {
			 try
			 {
			 
				std::auto_ptr<DBBridge::ICameraConfig> pCameraConfig( pDeviceConfig->GetCamera( CamIDArrFromOneDevice[idxCamera] ) );
				Cameras.push_back( boost::make_tuple( 
													pCameraConfig->GetCameraID(), 
													pDeviceConfig->GetDeviceUID(),
													pCameraConfig->GetCameraPin(),
													pDeviceConfig->GetIPAddress() 
													) 
												);
			 }
			 catch( DBBridge::IVideoConfig::VideoConfigException& )
			 {
				 // error get camera parameters, just skip
			 }
		 }
		 }
		 catch (DBBridge::IVideoConfig::VideoConfigException& ) 
		 {
			 // error get device parameters, just skip
		 }
	 }
}