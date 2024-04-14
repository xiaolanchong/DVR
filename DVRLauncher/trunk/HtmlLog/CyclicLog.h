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

//! \brief ������� ��� HtmlLog - ����������� ���-���� (����������� ������� ������ � ����������)
//! ����� ���������� <system>(x).html
//! \version 1.0
//! \date 07-06-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class CyclicLog
{
	//! ���������� ������ �����
	std::wstring	m_sPath;
	//! �������
	const std::wstring	m_sSystem;
	//! ����� �������� ����
	const CTime			m_timeStart;
	//! ������� ����� �����
	DWORD			m_nFileNo;

	//! ������������ ����� ������ � ����������
	const size_t			m_nFileNumber;
	//! ������������ ������ ����� � ������
	const size_t			m_nMaxSize;

	//! ������� ���������� ��� ������ ������
	//! \param sPath ��� ����������
	void	CheckDirectory( const std::wstring& sPath );

	//! ������� ���-����
	void	CreateLogFile();
	boost::shared_ptr< HtmlLog >	m_LogFile;
public:
	CyclicLog();
	virtual ~CyclicLog();

	//! ������� ���
	//! \param szPath		����������, ��� ����� ����������� �����
	//! \param szSystem		���������� �������, ���. ����� � ���
	//! \param nFileNumber	������������ ����� ������ � ����������
	//! \param nMaxSize		������������ ������ ����� � ������
	CyclicLog( const wchar_t*	szPath, const wchar_t*	szSystem, size_t nFileNumber, size_t nMaxSize );
	bool	AddRecord( HtmlLog::Severity sev, __time64_t time, const wchar_t* szValue );
};

#endif // _CYCLIC_LOG_7956605058336955_