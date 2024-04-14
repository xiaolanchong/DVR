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
#ifndef _REMOTE_VIDEO_PROVIDER_SERVER_2191975002556043_
#define _REMOTE_VIDEO_PROVIDER_SERVER_2191975002556043_

#include "../BasicNetExchange.h"
#include "../ipc/NetTCPConnection.h"
#include "../../ImageGraph/FramePin.h"

class RemoteVideoProvider;

class VideoServerConnection : public NetTCPServerConnection
{
	bool					m_bSendFormat;
	bool					m_bErrorWhileSending;
	virtual void	OnReceivePacket( const std::vector<BYTE>& Data);
	virtual bool	OnSocketError( const char* ErrorDesc )
	{
	//	m_bErrorWhileSending = true;
		return true;
	}

	virtual	void	OnPostCloseConnection();
public:
	VideoServerConnection( SOCKET s ):
		NetTCPServerConnection( s ),
		m_bSendFormat(false),
		m_bErrorWhileSending(false)
	{
	}
	void SendImage( CHCS::IFrame* pFrame );

	bool	CanBeDestroyed() const;	
};

//======================================================================================//
//                           class RemoteVideoProviderServer                            //
//======================================================================================//

//! \brief По запросу отправляет сетевым клиентам изображения
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   01.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class RemoteVideoProvider :	BasicNetServer<VideoServerConnection>, 
							public IInputFramePin
{
	virtual void	PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize );

	virtual VideoServerConnection* GetServerConnection( SOCKET s)
	{
		return new VideoServerConnection( s );
	}
public:
	RemoteVideoProvider();
	virtual ~RemoteVideoProvider();

	void	Start()
	{
		BasicNetServer<VideoServerConnection>::Start();
	}

	void	Stop()
	{
		BasicNetServer<VideoServerConnection>::Stop();
	}
};

#endif // _REMOTE_VIDEO_PROVIDER_SERVER_2191975002556043_