//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Cameras thread & queue server

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraServer.h"
//======================================================================================//
//                                  class CameraServer                                  //
//======================================================================================//

CameraServer::CameraServer(
						   DWORD dwVideoModeFlags,
						   boost::shared_ptr<Elvees::IMessage> pOwnMsg ,
						   boost::shared_ptr<Elvees::IMessage> pVideoMsg ,
						   IMessageDispatcher* pDsp
						   ) :
	m_Msg( pOwnMsg ),
	IMessageStream(pDsp),
	m_VideoFactory( dwVideoModeFlags, pVideoMsg) 
{
//	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[CameraServer]Started" );
} 

CameraServer::~CameraServer()
{
	m_CameraThreads.clear();
}

void CameraServer::Start(const std::vector<int>& CameraArr,const CameraSettings& CamSettings)
{
	_ASSERTE(CameraArr.size());

	for ( size_t i  =0; i < CameraArr.size(); ++i)
	{
		m_Msg.Print( Elvees::IMessage::mt_info, boost::format("[CameraServer]camid%d\n") % CameraArr[i] );
	}
	std::for_each( CameraArr.begin(), CameraArr.end(), boost::bind( &CameraServer::StartThread, this, _1, CamSettings ) );
}

void	CameraServer::StartThread( int nCameraID, const CameraSettings& CamSettings )
try
{
	boost::shared_ptr<IVideoProvider> p( m_VideoFactory.CreateVideoReader( nCameraID ) );
	m_CameraThreads.insert( std::make_pair( 
		nCameraID, 
		boost::shared_ptr<CameraThread>( new CameraThread( m_Msg.Get(), nCameraID, this, p, CamSettings ) ) ) );
}
catch ( /*Elvees::VideoReaderException*/std::runtime_error& ex ) 
{
	m_Msg.Print( Elvees::IMessage::mt_error, 
		boost::format("[CameraServer::Start]failed to start VideoReader for stream#%d : %s") % nCameraID % ex.what() );
};

void	CameraServer::OnMessage( boost::shared_ptr<IThreadMessage> pMsg )
{
	CameraThreads_t::iterator it = m_CameraThreads.begin();
	for ( ; it != m_CameraThreads.end() ; ++it )
	{
		it->second->OnMessage( pMsg );
	}
}

void	CameraServer::SendMessage( IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg )
{
	UNREFERENCED_PARAMETER(pSender);
	m_pDispatcher->SendMessage( this, pMsg );
}