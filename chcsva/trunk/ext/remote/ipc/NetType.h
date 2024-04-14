//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description:  россплатформенное объ€вление типов и констант

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_TYPE_4071646560885881_
#define _NET_TYPE_4071646560885881_

#if defined( _WIN32 )

//#include <winsock.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>

typedef int socklen_t;

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define WSAEWOULDBLOCK		EWOULDBLOCK
#define WSAEMSGSIZE			EMSGSIZE
#define WSAEADDRNOTAVAIL	EADDRNOTAVAIL
#define WSAEAFNOSUPPORT		EAFNOSUPPORT
#define WSAECONNRESET		ECONNRESET
#define WSAECONNREFUSED     ECONNREFUSED
#define WSAEADDRINUSE EADDRINUSE

#define ioctlsocket ioctl
#define closesocket close
#ifndef INVALID_SOCKET
#define	INVALID_SOCKET (~0)
#endif

#undef SOCKET
typedef int SOCKET;
#define FAR
#endif

#define	PORT_ANY	-1

#endif // _NET_TYPE_4071646560885881_