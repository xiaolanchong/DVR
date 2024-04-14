///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_compare.h
// ---------------------
// begin     : 2006
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_COMPARE_H__
#define __DEFINE_ALIB_COMPARE_H__

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks that dimensions of two objects are equal.
///
/// Objects must have methods 'width()' and 'height()'.
///
/// \param  a  reference to the 1st object.
/// \param  b  reference to the 2nd object.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T1, class T2 >
bool AreDimensionsEqual( const T1 & a, const T2 & b )
{
  return ((a.width() == b.width()) && (a.height() == b.height()));
}


///------------------------------------------------------------------------------------------------
/// \brief Function checks that two containers have equal sizes and their entries are identical.
///
/// \param  a  the first container to be checked.
/// \param  b  the second container to be checked.
/// \return    non-zero if containers are similar.
///------------------------------------------------------------------------------------------------
template< class CONTAINER >
bool AreContainersSimilar( const CONTAINER & a, const CONTAINER & b )
{
  return ((a.size() == b.size()) && ((std::mismatch( a.begin(), a.end(), b.begin() )).first == a.end()));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function-object compares points by their positions in (X,Y)-plane.
///
/// (a < b) <---> ((a.y < b.y) OR ((a.y == b.y) AND (a.x < b.x))) 
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class POINT_TYPE >
struct LessXY : public std::binary_function< POINT_TYPE, POINT_TYPE, bool >
{
  bool operator()( const POINT_TYPE & a, const POINT_TYPE & b ) const
  {
    return ((a.y < b.y) || ((a.y == b.y) && (a.x < b.x)));
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CmpPtr.
/// \brief  Object is used to find container's element by its address.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
struct CmpPtr
{
  const T * m_pointer;       //!< the pointer to compare with

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CmpPtr( const T * p = 0 ) : m_pointer(p) {}

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Operator returns 'true', if specified element is addressed by stored pointer.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  bool operator()( const T & t )
  {
    return (&t == m_pointer);
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CmpPairByFirst.
/// \brief  Object compares two 'std::pair' objects by their first fields.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
struct CmpPairByFirst
{
  bool operator()( const T & x, const T & y ) const
  {
    return (x.first < y.first);
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CmpPairBySecond.
/// \brief  Object compares two 'std::pair' objects by their second fields.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
struct CmpPairBySecond
{
  bool operator()( const T & x, const T & y ) const
  {
    return (x.second < y.second);
  }
};

} // namespace alib

#endif // __DEFINE_ALIB_COMPARE_H__

