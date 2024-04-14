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

//! создать интерфейс конфигурирования видеосистемы на данном компьютере
//! \param szConnectionString строка соединения, 0 - использовать по умолчанию
//! \return 
CHCS::IVideoSystemConfigEx* WINAPI CreateVideoSystemConfig(LPCWSTR szConnectionString);

CHCS::IVideoSystemConfigEx*	WINAPI CreateClientVideoSystemConfig( );

CHCS::IVideoSystemConfigEx*	WINAPI CreateServerVideoSystemConfig( );

}

}

#endif // __DVRDBBRIDGE_H__