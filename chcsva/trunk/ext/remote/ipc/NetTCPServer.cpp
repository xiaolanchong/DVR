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

//======================================================================================//
//                                  class NetTCPServer                                  //
//======================================================================================//

NetTCPServer::NetTCPServer(	unsigned short nPort, int nMaxSocketConnections	):
			m_bWorking(false),
			m_ServerSocket( )
{
	m_ServerSocket.Bind( nPort );
	m_ServerSocket.Listen( nMaxSocketConnections );

}


NetTCPServer::~NetTCPServer()
{

}

void	NetTCPServer::Start()
{
	m_bWorking = true;
	m_Thread.reset( new boost::thread(boost::bind( &NetTCPServer::ThreadFuncWrapped, this ) ) ); 
}

void	NetTCPServer::Stop()
{
	m_bWorking = false;
	m_Thread->join();
}

//////////////////////////////////////////////////////////////////////////

void	NetTCPServer::ThreadFuncWrapped()
{
	__try
	{
		ThreadFunc();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), "Network thread") )
	{

	}
}

void	NetTCPServer::ThreadFunc()
{
	while ( IsWorking() ) 
	{
		// sleeps until one of the net sockets has a message.  Lowers thread CPU usage immensely
		try
		{
			bool ready = m_ServerSocket.Select( true, 20 );
			if( ready )
			{
				std::pair<SOCKET, InternetAddress> p = m_ServerSocket.Accept();
				if( p.first != INVALID_SOCKET )
				{
					OnConnectClient( p.first, p.second );
				}
			}
		}
		catch( NetException&  )
		{
	//		bool bExit = OnSocketError( ex.what() );
	//		if( bExit )
			{
	//			return;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

