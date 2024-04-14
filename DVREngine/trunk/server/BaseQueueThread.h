//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Поток с интерефейсом отладки и очередью сообщений

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   25.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _BASE_QUEUE_THREAD_5351705001747334_
#define _BASE_QUEUE_THREAD_5351705001747334_

#include "AnalyzeThread.h"
#include "message/MessageQueue.h"
#include "message/IThreadMessage.h"
#include "MessageImpl.h"

//======================================================================================//
//                                class BaseQueueThread                                 //
//======================================================================================//

//! \brief Поток с интерефейсом отладки и очередью сообщений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class BaseQueueThread
{
protected:
	BaseQueueThread( boost::shared_ptr<Elvees::IMessage> pMsg );
	virtual ~BaseQueueThread();

	//! отладочный поток
	MessageImpl							m_Msg;
	//! очередь сообщений
	MessageSyncQueue<ThreadMessage_t>	m_Queue;
	//! поток выполнения
	std::auto_ptr<ControlledThread>		m_Thread;
};

#endif // _BASE_QUEUE_THREAD_5351705001747334_