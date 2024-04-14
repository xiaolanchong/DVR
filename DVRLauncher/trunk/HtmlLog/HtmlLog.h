//! 
//! ����� ��� ����� � ������� ������� HTML
//! 
//! Copyright � 2006, ElVEES
//! 
//! 2006-07-05	copied from CSLauncher

#pragma once

//#include <atlbase.h>
#include <time.h>
#include <fstream>
#include <vector>

//! ����� ��� ����� � ������� ������� HTML
//! 3 ������� - ����, �����, ����� ��������� � ������, ��������������� ����
class HtmlLog
{
	mutable std::vector<char>	m_Cache;
	std::ofstream	m_File;
public:
	//! ��� ��������� �� ����������� ��������
	enum Severity
	{
		//! ���������� (�����, �������� ������������)
		sev_debug,
		//! ����������
		sev_info,
		//! ��������������
		sev_warning,
		//! ������
		sev_error,
		//! ����������� ���
		sev_unknown
	};

	//! ������� ����
	//! \param szFileName ��� ���-����� (������������� ��� ����������)
	//! \param szSystem		��� ����������� ����������, ��������� ���� (� ���������, ��������, ������, ������)
	//! \param timeStart	����� ��������
	HtmlLog		(	const wchar_t*	szFileName, 
					const wchar_t*	szSystem, 
					__time64_t		timeStart = _time64(NULL) );
	//! �������� ������ � ���
	//! \param sev		���
	//! \param time		����� ������
	//! \param szValue	�����
	//! \return			���������/��� ��� (��������, ���� ����������)
	bool	AddRecord( 
						Severity sev, 
						__time64_t time, 
						const wchar_t* szValue );

	//! �������� ������ �����
	//! \return ������ � ������
	size_t	GetFileSize() { return m_File.tellp(); }
	~HtmlLog(void);
};
