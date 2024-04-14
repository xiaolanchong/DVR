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
#ifndef _SERVER_THREAD_GATHERING_8060347484245166_
#define _SERVER_THREAD_GATHERING_8060347484245166_

#include "ServerThreadState.h"
#include "../../Common/Timer.h"
#include "../MilliSecTimer.h"
#include "../CameraData.h"

//======================================================================================//
//                             class ServerThreadGathering                              //
//======================================================================================//

//! \brief Состояние сервера - сбор, отправка данных и сообщений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerThreadGathering : public ServerThreadState
{
	//! изменить состояние , не используется
//	bool				m_bChangeState;
	//! таймер для таймаута сбора данных
	MilliSecTimeoutTimer				m_Timer;
	//! здесь храним данные для отправки
//	CameraData			m_CameraData;

	virtual std::auto_ptr<ServerThreadState> PostProcess();
public:
	ServerThreadGathering(ServerImpl* p);
	virtual ~ServerThreadGathering();
private:
	//! переопределенная обработка - получение данных от камер
	void Process( MsgCameraResponseData*		/*p*/ );

	//! определить наличие данных от всех камера m_CameraData
	//! \return true - данные по всем камерам получены
	bool	IsAllData() const;
};

#endif // _SERVER_THREAD_GATHERING_8060347484245166_