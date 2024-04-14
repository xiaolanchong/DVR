//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: √лавный класс реализации интерфейса IStreamManager дл€ сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_MANAGER_3338654774616518_
#define _SERVER_MANAGER_3338654774616518_

#include "BaseManager.h"

class IOutputFramePin;
class IntermediateFilter;
class CaptureServer;
class RemoteVideoProvider;
class RemoteArchiveProvider;

//! \brief ќпредел€ет, запущен ли на данном компьютере экземпл€р реализации 
//!		   (даже под разными пользовател€ми)
//! \version 1.0
//! \date 06-30-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class ServerRunSentry
{
	//! событие дл€ проверки
	HANDLE	m_hDeviceOwner;
	//! ошибка при создании, если есть
	DWORD	m_dwLastError;
public:

	ServerRunSentry() :
	  m_hDeviceOwner ( NULL )
	{
		m_hDeviceOwner	= ::CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\CHCSDeviceOwner"));
		m_dwLastError	= GetLastError() ;
	}

	~ServerRunSentry()
	{
		if(m_hDeviceOwner)
			::CloseHandle(m_hDeviceOwner);
	}
	bool	IsAlreadyRunning() const
	{
		return !(m_hDeviceOwner && GetLastError() != ERROR_ALREADY_EXISTS);
	}
};

//======================================================================================//
//                                 class ServerManager                                  //
//======================================================================================//

//! \brief √лавный класс реализации интерфейса IStreamManager дл€ сервера
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerManager : public BaseManager
{
public:
	ServerManager(boost::shared_ptr<SystemSettings> pVideoConfig, DWORD WorkingMode);
	virtual ~ServerManager();

private:
	long    GetVideoRate();
	long    GetVideoMaxSize();
	
	bool LoadSettings();
	void SaveSettings();

	bool InitRegisteredDevices();

	//! Read Stream ID from database
	//! \param bShowDialogOnFailed  показать окно сообщени€ при ошибке
	//! \return считено/нет
	bool ReadStreamsFromDB(bool bShowDialogOnFailed = false);

	// base class overriding 
	virtual void InitializeWork() ;
	virtual void UninitializeWork();


	//! присоединить фильтры обработки к выходному потоку изображений
	//! \param pCaptureFilter	фильтр захвата
	//! \param StreamID			идентификатор потока, с которого идет захват
	//! \param lDevicePin		номер входа на устройстве
	//! \param &camUID			ид устройства
	void	AttachToCaptureFilter( 
		IOutputFramePin* pCaptureFilter, 
		long StreamID,
		long  lDevicePin, 
		const UUID &camUID,
		unsigned int CompressFPS
		);

	//! создать видеосервер дл€ передачи по сети
	void	CreateVideoProvider();
	//! удалить видеосервер
	void	DestroyVideoProvider();

	//! создать сервер сетевого архива
	void	CreateArchiveProvider();
	//! удалить сервер сетевого архива
	void	DestroyArchiveProvider();
private:

	bool	IsArchiveEnabled() const
	{
		return CheckMask< CHCS_MODE_STORE >();
	}

	bool	CreateNetServer() const
	{
		return CheckMask< CHCS_MODE_NETSERVER >();
	}

	bool	CreateNetArchive() const
	{
		return CheckMask< CHCS_MODE_NETARCHIVE >();
	}

	bool	UseOnlyDatabaseStreams() const
	{
		return CheckMask< CHCS_MODE_DATABASE >();
	}

	// Archive settings

	DWORD m_CodecFCC;				//!< Codec FOURCC

	long m_lVideoFPM;				//!< Video Frame per minute
	long m_lVideoMaxSize;			//!< Video max size in seconds

	long m_lCodecQuality;			//!<
	long m_lCodecKeyRate;			//!< Forced KeyRate
	long m_lCodecDataRate;			//!< Forced DataRate
	//! настройки кодека
	std::vector<BYTE>				m_CodecStateData;

	//! флаг, что настройки системы были изменены
	bool	m_bUpdateSettings;

	//! определ€ет, запущен ли уже сервер
	ServerRunSentry					m_Sentry;
	//! сервер захвата
	std::auto_ptr<CaptureServer>	m_pCaptureServer;
	//! сервер видеоподсистемы передачи по сети
	std::auto_ptr<RemoteVideoProvider>			m_VideoProvider;
	//! сетевой архив
	std::auto_ptr<RemoteArchiveProvider>		m_pArchiveProvider;

	//! создать фильтр дл€ алгоритмов, отсюда с сервера захватываютс€ кадры, и добавить его в доступные потоки
	//! \param pCaptureFilter исход€ций поставщие изображений
	//! \param StreamID		ид-р потока изобр-й
	//! \param lDevicePin	номер входа захвата на устройства
	//! \param camUID		идент-р устройства
	//! \return фильтр дл€ алгоритмов
	IntermediateFilter*			CreateServerStream(
										IOutputFramePin* pCaptureFilter, 
										long StreamID,
										long  lDevicePin, 
										const UUID &camUID
										);

	//! создать фильтр сжати€ кадров кодеком и добавить в массив фильтров
	//! \param StreamID ид потока изображений
	//! \param CompressFPS фпс компрессии/записи потока
	//! \return указатель на фильтр
	IntermediateFilter*			CreateCompressFilter(long StreamID, unsigned int CompressFPS);
	//! создать фильтр записи кадров и добавить в массив фильтров
	//! \param StreamID ид-р потока изображений, в который будет вставлен фильтр, и добавить в массив фильтров
	//! \param CompressFPS фпс компрессии/записи потока
	//! \return указатель на фильтр
	IntermediateFilter*			CreateArchiveFilter(long StreamID, unsigned int CompressFPS);
	//! создать фильтр записи в пул кадров дл€ клиента в том же процессе, и добавить в массив фильтров
	//! \return указатель на фильтр
	IntermediateFilter*			CreateCommonProcessFilter();

};

#endif // _SERVER_MANAGER_3338654774616518_