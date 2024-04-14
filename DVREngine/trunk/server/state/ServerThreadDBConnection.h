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
#ifndef _SERVER_THREAD_D_B_CONNECTION_9584354017847496_
#define _SERVER_THREAD_D_B_CONNECTION_9584354017847496_

#include "ServerThreadState.h"
#include "../MilliSecTimer.h"

//======================================================================================//
//                            class ServerThreadDBConnection                            //
//======================================================================================//

//! \brief Состояние сервера - соединение с БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   11.10.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerThreadDBConnection : public ServerThreadState
{
	MilliSecTimeoutTimer				m_Timer;
	bool								m_bFirstTime;
public:
	ServerThreadDBConnection(ServerImpl* p);
	virtual ~ServerThreadDBConnection();

private:
	std::auto_ptr<ServerThreadState>	PostProcess();
};

#endif // _SERVER_THREAD_D_B_CONNECTION_9584354017847496_