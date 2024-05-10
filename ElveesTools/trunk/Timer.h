// Timer.h: interface for the CTimer & CWaitableTimer classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_TIMER_INCLUDED_
#define ELVEESTOOLS_TIMER_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CTimer
//////////////////////////////////////////////////////////////////////

class CTimer
{
public:
	CTimer();

	void Start();		// Start timer
	unsigned Stop();	// Returns time, in milliseconds

	bool GetCounter(LARGE_INTEGER* lpCounter);
	bool GetFrequency(LARGE_INTEGER* lpFrequency);

protected:
	void DetermineTimer();

private:
	typedef enum tagTimer
	{
		ttuUnknown,
		ttuHiRes,
		ttuClock
	} Timer;
	
	static Timer m_TimerToUse;		 // used timer

	static int m_OverheadTicks;		 // overhead in calling timer
	static int m_PerfFreqAdjust;	 // in case frequency is too big
	static LARGE_INTEGER m_PerfFreq; // ticks per second

	LARGE_INTEGER m_tStartTime;
	
	// No copies do not implement
	CTimer(const CTimer &rhs);
	CTimer &operator=(const CTimer &rhs);
};

//////////////////////////////////////////////////////////////////////
// CWaitableTimer
//////////////////////////////////////////////////////////////////////

class CWaitableTimer
{
public:
	explicit CWaitableTimer(
		bool manualReset);

	explicit CWaitableTimer(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset);

	explicit CWaitableTimer(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset,
		LPCTSTR lpName);

	virtual ~CWaitableTimer();

	HANDLE GetTimer() const;

	void Wait() const;
	bool Wait(DWORD timeoutMillis) const;

	bool Set(LONG lPeriod);
	bool Set(DWORD dwDueTime, LONG lPeriod);
	bool Set(const LARGE_INTEGER* pDueTime, LONG lPeriod);
	
	void Cancel();

private:
	HANDLE m_hTimer;

	// No copies do not implement
	CWaitableTimer(const CWaitableTimer &rhs);
	CWaitableTimer &operator=(const CWaitableTimer &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_TIMER_INCLUDED_
