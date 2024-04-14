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
	
	//! ��������� �������� ���� DWORD
	//! \param szName ��� ���������
	//! \param dwValue ��������
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue);

	//!  ��������� ��������� ��������
	//! \param szPath ������
	//! \param szName ��� ���������
	//! \param sValue ��������
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue );

	//! ��������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param binValue ��������
	//! \exception NoSuchProperty
	virtual void GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue );

	//! ���������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param dwValue ��������
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue);

	//! ���������� ��������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param sValue ��������
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue );

	//! ���������� �������� ��������
	//! \param szPath ������
	//! \param szName ���
	//! \param binValue ��������
	virtual void SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue );

	//! �������� ������ ���� ���������
	//! \param DeviceArr ������ ���������������
	virtual void GetDevices( std::vector<std::tstring>& DeviceArr );

	//! �������� ��������� ������
	//! \param nID ������������� ������
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual DBBridge::IDeviceConfig*	GetDevice( LPCTSTR szUID  );

	//! ������� ����������
	//! \param szUID ��������� UID ������ � ������� {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxx}
	//! \return ��������� �� ���������, ������� delete ��� ����������
	virtual DBBridge::IDeviceConfig*	CreateDevice( LPCTSTR szUID );

	//! ���������� ������ �� ������������
	//! \param nID ������������� ������
	virtual void DeleteDevice( LPCTSTR szUID );

protected:
	//IP ����������
	std::tstring m_sComputerIP;
	//������ ����������
	std::tstring m_sConectionInfo;
	//�������� ������
	CDataSource m_db;
	//�����
	CSession m_session;
};
#endif