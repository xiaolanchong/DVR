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
	//ID - которое надо вернуть
	DWORD dwCameraID = DWORD(-1);	// warning, any invalid value

	//Наследует Accessor
	CProperty< DWORD > Property;

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT CameraID FROM Cameras WHERE DeviceID = '%d' AND PinNumber = '%d'"), m_nDeviceID, m_nPinNumber); 
	
	//Соеденение
	Property.OpenDataSource(m_session);
	
	// Выполнить команду (биндить результат в соответсвие с картой)
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается внутренний указатель на строку
		if( Property.MoveNext() == S_OK )
		{
			//Проверить статус
			DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
			if((dwFDiskStatus == DBSTATUS_S_OK))
			{
				//Заполнить значение 
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
	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	// Строка содержащая поле Description
	std::tstring sDescriptor;

	//Взять идентификатор камеры
	INT nCameraID = GetCameraID();

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT Description FROM Cameras WHERE CameraID = %d"), nCameraID);

	//Соеденение
	Property.OpenDataSource(m_session);

	// Выполнить команду (биндить результат в соответсвие с картой)
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается внутренний указатель на строку
		if( Property.MoveNext() == S_OK )
		{
			//Проверить статус
			DWORD dwFDiskStatus = Property.m_dwPropertyStatus;
			if((dwFDiskStatus == DBSTATUS_S_OK))
			{
				//Заполнить значение 
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
	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	//Взять идентификатор камеры
	INT nCameraID = GetCameraID();

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("UPDATE Cameras SET Description = '%s' WHERE CameraID = %d"), sDesc.c_str(), nCameraID);

	//Соеденение
	Property.OpenDataSource(m_session);

	// Выполнить команду и ничего не биндить
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
	}

	Property.CloseAll();
}