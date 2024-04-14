#include "stdafx.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "CCameraConfig.h"
#include "MainFunction.h"


CCameraConfig::CCameraConfig(CSession& Session, INT nDeviceID, INT nPinNumber):
	m_session(Session),
	m_nDeviceID(nDeviceID),
	m_nPinNumber(nPinNumber)
{
}
CCameraConfig:: ~CCameraConfig()
{
}
int CCameraConfig::GetCameraID()
{
	//ID - ������� ���� �������
	DWORD dwCameraID = DWORD(-1);	// warning, any invalid value

	//��������� Accessor
	CProperty< DWORD > Property;

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT CameraID FROM Cameras WHERE DeviceID = '%d' AND PinNumber = '%d'"), m_nDeviceID, m_nPinNumber); 
	
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
				dwCameraID = Property.m_Property;
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

	return dwCameraID;	
}

int CCameraConfig::GetCameraPin()
{
	return m_nPinNumber;
}

std::tstring CCameraConfig::GetDescription()
{
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	// ������ ���������� ���� Description
	std::tstring sDescriptor;

	//����� ������������� ������
	INT nCameraID = GetCameraID();

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT Description FROM Cameras WHERE CameraID = %d"), nCameraID);

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
				sDescriptor = Property.m_Property;
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

	return sDescriptor;
}

void CCameraConfig::SetDescription(const std::tstring& sDesc)
{
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//����� ������������� ������
	INT nCameraID = GetCameraID();

	//������������ �������
	std::tstring szSQLQuery = Format(_T("UPDATE Cameras SET Description = '%s' WHERE CameraID = %d"), sDesc.c_str(), nCameraID);

	//����������
	Property.OpenDataSource(m_session);

	// ��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
	}

	Property.CloseAll();
}