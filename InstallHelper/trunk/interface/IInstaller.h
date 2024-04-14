//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс для операций, необходимых при установке и подключении DVR

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_INSTALLER_2000773579556309_
#define _I_INSTALLER_2000773579556309_

//======================================================================================//
//                                   class IInstaller                                   //
//======================================================================================//

//! \brief Интерфейс для операций, необходимых при установке и подключении DVR
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   26.05.2006
//! \version 1.0
//! \bug 
//! \todo GetAvalableStreams и RemoveStream для изменения конфигурации

class IInstaller
{
public:
	virtual ~IInstaller() {};

	virtual bool SetBridgeForConfigure(bool UseBridge) = 0;

	//! внести данные конфигурации
	//! \param ServerIPAddress ip адрес, под которым работает сервер
	//! \return результат - успех/неудача
	virtual bool Register(const char* ServerIPAddress)				= 0;

	//! убрать данные о системе
	//! \return  результат - успех/неудача
	virtual bool Unregister()			= 0;

	//! получить информацию по всем видеопотока
	//! \param pAvailableStreams	доступные потоки
	//! \param pInslalledStreams	уже установленные, пока не работает
	//! \return результат - успех/неудача
	virtual bool GetAvalableStreams( CAUUID* pAvailableStreams, CAUUID* pInslalledStreams )	= 0; 

	//! добавить поток в систему DVR
	//! \param pStreamUID один из UUID, полученных в GetAvalableStreams
	//! \return результат - успех/неудача
	virtual bool AddStream		( const UUID* pStreamUID ) = 0;

	//! удалить поток из системы DVR
	//! \param pStreamUID один из UUID, полученных в GetAvalableStreams
	//! \return результат - успех/неудача
	virtual bool RemoveStream	( const UUID* pStreamUID ) = 0;

	//! отображать кадры потока в окно
	//! \param pStreamUID	 один из UUID, полученных в GetAvalableStreams
	//! \param hWndForRender окно для периодического отображения
	//! \return результат - успех/неудача
	virtual bool ShowStream		( const UUID* pStreamUID, HWND hWndForRender ) = 0;

	//! получить путь к папке архива (для удаления)
	//! \return файловый путь к папке архива
	virtual std::string	 GetArchivePath() = 0;

	//! установить путь к папке архива
	//! \param sArchivePath путь к папке видеоархива и БД
	//! \return результат - успех/неудача
	virtual bool		 SetArchivePath( const std::string& sArchivePath ) = 0;
};

#endif // _I_INSTALLER_2000773579556309_