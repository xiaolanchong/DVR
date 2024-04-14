//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс взаимодействия с сервером, запуск сервера, поставка событий от камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   28.02.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ServerHandler.h"

//#define LAUNCH_ALGO



//#define STUB
//#undef STUB
//======================================================================================//
//                                 class ServerHandler                                  //
//======================================================================================//

ServerHandler::ServerHandler( const DVREngine::ServerStartSettings& ss,
							 boost::shared_ptr<Elvees::IMessage> pClientMsg	,
							 bool bStartServer
							 )
try 
	:	
		m_pClient( 
#ifndef STUB
		DVREngine::CreateClientInterface(pClientMsg) 
#endif
		) 
{

	if( bStartServer ) 
	{
		m_pServer.reset( 
#ifndef STUB
			DVREngine::CreateServerInterface(ss) 
#endif
			);
	}
}
catch( DVREngine::IServer::ServerException& ex  )
{
	throw ServerHandlerException( ex.what() );
};

ServerHandler::~ServerHandler()
{
}

void	ServerHandler::GetCurrentFrames( Frames_t& Alarms) const
{
	if(m_pClient.get())
	{
		m_pClient->GetCurrentFrames( Alarms );
	}
}

int		ServerHandler::GetComputerState(const std::string& sName) const
{
	return 0;
}

int		ServerHandler::GetCameraState(int nID) const
{
	return 0;
}