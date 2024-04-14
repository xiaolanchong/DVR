//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Crossplatform millisecond timer

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "MilliSecTimer.h"

//======================================================================================//
//                                 class MilliSecTimer                                  //
//======================================================================================//

#ifndef UNIX_RELEASE

Win_MilliSecTimer::Win_MilliSecTimer()
{
	LARGE_INTEGER PerformanceFrequency;
	QueryPerformanceFrequency(&PerformanceFrequency);
	m_MSPerformanceFrequency.QuadPart = PerformanceFrequency.QuadPart / 1000;
	QueryPerformanceCounter(&m_ClockStart);
}

ulong Win_MilliSecTimer::GetTime() const
{
	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter( &CurrentTime );

	return ( ( CurrentTime.QuadPart - m_ClockStart.QuadPart ) / m_MSPerformanceFrequency.QuadPart);
}

#else

Posix_MilliSecTimer():
	secbase(0)
{
}

Posix_MilliSecTimer::GetTime()
{
	gettimeofday( &tp, NULL );

	if ( !secbase )
	{
		secbase = tp.tv_sec;
		return ( tp.tv_usec / 1000000.0 );
	}

	return (unsigned long)(( tp.tv_sec - secbase ) + tp.tv_usec / 1000000.0 );
}

#endif