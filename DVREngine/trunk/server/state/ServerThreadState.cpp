//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Èםעונפויס סמסעמÿםטÿ סונגונא

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerThreadState.h"
#include "ServerThreadGathering.h"
#include "ServerThreadDBConnection.h"
#include "../ServerImpl.h"

#include "../message/MsgHallResponseAlarmData.h"

//======================================================================================//
//                               class ServerThreadState                                //
//======================================================================================//

ServerThreadState::ServerThreadState(ServerImpl* p) :
	m_pImpl( p ) 
{
}

ServerThreadState::~ServerThreadState()
{
}

std::auto_ptr<ServerThreadState> ServerThreadState::GetStartState( ServerImpl* p )
{
	return std::auto_ptr<ServerThreadState>( new ServerThreadDBConnection(p) );
}

std::auto_ptr<ServerThreadState>	ServerThreadState::Process()
{
	::ProcessQueue( this, m_pImpl->GetQueue() );
	return PostProcess();
}

void	ServerThreadState::Process( MsgCameraResponseData*		/*p*/ )
{
	_ASSERT(false);
}

void	ServerThreadState::Process( MsgCameraSetHallData*		/*p*/ )
{
	_ASSERT(false);
}


void	ServerThreadState::Process( MsgHallRequestAlarmData*		/*p */)
{
	_ASSERTE(false);
}

void	ServerThreadState::Process( MsgHallResponseAlarmData*		p )
{
	const CameraData&		OuputCamData = p->GetCameraData();
	const Frames_t&			FramesForRender		 = p->GetFramesForRender();
//	const Frames_t&			FramesForDatabase	 = p->GetFramesForDatabase();
	m_pImpl->ReceiveHallData( OuputCamData, FramesForRender );
}

void	ServerThreadState::Process( MsgHallSetCameraData*		/*p*/ )
{
	_ASSERT(false);
}
