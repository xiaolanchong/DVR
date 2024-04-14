//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние сервера - соединение с БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.10.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ServerThreadDBConnection.h"
#include "ServerThreadGathering.h"
#include "../ServerImpl.h"

//======================================================================================//
//                            class ServerThreadDBConnection                            //
//======================================================================================//

ServerThreadDBConnection::ServerThreadDBConnection(ServerImpl* p):
	ServerThreadState(p),
	m_bFirstTime(true),
	m_Timer( 5000 )
{
}

ServerThreadDBConnection::~ServerThreadDBConnection()
{
}

std::auto_ptr<ServerThreadState>	ServerThreadDBConnection::PostProcess()
{
	bool bTick = m_Timer.Ticked();
	if(bTick || m_bFirstTime)
	{
		boost::format fmt("%s:%s");
		try
		{
			m_pImpl->LaunchThreads();
			return std::auto_ptr<ServerThreadState>( new ServerThreadGathering( m_pImpl ) ) ;
		}
		catch( ServerVideoFactory::ServerVideoException&  ex)
		{
			m_pImpl->DumpError( (fmt % "Video server error" % ex.what()).str() );
		}
		catch ( DBBridge::IDBServer::DBServerException& ex ) 
		{
			m_pImpl->DumpError( (fmt % "Database connection error" % ex.what()).str() );	
		}
		catch (std::exception& ex) 
		{
			m_pImpl->DumpError( (fmt % "Unknown error" % ex.what()).str() );
		}
	}
	m_bFirstTime = false;
	return std::auto_ptr<ServerThreadState>();
}