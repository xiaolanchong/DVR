#ifndef _MAINFUNCTION_H_
#define _MAINFUNCTION_H_

//! создать интферфейс
//! \param sConectionInfo информация о соединении
//! \param sComputerIP IP компьютера
//! \return указатель на интерфейс, вызвать delete при уничожении
DBBridge::IVideoConfig*	DBCreateVideoConfig(const std::tstring& sConectionInfo, const std::tstring& sComputerIP);

//! получить все IP компьютеров 
//! \param sConectionInfo информация о соединении
//! \param STLIpList выходной массив IP
void GetAllIPAdress(const std::tstring& sConection, std::vector<std::tstring>& STLIpList);

//! TRACE на произвольное количество аргументов
//! \param szFormat строка, выводимая в trace
void TRACE(LPCTSTR szFormat, ...);

//! Вывод информации об ошибке
//! \param hRes название полученой ошибки
//std::tstring AtlTraceRelease(HRESULT hRes);

void	CheckResult(HRESULT hRes);

//! форматирование строки с произвольным количеством аргументов
//! \param szFormat строка для форматирования
std::tstring Format(LPCTSTR szFormat, ...);

#endif