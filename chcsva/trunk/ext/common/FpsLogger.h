//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Выводит fps в отладочный поток через каждые N секунд

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FPS_LOGGER_6271540768279430_
#define _FPS_LOGGER_6271540768279430_

//#include "MilliSecTimer.h"

//======================================================================================//
//                                   class FpsLogger                                    //
//======================================================================================//

//! \brief подсчитывает fps через каждые N секунд
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class FpsLogger
{
	//! таймер для периодов
//	MilliSecTimer		m_Timer;
	//! предыдущее время отсчета
	unsigned int				m_PrevTime;
	//! период подсчета
	unsigned int				m_Period;
	//! кол-во кадров за подсчет
	unsigned int				m_FrameCounter;
public:
	//! \param nPeriod сек, период подсчета
	FpsLogger( unsigned int nPeriod = 10 );
	//! увеличить кадр
	//! \return fps, если значение установлено
	boost::optional<double>	GetFPS();

	void	IncrementFrame();
};

#endif // _FPS_LOGGER_6271540768279430_