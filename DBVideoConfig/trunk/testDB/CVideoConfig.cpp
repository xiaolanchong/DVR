#include "stdafx.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "MainFunction.h"

void	CheckResult(HRESULT hRes)
{
	std::tstring ErrorDesc = AtlTraceRelease(hRes);
	throw DBBridge::IVideoConfig::VideoConfigException(ErrorDesc);		
}

CVideoConfig::CVideoConfig(const std::tstring& sConection, const std::tstring& sComputerIP)
{
	m_sComputerIP = sComputerIP;
	
	//�������������� � Unicode ������
	CT2W psConvertToW( sConection.c_str());

	HRESULT hRes = m_db.OpenFromInitializationString(psConvertToW);
	if(SUCCEEDED(hRes))
	{
		hRes = m_session.Open(m_db);
		if(FAILED(hRes))
		{
			CheckResult(hRes);
			throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");
		}
	}
	else
	{
		CheckResult(hRes);
	}
	
}

CVideoConfig::~CVideoConfig()
{	
	
}

void CVideoConfig::GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//������������ ��������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str());
	
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
	}

	Property.CloseAll();
}

void CVideoConfig::GetValue( LPCTSTR szPath, LPCTSTR szName,DWORD& dwValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//����� ����������� Accessor
	CProperty< DWORD > Property;

	//������������ ������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
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

void CVideoConfig::GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//��������� Accessor
	CBlobProperty BlobProperty;

	//������������ ������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
	//����������
	BlobProperty.OpenDataSource( m_session);

	// ��������� ������� (������� ��������� � ����������� � ������)
	HRESULT hRes = BlobProperty.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� ���������� ��������� �� ������
		while( BlobProperty.MoveNext() == S_OK )
		{
			//���������� ��������� ����
			ULONG cb = 0;
			//������ ����������
			BYTE myBuffer[65536];
			if(BlobProperty.pBlobField != NULL)
			{
				//��������� ���� �� ��������� ������
				do
				{
					HRESULT hRes = BlobProperty.pBlobField->Read(myBuffer,65536,&cb);
					if(FAILED(hRes))
					{
						CheckResult(hRes);
						throw DBBridge::IVideoConfig::VideoConfigException("Error Read() ");
					}
					else
					{
						//��������������� ������
						binValue.reserve(cb);
						//�������� � ����� ��������� ������
						binValue.insert(binValue.end(),&myBuffer[0],&myBuffer[cb]);		
					}
				}while(cb != 0);
			}			
		}
	}
	else
	{
		CheckResult(hRes);
	}

	BlobProperty.CloseAll();
	
}

void CVideoConfig::SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//������������ �������
	std::tstring szSQLQuery = Format(_T("UPDATE Computers SET %s = %d WHERE ComputerIP = '%s'"), szName, dwValue, m_sComputerIP.c_str()); 

	//����� ����������� �� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource( m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");
	}

	Property.CloseAll();	
}


void CVideoConfig::SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//������������ �������
	std::tstring szSQLQuery = Format(_T("UPDATE Computers SET %s = '%s' WHERE ComputerIP = '%s'"), szName, sValue, m_sComputerIP.c_str()); 

	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//����������
	Property.OpenDataSource( m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
	}
	

	Property.CloseAll();
}

void CVideoConfig::SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue )
{
	UNREFERENCED_PARAMETER(szPath);
	//��������� Accessor
	CBlobProperty BlobProperty;

	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
	//���������� � ��
	BlobProperty.OpenDataSource(m_session);
	
	// ��������� ������� (������� ��������� � ����������� � ������)
	HRESULT hRes = BlobProperty.DoCommand(szSQLQuery.c_str(), FALSE);
	if(SUCCEEDED(hRes))
	{
		while(BlobProperty.MoveNext() == S_OK ) 
		{
			if(BlobProperty.pBlobField != NULL)
			{
				//Release field
				BlobProperty.pBlobField->Release();
			}

			//��������� ISequentialStream
			CISSHelper CissHelper;

			//���������� ������� ���������� ���� � ������������� ������
			ULONG cb = 0;

			//�������� � ��������� ������ ������ CISSHelper
			HRESULT hRes = CissHelper.Write(&(binValue[0]), (ULONG)binValue.size(), &cb);
			if(FAILED(hRes) || (cb != binValue.size()))
			{
				CheckResult(hRes);
				throw DBBridge::IVideoConfig::VideoConfigException("Error Write() ");
			}

			//���������� ������
			CissHelper.m_ulStatus = DBSTATUS_S_OK;

			//��������� ��������� ���� ISequentialStream 
			BlobProperty.pBlobField = &CissHelper;
			//���������� ������
			BlobProperty.m_dwFieldBlobLength = (ULONG)binValue.size();
			//���������� ������
			BlobProperty.m_dwFieldBlobStatus = DBSTATUS_S_OK;

			//��������� ������ 
			hRes = BlobProperty.SetData();
			if(FAILED(hRes))
			{
				CheckResult(hRes);
			}
		} 	
	}
	else
	{
		CheckResult(hRes);
	}
	
	BlobProperty.CloseAll();
}

