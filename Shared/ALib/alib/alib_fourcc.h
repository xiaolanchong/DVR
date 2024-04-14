///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_fourcc.h
// ---------------------
// begin     : 2006
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_FOURCC_H__
#define __DEFINE_ALIB_FOURCC_H__

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function combines four characters into a single integer.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline boost::int32_t MakeFourCC( int a, int b, int c, int d )
{
  return ((__int32(a & 0x0FF))       |
          (__int32(b & 0x0FF) << 8)  |
          (__int32(c & 0x0FF) << 16) |
          (__int32(d & 0x0FF) << 24));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function converts integer value into a string of four characters.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline const TCHAR * FourCC2Str( boost::int32_t x, TCHAR s[5] )
{
  s[0] = (TCHAR)((x)       & 0x0FF);
  s[1] = (TCHAR)((x >>  8) & 0x0FF);
  s[2] = (TCHAR)((x >> 16) & 0x0FF);
  s[3] = (TCHAR)((x >> 24) & 0x0FF);
  s[4] = (TCHAR)(_T('\0'));
  return s;
}

} // namespace alib

#endif // __DEFINE_ALIB_FOURCC_H__

