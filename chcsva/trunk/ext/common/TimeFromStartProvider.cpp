//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получить время от старта программы порядка мсек

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   07.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "TimeFromStartProvider.h"

class Win_MilliSecTimer
{
	//! частота таймера в 1/мс
	LARGE_INTEGER m_MSPerformanceFrequency;
	//! время старта таймера
	LARGE_INTEGER m_ClockStart;
public:
	Win_MilliSecTimer();

	//! вернуть кол-во мс от старта таймера
	//! \return кол-во мс от старта таймера
	boost::uint64_t GetTime() const;
};

Win_MilliSecTimer::Win_MilliSecTimer()
{
	LARGE_INTEGER PerformanceFrequency;
	QueryPerformanceFrequency(&PerformanceFrequency);
	m_MSPerformanceFrequency.QuadPart = PerformanceFrequency.QuadPart / 1000;
	QueryPerformanceCounter(&m_ClockStart);
}

boost::uint64_t Win_MilliSecTimer::GetTime() const
{
	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter( &CurrentTime );

	return ( ( CurrentTime.QuadPart - m_ClockStart.QuadPart ) / m_MSPerformanceFrequency.QuadPart);
}

static Win_MilliSecTimer g_time;
static const boost::uint64_t g_StartTime = g_time.GetTime();

boost::uint64_t GetTimeFromStart()
{
	return g_time.GetTime() - g_StartTime;
}