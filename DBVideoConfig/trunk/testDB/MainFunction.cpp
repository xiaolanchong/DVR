#include "stdafx.h"
#include "CTypeAccessor.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "CCameraConfig.h"
#include "MainFunction.h"

typedef std::vector<std::tstring> STLVectorTstring;

DBBridge::IVideoConfig*	DBCreateVideoConfig(const std::tstring& sConection, const std::tstring& sComputerIP)
{
	//������������ ���������
	CVideoConfig* pCVConfig = NULL;

	//�������� ��� ��������� IP ������
	STLVectorTstring STLIpAdressList;
	GetAllIPAdress(sConection, STLIpAdressList);
	BOOL bIpFind = FALSE;

	//��������� ��� �� ��� ������ IP
	for(STLVectorTstring::iterator i = STLIpAdressList.begin(); i != STLIpAdressList.end(); i++)
	{
		std::tstring sIp = *i;
		if (sIp == sComputerIP)
			bIpFind = TRUE;
	}

	//������� ������ � ����� IP, ���� ����� ���
	if (!bIpFind)
	{
		//������������ ��������
		std::tstring szSQLQuery = Format( _T("INSERT INTO Computers (ComputerIP) VALUES('%s')"), sComputerIP.c_str()); 
		
		//��������� Accessor
		CProperty< DWORD > Property;

		//����������
		HRESULT hRes = Property.OpenDataSource(sConection);
		
		if(SUCCEEDED(hRes))
		{
			//��������� ������� � ������ �� �������
			HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
			if(FAILED(hRes))
			{
				CheckResult(hRes);
				throw DBBridge::IVideoConfig::VideoConfigException("Error DoCommand()");
			}
		}
		else
		{
			CheckResult(hRes);
		}
	}

	pCVConfig = new CVideoConfig(sConection, sComputerIP);
	if(pCVConfig  == NULL)
	{
		TRACE(_T("%s::Error Memory Lock\n"), APPNAME);
		throw DBBridge::IVideoConfig::VideoConfigException ("Error Memory Lock");
	}

	return (pCVConfig);
}

void GetAllIPAdress(const std::tstring& sConection, std::vector<std::tstring>& STLIpList)
{
	//��������� Accessor
	CProperty< TCHAR[255] > Property;

	//������������ ��������
	std::tstring szSQLQuery = std::tstring(_T("SELECT ComputerIP FROM Computers"));

	//����������
	HRESULT hRes = Property.OpenDataSource(sConection);
	//TRACE(_T("The value of x is %d.\n"), hRes);
	if(SUCCEEDED(hRes))
	{
		// ��������� ������� (������� ��������� � ����������� � ������)
		HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
		if(SUCCEEDED(hRes))
		{
			// ��������� ���������� ��������� �� ������
			while( Property.MoveNext() == S_OK )
			{
				//��������� ������
				DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
				if((dwFDiskStatus == DBSTATUS_S_OK))
				{
					//��������� �������� 
					std::tstring sValue = Property.m_Property;
					STLIpList.push_back(sValue);
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
	
	}
	else
	{
		CheckResult(hRes);
	}

	Property.CloseAll();	
}

// ������� TRACE �� ������������ ���������� ����������
void TRACE(LPCTSTR szFormat, ...)
{

	va_list i;
	va_start(i, szFormat);  
	int Length;

	TCHAR Buffer[4096];

	Length = _vsntprintf_s(Buffer, sizeof(Buffer)/sizeof(TCHAR),_TRUNCATE, szFormat, i);

	va_end(i);

	// Print
	OutputDebugString(Buffer);
} 

//������� ���������� �� ������
std::tstring AtlTraceRelease(HRESULT hRes)
{
	std::tstring	ErrorDescription;
	CDBErrorInfo ErrorInfo;
	ULONG        cRecords;
	HRESULT      hr;
	ULONG        i;
	CComBSTR     bstrDesc, bstrHelpFile, bstrSource;
	GUID         guid;
	DWORD        dwHelpContext;
	WCHAR        wszGuid[40];

	// If the user passed in an HRESULT then trace it
	if (hRes != S_OK)
	{
		TRACE(_T("%s::OLE DB Error Record dump for hr = 0x%x\n"), APPNAME, hRes);
	}

	LCID lcLocale = GetSystemDefaultLCID();

	hr = ErrorInfo.GetErrorRecords(&cRecords);
	if (FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
	{
		TRACE(_T("%s::No OLE DB Error Information found: hr = 0x%x\n"),APPNAME, hr);
	}
	else
	{

		for (i = 0; i < cRecords; i++)
		{
			hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc, &bstrSource, &guid,
				&dwHelpContext, &bstrHelpFile);
			if (FAILED(hr))
			{
				TRACE(_T("%s::OLE DB Error Record dump retrieval failed: hr = 0x%x\n"),APPNAME, hr);
				return ErrorDescription;
			}
			StringFromGUID2(guid, wszGuid, sizeof(wszGuid) / sizeof(WCHAR));
			TCHAR DescBuffer[1024];
			_sntprintf_s( DescBuffer, 1024, _T("Row #: %4d Source: \"%s\" Description: \"%s\" Help File: \"%s\" Help Context: %4d GUID: %s\n"),
				i, static_cast<TCHAR*>(COLE2T(bstrSource)), static_cast<TCHAR*>(COLE2T(bstrDesc)), static_cast<TCHAR*>(COLE2T(bstrHelpFile)), dwHelpContext, static_cast<TCHAR*>(COLE2T(wszGuid)) );
			TRACE( _T("%s"), DescBuffer );
			ErrorDescription = DescBuffer;
			bstrSource.Empty();
			bstrDesc.Empty();
			bstrHelpFile.Empty();
		}
		TRACE(_T("%s::OLE DB Error Record dump end\n"),APPNAME);
	}
	return ErrorDescription;
}
// �������������� ������ � ������������ ����������� ����������
std::tstring Format(LPCTSTR szFormat, ...)
{
	va_list i;
	va_start(i, szFormat);  
	int Length;

	TCHAR Buffer[4096];

	Length = _vsntprintf_s(Buffer, sizeof(Buffer)/sizeof(TCHAR),_TRUNCATE, szFormat, i);

	va_end(i);

	std::tstring sBuffer(Buffer);

	return sBuffer;
}
