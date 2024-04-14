//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� ��� ����������������� ������� � ����������

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

//! \brief ������� ��� ������� � ����������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.04.2006
//! \version 1.0
//! \bug 
//! \todo 

template<typename T> class MessageQueue
{
	//! �������� ��������� ��������
	typename std::deque<  T >						m_MsgQueue;
	//! ������������ ������ �������
	const size_t									m_nMaxSize;
public:
	MessageQueue(size_t nMaxSize = 100) : m_nMaxSize(nMaxSize){};
	~MessageQueue() {};

	//! ��������� ��������� � �����
	void Push( T Msg)
	{
		if( m_nMaxSize < m_MsgQueue.size() )
		{
			m_MsgQueue.pop_front();
		}
		// comment if simulate no send
		m_MsgQueue.push_back( Msg );
	}
	//! ���������� ������� ���������
	void Pop( ) 
	{
		_ASSERTE( !m_MsgQueue.empty() );
		m_MsgQueue.pop_front();
	};

	//! ����������, ����� �� �������
	//! \return - �����/�� �����
	bool IsEmpty() const { return m_MsgQueue.empty();}

	//! ������� ������� ���������
	//! \param bPop true-���������� ��������� ����� �����������
	//! \return ���������
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

	//! �������� ��� �������
	void	Clear()
	{
		m_MsgQueue.clear();
	}

	//! ����������� ��� ��������� � ������ �������
	//! \param rhs �������-��������
	void	MoveTo(MessageQueue<T>& rhs)
	{
		if( &rhs != this )
		{
			rhs.m_MsgQueue = m_MsgQueue;
			m_MsgQueue.clear();
		}
	}
};

//! \brief ������� ��� ����������������� ������� � ����������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.06.2006
//! \version 2.0
//! \bug 
//! \todo 

template<typename T> class MessageSyncQueue : private MessageQueue<T>
{
	template <typename T> friend class	MessageQueueAutoEnter;
	//! ��� ������������� �������
	boost::mutex						m_MutexQueue;
};

//! \brief �������������� ������������� ������� � �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.06.2006
//! \version 1.0
//! \bug 
//! \todo 

template<typename T> class MessageQueueAutoEnter
{
	//! ��� ������������� �������
	boost::mutex::scoped_lock				m_Lock;
	//! �������, ��� ������� ���������
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

//! �-� ��������� �������
//! \param pThis �����, ������� ������������ ���������
//! \param Queue ������� ���������
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