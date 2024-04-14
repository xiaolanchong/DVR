#ifndef __DVRDBBRIDGE_H__
#define __DVRDBBRIDGE_H__

namespace CHCS
{
class IVideoSystemConfigEx;
}

namespace DBBridge
{

class IVideoConfig;

extern "C"
{

//! ������� ��������� ���������������� ������������ �� ������ ����������
//! \param szConnectionString ������ ����������, 0 - ������������ �� ���������
//! \return 
CHCS::IVideoSystemConfigEx* WINAPI CreateVideoSystemConfig(LPCWSTR szConnectionString);

CHCS::IVideoSystemConfigEx*	WINAPI CreateClientVideoSystemConfig( );

CHCS::IVideoSystemConfigEx*	WINAPI CreateServerVideoSystemConfig( );

}

}

#endif // __DVRDBBRIDGE_H__