//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных и соединение сервер-клиент

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   02.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "NetType.h"
#include "NetUtil.h"
#include "NetTCPConnection.h"

//======================================================================================//
//                                class NetTCPConnection                                //
//======================================================================================//

NetTCPConnection::NetTCPConnection():
	m_bWorking(false)
{

}

NetTCPConnection::~NetTCPConnection()
{

}

void	NetTCPConnection::Start()
{
	
	m_bWorking = true;
	m_Thread = std::auto_ptr<boost::thread>( new boost::thread( 
		boost::bind( &NetTCPConnection::ThreadFuncWrapped, this )
		) );
}

void	NetTCPConnection::Stop()
{
	ForceShutdown();
	if( m_Thread.get() )
	{
		m_Thread->join();
		m_Thread.reset();
	}
}

void	NetTCPConnection::ThreadFuncWrapped()
{
	__try
	{
		ThreadFunc();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), "Network thread") )
	{
	
	}
}

void	NetTCPConnection::ThreadFunc()
{
	if( !Connect() )
	{
		OnCloseConnection();
		return;
	}
	while ( IsWorking() )
	{
		try
		{
			if( ! m_Socket.IsConnected() )
			{
				OnDisconnectConnection();
			}

			bool ready = m_Socket.Select( true, 50 );
			if( ready )
			{
				bool res = NET_QueuePacket(  );
				if(!res)
				{
					//disconnect have occuried
					OnDisconnectConnection();
				}
			}
		}
		catch( NetException& ex )
		{
			if( OnSocketError( ex.what() ) )
			{
				break;
			}
		}
	}
	OnCloseConnection();
}

bool	NetTCPConnection::NET_QueuePacket ( )
{
	const size_t NET_MAX_MESSAGE = 65000;
	unsigned char	buf[ NET_MAX_MESSAGE ];

	std::pair<bool, size_t> p = m_Socket.Receive( buf, NET_MAX_MESSAGE );
	if(p.first)
	{
		ReceiveData( buf, p.second );
	}
	return p.first;
}

void NetTCPConnection::SendData( const void* pData, size_t DataSize)
{
	class SendWithSocket
	{
		NetSocket& m_Socket;
	public:
		SendWithSocket( NetSocket& sock ) : m_Socket(sock){}
		void operator() (const void* pData, size_t DataSize)
		{
			m_Socket.Send( pData, DataSize );
		}
	};

	bool res = m_Socket.Select(false, 20);
	NetPacketAssembler::SendData( SendWithSocket(m_Socket), pData, DataSize) ;
}

void NetTCPConnection::InitializeSocket()
{
	// near 64k buffer value
	m_Socket.SetBufferSize( 60000, true );
	m_Socket.SetBufferSize( 60000, false );
	m_Socket.SetSockOption( SO_DONTLINGER, true );
}

//////////////////////////////////////////////////////////////////////////

NetTCPServerConnection::NetTCPServerConnection( SOCKET RawSocket )
{
	m_Socket.Attach( RawSocket );
	InitializeSocket();
}


void	NetTCPServerConnection::OnPostCloseConnection()
{
	m_Socket.Close();
}

InternetAddress		NetTCPServerConnection::GetAddress() const
{
	return m_Socket.GetAddress();
}

void	NetTCPServerConnection::OnPostDisconnectConnection()
{
	// for server close whole conection
	ForceShutdown();
}

//////////////////////////////////////////////////////////////////////////


NetTCPAutoRecoverConnection::NetTCPAutoRecoverConnection 
	( const InternetAddress& ServerAddress ):
		m_ServerAddress(ServerAddress)	
{
}

bool	NetTCPAutoRecoverConnection::Connect()
try
{
	InitializeSocket();
	m_Socket.Connect( m_ServerAddress );
	return true;
}
catch ( NetException& ex ) 
{
	OnConnectError( ex.what() );
	return false;
};

void	NetTCPAutoRecoverConnection::OnPostDisconnectConnection()
try
{
	Sleep(100);
	m_Socket.Close();
	m_Socket.Create();
	Connect();
}
catch ( std::exception& ) 
{
};

void		NetTCPAutoRecoverConnection::OnPostCloseConnection()
{
	// empty
}