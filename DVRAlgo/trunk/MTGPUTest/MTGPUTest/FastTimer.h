// FastTimer.h: interface for the FastTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTTIMER_H__D1835248_DD7D_4478_8EB6_9A77790C3C46__INCLUDED_)
#define AFX_FASTTIMER_H__D1835248_DD7D_4478_8EB6_9A77790C3C46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "tier0/platform.h"

#ifdef _WIN32
#define PLATFORM_INTERFACE	extern
#else
#error Implement extern here
#endif

PLATFORM_INTERFACE __int64 g_ClockSpeed;
PLATFORM_INTERFACE unsigned long g_dwClockSpeed;

PLATFORM_INTERFACE double g_ClockSpeedMicrosecondsMultiplier;
PLATFORM_INTERFACE double g_ClockSpeedMillisecondsMultiplier;
PLATFORM_INTERFACE double g_ClockSpeedSecondsMultiplier;

// Processor Information:
struct CPUInformation
{
	int	 m_Size;		// Size of this structure, for forward compatability.

	bool m_bRDTSC : 1,	// Is RDTSC supported?
		 m_bCMOV  : 1,  // Is CMOV supported?
		 m_bFCMOV : 1,  // Is FCMOV supported?
		 m_bSSE	  : 1,	// Is SSE supported?
		 m_bSSE2  : 1,	// Is SSE2 Supported?
		 m_b3DNow : 1,	// Is 3DNow! Supported?
		 m_bMMX   : 1,	// Is MMX supported?
		 m_bHT	  : 1;	// Is HyperThreading supported?

	unsigned char m_nLogicalProcessors,		// Number op logical processors.
		          m_nPhysicalProcessors;	// Number of physical processors

	__int64 m_Speed;						// In cycles per second.

	char* m_szProcessorID;				// Processor vendor Identification.
};

PLATFORM_INTERFACE const CPUInformation& GetCPUInformation();


class CCycleCount
{
friend class CFastTimer;


public:
					CCycleCount();

	void			Sample();	// Sample the clock. This takes about 34 clocks to execute (or 26,000 calls per millisecond on a P900).

	void			Init();		// Set to zero.
	void			Init( float initTimeMsec );
	bool			IsLessThan( CCycleCount const &other ) const;					// Compare two counts.

	// Convert to other time representations. These functions are slow, so it's preferable to call them
	// during display rather than inside a timing block.
	unsigned long	GetCycles()  const;

	unsigned long	GetMicroseconds() const; 	
	double			GetMicrosecondsF() const; 	

	unsigned long	GetMilliseconds() const;
	double			GetMillisecondsF() const;

	double			GetSeconds() const;

	CCycleCount&	operator+=( CCycleCount const &other );

	// dest = rSrc1 + rSrc2
	static void		Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.
	
	// dest = rSrc1 - rSrc2
	static void		Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.


	__int64	m_Int64;
};

class CClockSpeedInit
{
public:
	CClockSpeedInit()
	{
		Init();
	}

	static void Init()
	{
		const CPUInformation& pi = GetCPUInformation();

		g_ClockSpeed = pi.m_Speed;
		g_dwClockSpeed = (unsigned long)g_ClockSpeed;

		g_ClockSpeedMicrosecondsMultiplier = 1000000.0 / (double)g_ClockSpeed;
		g_ClockSpeedMillisecondsMultiplier = 1000.0 / (double)g_ClockSpeed;
		g_ClockSpeedSecondsMultiplier = 1.0f / (double)g_ClockSpeed;
	}

};

class CFastTimer
{
public:
	// These functions are fast to call and should be called from your sampling code.
	void				Start();
	void				End();

	const CCycleCount &	GetDuration() const;	// Get the elapsed time between Start and End calls.
	CCycleCount 		GetDurationInProgress() const; // Call without ending. Not that cheap.

	// Return number of cycles per second on this processor.
	static inline unsigned long	GetClockSpeed();

private:

	CCycleCount	m_Duration;
};


// This is a helper class that times whatever block of code it's in
class CTimeScope
{
public:
				CTimeScope( CFastTimer *pTimer );
				~CTimeScope();

private:	
	CFastTimer	*m_pTimer;
};

inline CTimeScope::CTimeScope( CFastTimer *pTotal )
{
	m_pTimer = pTotal;
	m_pTimer->Start();
}

inline CTimeScope::~CTimeScope()
{
	m_pTimer->End();
}

// This is a helper class that times whatever block of code it's in and
// adds the total (int microseconds) to a global counter.
class CTimeAdder
{
public:
				CTimeAdder( CCycleCount *pTotal );
				~CTimeAdder();

	void		End();

private:	
	CCycleCount	*m_pTotal;
	CFastTimer	m_Timer;
};

inline CTimeAdder::CTimeAdder( CCycleCount *pTotal )
{
	m_pTotal = pTotal;
	m_Timer.Start();
}

inline CTimeAdder::~CTimeAdder()
{
	End();
}

inline void CTimeAdder::End()
{
	if( m_pTotal )
	{
		m_Timer.End();
		*m_pTotal += m_Timer.GetDuration();
		m_pTotal = 0;
	}
}



// -------------------------------------------------------------------------- // 
// Simple tool to support timing a block of code, and reporting the results on
// program exit
// -------------------------------------------------------------------------- // 

#define PROFILE_SCOPE(name) \
	class C##name##ACC : public CAverageCycleCounter \
	{ \
	public: \
		~C##name##ACC() \
		{ \
			Msg("%-48s: %6.3f avg (%8.1f total, %7.3f peak, %5d iters)\n",  \
				#name, \
				GetAverageMilliseconds(), \
				GetTotalMilliseconds(), \
				GetPeakMilliseconds(), \
				GetIters() ); \
		} \
	}; \
	static C##name##ACC name##_ACC; \
	CAverageTimeMarker name##_ATM( &name##_ACC )

