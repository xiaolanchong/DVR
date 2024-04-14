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

	//! ��������� �������� ���� ������ �� ������� � �������� ���������
	//! \param FrameLines ������ �����
	//! \param BeginTime ������ ���������
	//! \param EndTime ����� ���������
	void GetFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, std::vector<FRAMELINE>& FrameLines );

	//! �������� ������ � ������� ���������� ������� � ID ������
	//! \param dwCameraID ������������� ������
	//! \param BeginTime ������ ���������
	//! \param EndTime ����� ���������
	void InsertFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime, DWORD dwCameraID  );

	//! ������� ������ � ������� ��������� ������� 
	//! \param BeginTime ������ ���������
	//! \param EndTime ����� ���������
	void DeleteFrameLine( SYSTEMTIME BeginTime, SYSTEMTIME EndTime );


protected:
	//������ ����������
	std::tstring m_sConectionInfo; 
	//�������� ������
	CDataSource m_db;
	//�����
	CSession m_session;

	//! ����������� SYSTEMTIME � ������ � ������� yyyy.mm.dd hh:mm:ss
	//! \param SystemTime �������� ��� ��������������
	//! \return ������ ���������� ���� � �����
	std::tstring SystemTimeToString(SYSTEMTIME SystemTime);

	//! ��������� ������ � �������� ����������� ���������� �������
	//! \param sBeginTime ������ ���������
	//! \param sEndTime ����� ���������
	//! \return ������ ���������� ������� �����������
	std::tstring GetConditionIntersection(std::tstring sBeginTime, std::tstring sEndTime);
};
#endif