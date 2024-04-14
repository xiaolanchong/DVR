///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_clear.h
// ---------------------
// begin     : 2006
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_CLEAR_H__
#define __DEFINE_ALIB_CLEAR_H__

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function completely clears and deallocates STL container swapping it with an empty one.
///
/// \param  container  the container to be cleared.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CONTAINER >
void ReleaseContainer( CONTAINER & container )
{
  CONTAINER temp;
  container.swap( temp );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function clears container of pointers.
///
/// Function deallocates objects pointed by container's pointers and clears container itself.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CONTAINER >
void ClearPtrContainer( CONTAINER & container )
{
  ASSERT( sizeof(typename CONTAINER::value_type) == sizeof(void*) ); // weak check for pointer

  for (typename CONTAINER::iterator it = container.begin(); it != container.end(); it++)
  {
    typename CONTAINER::value_type p = (*it);
    delete p;
  }
  container.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function deletes specified object and sets pointer to zero.
///
/// \param  pObject  the pointer to the object to be deleted.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
inline void DeleteObject( T *& pObject )
{
  T * p = pObject;
  delete p;
  pObject = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function deletes specified array and sets pointer to zero.
///
/// \param  pArray  the pointer to the array to be deleted.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
inline void DeleteArray( T *& pArray )
{
  T * p = pArray;
  delete [] p;
  pArray = 0;
}

} // namespace alib

#endif // __DEFINE_ALIB_CLEAR_H__

