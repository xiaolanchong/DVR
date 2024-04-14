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
#include "NetSocket.h"

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
};

//======================================================================================//
//                                  class NetTCPServer                                  //
//======================================================================================//

//! \brief Кроссплатформенный мультипотоковый (i hope) сервер сокетов
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetTCPServer: boost::noncopyable
{
	std::auto_ptr<boost::thread>	m_Thread;
	bool							m_bWorking;

	NetSocket							m_ServerSocket;

	void	ThreadFunc();
	void	ThreadFuncWrapped();

	void	Start( unsigned short PortNumber,  int  nMaxSocketConnection ); 

	bool IsWorking() const
	{
		return m_bWorking == true;
	}

	virtual void OnConnectClient( SOCKET RawClientSocket, const InternetAddress& ClientAddr ) = 0;
public:

	static const int c_DefMaxSocketConnection = (-1);

	NetTCPServer( 
			unsigned short nPort, 
			/*int nMaxClientIdleTimeInSeconds,*/ 
			int nMaxSocketConnections = c_DefMaxSocketConnection );
	virtual ~NetTCPServer();

	void	Start();
	void	Stop();
};



#endif // _NET_T_C_P_SERVER_1127341544144030_