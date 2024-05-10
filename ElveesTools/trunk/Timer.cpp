// Timer.cpp: implementation of the CTimer & CWaitableTimer classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"
#include "Exceptions.h"

// #define USING_RDTSC

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

// Using RDTSC (Read Time Stamp Counter, Intel Pentium and higher)
inline unsigned __int64 GetCycleCount(void)
{
	_asm _emit 0x0F
	_asm _emit 0x31
}

//////////////////////////////////////////////////////////////////////
// CTimer
//////////////////////////////////////////////////////////////////////

CTimer::Timer CTimer::m_TimerToUse = CTimer::ttuUnknown;

int CTimer::m_OverheadTicks  = 0;
int CTimer::m_PerfFreqAdjust = 0;

LARGE_INTEGER CTimer::m_PerfFreq;

//////////////////////////////////////////////////////////////////////

CTimer::CTimer()
{
}

bool CTimer::GetCounter(LARGE_INTEGER* lpCounter)
{
#ifdef USING_RDTSC
	return lpCounter ? lpCounter->QuadPart = GetCycleCount(), true : false;
#else
	return QueryPerformanceCounter(lpCounter) ? true : false;
#endif
}

bool CTimer::GetFrequency(LARGE_INTEGER* lpFrequency)
{
#ifdef USING_RDTSC
	static unsigned __int64 Frequency = 0;

	if(Frequency == 0)
	{
		unsigned __int64 startCycle = GetCycleCount();
		Sleep(1000L);
		Frequency = GetCycleCount() - startCycle;
	}

	return lpFrequency? lpFrequency->QuadPart = Frequency, true : false;
#else
	// The frequency cannot change while the system is running
	return QueryPerformanceFrequency(lpFrequency) ? true : false;
#endif
}

void CTimer::Start()
{
	if(m_TimerToUse == ttuUnknown )
		DetermineTimer();

	if(m_TimerToUse == ttuHiRes)
	{
		GetCounter(&m_tStartTime);
	}
	else
	{
		m_tStartTime.HighPart = 0L;
		m_tStartTime.LowPart = clock();
	}
}

unsigned CTimer::Stop()
{
	if(m_TimerToUse == ttuUnknown )
		return 0L;

	unsigned uTime; // Elapsed time

	if(m_TimerToUse == ttuHiRes)
	{
		LARGE_INTEGER tStartTime, tStopTime, Freq;
		int nOverhead, nReduceMag;
				
		GetCounter(&tStopTime);

		tStartTime = m_tStartTime;
		Freq = m_PerfFreq;

		nOverhead  = m_OverheadTicks;
		nReduceMag = 0;

		// Results are 64 bits but we only do 32
		unsigned High32 = tStopTime.HighPart - tStartTime.HighPart;

		while(High32)
		{
			High32 >>= 1;
			nReduceMag++;
		}

		if(m_PerfFreqAdjust || nReduceMag)
		{
			if(m_PerfFreqAdjust > nReduceMag)
				nReduceMag = m_PerfFreqAdjust;

			tStartTime.QuadPart = Int64ShrlMod32(tStartTime.QuadPart, nReduceMag);
			tStopTime.QuadPart  = Int64ShrlMod32(tStopTime.QuadPart,  nReduceMag);
			Freq.QuadPart = Int64ShrlMod32(Freq.QuadPart, nReduceMag);
			nOverhead >>= nReduceMag;
		}

		if(Freq.LowPart == 0)
			return 0L;

		// Reduced numbers to 32 bits, now can do the math
		uTime = (unsigned)(1000.0*(tStopTime.LowPart - tStartTime.LowPart - nOverhead)/Freq.LowPart);
	}
	else
	{
		uTime = (unsigned)(1000.0*(clock() - m_tStartTime.LowPart)/CLOCKS_PER_SEC);
	}

	return uTime;
}

