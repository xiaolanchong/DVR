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
	
	//Конвертировать в Unicode строку
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
	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	//Сформировать комманду
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str());
	
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
	//Класс наследуемый Accessor
	CProperty< DWORD > Property;

	//Сформировать строку
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
	//Соеденение
	Property.OpenDataSource( m_session);
	
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
				//Запомнить значение
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
	//Наследует Accessor
	CBlobProperty BlobProperty;

	//Сформировать строку
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
	//Соеденение
	BlobProperty.OpenDataSource( m_session);

	// Выполнить команду (биндить результат в соответсвие с картой)
	HRESULT hRes = BlobProperty.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается внутренний указатель на строку
		while( BlobProperty.MoveNext() == S_OK )
		{
			//Количество считанных байт
			ULONG cb = 0;
			//Порция считывания
			BYTE myBuffer[65536];
			if(BlobProperty.pBlobField != NULL)
			{
				//Считывать пока не кончаться данные
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
						//Зарезервировать память
						binValue.reserve(cb);
						//Вставить в конец считанные данные
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
	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("UPDATE Computers SET %s = %d WHERE ComputerIP = '%s'"), szName, dwValue, m_sComputerIP.c_str()); 

	//Класс наследуемый от Accessor
	CProperty< DWORD > Property;

	//Соеденение
	Property.OpenDataSource( m_session);
	
	//Выполнить команду и ничего не биндить
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
	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("UPDATE Computers SET %s = '%s' WHERE ComputerIP = '%s'"), szName, sValue, m_sComputerIP.c_str()); 

	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	//Соеденение
	Property.OpenDataSource( m_session);
	
	//Выполнить команду и ничего не биндить
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
	//Наследует Accessor
	CBlobProperty BlobProperty;

	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT %s FROM Computers WHERE ComputerIP = '%s'"), szName, m_sComputerIP.c_str()); 
	
	//Соединение с БД
	BlobProperty.OpenDataSource(m_session);
	
	// Выполнить команду (биндить результат в соответсвие с картой)
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

			//Наследует ISequentialStream
			CISSHelper CissHelper;

			//Количество реально записанных байт в промежуточный буффер
			ULONG cb = 0;

			//выделить и заполнить буффер класса CISSHelper
			HRESULT hRes = CissHelper.Write(&(binValue[0]), (ULONG)binValue.size(), &cb);
			if(FAILED(hRes) || (cb != binValue.size()))
			{
				CheckResult(hRes);
				throw DBBridge::IVideoConfig::VideoConfigException("Error Write() ");
			}

			//Установить статус
			CissHelper.m_ulStatus = DBSTATUS_S_OK;

			//Присвоить указатель полю ISequentialStream 
			BlobProperty.pBlobField = &CissHelper;
			//Установить размер
			BlobProperty.m_dwFieldBlobLength = (ULONG)binValue.size();
			//Установить статус
			BlobProperty.m_dwFieldBlobStatus = DBSTATUS_S_OK;

			//Назначить данные 
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
	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT Devices.DeviceUID FROM Computers INNER JOIN Devices ON Computers.ComputerID = Devices.ComputerID AND Computers.ComputerIP = '%s'"), m_sComputerIP.c_str()); 

	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	//Соеденение
	Property.OpenDataSource(m_session);		
	
	//Выполнить команду и биндить результат в соответствие с картой
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается и перебирается внутренний указатель на строку
		while( Property.MoveNext() == S_OK )
		{
			//Проверить статус
			DWORD dwStatusComputer = Property.m_dwPropertyStatus;
			if((dwStatusComputer == DBSTATUS_S_OK))
			{
				//Конвертировать в string
				std::tstring sDUID(Property.m_Property);
				//Положить в массив
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
	//Возвращаемый интерфейс
	CDeviceConfig* pDeviceConfig  = NULL;
	
	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("SELECT Devices.DeviceID FROM Devices INNER JOIN Computers ON Computers.ComputerID = Devices.ComputerID AND Devices.DeviceUID = '%s' AND Computers.ComputerIP = '%s'"), szUID, m_sComputerIP.c_str()); 
	
	//Наследует Accessor
	CProperty< TCHAR[255] > Property;

	//Соеденение
	Property.OpenDataSource(m_session);

	//Выполнить команду и биндить результат в соответствие с картой
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		// Создается внутренний указатель на строку
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
//Получить указатель на интерфейс IDeviceConfig из интерфейса IVideoConfig
DBBridge::IDeviceConfig* CVideoConfig::CreateDevice( LPCTSTR szUID )
{
	//возвращаемый интерфейс
	CDeviceConfig* pDeviceConfig = NULL;

	//Взять ID компьютера и преобразовать  в формат string
	DWORD dwID = 0;
	GetValue(0,_T("ComputerID"), dwID);

	//Сформировать команду
	std::tstring szSQLQuery = Format( _T("INSERT INTO Devices (ComputerID, DeviceUID) VALUES(%d, '%s')"), dwID, szUID); 
	
	//Наследует Accessor
	CProperty< DWORD > Property;

	//Соеденение
	Property.OpenDataSource(m_session);
	
	//Выполнить команду и ничего не биндить
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(SUCCEEDED(hRes))
	{
		//Создать экземпляр класса IDeviceConfig в конструктор передать UID
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

	//Вернуть указатель на созданное устройство
	return pDeviceConfig;
	
}

//Удалить устройство с данным UID
void CVideoConfig::DeleteDevice( LPCTSTR szUID )
{
	
	//Сформировать команду
	std::tstring szSQLQuery = Format(_T("DELETE FROM Devices WHERE (DeviceUID = '%s')"), szUID); 
	
	//Класс, наследуемый от Accessor
	CProperty< DWORD > Property;

	//Соеденение
	Property.OpenDataSource(m_session);
	
	//Выполнить команду и ничего не биндить
	HRESULT hRes = Property.DoCommand(szSQLQuery.c_str(), FALSE);
	if(FAILED(hRes))
	{
		CheckResult(hRes);
	}
	
}
