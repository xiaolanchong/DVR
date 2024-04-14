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

#include "stdafx.h"
#include "CommonUtility.h"
#include "OwnIPAddress.h"

void	FromCRTTimeToSystemTime( time_t CRTTime, SYSTEMTIME& st )
{
	CTime t( CRTTime  );
	t.GetAsSystemTime( st );
}

time_t  FromSystemTimeToCRTTime( const SYSTEMTIME& st )
{
	CTime t( st  );
	return t.GetTime( );
}

void	GetServerCameraList(DBBridge::IVideoConfig* pVideoConfig, 
							std::vector<int>& CamLocalArr, 
							std::vector<int>& CamAllArr  )
{
	CamAllArr.clear();
	CamLocalArr.clear();
#if 0 // old code
	std::vector<std::tstring> DeviceFullArr;
	pVideoConfig->GetDevices( DeviceFullArr );

	std::locale loc1 ( "English_US" );

	WinSock _wsa;
	std::string sOwnIP = ::GetOwnIPAddress();
	std::use_facet<std::ctype<char> > ( loc1 ).tolower( &sOwnIP[0], &(*(sOwnIP.end() - 1))  ) ;

	for ( size_t i = 0; i < DeviceFullArr.size(); ++i )
	{
		const std::tstring DeviceUID& = DeviceFullArr[i];
		boost::shared_ptr<DBBridge::IDeviceConfig> pDeviceCfg ( pVideoConfig->GetDevice( DeviceUID.c_str() ) );
		std::string sCameraIP = pCameraCfg->GetIPAddress();
		std::use_facet<std::ctype<char> > ( loc1 ).tolower ( &sCameraIP[0], & (*(sCameraIP.end() - 1) ) );
		if( sCameraIP == sOwnIP ) 
		{
			CamLocalArr.push_back( nCameraID );
		}
		CamAllArr.push_back( nCameraID );
	}
#else
	GetAllCameras( pVideoConfig, CamAllArr );
	CamLocalArr = CamAllArr;
#endif
}


std::string GetTimeForSql( time_t nTime)
{
	struct tm SqlTime;
	localtime_s( &SqlTime, &nTime );
	// OLD: SqlTime = *localtime( &nTime );
	char buf[255];
	strftime( buf, 255, "%Y-%m-%d %H:%M:%S", &SqlTime );
	return std::string(buf);
}

namespace DBBridge
{





class AddIDToArr
{
	std::vector<int>& m_Arr;
public:
	AddIDToArr( std::vector<int>& Arr ) : m_Arr(Arr){}

	void operator() ( ICameraConfig* pCam, IDeviceConfig* )
	{
		m_Arr.push_back( pCam->GetCameraID() );
	}
};

class AddIDToArr2
{
	std::vector< std::pair<int, std::string> >& m_Arr;
public:
	AddIDToArr2( std::vector<std::pair<int, std::string> >& Arr ) : m_Arr(Arr){}

	void operator() ( ICameraConfig* pCam , IDeviceConfig*)
	{
		int ID = pCam->GetCameraID();
		std::tstring s;
		try
		{
			s = pCam->GetDescription();
		}
		catch ( std::exception& ) 
		{
			s.clear();
		}
		m_Arr.push_back( std::make_pair( ID, s ) );
	}
};


void	GetAllCameras( IVideoConfig* pVideoConfig, std::vector<int>& CamIDArr )
{
	CamIDArr.clear();
	AddIDToArr pred(CamIDArr);
	IterateCameras( pVideoConfig, pred );
}

void	GetAllCamerasWithName( IVideoConfig* pVideoConfig, std::vector<std::pair<int, std::string> >& CamIDArr )
{
	CamIDArr.clear();
	AddIDToArr2 pred(CamIDArr);
	IterateCameras( pVideoConfig, pred );
}
/* // OLD CODE
ICameraConfig* GetCamera(IVideoConfig* pVideoConfig, int CameraID)
{

	return 0;
}*/

}
