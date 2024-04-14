// testDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CTypeAccessor.h"
#include "CCategories.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "CCameraConfig.h"
#include "MainFunction.h"
#include "CFrame.h"

typedef std::vector<std::tstring> STLVectorTstring;
typedef std::vector<int> STLVectorInt;
typedef std::vector<BYTE> STLVectorBinary;
typedef std::vector<FRAMELINE> STLVectorFrameLine;

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(0);

	TCHAR sDeviceUID[255] = _T("{8BD2864D-401A-4748-BAD1-123D230AC181}");;
	
	STLVectorTstring STLDevicesList;
	STLVectorTstring STLIpAdressList;
	STLVectorInt STLCamerasIDList;
	STLVectorFrameLine STLFLineList;
	STLVectorBinary STLBinaryDataList;

	//������ ���������� IP
	std::tstring sIP(_T("23.34"));

	//��������� ������ ����������
	std::tstring sConection(_T("Provider=SQLOLEDB.1;Password=dvr;Persist Security Info=True;User ID=dvr;Initial Catalog=DVR;Data Source=eugene;"));


	CVideoConfig* pCVConfig = NULL;
	CDeviceConfig* pCDConfig = NULL;
	CCameraConfig* pCConfig = NULL;
	CFrame* pCFrame = NULL;
	std::tstring s;
	s.reserve (1024);
	
	//TCHAR s[1024];
	_stprintf_s((LPTSTR)s.c_str(), 1024, _T("IP: %s"), sIP.c_str());

	try
	{
		pCFrame = new CFrame(sConection);
		DWORD dwID = 41;
		
		SYSTEMTIME SystemTimeBegin;
		GetSystemTime(&SystemTimeBegin);
		SYSTEMTIME SystemTimeEnd = SystemTimeBegin;
		SystemTimeEnd .wHour++;

		pCFrame->InsertFrameLine(SystemTimeBegin, SystemTimeEnd, dwID);
		pCFrame->GetFrameLine(SystemTimeBegin, SystemTimeEnd, STLFLineList);
		pCFrame->DeleteFrameLine(SystemTimeBegin, SystemTimeEnd);
		
		//�������� ��������� IVideoConfig
		pCVConfig = (CVideoConfig*)DBCreateVideoConfig(sConection, sIP);
		
		if(pCVConfig != NULL)
		{
			
			//�������� ��������� IDeviceConfig ��� ������� pCVConfig �� UID
			pCDConfig = (CDeviceConfig*)pCVConfig->GetDevice(sDeviceUID);
			
			
			if(pCDConfig != NULL)
			{
				//�������� ��������� ICameraConfig ��� ������� pCDConfig �� ID
				pCConfig = (CCameraConfig*)pCDConfig->GetCamera(40);
				if(pCConfig != NULL)
				{
					//������ ������������� ������� ���������� ICameraConfig
					DWORD dwCameraID = pCConfig->GetCameraID();
					DWORD dwCameraPin = pCConfig->GetCameraPin();
					std::tstring sDesc = pCConfig->GetDescription();
					sDesc.clear();
					sDesc = _T("Version 2.0");
					pCConfig->SetDescription(sDesc);
					
				}
			}
		}
		
	
	}

	catch(DBBridge::IVideoConfig:: VideoConfigException& e )
	{
		
		std::cout << e.what();
	}
	

	_gettch();
	
	//������� � �������� ��� ���������
	SAFE_DELETE(pCVConfig);
	SAFE_DELETE(pCDConfig);
	SAFE_DELETE(pCConfig);
	sConection.clear();

	
	CoUninitialize();
	//_CrtDumpMemoryLeaks(  );
	return 0;
	
}


