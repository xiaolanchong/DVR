#ifndef _CDEVICECONFIG_H_
#define _CDEVICECONFIG_H_

#include "CTypeAccessor.h"

class ICameraConfig;

class CDeviceConfig: public DBBridge::IDeviceConfig
{
public:

	CDeviceConfig(CSession& sConection, LPCTSTR szUID);
	virtual ~CDeviceConfig();

	//!  ��������� �������� ���� DWORD
	//! \param szName ��� ���������
	//! \param dwValue ��������
	virtual void GetValue( LPCTSTR szName, DWORD& dwValue);

	//! �������� ��������� ��������
	//! \param szName ��� 
	//! \param sValue ��������
	virtual void GetValue( LPCTSTR szName, std::tstring& sValue );

	//! ���������� �������� �������� 
	//! \param szName ��� 
	//! \param dwValue ��������
	virtual void SetValue( LPCTSTR szName, DWORD dwValue);

	//! ���������� ��������� ��������
	//! \param szName ���
	//! \param sValue ��������
	virtual void SetValue( LPCTSTR szName, LPCTSTR sValue );	

	//! �������� IP-����� ������� ������
	//! \return ����� � ������� xxx.xxx.xxx.xxx
	virtual std::tstring GetIPAddress();

	//! �������� UID ������
	//! \return ��������� UID � ������� {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	virtual std::tstring GetDeviceUID();

	//! �������� ������ ���� �����
	//! \param CamIDArr ������ ���������������
	virtual void GetCameras( std::vector<int>& CamIDArr );

	//! �������� ��������� ������
	//! \param nID ������������� ������
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual DBBridge::ICameraConfig* GetCamera( int nID  );

	//! ������� ������
	//! \param nPinNumber ����� ���� (0-3) 
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual DBBridge::ICameraConfig* CreateCamera( int nCameraID, int nPinNumber );

	//! ���������� ������ �� ����������
	//! \param nID ������������� ������
	virtual void DeleteCamera( int nID );

protected:
	//UID ����������
	std::tstring m_szUID;
	//�����
	CSession m_session;
};
#endif