#include "stdafx.h"
#include "CFrame.h"
#include "IVideoConfig.h"
#include "CVideoConfig.h"
#include "CDeviceConfig.h"
#include "MainFunction.h"
#include "CFrameAccessor.h"

CFrame::CFrame(const std::tstring& sConection):
	m_sConectionInfo(sConection)	
{
	//Конвертировать в Unicode строку
	CT2W psConvertToW( sConection.c_str());

	HRESULT hRes = m_db.OpenFromInitializationString(psConvertToW);
	if(SUCCEEDED(hRes))
	{
		hRes = m_session.Open(m_db);
		if(FAILED(hRes))
		{
			CheckResult(hRes);
		}
	}
	else
	{
		CheckResult(hRes);
	}
}
CFrame::~CFrame()
{

}
void CFrame::GetFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, std::vector<FRAMELINE>& FrameLines  )
{
	//Наследует Accessor
	CFrameProperty FrameProperty;

	std::tstring sBeginTime = SystemTimeToString(BeginTime);
	std::tstring sEndTime = SystemTimeToString(EndTime);
	std::tstring sCondition = GetConditionIntersection(sBeginTime, sEndTime);

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT CameraID, TimeBegin, TimeEnd FROM Frames WHERE %s"),sCondition.c_str());

	//Соеденение
	FrameProperty.OpenDataSource(m_session);

	// Выполнить команду (биндить результат в соответсвие с картой)
	HRESULT hRes = FrameProperty.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается внутренний указатель на строку
		while( FrameProperty.MoveNext() == S_OK )
		{
			//Создать и обнулить содержимое структуры
			FRAMELINE frameline;
			ZeroMemory(&frameline, sizeof(FRAMELINE));

			//Проверить статус
			if((FrameProperty.m_dwCameraIDStatus == DBSTATUS_S_OK) && (FrameProperty.m_dwTimeBeginStatus == DBSTATUS_S_OK) && (FrameProperty.m_dwTimeEndStatus == DBSTATUS_S_OK))
			{
				frameline.dwCameraID = FrameProperty.m_dwCameraID;
				VariantTimeToSystemTime(FrameProperty.m_TimeBegin.date, &(frameline.TimeBegin));
				VariantTimeToSystemTime(FrameProperty.m_TimeEnd.date, &(frameline.TimeEnd));

				//Положить в массив
				FrameLines.push_back(frameline); 

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
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");	
	}

	FrameProperty.CloseAll();
}

void CFrame::InsertFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, DWORD dwCameraID  )
{
	//Наследует Accessor
	CProperty< DWORD> FrameProperty;

	//Конвертирвать из SYSTEMTIME в string
	std::tstring sBeginTime = SystemTimeToString(BeginTime);
	std::tstring sEndTime = SystemTimeToString(EndTime);

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("INSERT INTO Frames (CameraID, TimeBegin, TimeEnd) VALUES(%d, '%s', '%s')"), dwCameraID, sBeginTime.c_str(), sEndTime.c_str()); 
	
	//Соеденение
	FrameProperty.OpenDataSource(m_session);
	
	// Выполнить команду и ничего не биндить
	HRESULT hRes = FrameProperty.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");	
	}

	FrameProperty.CloseAll();
}

void CFrame::DeleteFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime )
{
	//Наследует Accessor
	CProperty< DWORD> FrameProperty;

	std::tstring sBeginTime = SystemTimeToString(BeginTime);
	std::tstring sEndTime = SystemTimeToString(EndTime);
	
	//Взять условие пересечения интервалов
	std::tstring sCondition = GetConditionIntersection(sBeginTime, sEndTime);

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("DELETE FROM Frames WHERE %s"),sCondition.c_str()); 

	//Соеденение
	FrameProperty.OpenDataSource( m_session);

	// Выполнить команду и ничего не биндить
	HRESULT hRes = FrameProperty.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
		throw DBBridge::IVideoConfig::VideoConfigException("Error Open() ");	
	}

	FrameProperty.CloseAll();
}

std::tstring CFrame::SystemTimeToString(SYSTEMTIME SystemTime)
{
	
	TCHAR aValue[255];
	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wYear, aValue, 255, 10);
	std::tstring sYear(aValue);

	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wMonth, aValue, 255, 10);
	std::tstring sMonth(aValue);

	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wDay, aValue, 255, 10);
	std::tstring sDay(aValue);

	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wHour, aValue, 255, 10);
	std::tstring sHour(aValue);

	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wMinute, aValue, 255, 10);
	std::tstring sMinute(aValue);

	ZeroMemory(aValue, sizeof(TCHAR)*255);
	_itot_s(SystemTime.wSecond, aValue, 255, 10);
	std::tstring sSecond(aValue);

	std::tstring sTime = Format(_T("%s.%s.%s %s:%s:%s"),sYear.c_str(), sMonth.c_str(), sDay.c_str(), sHour.c_str(), sMinute.c_str(), sSecond.c_str()); 
	
	return sTime;

}
std::tstring CFrame::GetConditionIntersection(std::tstring sBeginTime, std::tstring sEndTime)
{
	//Сформировать условие
	std::tstring sCondition = Format(_T("(TimeBegin <= '%s' AND TimeEnd >= '%s') OR (TimeBegin <= '%s' AND TimeEnd >= '%s') OR (TimeBegin >= '%s' AND TimeEnd <= '%s')"),sBeginTime.c_str(), sBeginTime.c_str(), sEndTime.c_str(), sEndTime.c_str(), sBeginTime.c_str(), sEndTime.c_str()); 
	
	//Вернуть строку с условием
	return sCondition;
}