void CTimer::DetermineTimer()
{
	// Assume the worst
	m_TimerToUse = ttuClock;

	if(GetFrequency(&m_PerfFreq))
	{
		// We can use hires timer, determine overhead
		m_TimerToUse = ttuHiRes;
		m_OverheadTicks  = 200;
		m_PerfFreqAdjust = 0;

		int i, nTicks, High32;
		LARGE_INTEGER lnStart, lnStop;
		
		for(i=0; i < 10; i++)
		{
			GetCounter(&lnStart);
			GetCounter(&lnStop);

			nTicks = lnStop.LowPart - lnStart.LowPart;

			if(nTicks >= 0 && nTicks < m_OverheadTicks)
				m_OverheadTicks = nTicks;
		}

		// See if Freq fits in 32 bits; if not lose some precision
		High32 = m_PerfFreq.HighPart;

		while(High32)
		{
			High32 >>= 1;
			m_PerfFreqAdjust++;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CWaitableTimer
//////////////////////////////////////////////////////////////////////

CWaitableTimer::CWaitableTimer(
	bool bManualReset)
	: m_hTimer(::CreateWaitableTimer(0, bManualReset, 0))
{
	if(m_hTimer == 0)
	{
		throw CWin32Exception(TEXT("CWaitableTimer::CWaitableTimer()"), ::GetLastError());
	}
}

CWaitableTimer::CWaitableTimer(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	bool bManualReset)
	: m_hTimer(::CreateWaitableTimer(lpEventAttributes, bManualReset, 0))
{
	if(m_hTimer == 0)
	{
		throw CWin32Exception(TEXT("CWaitableTimer::CWaitableTimer()"), ::GetLastError());
	}
}

CWaitableTimer::CWaitableTimer(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	bool bManualReset,
	LPCTSTR lpName)
	: m_hTimer(::CreateWaitableTimer(lpEventAttributes, bManualReset, lpName))
{
	if(m_hTimer == 0)
	{
		throw CWin32Exception(TEXT("CWaitableTimer::CWaitableTimer()"), ::GetLastError());
	}
}

CWaitableTimer::~CWaitableTimer()
{
	::CloseHandle(m_hTimer);
}

HANDLE CWaitableTimer::GetTimer() const
{
	return m_hTimer;
}

void CWaitableTimer::Wait() const
{
	if(!Wait(INFINITE))
	{
		throw CException(TEXT("CWaitableTimer::Wait()"), TEXT("Unexpected timeout on infinite wait"));
	}
}

bool CWaitableTimer::Wait(DWORD timeoutMillis) const
{
	bool ok;

	DWORD result = ::WaitForSingleObject(m_hTimer, timeoutMillis);

	if(result == WAIT_TIMEOUT)
	{
		ok = false;
	}
	else if(result == WAIT_OBJECT_0)
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception(TEXT("CWaitableTimer::Wait() - WaitForSingleObject"), ::GetLastError());
	}

	return ok;
}

bool CWaitableTimer::Set(LONG lPeriod)
{
	LARGE_INTEGER li = { 0 };
	return ::SetWaitableTimer(m_hTimer, &li, lPeriod, NULL, NULL, FALSE) ? true : false;
}

bool CWaitableTimer::Set(DWORD dwDueTime, LONG lPeriod)
{
	LARGE_INTEGER li = { 0 };
	li.QuadPart = -10000i64 * dwDueTime;

	return ::SetWaitableTimer(m_hTimer, &li, lPeriod, NULL, NULL, FALSE) ? true : false;
}

bool CWaitableTimer::Set(const LARGE_INTEGER* pDueTime, LONG lPeriod)
{
	return ::SetWaitableTimer(m_hTimer, pDueTime, lPeriod, NULL, NULL, FALSE) ? true : false;
}

void CWaitableTimer::Cancel()
{
	if(!::CancelWaitableTimer(m_hTimer))
	{
		throw CWin32Exception(TEXT("CWaitableTimer::Cancel()"), ::GetLastError());
	}
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
