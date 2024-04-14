//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Кроссплатформенный мультипотоковый (i hope) сервер сокетов

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "NetTCPServer.h"
#include "NetType.h"
#include "NetUtil.h"



std::string InternetAddressToString (const InternetAddress& Addr)
{
	char	s[64];

	memset(s, 0, 64 * sizeof(char) );

	_snprintf ( s, sizeof( s )/sizeof(char), ("%i.%i.%i.%i:%i"), 
		Addr.sin_addr.S_un.S_un_b.s_b1, 
		Addr.sin_addr.S_un.S_un_b.s_b2, 
		Addr.sin_addr.S_un.S_un_b.s_b3, 
		Addr.sin_addr.S_un.S_un_b.s_b4, 
		ntohs(Addr.sin_port)
		);

	return std::string(s);
}

bool IsEqualAddresses( const InternetAddress& lhs, const InternetAddress& rhs)
{
	return 0 == memcmp( &lhs, &rhs, sizeof(InternetAddress) );
}

SOCKET CreateSocket ( const TCHAR *net_interface, unsigned short port)
{
	SOCKET				newsocket;

	if ((newsocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		int err = NetLastError();
		if (err != WSAEAFNOSUPPORT)
		{
			// throw
			Sys_Error ("WARNING: TCP_OpenSocket: socket: %s", NetErrorString(err));
		}
		return 0;
	}

	// make it non-blocking
	unsigned long		_true = 1;
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		int err = NetLastError();
		Sys_Error ("CreateSocket: ioctl FIONBIO: %s\n", NetErrorString(err));
		return 0;
	}

	// make it reusable
	if ( 0 )
	{
		if (setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_true, sizeof(DWORD)) == -1)
		{
			int err = NetLastError();
			Sys_Error ("CreateSocket: setsockopt SO_REUSEADDR: %s\n", NetErrorString(err));
			return 0;
		}
	}
#if 0
	if (!net_interface || !net_interface[0] || !_tcsicmp(net_interface, _T("localhost")))
		address.sin_addr.s_addr = INADDR_ANY;
	else
		NET_StringToSockaddr (net_interface, (struct sockaddr *)&address);

	address.sin_family = AF_INET;
#else
	InternetAddress NetAddr( net_interface, port );
#endif

	if (port == PORT_ANY)
	{
		NetAddr.sin_port = 0;
	}

	sockaddr_in sa(NetAddr);
	if (  bind (newsocket, (struct sockaddr *)&sa, sizeof(sockaddr))  )
	{
		int err = NetLastError();
		closesocket (newsocket);
		Sys_Error( "CreateSocket: unable to bind: %s", NetErrorString(err) );
		return 0;
	}
	return newsocket;
}

//======================================================================================//
//                                  class NetTCPServer                                  //
//======================================================================================//

NetTCPServer::NetTCPServer(
				unsigned short nPort, 
				int nMaxClientIdleTime, 
				int nMaxSocketConnections
				):
			m_bWorking(false),
			m_nMaxClientIdleTimeInSeconds(nMaxClientIdleTime)
{
	Start( nPort, nMaxSocketConnections );

	m_bWorking = true;
	m_Thread.reset( new boost::thread(boost::bind( &NetTCPServer::ThreadFunc, this ) ) ); 
}

NetTCPServer::~NetTCPServer()
{
	m_bWorking = false;
	m_Thread->join();

	closesocket ( m_ServerSocket );
	m_ServerSocket = INVALID_SOCKET;
}

//////////////////////////////////////////////////////////////////////////

void	NetTCPServer::CloseClient( const InternetAddress& NetAddr )
{

}

void	NetTCPServer::ThreadFunc()
{
	bool done = false;
	bool queued;
	int sockets_ready;

	while ( InWorking() ) 
	{
		// Wait for messages
		struct sockaddr_in address;
		int addrlen = sizeof( struct sockaddr_in );

		NET_ThreadLock();
		// for idle sockets
		CheckIdleSockets();
		SOCKET sock = accept( GetSocket(), reinterpret_cast<sockaddr*>(&address), &addrlen );
		if( sock != INVALID_SOCKET )
		{
			InternetAddress NewClient( address );
			m_Clients.push_back( boost::make_tuple( NewClient, sock, _time64(NULL) ) );
		}
		NET_ThreadUnlock();
		sockets_ready = NET_Sleep();
		// Service messages
		//
		done = false;
		while ( !done && sockets_ready )
		{
			done = true;

			// Link
			NET_ThreadLock();

			for( size_t i = 0; i < m_Clients.size(); ++i )
			{
				SOCKET sock = boost::get<1>(m_Clients[i]);
				queued = NET_QueuePacket( sock , boost::get<0>( m_Clients[i] )  );
				if ( queued )
				{
					boost::get<2>( m_Clients[i] ) = _time64(0);
					done = false;
				}
			}
			NET_ThreadUnlock();
			if( !InWorking() ) 
			{
				return ;
			}
		}
		//		Sleep( 1 );
	}
}

void	NetTCPServer::Start( unsigned short PortNumber, int  nMaxSocketConnection )
{
	SOCKET s = CreateSocket ( GetLocalIP() , PortNumber); 
	if (s == INVALID_SOCKET)
	{
		s = CreateSocket (GetLocalIP() , PORT_ANY);
	}

	SetSocket( s );

	int err = listen ( GetSocket(), nMaxSocketConnection < 0 ? SOMAXCONN : nMaxSocketConnection);
	if(err == SOCKET_ERROR)
	{
		int net_error = NetLastError();
		Sys_Error("Could not listen, reason:  %s\n", NetErrorString(net_error) );
	}
}

