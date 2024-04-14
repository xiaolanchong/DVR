//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Соединение с архивным сервером на стороне клиента

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_CLIENT_CONNECTION_1572502429448204_
#define _ARCHIVE_CLIENT_CONNECTION_1572502429448204_

#include "../NetProtocol.h"
#include "../ipc/NetUtil.h"
#include "../ipc/NetTCPConnection.h"
#include "../../archive/ArchiveDataExchange.h"

//======================================================================================//
//                            class ArchiveClientConnection                             //
//======================================================================================//

//! \brief Соединение с архивным сервером на стороне клиента
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveClientConnection:	public	NetTCPAutoRecoverConnection,
								public	IArchiveDataRequest,
								private IArchiveClientPacketProcessor
{
	IArchiveDataResponse*	m_pResponse;
	bool					m_bStreamFormatWasSent;
	long					m_StreamID;

	boost::mutex			m_CommandQueueSync;
	// command cache
	std::queue<std::string>	m_CommandQueue;

	bool SendCommandsInQueue();

	virtual void	OnConnectError( const char* stErrorDesc )
	{

	}
	virtual bool	OnSocketError( const char* stErrorDesc )
	{
		return false;
	}

	virtual void	OnReceivePacket( const std::vector<BYTE>& Data);

	virtual bool	Connect();

	bool			SendCommandWithoutLock(LPCSTR szCommandText);

	virtual void	OnDisconnectConnection()
	{
		// shutdown, not reconnect
		ForceShutdown();
	}
private:
	virtual void	OnImageData(
		long StreamID, 
		const BITMAPINFOHEADER* pbih, 
		const void* pImageData, size_t ImageDataSize,
		const void* pUserData, size_t UserDataSize,
		boost::uint64_t	FrameTime, boost::uint32_t RelativeFrameTime
		) ;

	virtual void	OnCloseDecompressor() ;

	virtual void ShutdownRequests()
	{
		NetTCPAutoRecoverConnection::Stop();
	}
public:
	ArchiveClientConnection(
		long StreamID,
		const InternetAddress& ServerAddress ,
		IArchiveDataResponse* pResponse);

	bool	SendCommand( LPCSTR szCommandText );
	virtual ~ArchiveClientConnection();
};

#endif // _ARCHIVE_CLIENT_CONNECTION_1572502429448204_