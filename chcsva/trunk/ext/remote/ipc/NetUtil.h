//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Утилиты для сетевого взаимодействия

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_UTIL_4089792047501293_
#define _NET_UTIL_4089792047501293_

const char *	NetErrorString (int code);

inline int NetLastError()
{
#if defined( _WIN32 )
	return WSAGetLastError();
#else
	return errno;
#endif
}

LPCTSTR GetLocalIP();



//////////////////////////////////////////////////////////////////////////

class NetException : public std::runtime_error
{
public:
	NetException(const char* DescriptionText) : 
	  std::runtime_error( DescriptionText )
	  {
	  }
};

void Sys_Error (const char *FmtString, ...);
void Sys_Debug (const char *FmtString, ...);

#endif // _NET_UTIL_4089792047501293_