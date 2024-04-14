///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_util.h
// ---------------------
// begin     : 1998
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_UTILITY_H__
#define __DEFINE_ALIB_UTILITY_H__

namespace alib
{

///------------------------------------------------------------------------------------------------
/// \brief Function initializes a rectangle.
///
/// \param  rect  the rectangle to be set.
/// \param  x1    new value of the first abscissa.
/// \param  y1    new value of the first ordinate.
/// \param  x2    new value of the second abscissa.
/// \param  y2    new value of the second ordinate.
/// \return       reference to modified rectangle.
///------------------------------------------------------------------------------------------------
inline ARectBase & SetRectangle( ARectBase & rect, boost::int32_t x1,
                                                   boost::int32_t y1,
                                                   boost::int32_t x2,
                                                   boost::int32_t y2 )
{
  rect.x1 = x1;
  rect.y1 = y1;
  rect.x2 = x2;
  rect.y2 = y2;
  return rect;
}


///------------------------------------------------------------------------------------------------
/// \brief Function initializes a point.
///
/// \param  p  the point to be initialized.
/// \param  x  new abscissa.
/// \param  y  new ordinate.
/// \return    reference to modified point.
///------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
inline Point2D<TYPE,PRECISE> & SetPoint( Point2D<TYPE,PRECISE> & p, TYPE x, TYPE y )
{
  p.x = x;
  p.y = y;
  return p;
}


///------------------------------------------------------------------------------------------------
/// \brief Function completely clears and deallocates STL container
///        by swapping it with an empty container of the same type.
///
/// \param  container  the container to be cleared.
///------------------------------------------------------------------------------------------------
template< class CONTAINER >
void CompleteClear( CONTAINER & container )
{
  CONTAINER temp;
  container.swap( temp );
}

} // namespace alib

#endif // __DEFINE_ALIB_UTILITY_H__

