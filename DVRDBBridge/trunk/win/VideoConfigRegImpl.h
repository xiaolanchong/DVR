//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса выдачи списка камер на БД

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   02.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_CONFIG_IMPL_2047597175875570_
#define _VIDEO_CONFIG_IMPL_2047597175875570_

#include "../interface/IVideoConfig.h"
#include "../common/OwnIPAddress.h"

//======================================================================================//
//                                class VideoConfigRegImpl                                 //
//======================================================================================//

//! \brief Реализация интерфейса выдачи списка камер на БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   02.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class VideoConfigRegImpl : public DBBridge::IVideoConfig
{
	WinSock wsa;
public:
	VideoConfigRegImpl();
	virtual ~VideoConfigRegImpl();
private:
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) ;
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue ) ;
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue );

	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue) ;
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue ) ;
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue );

	virtual void						GetDevices( std::vector<std::tstring>& DeviceArr ) ;
	virtual DBBridge::IDeviceConfig*	GetDevice( LPCTSTR szUID  );
	virtual DBBridge::IDeviceConfig*	CreateDevice( LPCTSTR szUID );
	virtual void						DeleteDevice( LPCTSTR szUID );

	//////////////////////////////////////////////////////////////////////////

	boost::shared_ptr<CRegKey>	m_pkeyMain;

	boost::shared_ptr<CRegKey>	OpenOrCreateKey( LPCTSTR szKeyName );
	boost::shared_ptr<CRegKey>	OpenKey( LPCTSTR szKeyName );

//	static void	GetCameraMap( CRegKey& regDevices, std::map< int, std::tstring >& CameraMap ) ;
public:
	static std::tstring GetRegistryPath() ;
	static std::tstring GetDevicePath()
	{
		std::tstring sMainPath = GetRegistryPath();
		sMainPath += _T("\\Devices");
		return sMainPath;
	}
//	static std::tstring GetOwnIPAddress() ;
};

#endif // _VIDEO_CONFIG_IMPL_2047597175875570_