//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: �������� ����� �� ������ ��������� ������� ����

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   07.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "TimeFromStartProvider.h"

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