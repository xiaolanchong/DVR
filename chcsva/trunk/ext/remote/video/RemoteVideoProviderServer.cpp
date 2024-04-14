//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: По запросу отправляет сетевым клиентам изображения

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "RemoteVideoProviderServer.h"
#include "../NetProtocol.h"
#include "../ipc/NetUtil.h"

typedef boost::mutex::scoped_lock	AutoLock_t;

//======================================================================================//
//                           class RemoteVideoProviderServer                            //
//======================================================================================//

RemoteVideoProvider::RemoteVideoProvider():
	BasicNetServer<VideoServerConnection>( 
		c_StreamServerPort, 
		c_DefMaxSocketConnection, 
		_T("Video") ) 
{
}

RemoteVideoProvider::~RemoteVideoProvider()
{
}

void	RemoteVideoProvider::PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize )
{
	DeleteOldClients();

//	VideoServerConnection* pCon = 0;

	AutoLock_t _lock( m_MapSync );

	std::vector< Client_t >::iterator it = m_Clients.begin();
	for ( ; it != m_Clients.end(); ++it )
	{
		it->second->SendImage( pFrame );
	}
}

//////////////////////////////////////////////////////////////////////////

bool	VideoServerConnection::CanBeDestroyed() const
{
//	AutoLock_t _lock( m_RequiredStreamsSync );
	return	m_bErrorWhileSending		|| 
		//	m_RequiredStreams.empty()	|| 
			NetTCPServerConnection::CanBeDestroyed();
}

void VideoServerConnection::OnPostCloseConnection()
{
	InternetAddress addr = GetAddress();
	std::string s = InternetAddressToString( addr );
	Elvees::OutputF(Elvees::TInfo, TEXT("OnPostCloseConnection %hs"), s.c_str() );
}


void VideoServerConnection::SendImage( CHCS::IFrame* pFrame )
{
	_ASSERT(pFrame);
	long StreamID = pFrame->GetSourceID();

	try
	{
		BITMAPINFO bi;
		pFrame->GetFormat( reinterpret_cast<BITMAPINFO*>( &bi ) );

		const void*  pData		= pFrame->GetBytes();
		const size_t DataSize	= bi.bmiHeader.biSizeImage;

		std::vector<unsigned char> UserDataBuf;
		GetFrameUserData( pFrame, UserDataBuf );	

		const void* pUserData = UserDataBuf.size() ? &UserDataBuf[0] : 0;

		NPacker_VideoImage packet( 
			StreamID, &bi.bmiHeader, 
			pData, DataSize, 
			pUserData, UserDataBuf.size(),
			pFrame->GetTime(), pFrame->GetRelativeTime()
			);
		SendData( packet.GetData(), packet.GetDataSize() );
	}
	catch ( NetException& ex ) 
	{
		m_bErrorWhileSending = true;
		Elvees::OutputF(Elvees::TError, TEXT("SendImage error : %hs"), ex.what() );
	}
}

void	VideoServerConnection::OnReceivePacket( const std::vector<BYTE>& Data)
{
	// ignore, just send video
}