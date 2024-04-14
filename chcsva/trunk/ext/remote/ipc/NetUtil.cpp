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
#include "chcs.h"
#include "NetUtil.h"

void Sys_Error (const char *FmtString, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, FmtString);
	vsnprintf (text, 1024, FmtString, argptr);
	va_end (argptr);
#ifdef USE_INT3
	__asm int 3 
#endif
		throw NetException(text);
}

void Sys_Debug (const char *FmtString, ...)
{
#if defined(_DEBUG) && 0
	va_list		argptr;
	char		text[1024];

	va_start (argptr, FmtString);
	vsnprintf (text, 1024, FmtString, argptr);
	va_end (argptr);

	OutputDebugStringA(text);
	OutputDebugStringA("\n");
#endif
}

//////////////////////////////////////////////////////////////////////////

#if defined( _WIN32 )
/*
====================
NET_ErrorString
====================
*/
const char *NetErrorString (int code)
{
	switch (code)
	{
	case WSAEINTR: return "WSAEINTR";
	case WSAEBADF: return "WSAEBADF";
	case WSAEACCES: return "WSAEACCES";
	case WSAEDISCON: return "WSAEDISCON";
	case WSAEFAULT: return "WSAEFAULT";
	case WSAEINVAL: return "WSAEINVAL";
	case WSAEMFILE: return "WSAEMFILE";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS: return "WSAEINPROGRESS";
	case WSAEALREADY: return "WSAEALREADY";
	case WSAENOTSOCK: return "WSAENOTSOCK";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE: return "WSAEMSGSIZE";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN: return "WSAENETDOWN";
	case WSAENETUNREACH: return "WSAENETUNREACH";
	case WSAENETRESET: return "WSAENETRESET";
	case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: return "WSAECONNRESET";
	case WSAENOBUFS: return "WSAENOBUFS";
	case WSAEISCONN: return "WSAEISCONN";
	case WSAENOTCONN: return "WSAENOTCONN";
	case WSAESHUTDOWN: return "WSAESHUTDOWN";
	case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
	case WSAETIMEDOUT: return "WSAETIMEDOUT";
	case WSAECONNREFUSED: return "WSAECONNREFUSED";
	case WSAELOOP: return "WSAELOOP";
	case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
	case WSASYSNOTREADY: return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: return "WSATRY_AGAIN";
	case WSANO_RECOVERY: return "WSANO_RECOVERY";
	case WSANO_DATA: return "WSANO_DATA";
	default: return "NO ERROR";
	}
}
#else
/*
====================
NET_ErrorString
====================
*/
const char *NET_ErrorString (int code)
{
	return strerror(code);
}
#endif

LPCTSTR GetLocalIP()
{
	static TCHAR buff[512];
#if defined(UNICODE) || defined(UNICODE)
	char tmp[512];
	gethostname(tmp, 512);
	mbstowcs( buff, tmp, 512 );
#else
	gethostname(buff, 512);
#endif
	return buff;
}