void CVideoConfig::GetDevices( std::vector<std::tstring>& DeviceArr )
{
	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT Devices.DeviceUID FROM Computers INNER JOIN Devices ON Computers.ComputerID = Devices.ComputerID AND Computers.ComputerIP = '%s'"), m_sComputerIP.c_str()); 

	//��������� Accessor
	CProperty< TCHAR[255] > Property;

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
				//�������������� � string
				std::tstring sDUID(Property.m_Property);
				//�������� � ������
				DeviceArr.push_back(sDUID);	
			}
			else
			{
				TRACE(_T("%s::Error status at get data \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("Error status at get data");
			}
		}	
	}
	else
	{
		CheckResult(hRes);
	}
	
	Property.CloseAll();
}

DBBridge::IDeviceConfig* CVideoConfig::GetDevice( LPCTSTR szUID  )
{
	//������������ ���������
	CDeviceConfig* pDeviceConfig  = NULL;
	
	//������������ �������
	std::tstring szSQLQuery = Format(_T("SELECT Devices.DeviceID FROM Devices INNER JOIN Computers ON Computers.ComputerID = Devices.ComputerID AND Devices.DeviceUID = '%s' AND Computers.ComputerIP = '%s'"), szUID, m_sComputerIP.c_str()); 
	
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//����������
	Property.OpenDataSource(m_session);

	//��������� ������� � ������� ��������� � ������������ � ������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// ��������� ���������� ��������� �� ������
		if( Property.MoveNext() == S_OK )
		{
			pDeviceConfig  = new CDeviceConfig(m_session,szUID);
			if(pDeviceConfig == NULL)
			{
				TRACE(_T("%s::Error memory lock \n"), APPNAME);
				throw DBBridge::IVideoConfig::VideoConfigException("Error Memory");
			}
		}
	}
	else
	{
		CheckResult(hRes);
	}
	

	Property.CloseAll();

	return pDeviceConfig;
}
//�������� ��������� �� ��������� IDeviceConfig �� ���������� IVideoConfig
DBBridge::IDeviceConfig* CVideoConfig::CreateDevice( LPCTSTR szUID )
{
	//������������ ���������
	CDeviceConfig* pDeviceConfig = NULL;

	//����� ID ���������� � �������������  � ������ string
	DWORD dwID = 0;
	GetValue(0,_T("ComputerID"), dwID);

	//������������ �������
	std::tstring szSQLQuery = Format( _T("INSERT INTO Devices (ComputerID, DeviceUID) VALUES(%d, '%s')"), dwID, szUID); 
	
	//��������� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(SUCCEEDED(hRes))
	{
		//������� ��������� ������ IDeviceConfig � ����������� �������� UID
		pDeviceConfig = new CDeviceConfig(m_session, szUID);
		if(pDeviceConfig == NULL)
		{
			TRACE(_T("%s::Error memory lock \n"), APPNAME);
			throw DBBridge::IVideoConfig::VideoConfigException("Error Memory");	
		}
	}
	else
	{
		CheckResult(hRes);	
	}

	//������� ��������� �� ��������� ����������
	return pDeviceConfig;
	
}

//������� ���������� � ������ UID
void CVideoConfig::DeleteDevice( LPCTSTR szUID )
{
	
	//������������ �������
	std::tstring szSQLQuery = Format(_T("DELETE FROM Devices WHERE (DeviceUID = '%s')"), szUID); 
	
	//�����, ����������� �� Accessor
	CProperty< DWORD > Property;

	//����������
	Property.OpenDataSource(m_session);
	
	//��������� ������� � ������ �� �������
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
	}
	
}
