//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс состояния сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_THREAD_STATE_6131551782488200_
#define _SERVER_THREAD_STATE_6131551782488200_

#include "../message/IMessageProcessor.h"

class ServerImpl;

//======================================================================================//
//                               class ServerThreadState                                //
//======================================================================================//

//! \brief Интерфейс состояния сервера, основные действия по обработке сообщений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerThreadState : public  IMessageProcessor
{
protected:
	//! указатель на основной интерфейс
	ServerImpl*		m_pImpl;

public:
	ServerThreadState(ServerImpl* p);
	virtual ~ServerThreadState();

	//! обработка, действия состояния
	//! \return NULL - состояние сохраняется, иначе новое состояние
	std::auto_ptr<ServerThreadState> Process();

	//! получить стартовое состояние
	//! \param p указатель на сервер класса
	static std::auto_ptr<ServerThreadState> GetStartState(ServerImpl* p);

private:
	//! ф-я для потомков - постобработка состояния, Template шаблон
	//! \return NULL - состояние сохраняется, иначе новое состояние
	virtual std::auto_ptr<ServerThreadState>	PostProcess() = 0;

	virtual void	Process( MsgCameraResponseData*		p );
	virtual void	Process( MsgCameraSetHallData*		p );
	virtual void	Process( MsgHallRequestAlarmData*		p );
	virtual void	Process( MsgHallResponseAlarmData*		p );
	virtual void	Process( MsgHallSetCameraData*		p );
};

#endif // _SERVER_THREAD_STATE_6131551782488200_