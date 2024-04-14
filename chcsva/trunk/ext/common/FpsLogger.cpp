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

#include "chcs.h"
#include "FpsLogger.h"

//======================================================================================//
//                                   class FpsLogger                                    //
//======================================================================================//

FpsLogger::FpsLogger(unsigned int nPeriod):
	m_Period(nPeriod * 1000),
	m_FrameCounter(0)
{
	_ASSERTE(m_Period);
	m_PrevTime = timeGetTime();
}

void	FpsLogger::IncrementFrame()
{
	++m_FrameCounter;
}

boost::optional<double>	FpsLogger::GetFPS()
{
	boost::optional<double> fFPS;
	unsigned int Now = timeGetTime();
	if( (Now - m_PrevTime) >=  m_Period )
	{
		// time in ms
		fFPS = m_FrameCounter * 1000.0 / ( Now - m_PrevTime ); 
		m_PrevTime = Now;
		m_FrameCounter = 0;
	}
	return fFPS;
}