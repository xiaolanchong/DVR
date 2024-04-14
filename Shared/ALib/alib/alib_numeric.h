///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_numeric.h
// ---------------------
// begin     : 1998
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_NUMERIC_H__
#define __DEFINE_ALIB_NUMERIC_H__

namespace alib
{

///------------------------------------------------------------------------------------------------
/// \brief Function returns maximal number among three specified ones.
///------------------------------------------------------------------------------------------------
template< class T >
inline T Max3( T a, T b, T c )
{
  return ((a > b) ? std::max( a, c ) : std::max( b, c ));
}


///------------------------------------------------------------------------------------------------
/// \brief Function rounds floating-point digit to the nearest integer.
///------------------------------------------------------------------------------------------------
template< class T >
inline int Round( T x ) 
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );

  return ((int)(x + ((x < (T)(0.0)) ? (T)(-0.5) : (T)(+0.5))));
}


///------------------------------------------------------------------------------------------------
/// \brief Function rounds 32-bit floating-point digit to the nearest integer.
///
/// Function is optimized for x86 platforms with SSE instruction enabled.
///------------------------------------------------------------------------------------------------
inline int RoundFloat( float x )
{
#ifdef ALIB_X86
  boost::int32_t h = (((boost::int32_t&)x) & 0x80000000) + 0x3f000000;
  return (int)(x + ((float&)h));
#else
  return (int)(x + ((x < 0.0f) ? (-0.5f) : (+0.5f)));
#endif
}


///------------------------------------------------------------------------------------------------
/// \brief Function returns the sign of the specified value.
///------------------------------------------------------------------------------------------------
template< class T >
inline int Sign( T x ) 
{ 
  return ((x >= (T)0) ? +1 : -1);
}


///------------------------------------------------------------------------------------------------
/// \brief Function returns module of specified value.
///------------------------------------------------------------------------------------------------
template< class T >
inline T Abs( T x )
{
  return ((x >= (T)0) ? x : -x);
}


///------------------------------------------------------------------------------------------------
/// \brief Function checks whether specified value lies within <b>semi-open</b> interval [min,max).
///
/// \param  x    the value to be checked.
/// \param  min  the minimal margin.
/// \param  max  the maximal margin.
/// \return      nonzero if the value lies within semi-open interval.
///------------------------------------------------------------------------------------------------
template< class T > 
inline bool IsRange( T x, T min, T max )
{
  ASSERT( min < max );
  return ((min <= x) && (x < max));
}


///------------------------------------------------------------------------------------------------
/// \brief Function checks whether specified value lies within <b>closed</b> interval [min,max].
///
/// \param  x    the value to be checked.
/// \param  min  the minimal margin.
/// \param  max  the maximal margin.
/// \return      nonzero if the value lies within closed interval.
///------------------------------------------------------------------------------------------------
template< class T > 
inline bool IsLimited( T x, T min, T max )
{
  ASSERT( min <= max );
  return ((min <= x) && (x <= max));
}


///------------------------------------------------------------------------------------------------
/// \brief Function bounds specified value by the minimal and the maximal margins [min,max].
///
/// \param  x    the value to be bounded.
/// \param  min  the minimal margin.
/// \param  max  the maximal margin.
/// \return      bounded value.
///------------------------------------------------------------------------------------------------
template< class T >
inline T Limit( T x, T min, T max )
{
  ASSERT( min <= max );
  return ((x < min) ? min : ((x > max) ? max : x));
}


///------------------------------------------------------------------------------------------------
/// \brief Function returns reciprocal value.
///
/// \return  <i>1/x</i> if <i>x</i> is nonzero, otherwise 0.
///------------------------------------------------------------------------------------------------
template< class T >
inline T Reciprocal( T x )
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );

  const T min = (std::numeric_limits<T>::min()) / (std::numeric_limits<T>::epsilon());
  return ((fabs( x ) > min) ? (T)(1.0/x) : (T)(0.0));
}


///------------------------------------------------------------------------------------------------
/// \brief Function returns the absolute value of difference between two digits.
///
/// \param  a  the first operand.
/// \param  b  the second operand.
/// \return    |a-b|.
///------------------------------------------------------------------------------------------------
template< class T >
inline T ModuleOfDifference( T a, T b )
{
  ASSERT( std::numeric_limits<T>::is_specialized );

  return ((a >= b) ? (a-b) : (b-a));
}


///------------------------------------------------------------------------------------------------
/// \brief Function divides two positive integers rounding result (a/b) to the nearest integer.
///------------------------------------------------------------------------------------------------
template< class T >
inline T RoundPosDiv( T a, T b )
{
  ASSERT( std::numeric_limits<T>::is_specialized && std::numeric_limits<T>::is_integer );
  ASSERT( (a >= 0) && (b > 0) );

  return ((a+(b>>1))/b);
}


///------------------------------------------------------------------------------------------------
/// \brief Function returns squared value of an input one.
///------------------------------------------------------------------------------------------------
template< class T >
inline T Sqr( T x )
{
  return (x*x);
}


///------------------------------------------------------------------------------------------------
/// \brief Function squared difference between two digits.
///------------------------------------------------------------------------------------------------
template< class T >
inline T SqDiff( T a, T b )
{
  T t = ((a >= b) ? (a-b) : (b-a));
  return (t*t);
}


///------------------------------------------------------------------------------------------------
/// \brief Function interpolates between two values:
/// \f[ v = v_1 + (v_2 - v_1) \cdot t \f]. 
///------------------------------------------------------------------------------------------------
template< class T, class P >
inline T Interpolate( T v1, T v2, P t )
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );
  ASSERT( std::numeric_limits<P>::is_specialized && !(std::numeric_limits<P>::is_integer) );

  return (T)(v1 + (v2-v1)*t);
}


///------------------------------------------------------------------------------------------------
/// \brief Function interpolates between two values:
/// \f[ v = v_1 + (v_2 - v_1) \cdot \frac{t - t1}{t2 - t1} \f]. 
///------------------------------------------------------------------------------------------------
template< class T, class P >
inline T Interpolate2(const T & v1, const T & v2, P t1, P t, P t2 )
{
  const T min = (std::numeric_limits<T>::min()) / (std::numeric_limits<T>::epsilon());

  return ((fabs( t2-t1 ) > min) ? (T)(v1 + (v2-v1)*((t-t1)/(t2-t1))) : v1);
}

} // namespace alib

#endif // __DEFINE_ALIB_NUMERIC_H__

