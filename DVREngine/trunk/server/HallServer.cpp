//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Hall thread & queue server

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "HallServer.h"
//======================================================================================//
//                                   class HallServer                                   //
//======================================================================================//

HallServer::HallServer( boost::shared_ptr<Elvees::IMessage> pMsg, IMessageDispatcher* pDsp, 
					    bool bLaunchAlgo) :
	m_Msg( pMsg ),
	m_Thread( pMsg, this ),
	IMessageStream( pDsp )
{
//	m_Thread.Start();
}

HallServer::~HallServer()
{
//	m_Thread.Stop();
}

void	HallServer::OnMessage( boost::shared_ptr<IThreadMessage> pMsg )
{
	m_Thread.OnMessage( pMsg );
}

void	HallServer::SendMessage( IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg )
{
	UNREFERENCED_PARAMETER(pSender);
	m_pDispatcher->SendMessage( this, pMsg );
}