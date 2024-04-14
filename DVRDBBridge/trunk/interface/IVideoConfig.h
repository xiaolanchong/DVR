 //======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Video configuration interface - ��������� ��� ������ �� ������������� 
				(������, ��������� �����������, ������ � �.�.)

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

//! \brief ��������� ��� ��������� ������������ ������������
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
	//! ������� ����� ���������� ��� ����������
	typedef BaseException<std::runtime_error>  VideoConfigException;
	//! ��� ������ ���������, ��� GetValue
	typedef BaseException<VideoConfigException>  NoSuchProperty;
	//! ������ ��� ������, ��� SetValue
	typedef BaseException<VideoConfigException>  InvalidProperty;
	//! ������ ��� ���������� ��������
	typedef BaseException<VideoConfigException>  InitFailed;

	virtual ~IVideoConfig() {};

	//! ��������� �������� ���� DWORD
	//! \param szPath ������(������� ����������), �������������� ������ 
	//!			\li NULL or ""	- global
	//!			\li "Storage"	- coder settings 
	//! \param szName ��� ���������
	//! \param dwValue ��������
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) = 0;

	//!  ��������� ��������� ��������
	//! \param szPath ������
	//! \param szName ��� ���������
	//! \param sValue ��������
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue ) = 0;

	//! ��������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param binValue ��������
	//! \exception NoSuchProperty
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue ) = 0;

	//! ���������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param dwValue ��������
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue) = 0;

	//! ���������� ��������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param sValue ��������
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue ) = 0;

	//! ���������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param binValue ��������
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue ) = 0;

	//! �������� ������ ���� �����
	//! \param CamIDArr ������ ���������������
	virtual void	GetDevices( std::vector<std::tstring>& DeviceArr ) = 0;

	//! �������� ��������� ������
	//! \param nID ������������� ������
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual IDeviceConfig*	GetDevice( LPCTSTR szUID  ) = 0;

	//! ������� ����������
	//! \param szUID ��������� UID ������ � ������� {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual IDeviceConfig*	CreateDevice( LPCTSTR szUID ) = 0;

	//! ���������� ������ �� ������������
	//! \param nID ������������� ������
	virtual void			DeleteDevice( LPCTSTR szUID ) = 0;

};

//! \brief ������������ ����������
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

	//!  ��������� �������� ���� DWORD
	//! \param szName ��� ���������
	//! \param dwValue ��������
	virtual void	GetValue( LPCTSTR szName, DWORD& dwValue) = 0;

	//! �������� ��������� ��������
	//! \param szName ��� 
	//! \param sValue ��������
	virtual void	GetValue( LPCTSTR szName, std::tstring& sValue ) = 0;

	//! ���������� �������� �������� 
	//! \param szName ��� 
	//! \param dwValue ��������
	virtual void	SetValue( LPCTSTR szName, DWORD dwValue) = 0;

	//! ���������� ��������� ��������
	//! \param szName ���
	//! \param sValue ��������
	virtual void	SetValue( LPCTSTR szName, LPCTSTR sValue ) = 0;	

	//! �������� IP-����� ������� ������
	//! \return ����� � ������� xxx.xxx.xxx.xxx
	virtual std::tstring	GetIPAddress() = 0;

	//! �������� UID ������
	//! \return ��������� UID � ������� {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	virtual std::tstring	GetDeviceUID() = 0;

//----------------------------------------------------------//

	//! �������� ������ ���� �����
	//! \param CamIDArr ������ ���������������
	virtual void	GetCameras( std::vector<int>& CamIDArr ) = 0;

	//! �������� ��������� ������
	//! \param nID ������������� ������
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual ICameraConfig*	GetCamera( int nID  ) = 0;

	//! ������� ������
	//! \param nPinNumber ����� ���� (0-3) 
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual ICameraConfig*	CreateCamera( int nCameraID, int nPinNumber ) = 0;

	//! ���������� ������ �� ����������
	//! \param nID ������������� ������
	virtual void			DeleteCamera( int nID ) = 0;
};

//======================================================================================//
//                                struct ICameraConfig                                  //
//======================================================================================//

//! \brief ��������� ��� ��������� ������������ �� ������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class ICameraConfig
{
public:
	//! �������� ������������ ������ � ��
	//! \return ������������ ��
	virtual int				GetCameraID() = 0;

	//! �������� ��� ������
	//! \return ����� ����
	virtual int				GetCameraPin() = 0;

	virtual std::tstring	GetDescription() = 0;

	virtual void			SetDescription(const std::tstring& sDesc) = 0;

	virtual ~ICameraConfig() {};
};

//! ������� ����������
//! \param szIPAddress ip-����� �������, � ������ ���������� ������������,0 - ��������� ip
//! \return ��������� �� ���������, ������� delete ��� ����������
IVideoConfig*	CreateVideoConfig(LPCTSTR szIPAddress = 0);

//! �������� ��� ip-������ �������� � �������
//! \param IPAddressArr ������ ip-������� xxx.xxx.xxx.xxx �������� �������
void		GetAllIPAddresses( std::vector<std::tstring>& IPAddressArr );


} // namespace

#endif // _I_VIDEO_CONFIG_1886981946031052_