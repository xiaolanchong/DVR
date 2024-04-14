//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Crossplatform millisecond timer

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MILLI_SEC_TIMER_9373405719790600_
#define _MILLI_SEC_TIMER_9373405719790600_

//======================================================================================//
//                                 class MilliSecTimer                                  //
//======================================================================================//

//! \brief Crossplatform millisecond timer
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.03.2006
//! \version 1.0
//! \bug 
//! \todo 

#ifndef UNIX_RELEASE

class Win_MilliSecTimer
{
	//! ������� ������� � 1/��
	LARGE_INTEGER m_MSPerformanceFrequency;
	//! ����� ������ �������
	LARGE_INTEGER m_ClockStart;
public:
	Win_MilliSecTimer();

	//! ������� ���-�� �� �� ������ �������
	//! \return ���-�� �� �� ������ �������
	ulong GetTime() const;
};

typedef Win_MilliSecTimer MilliSecTimer;

#else

class Posix_MilliSecTimer
{
	struct timeval  tp;
	int      secbase ;
public:
	Posix_MilliSecTimer();

	ulong GetTime() const;
};

typedef Posix_MilliSecTimer MilliSecTimer;

#endif

class MilliSecTimeoutTimer
{
	//! ������ ������, ��
	ulong		m_nStartClock;
	//! ������ �������
	ulong		m_nPeriod;
	
	MilliSecTimer	m_Timer;
public:
	//! \param fPeriod ������ ������� � ���
	MilliSecTimeoutTimer( ulong fPeriod ) : 
		  m_nStartClock( ),
		  m_nPeriod(fPeriod) 
	  {
		  Reset();
	  };

	  //! �������� ������ �������
	  void	Reset()		
	  { 
		  m_nStartClock = m_Timer.GetTime() ;
	  }

	  //! ��� �������
	  //! \return ������ � ��������� �������
	  bool	Ticked()	
	  {
		  ulong CurTime = m_Timer.GetTime();
		  ulong nLast = CurTime - m_nStartClock ;
		  if( nLast >= m_nPeriod )
		  {
			  Reset();
			  return true;
		  }
		  else
			  return false;
	  }
};

#endif // _MILLI_SEC_TIMER_9373405719790600_