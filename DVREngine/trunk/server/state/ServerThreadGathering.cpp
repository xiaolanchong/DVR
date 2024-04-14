//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние сервера - сбор данных

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerThreadGathering.h"
#include "../ServerImpl.h"
#include "../message/MsgCameraResponseData.h"

//======================================================================================//
//                             class ServerThreadGathering                              //
//======================================================================================//

ServerThreadGathering::ServerThreadGathering(ServerImpl* p):
	m_Timer(/*1*/100),
	ServerThreadState(p)
//	m_bChangeState(false)
{
	m_pImpl->GetDebugOuput().Print( Elvees::IMessage::mt_debug_info, "==Gathering==" );
//	m_pImpl->SendRequestToAllCameras();
}

ServerThreadGathering::~ServerThreadGathering()
{
}

std::auto_ptr<ServerThreadState> ServerThreadGathering::PostProcess()
{
	bool bTick = m_Timer.Ticked();
	if(bTick)
	{
		m_pImpl->SendRequestToHall();
	}

	//! переход в другое состояние не предусмотрен
	return std::auto_ptr<ServerThreadState>();
}

void	ServerThreadGathering::Process( MsgCameraResponseData*	p )
{
	int						nCameraID = p->GetCameraID();
	const	ByteArray_t&	DataArr = p->GetCameraData();
#if 0
	m_pImpl->GetDebugOuput().Print
		(   Elvees::IMessage::mt_debug_info, 
			boost::format("[ServerThreadGathering]MsgCameraResponseData id=%d, size=%u") 
					 % nCameraID 
					 % DataArr.size() 
					 );
#endif
	m_pImpl->SendCameraDataToHall( nCameraID, DataArr );
}