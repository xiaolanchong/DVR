#ifndef _CDEVICECONFIG_H_
#define _CDEVICECONFIG_H_

#include "CTypeAccessor.h"

class ICameraConfig;

class CDeviceConfig: public DBBridge::IDeviceConfig
{
public:

	CDeviceConfig(CSession& sConection, LPCTSTR szUID);
	virtual ~CDeviceConfig();

	//!  прочитать параметр типа DWORD
	//! \param szName имя параметра
	//! \param dwValue значение
	virtual void GetValue( LPCTSTR szName, DWORD& dwValue);

	//! получить строковый параметр
	//! \param szName имя 
	//! \param sValue значение
	virtual void GetValue( LPCTSTR szName, std::tstring& sValue );

	//! установить числовой параметр 
	//! \param szName имя 
	//! \param dwValue значение
	virtual void SetValue( LPCTSTR szName, DWORD dwValue);

	//! установить строковый параметр
	//! \param szName имя
	//! \param sValue значение
	virtual void SetValue( LPCTSTR szName, LPCTSTR sValue );	

	//! получить IP-адрес сервера камеры
	//! \return адрес в формате xxx.xxx.xxx.xxx
	virtual std::tstring GetIPAddress();

	//! получить UID камеры
	//! \return строковый UID в формате {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	virtual std::tstring GetDeviceUID();

	//! получить список всех камер
	//! \param CamIDArr массив идентификаторов
	virtual void GetCameras( std::vector<int>& CamIDArr );

	//! получить интерфейс камеры
	//! \param nID идентификатор камеры
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual DBBridge::ICameraConfig* GetCamera( int nID  );

	//! создать камеру
	//! \param nPinNumber ноиер пина (0-3) 
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual DBBridge::ICameraConfig* CreateCamera( int nCameraID, int nPinNumber );

	//! уничтожить камеру из устройства
	//! \param nID идентификатор камеры
	virtual void DeleteCamera( int nID );

protected:
	//UID устройства
	std::tstring m_szUID;
	//Сеанс
	CSession m_session;
};
#endif