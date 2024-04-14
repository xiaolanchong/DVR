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
#ifndef _NET_T_C_P_CONNECTION_4231808619049401_
#define _NET_T_C_P_CONNECTION_4231808619049401_

#include "InternetAddress.h"
#include "NetSocket.h"
#include "NetPacketAssembler.h"

inline void	OutputDebugInformation( const std::string& Info )
{
	OutputDebugStringA( Info.c_str() );
	OutputDebugStringA( "\n" );
}

inline void	OutputDebugInformation( const char* Info )
{
	OutputDebugStringA( Info );
	OutputDebugStringA( "\n" );
}
//======================================================================================//
//                                class NetTCPConnection                                //
//======================================================================================//

//! \brief Передача данных и соединение сервер-клиент
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   02.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetTCPConnection : boost::noncopyable, 
						 NetPacketAssembler
{
protected:
	NetSocket						m_Socket;
	
	void	ForceShutdown() 
	{
		m_bWorking = false;
	}
	bool	CanBeDestroyed() const
	{
		return !IsWorking();
	}

	NetTCPConnection();
	~NetTCPConnection();

	bool IsWorking() const
	{
		return m_bWorking == true;
	}
private:
	bool							m_bWorking;
	std::auto_ptr<boost::thread>	m_Thread;

	void	ThreadFuncWrapped();
	void	ThreadFunc();

	int		NET_Sleep();
	bool	NET_QueuePacket ( );

	//! установить соединение
	//! \return false - прекратить работу
	virtual bool	Connect() = 0;

	//! ошибка при работе с соектом
	//! \param stErrorDesc описание исключения
	//! \return true - закрыть соединение
	virtual bool	OnSocketError( const char* stErrorDesc ) = 0; 

	//! обработка закрытия соедиения
	void	OnCloseConnection()
	{
		NetPacketAssembler::ClearDataCache();
		OnPostCloseConnection();
	}

	//! обработка повторного установки соедиения
	void	OnDisconnectConnection()
	{
		NetPacketAssembler::ClearDataCache();
		OnPostDisconnectConnection();
	}

	virtual void	OnPostCloseConnection() = 0;
	virtual void	OnPostDisconnectConnection() = 0;
protected:

	void InitializeSocket();

public:

	void SendData( const void* pData, size_t DataSize);

	void	Start();
	void	Stop();
};

//! server listenning connection
class NetTCPServerConnection :  public NetTCPConnection
{
	virtual bool	Connect()
	{
		return m_Socket.IsValid();
	}

	virtual void		OnPostDisconnectConnection();
protected:
	virtual void		OnPostCloseConnection();
	InternetAddress		GetAddress() const;
public:
	NetTCPServerConnection(SOCKET RawSocket);

};

//! automatically recovers connection after one has been broken
class NetTCPAutoRecoverConnection :  public NetTCPConnection
{
	// error dumping
	virtual void	OnConnectError( const char* stErrorDesc )
	{
	}
	virtual void	OnPostDisconnectConnection();
	virtual void	OnPostCloseConnection();

	InternetAddress		m_ServerAddress;
protected:
	virtual bool	Connect();
	InternetAddress		GetServerAddress() const
	{
		return m_ServerAddress;
	}
public:
	NetTCPAutoRecoverConnection( const InternetAddress& ServerAddress );

};

#endif // _NET_T_C_P_CONNECTION_4231808619049401_