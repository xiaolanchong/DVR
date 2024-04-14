///////////////////////////////////////////////////////////////////////////////////////////////////
// milli_timer.h
// ---------------------
// begin     : 2006
// modified  : 10 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_TIMER_H__
#define __DEFINE_ELVEES_TIMER_H__

#include "boost/thread/mutex.hpp"

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class MilliTimer.
/// \brief High-precision timer that provides timestamps in milliseconds.
///////////////////////////////////////////////////////////////////////////////////////////////////
class MilliTimer
{
private:
  bool             m_bGood;       //!< nonzero means successfull initialization
  LARGE_INTEGER    m_frequency;   //!< performance frequency, in counts per millisecond
  LARGE_INTEGER    m_clockStart;  //!< the start value of the high-resolution performance counter 
  boost::mutex     m_mutex;       //!< data protector

public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
MilliTimer()
{
	boost::mutex::scoped_lock lock( m_mutex );
    m_bGood = false;
    LARGE_INTEGER countPerSec;
    if (QueryPerformanceFrequency( &countPerSec ))
    {
      m_frequency.QuadPart = countPerSec.QuadPart / 1000;
      if (QueryPerformanceCounter( &m_clockStart ))
        m_bGood = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function return the current time in milliseconds.
///////////////////////////////////////////////////////////////////////////////////////////////////
ulong Time() 
{
  ELVEES_ASSERT( m_bGood );
  boost::mutex::scoped_lock lock( m_mutex );
  LARGE_INTEGER time;
  QueryPerformanceCounter( &time );
  return (ulong)((time.QuadPart - m_clockStart.QuadPart) / m_frequency.QuadPart);
}

}; // class MilliTimer

} // namespace Elvees

#endif // __DEFINE_ELVEES_TIMER_H__

