#include "stdafx.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "CCameraConfig.h"
#include "MainFunction.h"

CDeviceConfig::CDeviceConfig(CSession& Session, LPCTSTR szUID):
	m_session(Session),
	m_szUID (szUID)
{	
}

CDeviceConfig:: ~CDeviceConfig()
{
}

void CDeviceConfig::GetValue(LPCTSTR szName, DWORD& dwValue)
{
	//��������� Accessor
	CProperty< DWORD > Property;

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Devices WHERE (DeviceUID = '%s')"), szName, m_szUID.c_str()); 
	
	//����������
	Property.OpenDataSource(m_session);
	
	// ��������� ������� (������� ��������� � ����������� � ������)
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� ���������� ��������� �� ������
		if( Property.MoveNext() == S_OK )
		{
			//��������� ������
			DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
			if((dwFDiskStatus == DBSTATUS_S_OK))
			{
				//��������� �������� 
				dwValue = Property.m_Property;
			}
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("ErrorStatus");	
			}
		}
		
	}
	else
	{
		CheckResult(hRes);	
	}

	Property.CloseAll();
}

void CDeviceConfig::GetValue( LPCTSTR szName, std::tstring& sValue )
{
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Devices WHERE (DeviceUID = '%s')"), szName, m_szUID.c_str()); 
	
	//����������
	Property.OpenDataSource( m_session);
	
	// ��������� ������� (������� ��������� � ����������� � ������)
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� ���������� ��������� �� ������
		if( Property.MoveNext() == S_OK )
		{
			//��������� ������
			DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
			if((dwFDiskStatus == DBSTATUS_S_OK))
			{
				//��������� �������� 
				sValue = Property.m_Property;
			}
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("ErrorStatus");
			}
		}
	}
	else
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open");	
	}
	
	Property.CloseAll();
}

void CDeviceConfig::SetValue( LPCTSTR szName, DWORD dwValue)
{
	//������������ �������
	std::tstring szSQLQuery = Format(_T("UPDATE Devices SET %s = %d WHERE (DeviceUID = '%s')"), szName, dwValue, m_szUID.c_str()); 
	
	//��������� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");	
	}
	
	Property.CloseAll();
}

void CDeviceConfig::SetValue( LPCTSTR szName, LPCTSTR sValue )
{
	//������������ �������
	std::tstring szSQLQuery = Format(_T("UPDATE Devices SET %s = '%s' WHERE (DeviceUID = '%s')"), szName, sValue,  m_szUID.c_str()); 
	
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//����������
	Property.OpenDataSource(m_session);

	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		AtlTraceRelease(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");
	}

	Property.CloseAll();
}

std::tstring CDeviceConfig::GetIPAddress()
{
	//������������ IP
	std::tstring sComputerIP;
	
	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT Computers.ComputerIP FROM Devices INNER JOIN Computers ON Computers.ComputerID = Devices.ComputerID AND Devices.DeviceUID = '%s'"), m_szUID.c_str()); 
	
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//����������
	Property.OpenDataSource(m_session);
				
	//��������� ������� � ������� ��������� � ������������ � ������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� � ������������ ���������� ��������� �� ������
		if( Property.MoveNext() == S_OK )
		{
			//��������� ������
			DWORD dwStatusComputer = Property.m_dwPropertyStatus;
			if((dwStatusComputer == DBSTATUS_S_OK))
				sComputerIP = Property.m_Property;	
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("ErrorStatus ");
			}
		}	
	}
	else
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open()");
	}

	Property.CloseAll();
	
	//������� IP
	return sComputerIP;
}

std::tstring CDeviceConfig::GetDeviceUID()
{
	std::tstring sValueUID;
	GetValue(_T("DeviceUID"),sValueUID);
	return sValueUID;
}

void	CDeviceConfig::GetCameras( std::vector<int>& CamIDArr )
 {

	 //������������ �������
	 std::tstring szSQLQuery = Format(_T("SELECT Cameras.CameraID FROM Devices INNER JOIN Cameras ON Devices.DeviceID = Cameras.DeviceID AND Devices.DeviceUID = '%s'"), m_szUID.c_str()); 
	
	//�����, ����������� �� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
				
	//��������� ������� � ������� ��������� � ������������ � ������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� � ������������ ���������� ��������� �� ������
		while( Property.MoveNext() == S_OK )
		{
			//��������� ������
			DWORD dwStatusComputer = Property.m_dwPropertyStatus;
			if((dwStatusComputer == DBSTATUS_S_OK))
			{
				DWORD dwCameraID = Property.m_Property;

				//�������� � ������
				CamIDArr.push_back(dwCameraID);
				
			}
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("ErrorStatus ");
			}
		}			
	}
	else
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");
	}

	
	Property.CloseAll();
 }

DBBridge::ICameraConfig* CDeviceConfig::GetCamera( int nID  )
{
	//������������ ���������
	CCameraConfig* pCameraConfig = NULL;

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT Cameras.PinNumber FROM Devices INNER JOIN Cameras ON Devices.DeviceID = Cameras.DeviceID AND Devices.DeviceUID = '%s' AND Cameras.CameraID = '%d'"), m_szUID.c_str(), nID); 
	
	//��������� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
				
	//��������� ������� � ������� ��������� � ������������ � ������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� ���������� ��������� �� ������
		if( Property.MoveNext() == S_OK )
		{
			//��������� ������
			DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
			if((dwFDiskStatus == DBSTATUS_S_OK))
			{
				DWORD dwPinNumber = Property.m_Property;
				DWORD dwDeviceID = 0;
				GetValue(_T("DeviceID"), dwDeviceID);
				
				//����� ������ � ������ �����������
				pCameraConfig  = new CCameraConfig(m_session,dwDeviceID, dwPinNumber);
				if(pCameraConfig == NULL)
				{
					TRACE(_T("%s::Error memory lock\n"), APPNAME);
					throw DBBridge::IVideoConfig::VideoConfigException("Error Memory");	
				}
			}
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("Error Status");	
			}
		}
	}
	else
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open()");
	}

	Property.CloseAll();

	return pCameraConfig;
}

DBBridge::ICameraConfig* CDeviceConfig::CreateCamera( int nCameraID, int nPinNumber )
{
	UNREFERENCED_PARAMETER(nCameraID);
	
	//������������ ���������
	CCameraConfig* pCameraConfig = NULL;

	//����� ID ���������� � �������������  � ������ string
	DWORD dwID = 0;
	GetValue(_T("DeviceID"), dwID);

	//������������ �������
	std::tstring szSQLQuery = Format(_T("INSERT INTO Cameras (DeviceID, PinNumber) VALUES(%d, %d)"), dwID, nPinNumber); 
	
	//��������� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(SUCCEEDED(hRes))
	{
		//������� ��������� ������ ICameraConfig 
		pCameraConfig = new CCameraConfig(m_session, dwID, nPinNumber);
		if(pCameraConfig == NULL)
		{
			TRACE(_T("%s::Error memory lock\n"), APPNAME);
			throw DBBridge::IVideoConfig::VideoConfigException("Error Memory");	
		}
	}
	else
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open()");	
	}

	//������� ��������� �� ��������� ����������
	return pCameraConfig;
}
void CDeviceConfig::DeleteCamera( int nID )
{
	//������������ �������
	std::tstring szSQLQuery = Format(_T("DELETE FROM Cameras WHERE (CameraID = '%d')"), nID); 
	
	//��������� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open()");
	}

}
