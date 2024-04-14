//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Очередь для потокобезопасного доступа к сообщениям

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MESSAGE_QUEUE_2390374307955050_
#define _MESSAGE_QUEUE_2390374307955050_

class IThreadMessage;

//======================================================================================//
//                                  class MessageQueue                                  //
//======================================================================================//

//! \brief Очередь для доступа к сообщениям
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.04.2006
//! \version 1.0
//! \bug 
//! \todo 

template<typename T> class MessageQueue
{
	//! основной контейнер хранения
	typename std::deque<  T >						m_MsgQueue;
	//! максимальный размер очереди
	const size_t									m_nMaxSize;
public:
	MessageQueue(size_t nMaxSize = 100) : m_nMaxSize(nMaxSize){};
	~MessageQueue() {};

	//! поставить сообщение в конец
	void Push( T Msg)
	{
		if( m_nMaxSize < m_MsgQueue.size() )
		{
			m_MsgQueue.pop_front();
		}
		// comment if simulate no send
		m_MsgQueue.push_back( Msg );
	}
	//! вытолкнуть верхнее сообщение
	void Pop( ) 
	{
		_ASSERTE( !m_MsgQueue.empty() );
		m_MsgQueue.pop_front();
	};

	//! опредлеить, пуста ли очередь
	//! \return - пуста/не пуста
	bool IsEmpty() const { return m_MsgQueue.empty();}

	//! вернуть верхнее сообщение
	//! \param bPop true-вытолкнуть сообщение после возвращения
	//! \return сообщение
	T Front( bool bPop )  
	{ 
		_ASSERTE( !m_MsgQueue.empty() );
		T p = m_MsgQueue.front();
		if (bPop)
		{
			Pop();
		}
		return p;
	};

	//! очистить всю очередь
	void	Clear()
	{
		m_MsgQueue.clear();
	}

	//! переместить все сообщения в другую очередь
	//! \param rhs очередь-приемник
	void	MoveTo(MessageQueue<T>& rhs)
	{
		if( &rhs != this )
		{
			rhs.m_MsgQueue = m_MsgQueue;
			m_MsgQueue.clear();
		}
	}
};

//! \brief Очередь для потокобезопасного доступа к сообщениям
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.06.2006
//! \version 2.0
//! \bug 
//! \todo 

template<typename T> class MessageSyncQueue : private MessageQueue<T>
{
	template <typename T> friend class	MessageQueueAutoEnter;
	//! для синхронизации доступа
	boost::mutex						m_MutexQueue;
};

//! \brief Автоматическая синхронизация доступа к очереди
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.06.2006
//! \version 1.0
//! \bug 
//! \todo 

template<typename T> class MessageQueueAutoEnter
{
	//! для синхронизации доступа
	boost::mutex::scoped_lock				m_Lock;
	//! очередь, над которой оперируем
	typename MessageSyncQueue<T>&			m_Queue;
public:
	MessageQueueAutoEnter( MessageSyncQueue<T>& mq ):
	  m_Lock( mq.m_MutexQueue ),
	  m_Queue( mq )
	  {
	  }

	  void Push( T Msg)	{ m_Queue.Push( Msg );		}
	  void Pop( )						{ m_Queue.Pop();			}

	  bool IsEmpty() const				{ return m_Queue.IsEmpty();	}

	  T Front( bool bPop = true ) 		{ return m_Queue.Front(bPop);	}

	  void	Clear()						{ m_Queue.Clear();		}

	  void	MoveTo(MessageQueue<T>& rhs)
	  {
		  m_Queue.MoveTo(rhs);
	  }
};

//! ф-я обработки очереди
//! \param pThis класс, который обрабатывает сообщения
//! \param Queue очередь обработки
template<class ProcessedClass, typename MsgType> 
	void ProcessQueue(ProcessedClass * pThis, MessageSyncQueue<MsgType>&  Queue)
{
	MessageQueue<MsgType> ProcessedQueue;
	{
		MessageQueueAutoEnter<MsgType> mq( Queue );
		mq.MoveTo(ProcessedQueue);
	}

	while ( !ProcessedQueue.IsEmpty() )
	{
		MsgType m = ProcessedQueue.Front(true);
		m->Process( pThis );
	}
}

#endif // _MESSAGE_QUEUE_2390374307955050_