///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_load_save.h
// ---------------------
// begin     : 2006
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_LOAD_SAVE_H__
#define __DEFINE_ALIB_LOAD_SAVE_H__

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function loads STL vector from a binary file.
///
/// \param  f    source binary file.
/// \param  arr  destination array.
/// \return      Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
bool LoadArray( FILE * f, std::vector<T> & arr )
{
  typedef  typename std::vector<T>::value_type  value_type;

  int n = 0;
  arr.clear();
  if ((f != 0) && (fread( &n, sizeof(n), 1, f ) == 1) && (0 <= n) && (n < INT_MAX/2))
  {
    arr.resize( n );
    if (fread( &(arr[0]), n*sizeof(value_type), 1, f ) == 1)
      return true;
  }
  arr.clear();
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function loads STL vector from a binary file.
///
/// \param  fname  file name.
/// \param  arr    destination array.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
bool LoadArray( LPCTSTR fname, std::vector<T> & arr )
{
  FILE * f = fopen( fname, _T("rb") );
  bool   ok = LoadArray( f, arr );

  if (f != 0)
    fclose( f );
  return ok;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function saves STL vector in a binary file.
///
/// \param  f    destination binary file.
/// \param  arr  source array.
/// \return      Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
bool SaveArray( FILE * f, const std::vector<T> & arr )
{
  typedef  typename std::vector<T>::value_type  value_type;

  int n = (int)(arr.size());
  return ((f != 0) &&
          (fwrite( &n, sizeof(n), 1, f ) == 1) &&
          (fwrite( &(*(arr.begin())), n*sizeof(value_type), 1, f ) == 1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function saves STL vector in a binary file.
///
/// \param  fname  file name.
/// \param  arr    source array.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
bool SaveArray( LPCTSTR fname, const std::vector<T> & arr )
{
  FILE * f = fopen( fname, _T("wb") );
  bool   ok = alib::SaveArray( f, arr );

  if (f != 0)
    fclose( f );
  return ok;
}

} // namespace alib

#endif // __DEFINE_ALIB_LOAD_SAVE_H__


