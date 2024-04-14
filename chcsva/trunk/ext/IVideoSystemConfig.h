//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ������/������ �������� ���������������, 
//	��� ������/���������� �������� ��� �������� ���������� ���������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_VIDEO_SYSTEM_CONFIG_4941905171593888_
#define _I_VIDEO_SYSTEM_CONFIG_4941905171593888_

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

namespace CHCS
{

//======================================================================================//
//                               class IVideoSystemConfig                               //
//======================================================================================//

//! \brief ��������� ������/������ �������� ���������������, 
//	��� ������/���������� �������� ��� �������� ���������� ���������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   11.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class IVideoSystemConfig
{
public:
	//! ��������� ������
	struct CodecSettings
	{
		DWORD m_CodecFCC;				//!< Codec FOURCC

		long m_lVideoFPM;				//!< Video Frame per minute
		long m_lVideoMaxSize;			//!< Video max size in seconds

		long m_lCodecQuality;			//!<
		long m_lCodecKeyRate;			//!< Forced KeyRate
		long m_lCodecDataRate;			//!< Forced DataRate
		
		std::vector<BYTE>				m_CodecStateData; //!< ��������� ������ � �������� �������
	};

	//! ��������� ���������
	struct DeviceSettings
	{
		UUID streamID;				//!< Unique stream ID
		std::tstring stMoniker;		//!< Device moniker - certaing string which uniquelly 
									//!< identify device in the whole system
		long lWidth;		//! Width - pixels
		long lHeight;		//! Height - pixels
		long lFrmRate;		//! Frame rate - fps
		long lVStandard ;	//! Video Standart (see DX documentation)
		bool bBWMode;		//!< true - blackwhite, false - colour video
	};

	//! ��������� ������
	struct CameraSettings
	{
		//! ������������ ������
		LONG	m_lCameraID;
		//! ��������� UID ����������
		UUID	m_DeviceUID;
		//! ����� �������
		std::tstring m_stCameraIP;
		//! ��� ����������
		LONG	m_lDevicePin;
	};

	virtual ~IVideoSystemConfig() {};

	//! �������� ��������� ����������
	//! \param params ��������� ��������� ���������
	virtual void		 GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params ) = 0;
	//! �������� ��������� ���� ����� ������� (�� ������ ���������)
	//! \param params 
	virtual void		 GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params) = 0;

	//! �������� ���� � ������
	//! \return ������ ���� � ����� ������
	virtual std::tstring GetArchivePath() = 0;

	//! �������� ��������� ������
	//! \param params 
	virtual void		 GetCodecSettings(IVideoSystemConfig::CodecSettings& params) = 0;
};

class IDeviceRegistrar
{
public:
	virtual ~IDeviceRegistrar() {}
	//! ���������������� ����-�� � �������
	//! \param NewDevice ��������� ������ ����������
	virtual void	__stdcall	 RegisterLocalDevice( LPCWSTR szMoniker, LPCWSTR szFriendlyName ) = 0; 
};

class IVideoSystemConfigEx : public IVideoSystemConfig
{
public:
	virtual void	DeleteAllDevicesAndCameras() = 0;
	virtual void	CreateDevice( const IVideoSystemConfig::DeviceSettings& ds ) = 0; 
	void	CreateDevice( LPCTSTR szMoniker )
	{
		IVideoSystemConfig::DeviceSettings ds;
		GetDefaultDeviceSetting( szMoniker, ds );
		return CreateDevice( ds );
	}

	virtual LONG	CreateCamera( const UUID& DeviceUID, unsigned int PinNumber ) = 0;
	virtual void	DeleteCamera( LONG CameraID ) = 0; 

	//! ���������� ����� ����� ������
	//! \param sArchivePath 
	virtual void		 SetArchivePath(const std::tstring& sArchivePath)		= 0;

	//! ���������� ��������� ������
	//! \param params 
	virtual void		 SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) = 0;
public:
	static void	GetDefaultDeviceSetting( LPCTSTR szMoniker, IVideoSystemConfig::DeviceSettings& ds )
	{
		UUID streamUID;
		UuidCreate(&streamUID);
		ds.streamID		=	streamUID;
		ds.stMoniker	=	szMoniker;
		ds.bBWMode		=	false;
		ds.lWidth		=	352;
		ds.lHeight		=	288;
		ds.lFrmRate		=	25;
		ds.lVStandard	=	16;
	}
};

}

#endif // _I_VIDEO_SYSTEM_CONFIG_4941905171593888_