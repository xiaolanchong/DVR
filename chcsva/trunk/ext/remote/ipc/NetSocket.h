//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс операций с сокетами

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   02.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_SOCKET_2494368010803392_
#define _NET_SOCKET_2494368010803392_

class InternetAddress;

//======================================================================================//
//                                   class NetSocket                                    //
//======================================================================================//

//! \brief Класс операций с сокетами
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   02.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetSocket : boost::noncopyable
{
	SOCKET	m_RawSocket;
public:
	explicit NetSocket( SOCKET RawSocket );

	NetSocket( bool bNonBlocking = true, bool bReusable = true ); 

	void	Attach(  SOCKET RawSocket )
	{
		Close();
		m_RawSocket = RawSocket;
	}
	void	Create(bool bNonBlocking = true, bool bReusable = true);
	bool	IsValid() const
	{
		return m_RawSocket != INVALID_SOCKET;
	}
	void	Close();
	void	Bind( unsigned short PortNumber );
	void	Connect( const InternetAddress& Addr );
	void	Send( const void* pData, size_t DataSize);
	void	Listen( int MaxConnectionNumber );
	bool	Select( bool bForRead, unsigned int TimeoutInSec ); 
	std::pair<SOCKET, InternetAddress>	Accept();

	std::pair<bool, size_t>	Receive( void* pBuffer, size_t BufferSize );

	void	SetBufferSize( unsigned int BufferSize, bool bForReceive);

	InternetAddress	GetAddress() const;

	void	SetSockOption( int opt, int		value );
	void	SetSockOption( int opt, bool	value );

	bool	IsConnected();

	virtual ~NetSocket();
};

#endif // _NET_SOCKET_2494368010803392_