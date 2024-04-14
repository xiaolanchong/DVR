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
#ifndef _NET_T_C_P_SERVER_1127341544144030_
#define _NET_T_C_P_SERVER_1127341544144030_

#include "InternetAddress.h"

//======================================================================================//
//                                  class NetTCPServer                                  //
//======================================================================================//

//! \brief Кроссплатформенный мультипотоковый (i hope) сервер сокетов
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetTCPServer
{
	struct SockInit
	{
		SockInit()
		{
			WSADATA wsData;
	
			WORD wVersionRequested = MAKEWORD(2, 0);
			int nResult = WSAStartup(wVersionRequested, &wsData);	
		}
	
		~SockInit(  )
		{
			WSACleanup();
		}
	} m_SockInit;


	std::auto_ptr<boost::thread>	m_Thread;
	bool							m_bWorking;

	SOCKET							m_ServerSocket;

	void ThreadFunc();

	void	Start( unsigned short PortNumber,  int  nMaxSocketConnection ); 

	SOCKET		GetSocket() const 
	{
		return m_ServerSocket;
	}

	void		SetSocket(SOCKET s)  
	{
		m_ServerSocket = s;
	}

	void NET_ThreadLock();
	void NET_ThreadUnlock();
	int NET_Sleep();
	bool	NET_QueuePacket ( SOCKET sock, const InternetAddress& From);

	bool InWorking() const
	{
		return m_bWorking == true;
	}

	struct Marker
	{
		int nID;
		int Length;
	};

	struct StreamData
	{
		InternetAddress		NetAddress;
		int					TotalBytes;
		bool				TotalValid;
		std::vector<BYTE>	Data;

		StreamData(const InternetAddress& addr):
			NetAddress(addr),
			TotalBytes(0),
			TotalValid(false)
			{
			}
	};

	typedef std::list<StreamData>	StreamDataList_t;
	StreamDataList_t				m_StreamList;

	StreamData* FindData( const InternetAddress& NetAddress);
	void		AddData(	const InternetAddress& NetAddress, 
							const void* pBytes, 
							size_t nSize );

	void	CheckIdleSockets();
	virtual void	OnReceivePacket( const InternetAddress& ClientAddr, const std::vector<BYTE>& Data); 

	typedef std::vector< boost::tuple< InternetAddress, SOCKET, INT64 > >	SocketArr_t;
	SocketArr_t			m_Clients;
	enum
	{
		InvalidClient = -1
	};
	int		m_nMaxClientIdleTimeInSeconds;

public:

	static const int c_DefMaxSocketConnection = (-1);

	NetTCPServer( 
			unsigned short nPort, 
			int nMaxClientIdleTimeInSeconds, 
			int nMaxSocketConnections = c_DefMaxSocketConnection );
	virtual ~NetTCPServer();

	void	CloseClient( const InternetAddress& NetAddr );
};

#endif // _NET_T_C_P_SERVER_1127341544144030_