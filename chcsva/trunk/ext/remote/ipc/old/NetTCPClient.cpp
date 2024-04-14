//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Кроссплатформенный мультипотоковый клиент для отправки данных

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   29.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "NetTCPClient.h"
#include "NetUtil.h"

namespace
{

void	Send( 
				SOCKET sock,
				const void* pData, 
				size_t nSize)
{
	int		ret;
	int length = nSize;
	const char* pMarker = (const char*)&length;
	send( sock, pMarker, sizeof(int) , 0 );
	ret = send(  sock, (const char *)pData, length, 0 );
	if (ret == -1)
	{
		int err = NetLastError();
		// wouldblock is silent
		if (err == WSAEWOULDBLOCK)
			return;

		if ( err == WSAECONNRESET )
			return;

		Sys_Error ( "NET_SendPacket ERROR: %s\n", NetErrorString(err));
	}
}

int	WaitForSend( SOCKET sock, unsigned int timeout)
{
	fd_set	fdset;
	struct timeval tv;
	int number;

	FD_ZERO(&fdset);
	FD_SET( sock, &fdset );

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	// Block infinitely until a message is in the queue
	number = select( 1, NULL, &fdset, NULL, /*net_sleepforever */ FALSE ? NULL : &tv );
	return number;
}

}
//======================================================================================//
//                                  class NetTCPClient                                  //
//======================================================================================//

NetTCPClient::NetTCPClient()
{
}

NetTCPClient::~NetTCPClient()
{
}

void	NetTCPClient::Send(	
				const InternetAddress& ServerAddress, 
				const void* pData, 
				size_t nSize) 
{
	SOCKET newsocket;
	if ((newsocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		int err = NetLastError();
		Sys_Error ("NetTCPClient: socket: %s", NetErrorString(err));
	}

	sockaddr_in sa( ServerAddress );

	int err = connect( newsocket, 
						reinterpret_cast<sockaddr*>(& sa), 
						sizeof( struct sockaddr ) );
	if(err == SOCKET_ERROR)
	{
		int net_error = NetLastError();
		if( net_error != WSAEWOULDBLOCK )
		{
			Sys_Error("Could not connect socket, reason:  %s", NetErrorString(net_error) );
		}
	}

	int res = WaitForSend( newsocket, 20 );

	::Send( newsocket, pData, nSize );

	closesocket(newsocket);
}

//////////////////////////////////////////////////////////////////////////

NetTCPLongRunClient::NetTCPLongRunClient( 
					const InternetAddress& ServerAddress, 
					unsigned short ClientPort ):
	m_Socket( )
{
	m_Socket.Bind( ClientPort );
}

void	NetTCPLongRunClient::Send( const void* pData, size_t nSize ) 
{
	m_Socket.Send( pData, nSize );
}

