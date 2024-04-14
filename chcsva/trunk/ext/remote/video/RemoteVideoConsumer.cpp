//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Запрашивает удаленные потоки видео

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   02.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "RemoteVideoConsumer.h"
#include "../NetProtocol.h"
#include "../../imagegraph/FrameWrapper.h"

VideoServerProxy::VideoServerProxy( 
				VideoServerProxyCallback * pCallback,
				const InternetAddress& ServerAddr ):
	NetTCPAutoRecoverConnection( ServerAddr ),
	m_pCallback( pCallback ) 
{
	
}

void	VideoServerProxy::OnConnectError( const char* stErrorDesc ) 
{
	OutputDebugInformation( stErrorDesc );
	Elvees::OutputF(Elvees::TError, 
		TEXT("OnConnectError : %hs"), stErrorDesc);
}

bool	VideoServerProxy::OnSocketError( const char* stErrorDesc ) 
{
	OutputDebugInformation( stErrorDesc );
	Elvees::OutputF(Elvees::TError, 
		TEXT("OnSocketError : %hs"), stErrorDesc);
	return false;
}

void VideoServerProxy::OnPostCloseConnection()
{
	InternetAddress addr = GetServerAddress();
	std::string s = InternetAddressToString( addr );
	Elvees::OutputF(Elvees::TInfo, TEXT("Close connection with %hs"), s.c_str() );
}

bool	ValidatePacket(  const NP_VideoImage& pPacket, size_t PacketSize );

void	VideoServerProxy::OnReceivePacket( const std::vector<BYTE>& Data)
{
	if( sizeof( NP_VideoImage ) > Data.size())
	{
		Elvees::Output(Elvees::TError, 
			TEXT("OnReceivePacket : too small packet"));
		return;
	}

	const NP_VideoImage& pPacket = reinterpret_cast<const NP_VideoImage&>( Data[0] );

	if( !ValidatePacket( pPacket, Data.size() ) )
	{
		return;
	}

	const BYTE* pHeader		= &Data[0] + sizeof(NP_VideoImage);
	const BYTE* pImage		= pHeader + pPacket.m_ImageDataOffset;
	const BYTE* pUserData	= pHeader + pPacket.m_UserDataOffset;
	m_pCallback->OnImageData( 
		pPacket.m_StreamID, 
		reinterpret_cast<const BITMAPINFO*>( pHeader ),
		pImage,		pPacket.m_UserDataOffset - pPacket.m_ImageDataOffset,
		pUserData,	Data.size() - pPacket.m_UserDataOffset - sizeof( NP_VideoImage ),
		pPacket.m_FrameTime, pPacket.m_RelativeFrameTime
		);
}

bool	ValidatePacket(  const NP_VideoImage& pPacket, size_t PacketSize )
{
	if( pPacket.m_ImageDataOffset > PacketSize )
	{
		Elvees::OutputF(Elvees::TError, 
			TEXT("OnReceivePacket : invalid ImageDataOffset %u > %u"), 
			static_cast<unsigned>( pPacket.m_ImageDataOffset ),
			static_cast<unsigned>( PacketSize )
			);
		return false;
	}

	if( pPacket.m_UserDataOffset > PacketSize )
	{
		Elvees::OutputF(Elvees::TError, 
			TEXT("OnReceivePacket : invalid UserDataOffset %u > %u"), 
			static_cast<unsigned>( pPacket.m_UserDataOffset ),
			static_cast<unsigned>( PacketSize )
			);
		return false;
	}

	if( pPacket.m_ImageDataOffset > pPacket.m_UserDataOffset )
	{
		Elvees::OutputF(Elvees::TError, 
			TEXT("OnReceivePacket : ImageDataOffset > UserDataOffset, %u > %u"), 
			static_cast<unsigned>( pPacket.m_ImageDataOffset ),
			static_cast<unsigned>( pPacket.m_UserDataOffset )
			);
		return false;
	}
	return true;
}

//======================================================================================//
//                              class RemoteVideoConsumer                               //
//======================================================================================//

RemoteVideoConsumer::RemoteVideoConsumer()
{
}

RemoteVideoConsumer::~RemoteVideoConsumer()
{
	for ( size_t i = 0; i < m_ServerProxies.size(); ++i )
	{
		m_ServerProxies[i].second->Stop();
	}
}

class FindProxy
{
	InternetAddress m_SearchedAddr;
public:
	bool operator () ( const RemoteVideoConsumer::Server_t& srv ) const
	{
		//! можно также сравнить только по ip адресу и порту
		return m_SearchedAddr == srv.first;
	}
	FindProxy( InternetAddress SearchedAddr ): 
		m_SearchedAddr( SearchedAddr )
		{
		}
};

void RemoteVideoConsumer::ConnectPin( 
					IInputFramePin* pNextInputPin , 
					long StreamID, 
					const InternetAddress& ServerAddr )
{
	//! коннект происходит только в начале и не повлияет на производит-ть
	boost::mutex::scoped_lock _lock( m_FilterMapSync );
	m_pDownStreamFilters[ StreamID ] = pNextInputPin;

	if( std::find_if( m_ServerProxies.begin(), m_ServerProxies.end(), FindProxy( ServerAddr )) ==
		m_ServerProxies.end() 
		)
	{
		boost::shared_ptr<VideoServerProxy> p( new VideoServerProxy(this, ServerAddr) );
		m_ServerProxies.push_back( std::make_pair( ServerAddr, p ) );
		p->Start();
	}
}

void RemoteVideoConsumer::OnImageData(
						 long StreamID, 
						 const BITMAPINFO* pbih,
						 const void* pImageData, size_t ImageDataSize ,
						 const void* pUserData, size_t UserDataSize ,
						 INT64 FrameTime, unsigned int RelativeFrameTime
						 )
{
//	Elvees::OutputF(Elvees::TInfo, 
//		TEXT("OnImageData : stream=%ld, size=u"), StreamID, DataSize);
	IInputFramePin* pInputPin = 0;
	{
		//! блокирование только для поиска
		boost::mutex::scoped_lock _lock( m_FilterMapSync );

		std::map<long, IInputFramePin*>::iterator it = m_pDownStreamFilters.find( StreamID );
		if ( it != m_pDownStreamFilters.end() )
		{
			pInputPin = it->second;
		}
	}
	if( pInputPin )
	{
		FrameFullStackWrapper wrp( 
				StreamID, pbih, 
				pImageData, ImageDataSize, 
				pUserData, UserDataSize, 
				FrameTime, RelativeFrameTime );
		
		pInputPin->PushImage( &wrp, false, ImageDataSize );
	}
}
