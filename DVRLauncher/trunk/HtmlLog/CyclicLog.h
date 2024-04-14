//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Implements cyclic html log, gets the max file number in the directory
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 21.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _CYCLIC_LOG_7956605058336955_
#define _CYCLIC_LOG_7956605058336955_

#include "HtmlLog.h"
#include <boost/shared_ptr.hpp>

//======================================================================================//
//                                   class CyclicLog                                    //
//======================================================================================//

//! \brief обертка над HtmlLog - циклический лог-файл (циклическая очередь файлов в директории)
//! файлы называются <system>(x).html
//! \version 1.0
//! \date 07-06-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class CyclicLog
{
	//! директория файлов логов
	std::wstring	m_sPath;
	//! система
	const std::wstring	m_sSystem;
	//! время созадния лога
	const CTime			m_timeStart;
	//! текущий номер файла
	DWORD			m_nFileNo;

	//! максимальное число файлов в директории
	const size_t			m_nFileNumber;
	//! максимальный размер файла в байтах
	const size_t			m_nMaxSize;

	//! создать директорию для записи файлов
	//! \param sPath имя директории
	void	CheckDirectory( const std::wstring& sPath );

	//! создать лог-файл
	void	CreateLogFile();
	boost::shared_ptr< HtmlLog >	m_LogFile;
public:
	CyclicLog();
	virtual ~CyclicLog();

	//! создать лог
	//! \param szPath		директория, где будут создаваться файлы
	//! \param szSystem		програмная система, кот. пишет в лог
	//! \param nFileNumber	максимальное число файлов в директории
	//! \param nMaxSize		максимальный размер файла в байтах
	CyclicLog( const wchar_t*	szPath, const wchar_t*	szSystem, size_t nFileNumber, size_t nMaxSize );
	bool	AddRecord( HtmlLog::Severity sev, __time64_t time, const wchar_t* szValue );
};

#endif // _CYCLIC_LOG_7956605058336955_