//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Êëàññ îïåðàöèé ñ ñîêåòàìè

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   02.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "NetSocket.h"
#include "InternetAddress.h"
#include "NetType.h"
#include "NetUtil.h"

class GuardSocket
{
	SOCKET&	m_GuardedSocket;
	bool	m_bOk;
public:
	GuardSocket(SOCKET& s): m_GuardedSocket(s), m_bOk(false){}
	~GuardSocket()
	{
		if(!m_bOk)
		{
			closesocket( m_GuardedSocket );
			m_GuardedSocket = INVALID_SOCKET;
		}
	}
	void Ok()
	{
		m_bOk = true;
	}
};
//======================================================================================//
//                                   class NetSocket                                    //
//======================================================================================//

NetSocket::NetSocket( SOCKET RawSocket ):
	m_RawSocket(RawSocket)
{

}

NetSocket::NetSocket( bool bNonBlocking , bool bReusable  ):
	m_RawSocket( ) 
{
	Create( bNonBlocking , bReusable);
}



NetSocket::~NetSocket()
{
	Close();
}

void	NetSocket::Create(bool bNonBlocking , bool bReusable)
{
	if ((m_RawSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		int err = NetLastError();
		Sys_Error ("NetTCPClient: socket: %s", NetErrorString(err));
	}
	GuardSocket _ggg(m_RawSocket);

	unsigned long		_true = 1;
	if( bNonBlocking)
	{
		if (ioctlsocket (m_RawSocket, FIONBIO, &_true) == -1)
		{
			int err = NetLastError();
			Sys_Error ("CreateSocket: ioctl FIONBIO: %s\n", NetErrorString(err));
		}
	}

	// make it reusable
	if ( bReusable )
	{
		if (setsockopt(m_RawSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_true, sizeof(DWORD)) == -1)
		{
			int err = NetLastError();
			Sys_Error ("CreateSocket: setsockopt SO_REUSEADDR: %s\n", NetErrorString(err));
		}
	}
	_ggg.Ok();
}

void	NetSocket::Close()
{
	if( m_RawSocket != INVALID_SOCKET )
	{
		closesocket( m_RawSocket );
		m_RawSocket = INVALID_SOCKET;
	}
}

void	NetSocket::Bind( unsigned short PortNumber )
{
	unsigned short RealPortNumber( PortNumber != PORT_ANY ? PortNumber : 0);
	InternetAddress OwnAddr( ADDR_ANY, RealPortNumber );
	
	sockaddr_in sa(OwnAddr);
	if (  bind (m_RawSocket, (struct sockaddr *)&sa, sizeof(sockaddr))  )
	{
		int err = NetLastError();
		Sys_Error( "CreateSocket: unable to bind: %s", NetErrorString(err) );
	}
}

void	NetSocket::Connect( const InternetAddress& Addr )
{
	sockaddr_in sa( Addr );

	int err = connect( m_RawSocket, 
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
}

void	NetSocket::Send( const void* pData, size_t DataSize)
{
	int length = static_cast<int>( DataSize );
	int ret = send(  m_RawSocket, (const char *)pData, length, 0 );
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

void	NetSocket::Listen( int MaxConnectionNumber )
{
	int err = listen ( m_RawSocket, MaxConnectionNumber < 0 ? SOMAXCONN : MaxConnectionNumber);
	if(err == SOCKET_ERROR)
	{
		int net_error = NetLastError();
		Sys_Error("Could not listen, reason:  %s\n", NetErrorString(net_error) );
	}
}

bool	NetSocket::Select( bool bForRead, unsigned int TimeoutInSec )
{
	fd_set	fdset;
	struct timeval tv;
	int number;

	FD_ZERO(&fdset);
	FD_SET( m_RawSocket, &fdset );
	tv.tv_sec	= 0;
	tv.tv_usec	= TimeoutInSec * 1000;

	number = bForRead?	select( 1, &fdset, NULL,   NULL, &tv ) : 
						select( 1, NULL,   &fdset, NULL, &tv ) ;
	return number == 1;
}

bool	NetSocket::IsConnected()
{
	fd_set	fdset;
	struct timeval tv = { 0, 0 };

	FD_ZERO(&fdset);
	FD_SET( m_RawSocket, &fdset );

	int number = select( 1, NULL , NULL, &fdset, &tv );
	return number == 0;
}

std::pair<SOCKET, InternetAddress>	NetSocket::Accept()
{
	struct sockaddr_in address;
	int addrlen = sizeof( struct sockaddr_in );

	SOCKET sock = accept( m_RawSocket, reinterpret_cast<sockaddr*>(&address), &addrlen );
	return std::make_pair( sock, InternetAddress(address) );
}

std::pair<bool, size_t>	NetSocket::Receive( void* pBuffer, size_t BufferSize )
{
	int ret = recv(m_RawSocket, (char *)pBuffer, static_cast<int>(BufferSize), 0);
	if ( ret> 0)
	{
		return std::make_pair(true, ret);
	}
	if( ret == 0 )
	{
		return std::make_pair(false, 0);
	}
	else
	{
		int err = NetLastError();

		switch ( err )
		{
		case WSAEWOULDBLOCK:
		case WSAECONNRESET:
		case WSAECONNREFUSED:
		case WSAEMSGSIZE:
			break;
		default:
			// Let's continue even after errors
		//	Sys_Debug( "NET_QueuePacket: %s", NetErrorString(err));
			break;
		}
	}
	return std::make_pair(false, 0);
}

void	NetSocket::SetBufferSize(unsigned int BufferSize, bool bForReceive)
{
	const int opt = bForReceive? SO_RCVBUF : SO_SNDBUF;
	const int value = BufferSize;
	if (setsockopt(m_RawSocket, SOL_SOCKET, opt, (const char *)&value, sizeof(int)) == -1)
	{
		int err = NetLastError();
		Sys_Error ("SetBufferSize: setsockopt SO_*BUF: %s\n", NetErrorString(err));
	}

	const int flag = 1;
	if (setsockopt( m_RawSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag)) == -1)
	{
		int err = NetLastError();
		Sys_Error ("SetBufferSize: setsockopt TCP_NODELAY: %s\n", NetErrorString(err));
	}
}

InternetAddress	NetSocket::GetAddress() const
{
	sockaddr_in addr;
	int len = sizeof(addr);
	if( SOCKET_ERROR == getsockname( m_RawSocket, reinterpret_cast<sockaddr*>(&addr), &len ) )
	{
		memset(&addr, 0, sizeof(addr));
	}
	return InternetAddress(addr);
}

void	NetSocket::SetSockOption( int opt, int		value )
{
	if (setsockopt(m_RawSocket, SOL_SOCKET, opt, (char *)&value, sizeof(int)) == -1)
	{
		int err = NetLastError();
		Sys_Error ("SetSockOption: %s\n", NetErrorString(err));
	}
}

void	NetSocket::SetSockOption( int opt, bool	value )
{
	SetSockOption( opt, value ? 1 : 0 );
}