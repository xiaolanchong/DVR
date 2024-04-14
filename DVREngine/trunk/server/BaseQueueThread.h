//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����� � ������������ ������� � �������� ���������

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

//! \brief ����� � ������������ ������� � �������� ���������
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

	//! ���������� �����
	MessageImpl							m_Msg;
	//! ������� ���������
	MessageSyncQueue<ThreadMessage_t>	m_Queue;
	//! ����� ����������
	std::auto_ptr<ControlledThread>		m_Thread;
};

#endif // _BASE_QUEUE_THREAD_5351705001747334_