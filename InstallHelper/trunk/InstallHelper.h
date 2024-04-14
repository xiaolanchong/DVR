//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейса к функциям библиотеки установки

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _INSTALL_HELPER_2842084057089803_
#define _INSTALL_HELPER_2842084057089803_

/* ************************************************************************************

Функции доступа к интефейсу установки и конфигурирования системы для InstallShield
строковый UID для всех функций - 
представление UID в форме 
1)"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"	( формат UuidToString )
2)"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"	( StringFromCLSID )

************************************************************************************* */

//! общая ошибка
const DWORD Res_Error			= 0;
//! успех
const DWORD	Res_Success			= 1;
//! сервер запущен в данное время
const DWORD Res_ServerLaunched	= 2;

extern "C"
{

bool	WINAPI	SetBridgeForConfigure( bool UseBridge = true );

//! занести в конфигурацию первоначальные данные
//! \param ServerIPAddress	ip адрес, назначенный для сервера (в том случае, если их много)	
//! \param szArchivePath	файловый путь к архиву	
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI Register( const char* ServerIPAddress, const char* szArchivePath);

//! удалить из конфигурации первоначальные данные
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI Unregister();

//! получить возможные видео потоки (точки подключения камер)
//! \param pAvailableStreams		список потоков
//! \param pAlreadyInstalledStreams список уже установленных потоков
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI GetAvailableStreams( CAUUID* pAvailableStreams/*, CAUUID* pAlreadyInstalledStreams*/ );

//! добавить поток в систему, если уже добавлен, то ошибки нет
//! \param szStreamUID идентификатор, полученный GetAvailableStreams
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI AddStream( const char* szStreamUID);

//! удалить поток из системы, если уже удален, то ошибки нет
//! \param szStreamUID идентификатор, полученный GetAvailableStreams
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI RemoveStream( const char* szStreamUID );

//! показывать видеопоток в окно
//! \param hWndForStreamRender окно для отображения потока
//! \param szStreamUID	поток
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD	WINAPI ShowStream( HWND hWndForStreamRender, const char* szStreamUID );

//! возвратит файловый путь к архиву, для удаления папок,
//! вызывать в любой момент между Register и Unregister
//! результат действителен до следующего вызова GetArchivePath
//! \return 0, "" - ошибка, иначе строка пути
const char*		WINAPI GetArchivePath();

//! установить папку с архивом
//! \param szArchivePath строка пути, пустая или NULL - сбросить путь
//! \return -Res_Error-ошибка, 
//!         -Res_Success - успех, 
//!         -Res_ServerLaunched - запущен сервер(ошибка)
DWORD			WINAPI SetArchivePath(const char* szArchivePath);

//! освободить все ресурсы
void	WINAPI FreeMemory();

/*!	Install actions
	\code

	Register("192.168.0.1", "e:\\DVR");
	//...
	GetAvailableStreams( pAvailableStreams );
	//...
	while( ... )
	{
		ShowStream( hWnd, szStream );
	}
	//..
	//! для всех выбранных пользователем потоков
	AddStream( szStream )

	//
	FreeMemory();

	\endcode
*/

/*! Uninstall actions
\code

	szArchivePath = GetArchivePath();
	// delete archive directory
	Unregister()

\endcode
*/
}

#endif // _INSTALL_HELPER_2842084057089803_