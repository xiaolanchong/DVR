//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������/������ �������� �������

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
	//! ������������ ������
	LONG	m_lCameraID;
	//! ��������� UID ����������
	std::tstring m_stCameraUID;
	//! ����� �������
	std::tstring m_stCameraIP;
	//! ��� ����������
	LONG	m_lDevicePin;
};

//======================================================================================//
//                                 class SystemSettings                                 //
//======================================================================================//

//! \brief ������/������ �������� �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo	��������� �������� � ���������� ������������ CManager 
//!	        (���� �������� ��� ����� � ��, �������� ��� �� ���������), ��
//!	        �������� ��� ������ ���� ���������������. �� ������� ���� ��� �������-�
//!         � �������� ����������� � ����� �����

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

	//! �������� ip ����� �������, ��� ���������� �����
	//! \param  stDBInitString ������ ���������� � ��
	//! \param	StreamID ������������� ������
	//! \return	ip v4  ����� ��� INADDR_NONE, ���� ������ ������ ��� ������� ���
	//! \exception	
	virtual ULONG	GetIPAddressOfStreamServer( long StreamID ) = 0;

	static bool	IsLocalIPAddress( ULONG IPAddress ) ;
};

#endif // _SYSTEM_SETTINGS_3416446349689008_