//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� fps � ���������� ����� ����� ������ N ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.04.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "FpsLogger.h"

//======================================================================================//
//                                   class FpsLogger                                    //
//======================================================================================//

FpsLogger::FpsLogger(const size_t nPeriod):
	m_Period(nPeriod * 1000),
	m_FrameCounter(0)
{
	_ASSERTE(m_Period);
	m_PrevTime = m_Timer.GetTime();
}

void	FpsLogger::IncrementFrame()
{
	++m_FrameCounter;
}

boost::optional<double>	FpsLogger::GetFPS()
{
	boost::optional<double> fFPS;
	ulong Now = m_Timer.GetTime();
	if( (Now - m_PrevTime) >=  m_Period )
	{
		// time in ms
		fFPS = m_FrameCounter * 1000.0 / ( Now - m_PrevTime ); 
		m_PrevTime = Now;
		m_FrameCounter = 0;
	}
	return fFPS;
}