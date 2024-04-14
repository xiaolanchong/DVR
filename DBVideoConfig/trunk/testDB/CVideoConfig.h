#ifndef _CVIDEOCONFIG_H_
#define _CVIDEOCONFIG_H_

#include "CTypeAccessor.h"
#include "CCategories.h"

class ICameraConfig;
class IDeviceConfig;

class CVideoConfig: public DBBridge::IVideoConfig
{
public:

	CVideoConfig(const std::tstring& sConection, const std::tstring& sComputerIP );
	virtual ~CVideoConfig(void);
	
	//! прочитать параметр типа DWORD
	//! \param szName имя параметра
	//! \param dwValue значение
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue);

	//!  прочитать строковый параметр
	//! \param szPath секция
	//! \param szName имя параметра
	//! \param sValue значение
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue );

	//! прочитать двоичный параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param binValue значение
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue );

	//! установить числовой параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param dwValue значение
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue);

	//! установить строковый параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param sValue значение
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue );

	//! установить двоичный параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param binValue значение
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue );

	//! получить список всех устройств
	//! \param DeviceArr массив идентификаторов
	virtual void GetDevices( std::vector<std::tstring>& DeviceArr );

	//! получить интерфейс камеры
	//! \param nID идентификатор камеры
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual DBBridge::IDeviceConfig*	GetDevice( LPCTSTR szUID  );

	//! создать устройство
	//! \param szUID строковый UID камеры в формате {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual DBBridge::IDeviceConfig*	CreateDevice( LPCTSTR szUID );

	//! уничтожить камеру из конфигурации
	//! \param nID идентификатор камеры
	virtual void DeleteDevice( LPCTSTR szUID );

protected:
	//IP Компьютера
	std::tstring m_sComputerIP;
	//Строка соединения
	std::tstring m_sConectionInfo;
	//Источник данных
	CDataSource m_db;
	//Сеанс
	CSession m_session;
};
#endif