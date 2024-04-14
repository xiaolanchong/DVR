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
#ifndef _REMOTE_VIDEO_CONSUMER_1949443531867041_
#define _REMOTE_VIDEO_CONSUMER_1949443531867041_

#include "../../ImageGraph/FramePin.h"
#include "../ipc/NetTCPConnection.h"

class InternetAddress;

class VideoServerProxyCallback
{
public:
//	virtual void OnImageFormat( long StreamID, const BITMAPINFO* pbi ) = 0;
//	virtual void OnImageData(long StreamID, const void* pImageData, size_t DataSize ) = 0;
	virtual void OnImageData(
		long StreamID, 
		const BITMAPINFO* pbih,
		const void* pImageData, size_t ImageDataSize ,
		const void* pUserData, size_t UserDataSize,
		INT64 FrameTime, unsigned int RelativeFrameTime
		) = 0;
};

class VideoServerProxy : public NetTCPAutoRecoverConnection
{
	virtual void	OnConnectError( const char* stErrorDesc ) ;
	virtual bool	OnSocketError( const char* stErrorDesc ) ;

	virtual void	OnReceivePacket( const std::vector<BYTE>& Data);
	virtual void OnPostCloseConnection();
	VideoServerProxyCallback*	m_pCallback;
public:
	VideoServerProxy( VideoServerProxyCallback * pCallback,
					  const InternetAddress& ServerAddr );

	void	RequestStream();
	void	RefuseStream();
};

//======================================================================================//
//                              class RemoteVideoConsumer                               //
//======================================================================================//

//! \brief Запрашивает удаленные потоки видео
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   02.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class RemoteVideoConsumer : VideoServerProxyCallback
{
public:
	typedef std::pair< InternetAddress , 
		boost::shared_ptr<VideoServerProxy> >	Server_t;
private:
	boost::mutex	m_FilterMapSync;
	std::map<long, IInputFramePin*>								m_pDownStreamFilters;

	std::vector< Server_t >		m_ServerProxies;

	virtual void OnImageData(
		long StreamID, 
		const BITMAPINFO* pbih,
		const void* pImageData, size_t ImageDataSize ,
		const void* pUserData, size_t UserDataSize ,
		INT64 FrameTime, unsigned int RelativeFrameTime
		);
public:
	RemoteVideoConsumer();
	virtual ~RemoteVideoConsumer();

	void ConnectPin( IInputFramePin* pNextInputPin , long StreamID, 
					 const InternetAddress& ServerAddr );

//	void Start();
//	void Stop();
};

#endif // _REMOTE_VIDEO_CONSUMER_1949443531867041_