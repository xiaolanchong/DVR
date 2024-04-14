//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Чтение/запись настроек системы

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SYSTEM_SETTINGS_3416446349689008_
#define _SYSTEM_SETTINGS_3416446349689008_

struct StorageSystemSettings
{
	std::tstring	m_stBasePath;	// Archive base path

	DWORD m_CodecFCC;				// Codec FOURCC

	long m_lVideoFPM;				// Video Frame per minute
	long m_lVideoMaxSize;			// Video max size in seconds

	long m_lCodecQuality;			//
	long m_lCodecKeyRate;			// Forced KeyRate
	long m_lCodecDataRate;			// Forced DataRate

	std::vector<BYTE>				m_CodecStateData;
};

struct DeviceSettings
{
	UUID streamID;				//!< Unique stream ID, unique for system
	std::tstring stMoniker;		//!< Device moniker, unique for OS
	long lWidth;		//!< Camera image width
	long lHeight;		//!< Camera image Height
	long lFrmRate;		//!< Camera capture frame rate
	long lVStandard ;	//!< Video Standart (see DX documentation)
	bool bBWMode;		//!< black(true) or colour(false) capture
};

struct CameraSettings
{
	//! идентфикатор камеры
	LONG	m_lCameraID;
	//! строковый UID устройства
	std::tstring m_stCameraUID;
	//! адрес сервера
	std::tstring m_stCameraIP;
	//! пин устройства
	LONG	m_lDevicePin;
};

//======================================================================================//
//                                 class SystemSettings                                 //
//======================================================================================//

//! \brief Чтение/запись настроек системы
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo	поскольку появился в результате рефакторинга CManager 
//!	        (сюда вынесены все связи с БД, реестром или их обертками), то
//!	        возможно его самого надо рефакторизовать. По крайней мере все взаимод-я
//!         с внешними настройками в одном месте

class SystemSettings
{
public:
	virtual ~SystemSettings() {};

	virtual bool LoadStorageSettings( StorageSystemSettings& param ) = 0;

	virtual void SaveStorageSettings( LPCTSTR stBasePath ) = 0;

	virtual bool LoadLocalDeviceSettings( std::vector<DeviceSettings>& param) = 0;

	bool RegisterLocalDevice( const UUID& streamID, LPCTSTR stMoniker ) 
	{
		// not implemented
		return false;
	}

	virtual bool LoadCameraSettings( bool bShowDialogOnFailed, std::vector<CameraSettings>& param ) = 0; 

	//! получить ip адрес сервера, где находиться поток
	//! \param  stDBInitString строка соединения с БД
	//! \param	StreamID идентификатор потока
	//! \return	ip v4  адрес или INADDR_NONE, если такого потока или сервера нет
	//! \exception	
	virtual ULONG	GetIPAddressOfStreamServer( long StreamID ) = 0;

	static bool	IsLocalIPAddress( ULONG IPAddress ) ;
};

#endif // _SYSTEM_SETTINGS_3416446349689008_