bool	NetTCPServer::NET_QueuePacket ( SOCKET sock, const InternetAddress& From)
{
	int				ret;
	int				err;
	const size_t NET_MAX_MESSAGE = 65000;
	unsigned char	buf[ NET_MAX_MESSAGE ];

	if (sock != INVALID_SOCKET)
	{
		ret = recv(sock, (char *)buf, NET_MAX_MESSAGE, 0);
		if ( /*ret != -1 && */ret> 0)
		{
			AddData( From,  buf, ret );
			return true;
		}
		if( ret == 0 )
		{
			return false;
		}
		else
		{
			err = NetLastError();

			switch ( err )
			{
			case WSAEWOULDBLOCK:
			case WSAECONNRESET:
			case WSAECONNREFUSED:
			case WSAEMSGSIZE:
				break;
			default:
				// Let's continue even after errors
				Sys_Debug( "NET_QueuePacket: %s", NetErrorString(err));
				break;
			}
		}
	}
	return false;
}

void NetTCPServer::NET_ThreadLock()
{
}

void NetTCPServer::NET_ThreadUnlock()
{

}


void	NetTCPServer::CheckIdleSockets()
{
	if( m_nMaxClientIdleTimeInSeconds <= 0 ) return;
	SocketArr_t::iterator it = m_Clients.begin();
	for(; it != m_Clients.end();)
	{
		INT64 nLastAccessTime =  boost::get<2>( *it );
		if( (_time64(NULL) - nLastAccessTime ) > m_nMaxClientIdleTimeInSeconds )
		{
			closesocket( boost::get<1>(*it) );
			std::string ps = InternetAddressToString (boost::get<0>(*it));
			Sys_Debug("Idle socket timeout for %s.Socket will be closed explicitly", ps.c_str() );
			it = m_Clients.erase( it );
		}
		else
			++it;
	}
}

// sleeps until one of the net sockets has a message.  Lowers thread CPU usage immensely
int NetTCPServer::NET_Sleep(  )
{
	fd_set	fdset;
	int i;
	struct timeval tv;
	int number;

		NET_ThreadLock();
	FD_ZERO(&fdset);
	i = 0;
	int cl = m_Clients.size();
	for( i = 0; i < (int)m_Clients.size(); ++i)
	{
		SOCKET sock = boost::get<1>(m_Clients[i]);
		FD_SET( sock, &fdset );
		if ( int( sock) > i )
			i = (int)sock;
	}
		NET_ThreadUnlock();
	tv.tv_sec = 0;
	tv.tv_usec = 20 * 1000;

	// Block infinitely until a message is in the queue
	number = select( i+1, &fdset, NULL, NULL, /*net_sleepforever ? NULL :*/ &tv );
	return number;
}

void		NetTCPServer::AddData( 
				const InternetAddress& NetAddress, 
				const void* pBytes, 
				size_t nSize )
{
	StreamData* pData = FindData(NetAddress);
	if( !pData )
	{
		StreamData sd(NetAddress);
		m_StreamList.push_back( sd );
		pData = &m_StreamList.back();
	}
	std::vector<BYTE> & Arr = pData->Data;
	while( nSize )
	{

		if( pData->TotalValid )
		{
			int nDiff = pData->Data.size() + nSize -  pData->TotalBytes;
			if( nDiff == 0)
			{
				Sys_Debug( "<-- + %u bytes", (unsigned int)nSize );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize );	
				Sys_Debug( "<-- Save message %u bytes", (unsigned int)Arr.size() );
				OnReceivePacket( NetAddress, pData->Data ) ;
				pData->Data.clear();
				pData->TotalValid = false;
				break;

			}
			else if( nDiff < 0 )
			{
				Sys_Debug( "<-- + %d bytes\n", nSize );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize);
				break;
			}
			else if( nDiff > 0 )
			{
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize - nDiff );
				Sys_Debug( "<-- Save message %Id bytes\n", Arr.size() );
				OnReceivePacket( NetAddress, pData->Data ) ;
				Arr.clear();
				pBytes = (const BYTE*)pBytes + nSize - nDiff;
				nSize = nDiff;
				pData->TotalValid = false;
				continue;
			}
		}
		else
		{
			size_t ForTotal = nSize + pData->Data.size();
			if( ForTotal >= sizeof(int) )
			{
				size_t OldSize = Arr.size() ;
				Arr.insert( pData->Data.end(), 
					(const BYTE*)pBytes, (const BYTE*)pBytes + sizeof(int) - Arr.size() );
				pData->TotalBytes = *(int*)&Arr[0];
				Sys_Debug( "<-- New total %d bytes\n", pData->TotalBytes  );
				Arr.clear();
				nSize -= sizeof(int) - Arr.size();
				pBytes = (const BYTE*)pBytes + sizeof(int) - Arr.size();
				pData->TotalValid = true;
				continue;
			}
			else
			{
				Sys_Debug( "<-- + total %Id bytes\n", nSize  );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, (const BYTE*)pBytes + nSize);
				break;
			}
		}

	}
}

NetTCPServer::StreamData* NetTCPServer::FindData( const InternetAddress& NetAddress)
{
	StreamDataList_t::iterator it = m_StreamList.begin();
	for( ; it != m_StreamList.end(); ++it )
	{
		if( IsEqualAddresses( it->NetAddress, NetAddress) ) 
		{
			return &(*it);
		}
	}
	return NULL;
}

void	NetTCPServer::OnReceivePacket( 
				const InternetAddress& ClientAddr, 
				const std::vector<BYTE>& Data
				)
{

}