// -------------------------------------------------------------------------- // 

class CAverageCycleCounter
{
public:
	CAverageCycleCounter();
	
	void Init();
	void MarkIter( const CCycleCount &duration );
	
	unsigned GetIters() const;
	
	double GetAverageMilliseconds() const;
	double GetTotalMilliseconds() const;
	double GetPeakMilliseconds() const;

private:
	unsigned	m_nIters;
	CCycleCount m_Total;
	CCycleCount	m_Peak;
	bool		m_fReport;
	const char *m_pszName;
};

// -------------------------------------------------------------------------- // 

class CAverageTimeMarker
{
public:
	CAverageTimeMarker( CAverageCycleCounter *pCounter );
	~CAverageTimeMarker();
	
private:
	CAverageCycleCounter *m_pCounter;
	CFastTimer	m_Timer;
};
// -------------------------------------------------------------------------- // 
// CCycleCount inlines.
// -------------------------------------------------------------------------- // 

inline CCycleCount::CCycleCount()
{
	m_Int64 = 0;
}

inline void CCycleCount::Init()
{
	m_Int64 = 0;
}

inline void CCycleCount::Init( float initTimeMsec )
{
	if ( g_ClockSpeedMillisecondsMultiplier > 0 )
		m_Int64 = __int64(initTimeMsec / g_ClockSpeedMillisecondsMultiplier);
	else
		m_Int64 = 0;
}

inline void CCycleCount::Sample()
{
	unsigned long* pSample = (unsigned long *)&m_Int64;
	__asm
	{
		// force the cpu to synchronize the instruction queue
		// NJS: CPUID can really impact performance in tight loops.
		//cpuid
		//cpuid
		//cpuid
		mov		ecx, pSample
		rdtsc
		mov		[ecx],     eax
		mov		[ecx+4],   edx
	}
}


inline CCycleCount& CCycleCount::operator+=( CCycleCount const &other )
{
	m_Int64 += other.m_Int64;
	return *this;
}


inline void CCycleCount::Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 + rSrc2.m_Int64;
}

inline void CCycleCount::Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 - rSrc2.m_Int64;
}

inline bool CCycleCount::IsLessThan(CCycleCount const &other) const
{
	return m_Int64 < other.m_Int64;
}


inline unsigned long CCycleCount::GetCycles() const
{
	return (unsigned long)m_Int64;
}


inline unsigned long CCycleCount::GetMicroseconds() const
{
	return (unsigned long)((m_Int64 * 1000000) / g_ClockSpeed);
}


inline double CCycleCount::GetMicrosecondsF() const
{
	return (double)( m_Int64 * g_ClockSpeedMicrosecondsMultiplier );
}


inline unsigned long CCycleCount::GetMilliseconds() const
{
	return (unsigned long)((m_Int64 * 1000) / g_ClockSpeed);
}


inline double CCycleCount::GetMillisecondsF() const
{
	return (double)( m_Int64 * g_ClockSpeedMillisecondsMultiplier );
}


inline double CCycleCount::GetSeconds() const
{
	return (double)( m_Int64 * g_ClockSpeedSecondsMultiplier );
}


// -------------------------------------------------------------------------- // 
// CFastTimer inlines.
// -------------------------------------------------------------------------- // 
inline void CFastTimer::Start()
{
	m_Duration.Sample();
}


inline void CFastTimer::End()
{
	CCycleCount cnt;
	cnt.Sample();
	m_Duration.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;
}

inline CCycleCount CFastTimer::GetDurationInProgress() const
{
	CCycleCount cnt;
	cnt.Sample();
	
	CCycleCount result;
	result.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;
	
	return result;
}


inline unsigned long CFastTimer::GetClockSpeed()
{
	return g_dwClockSpeed;
}


inline CCycleCount const& CFastTimer::GetDuration() const
{
	return m_Duration;
}


// -------------------------------------------------------------------------- // 
// CAverageCycleCounter inlines

inline CAverageCycleCounter::CAverageCycleCounter()
 :	m_nIters( 0 )
{
}

inline void CAverageCycleCounter::Init()
{
	m_Total.Init();
	m_Peak.Init();
	m_nIters = 0;
}

inline void CAverageCycleCounter::MarkIter( const CCycleCount &duration )
{
	++m_nIters;
	m_Total += duration;
	if ( m_Peak.IsLessThan( duration ) )
		m_Peak = duration;
}

inline unsigned CAverageCycleCounter::GetIters() const
{
	return m_nIters;
}

inline double CAverageCycleCounter::GetAverageMilliseconds() const
{
	if ( m_nIters )
		return (m_Total.GetMillisecondsF() / (double)m_nIters);
	else
		return 0;
}

inline double CAverageCycleCounter::GetTotalMilliseconds() const
{
	return m_Total.GetMillisecondsF();
}

inline double CAverageCycleCounter::GetPeakMilliseconds() const
{
	return m_Peak.GetMillisecondsF();
}

// -------------------------------------------------------------------------- // 

inline CAverageTimeMarker::CAverageTimeMarker( CAverageCycleCounter *pCounter )
{
	m_pCounter = pCounter;
	m_Timer.Start();
}

inline CAverageTimeMarker::~CAverageTimeMarker()
{
	m_Timer.End();
	m_pCounter->MarkIter( m_Timer.GetDuration() );
}

#endif // !defined(AFX_FASTTIMER_H__D1835248_DD7D_4478_8EB6_9A77790C3C46__INCLUDED_)
