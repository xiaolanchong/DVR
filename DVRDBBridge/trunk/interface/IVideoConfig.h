 //======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Video configuration interface - интерфейс для данных по видеосисстемы 
				(камеры, параметры изображения, кодека и т.д.)

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   31.10.2005
*/                                                                                      //
//======================================================================================//
#ifndef _I_VIDEO_CONFIG_1886981946031052_
#define _I_VIDEO_CONFIG_1886981946031052_

#ifndef UNIX_RELEASE
	#ifdef _UNICODE
		#define tstring wstring
	#else
		#define tstring string
	#endif
#else
	#define tstring string
#endif

namespace DBBridge
{

class ICameraConfig;
class IDeviceConfig;

//======================================================================================//
//                                class IVideoConfig                                   //
//======================================================================================//

//! \brief интерфейс для получения конфигурации видеосистемы
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class IVideoConfig
{
public:
	template <class T> class BaseException : public T 
	{
	public:
		BaseException( const char* p ) : T(p){}
		BaseException( const std::string& p ) : T(p){}
	};
	//! базовый класс исключения для интерфейса
	typedef BaseException<std::runtime_error>  VideoConfigException;
	//! нет такого параметра, для GetValue
	typedef BaseException<VideoConfigException>  NoSuchProperty;
	//! ошибка при чтении, для SetValue
	typedef BaseException<VideoConfigException>  InvalidProperty;
	//! ошибка при выполнении операции
	typedef BaseException<VideoConfigException>  InitFailed;

	virtual ~IVideoConfig() {};

	//! прочитать параметр типа DWORD
	//! \param szPath секция(каталог параметров), поддерживаемые секции 
	//!			\li NULL or ""	- global
	//!			\li "Storage"	- coder settings 
	//! \param szName имя параметра
	//! \param dwValue значение
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) = 0;

	//!  прочитать строковый параметр
	//! \param szPath секция
	//! \param szName имя параметра
	//! \param sValue значение
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue ) = 0;

	//! прочитать двоичный параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param binValue значение
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue ) = 0;

	//! установить числовой параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param dwValue значение
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue) = 0;

	//! установить строковый параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param sValue значение
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue ) = 0;

	//! установить двоичный параметр
	//! \param szPath секция
	//! \param szName имя
	//! \param binValue значение
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue ) = 0;

	//! получить список всех камер
	//! \param CamIDArr массив идентификаторов
	virtual void	GetDevices( std::vector<std::tstring>& DeviceArr ) = 0;

	//! получить интерфейс камеры
	//! \param nID идентификатор камеры
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual IDeviceConfig*	GetDevice( LPCTSTR szUID  ) = 0;

	//! создать устройство
	//! \param szUID строковый UID камеры в формате {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual IDeviceConfig*	CreateDevice( LPCTSTR szUID ) = 0;

	//! уничтожить камеру из конфигурации
	//! \param nID идентификатор камеры
	virtual void			DeleteDevice( LPCTSTR szUID ) = 0;

};

//! \brief конфигурация устройства
//! \version 1.0
//! \date 05-30-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class IDeviceConfig
{
public:
	virtual ~IDeviceConfig(){}

	//!  прочитать параметр типа DWORD
	//! \param szName имя параметра
	//! \param dwValue значение
	virtual void	GetValue( LPCTSTR szName, DWORD& dwValue) = 0;

	//! получить строковый параметр
	//! \param szName имя 
	//! \param sValue значение
	virtual void	GetValue( LPCTSTR szName, std::tstring& sValue ) = 0;

	//! установить числовой параметр 
	//! \param szName имя 
	//! \param dwValue значение
	virtual void	SetValue( LPCTSTR szName, DWORD dwValue) = 0;

	//! установить строковый параметр
	//! \param szName имя
	//! \param sValue значение
	virtual void	SetValue( LPCTSTR szName, LPCTSTR sValue ) = 0;	

	//! получить IP-адрес сервера камеры
	//! \return адрес в формате xxx.xxx.xxx.xxx
	virtual std::tstring	GetIPAddress() = 0;

	//! получить UID камеры
	//! \return строковый UID в формате {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	virtual std::tstring	GetDeviceUID() = 0;

//----------------------------------------------------------//

	//! получить список всех камер
	//! \param CamIDArr массив идентификаторов
	virtual void	GetCameras( std::vector<int>& CamIDArr ) = 0;

	//! получить интерфейс камеры
	//! \param nID идентификатор камеры
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual ICameraConfig*	GetCamera( int nID  ) = 0;

	//! создать камеру
	//! \param nPinNumber ноиер пина (0-3) 
	//! \return указатель на интерфейс, вызвать delete при уничожении
	virtual ICameraConfig*	CreateCamera( int nCameraID, int nPinNumber ) = 0;

	//! уничтожить камеру из устройства
	//! \param nID идентификатор камеры
	virtual void			DeleteCamera( int nID ) = 0;
};

//======================================================================================//
//                                struct ICameraConfig                                  //
//======================================================================================//

//! \brief интерфейс для получения конфигурации по камере
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class ICameraConfig
{
public:
	//! получить идентфикатор камеры в БД
	//! \return идентфикатор БД
	virtual int				GetCameraID() = 0;

	//! получить пин камеры
	//! \return номер пина
	virtual int				GetCameraPin() = 0;

	virtual std::tstring	GetDescription() = 0;

	virtual void			SetDescription(const std::tstring& sDesc) = 0;

	virtual ~ICameraConfig() {};
};

//! создать интферфейс
//! \param szIPAddress ip-адрес сервера, с котого получается конфигурация,0 - локальный ip
//! \return указатель на интерфейс, вызвать delete при уничожении
IVideoConfig*	CreateVideoConfig(LPCTSTR szIPAddress = 0);

//! получить все ip-адреса серверов в системе
//! \param IPAddressArr список ip-адресов xxx.xxx.xxx.xxx серверов системы
void		GetAllIPAddresses( std::vector<std::tstring>& IPAddressArr );


} // namespace

#endif // _I_VIDEO_CONFIG_1886981946031052_