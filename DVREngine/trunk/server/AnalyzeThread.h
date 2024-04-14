//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Working thread actions - start, work, end

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ANALYZE_THREAD_2608517437012132_
#define _ANALYZE_THREAD_2608517437012132_

#include "MessageImpl.h"

//======================================================================================//
//                                 class ControlledThread                                  //
//======================================================================================//

//! \brief Working thread actions - start, work, end
//! don't inherit classes from one, instead include
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ControlledThread
{
	typedef boost::mutex::scoped_lock lock_t;
	//! for dynamic creation
	std::auto_ptr<boost::thread>	m_Thread;

	//! �������� �� ������ ������� ��������� �-�
	const int						m_SecWaiting;
	//! ������� �-� ������
	boost::function0<bool>			m_func;
	//! ���� ������
	bool							m_CondWorking;
	//! ��� �������������
	boost::mutex					m_MutexWorking;

	//! ��������� �-�
	void	ThreadProc()
	{
		for (;;)
		{
			{
				lock_t lock( m_MutexWorking );
				if(!m_CondWorking ) break;
			}
			if( m_func()) break;
		}
	}
public:
	
	//! \param func ������� �-�
	//! \param SecWaiting �������� ����� ������� func, � ���
	ControlledThread( const boost::function0<bool>& func, int SecWaiting ):
		m_func(func),
		m_SecWaiting(SecWaiting)
	{
		boost::function0<void> b = boost::bind( &ControlledThread::ThreadProc, this ) ;
		std::auto_ptr< boost::thread > p( new boost::thread( b ) );
		m_Thread =  p ;	
	}

	~ControlledThread()
	{
		{
			lock_t lock( m_MutexWorking );
			m_CondWorking = false;
		}
		m_Thread->join();
	}
};

#endif // _ANALYZE_THREAD_2608517437012132_