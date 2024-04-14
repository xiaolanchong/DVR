#ifndef _CCAMERACONFIG_H_
#define _CCAMERACONFIG_H_

#include "CTypeAccessor.h"

class CCameraConfig: public DBBridge::ICameraConfig
{
public:
	CCameraConfig(CSession& Session, INT nDeviceID, INT nPinNumber);
	virtual ~CCameraConfig();
	
	//! �������� ������������ ������ � ��
	//! \return ������������ ��
	virtual int GetCameraID();

	//! �������� ��� ������
	//! \return ����� ����
	virtual int GetCameraPin();

	//! �������� ���������� ������
	//! \return ����������� ������ 
	virtual std::tstring GetDescription();

	//! ���������� ���������� ������
	//! \param sDesc ���������� ��������
	virtual void SetDescription(const std::tstring& sDesc);

protected:
	//ID ������
	DWORD m_nDeviceID;
	//Pin ������
	DWORD m_nPinNumber;
	//�����
	CSession m_session;
};
#endif