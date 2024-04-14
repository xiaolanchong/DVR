//! 
//! класс лог файла в формате таблицы HTML
//! 
//! Copyright © 2006, ElVEES
//! 
//! 2006-07-05	copied from CSLauncher

#pragma once

//#include <atlbase.h>
#include <time.h>
#include <fstream>
#include <vector>

//! класс лог файла в формате таблицы HTML
//! 3 столбца - дата, время, текст сообщения с цветом, соответствующим типу
class HtmlLog
{
	mutable std::vector<char>	m_Cache;
	std::ofstream	m_File;
public:
	//! тип сообщения по возрастанию значения
	enum Severity
	{
		//! отладочное (иожно, например игнорировать)
		sev_debug,
		//! информация
		sev_info,
		//! предупреждение
		sev_warning,
		//! ошибка
		sev_error,
		//! неизвестный тип
		sev_unknown
	};

	//! создать файл
	//! \param szFileName имя лог-файла (относительное или абсолютное)
	//! \param szSystem		имя программной подсистемы, создавшей файл (в заголовке, например, клиент, сервер)
	//! \param timeStart	время создания
	HtmlLog		(	const wchar_t*	szFileName, 
					const wchar_t*	szSystem, 
					__time64_t		timeStart = _time64(NULL) );
	//! добавить запись в лог
	//! \param sev		тип
	//! \param time		время записи
	//! \param szValue	текст
	//! \return			добавлено/или нет (например, диск переполнен)
	bool	AddRecord( 
						Severity sev, 
						__time64_t time, 
						const wchar_t* szValue );

	//! получить размер файла
	//! \return размер в байтах
	size_t	GetFileSize() { return m_File.tellp(); }
	~HtmlLog(void);
};
