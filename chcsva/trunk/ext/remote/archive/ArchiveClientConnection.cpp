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

#include "chcs.h"
#include "ArchiveClientConnection.h"

//======================================================================================//
//                            class ArchiveClientConnection                             //
//======================================================================================//

ArchiveClientConnection::ArchiveClientConnection(
	long StreamID,
	const InternetAddress& ServerAddress, IArchiveDataResponse* pResponse):
	m_pResponse( pResponse ) ,
	NetTCPAutoRecoverConnection( ServerAddress ),
	m_bStreamFormatWasSent(false),
	m_StreamID( StreamID ) 
{

}

ArchiveClientConnection::~ArchiveClientConnection()
{

}

void	ArchiveClientConnection::OnReceivePacket( const std::vector<BYTE>& Data)
try
{
	if( Data.empty() )
	{
		return;
	}
	ArchivePacketValidator v;
	v.ValidateArchiveClientPacket( &Data[0], Data.size(), this );
}
catch( std::runtime_error&  )
{

}
void	ArchiveClientConnection::OnImageData(
							long StreamID, 
							const BITMAPINFOHEADER* pbih, 
							const void* pImageData, size_t ImageDataSize,
							const void* pUserData, size_t UserDataSize,
							boost::uint64_t	FrameTime, boost::uint32_t RelativeFrameTime
							)
{
	if( !m_bStreamFormatWasSent )
	{
		m_bStreamFormatWasSent = true;
		BITMAPINFO bi = { 0 };
		bi.bmiHeader = *pbih;
		m_pResponse->SendStreamFormat( pbih->biCompression, &bi );
	}
	m_pResponse->SendFrameData(
		pImageData, ImageDataSize,
		pUserData, UserDataSize,
		FrameTime, RelativeFrameTime
		);
}

void	ArchiveClientConnection::OnCloseDecompressor()
{
	m_bStreamFormatWasSent = false;
	m_pResponse->SendEndOfFormat();
}


bool	ArchiveClientConnection::SendCommandsInQueue()
{
	boost::mutex::scoped_lock _lock( m_CommandQueueSync  );
	while( !m_CommandQueue.empty() )
	{
		if( !SendCommandWithoutLock( m_CommandQueue.front().c_str() ) )
		{
			return false;
		}
		else
		{
			m_CommandQueue.pop();
		}
	}
	return true;
}

bool	ArchiveClientConnection::SendCommandWithoutLock( LPCSTR szCommandText )
try
{
	NPacker_ArchiveCommand packet( m_StreamID, std::string(szCommandText) );
	SendData( packet.GetData(), packet.GetDataSize() );
	return true;
}
catch ( std::exception&  ) 
{
	return false;
};


bool	ArchiveClientConnection::SendCommand( LPCSTR szCommandText )
{
	SendCommandsInQueue();
	boost::mutex::scoped_lock _lock( m_CommandQueueSync );
	if( !SendCommandWithoutLock(szCommandText) )
	{
		m_CommandQueue.push( szCommandText );
		return false;
	}
	return true;
}

bool	ArchiveClientConnection::Connect()
{
	bool res = NetTCPAutoRecoverConnection::Connect();
	if( res )
	{
		SendCommandsInQueue();
	}
	return res;
}