#ifndef _MAINFUNCTION_H_
#define _MAINFUNCTION_H_

//! ������� ����������
//! \param sConectionInfo ���������� � ����������
//! \param sComputerIP IP ����������
//! \return ��������� �� ���������, ������� delete ��� ����������
DBBridge::IVideoConfig*	DBCreateVideoConfig(const std::tstring& sConectionInfo, const std::tstring& sComputerIP);

//! �������� ��� IP ����������� 
//! \param sConectionInfo ���������� � ����������
//! \param STLIpList �������� ������ IP
void GetAllIPAdress(const std::tstring& sConection, std::vector<std::tstring>& STLIpList);

//! TRACE �� ������������ ���������� ����������
//! \param szFormat ������, ��������� � trace
void TRACE(LPCTSTR szFormat, ...);

//! ����� ���������� �� ������
//! \param hRes �������� ��������� ������
//std::tstring AtlTraceRelease(HRESULT hRes);

void	CheckResult(HRESULT hRes);

//! �������������� ������ � ������������ ����������� ����������
//! \param szFormat ������ ��� ��������������
std::tstring Format(LPCTSTR szFormat, ...);

#endif