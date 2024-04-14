//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Базовый код соединения через сеть

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   04.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _BASIC_NET_EXCHANGE_7197486400470304_
#define _BASIC_NET_EXCHANGE_7197486400470304_

#include "ipc/NetTCPServer.h"
#include "ipc/NetTCPConnection.h"

//////////////////////////////////////////////////////////////////////////
//	server side
//////////////////////////////////////////////////////////////////////////

template<typename ClientType> class BasicNetServer : NetTCPServer
{
	std::tstring m_sServerDesc;
protected:
	typedef  boost::mutex::scoped_lock AutoLock_t;
private:
	virtual void	OnConnectClient( 
			SOCKET RawClientSocket, 
			const InternetAddress& ClientAddr  )
	try
	{
		AutoLock_t _lock( m_MapSync );
		boost::shared_ptr<ClientType> p( GetServerConnection(RawClientSocket) );
		m_Clients.push_back( std::make_pair( ClientAddr, p ) );
		p->Start();
		std::string s = InternetAddressToString( ClientAddr );
		Elvees::OutputF(Elvees::TInfo, TEXT("[%s]Connected from %hs"), m_sServerDesc.c_str(), s.c_str() );
	}
	catch ( NetException & ex ) 
	{
		Elvees::OutputF(Elvees::TError, TEXT("[%s]OnConnectClient error - %hs"), m_sServerDesc.c_str(), ex.what() );
	};
protected:
	boost::mutex									m_MapSync;
	typedef std::pair<InternetAddress, boost::shared_ptr<ClientType> > Client_t;
	std::vector< Client_t >	m_Clients;

	void	DeleteOldClients()
	{
		AutoLock_t _lock( m_MapSync );

		std::vector<Client_t>::iterator it = m_Clients.begin();
		for ( ; it != m_Clients.end();  )
		{
			if( it->second->CanBeDestroyed() )
			{
				it->second->Stop();
				std::string s = InternetAddressToString( it->first );
				Elvees::OutputF(Elvees::TInfo, TEXT("[%s]Stop unused connection from %hs"), m_sServerDesc.c_str(), s.c_str() );
				it = m_Clients.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	virtual ClientType*	GetServerConnection(SOCKET s) = 0;
/*	{
		return new ClientType();
	}*/

	static const int c_DefMaxSocketConnection = NetTCPServer::c_DefMaxSocketConnection;
public:
	BasicNetServer(	unsigned short PortNumber,  
					int  nMaxSocketConnection, 
					const std::tstring& sServerDesc): 
		NetTCPServer(  PortNumber,   nMaxSocketConnection ),
		m_sServerDesc(sServerDesc) 
	{
	}
	virtual ~BasicNetServer()
	{
	}

	void	Start()
	{
		NetTCPServer::Start();
	}

	void	Stop()
	{
		NetTCPServer::Stop();
		AutoLock_t _lock( m_MapSync );
		for ( size_t i = 0; i < m_Clients.size(); ++i )
		{
			m_Clients[i].second->Stop();
		}
		m_Clients.clear();
	}
};


#endif // _BASIC_NET_EXCHANGE_7197486400470304_