#ifndef _CFRAME_H_
#define _CFRAME_H_

#include "CTypeAccessor.h"

struct FRAMELINE
{
	DWORD dwCameraID;
	SYSTEMTIME TimeBegin;
	SYSTEMTIME TimeEnd;

};

class CFrame
{
public:
	CFrame(const std::tstring& sConection);
	virtual ~CFrame();

	//! прочитать параметр типа строку из таблицы в заданном интервале
	//! \param FrameLines массив строк
	//! \param BeginTime начало интервала
	//! \param EndTime конец интервала
	void GetFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, std::vector<FRAMELINE>& FrameLines );

	//! вставить строку с заданым интервалом времени и ID камеры
	//! \param dwCameraID идентификатор камеры
	//! \param BeginTime начало интервала
	//! \param EndTime конец интервала
	void InsertFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, DWORD dwCameraID  );

	//! удалить строку в заданом интервале времени 
	//! \param BeginTime начало интервала
	//! \param EndTime конец интервала
	void DeleteFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime );


protected:
	//Строка соединения
	std::tstring m_sConectionInfo; 
	//Источник данных
	CDataSource m_db;
	//Сеанс
	CSession m_session;

	//! преобразует SYSTEMTIME в строку в формате yyyy.mm.dd hh:mm:ss
	//! \param SystemTime параметр для преобразования
	//! \return строка содержащая дату и время
	std::tstring SystemTimeToString(SYSTEMTIME SystemTime);

	//! формирует строку с условием пересечения интервалов времени
	//! \param sBeginTime начало интервала
	//! \param sEndTime конец интервала
	//! \return строка содержащая условие пересечения
	std::tstring GetConditionIntersection(std::tstring sBeginTime, std::tstring sEndTime);
};
#endif