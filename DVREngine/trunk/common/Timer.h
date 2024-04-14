//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Timer classes

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   19.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _TIMER_2349643854834625_
#define _TIMER_2349643854834625_
//======================================================================================//
//                                     class Timer                                      //
//======================================================================================//

//! \brief класс секундного кроссплатформенного таймера отсчитывает промежутки времени
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class Timer
{
	//! начало работы
	std::time_t		m_nStartClock;
	//! период таймера
	double		m_fPeriod;
public:
	//! \param fPeriod период таймера в сек
	Timer( double fPeriod ) : 
		m_nStartClock(  ),
		m_fPeriod(fPeriod) 
		{
			Reset();
		};
	~Timer() {};

	//! сбросить начало отсчета
	void	Reset()		
	{ 
		std::time( &m_nStartClock ) ;
	}

	//! тик таймера
	//! \return сигнал о истечении периода
	bool	Ticked()	
	{
		std::time_t CurTime;
		std::time( &CurTime) ; 
		double fPeriod = std::difftime( CurTime , m_nStartClock );
		if( fPeriod >= m_fPeriod )
		{
			Reset();
			return true;
		}
		else
			return false;
	}
};

#endif // _TIMER_2349643854834625_