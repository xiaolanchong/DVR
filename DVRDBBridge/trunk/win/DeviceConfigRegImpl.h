//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса на реестре

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DEVICE_CONFIG_REG_IMPL_3248842267024430_
#define _DEVICE_CONFIG_REG_IMPL_3248842267024430_

#include "../interface/IVideoConfig.h"
#include "../common/OwnIPAddress.h"

//======================================================================================//
//                              class DeviceConfigRegImpl                               //
//======================================================================================//

//! \brief Реализация интерфейса на реестре
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class DeviceConfigRegImpl : public DBBridge::IDeviceConfig
{
	virtual void	GetValue( LPCTSTR szName, DWORD& dwValue);
	virtual void	GetValue( LPCTSTR szName, std::tstring& sValue );

	virtual void	SetValue( LPCTSTR szName, DWORD dwValue);
	virtual void	SetValue( LPCTSTR szName, LPCTSTR sValue );	

	virtual std::tstring	GetDeviceUID()
	{
		return m_sUID;
	}

	virtual std::tstring	GetIPAddress();

	virtual void	GetCameras( std::vector<int>& CamIDArr ) ;
	virtual DBBridge::ICameraConfig*	GetCamera( int nCameraID  );
	virtual DBBridge::ICameraConfig*	CreateCamera( int nCameraID, int nPinNumber );
	virtual void			DeleteCamera( int nID );
public:

	DeviceConfigRegImpl( LPCTSTR szPath, LPCTSTR szUID, bool bCreate = false  );
	virtual ~DeviceConfigRegImpl();
private:
	CRegKey	m_keyDevice;

	std::tstring	m_sPath;
	std::tstring	m_sUID;

	std::string GetCameraPath() const
	{
		std::tstring sCameraPath( m_sPath );
		sCameraPath += _T("\\");
		sCameraPath +=  m_sUID;
		return sCameraPath;
	}
};

#endif // _DEVICE_CONFIG_REG_IMPL_3248842267024430_