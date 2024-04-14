///////////////////////////////////////////////////////////////////////////////////////////////////
// integers.h
// ---------------------
// begin     : Nov 2005
// modified  : 29 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ELVEES_INTEGERS_H__
#define __ELVEES_INTEGERS_H__

#ifdef BOOST_INTEGER_HPP

  typedef  boost::int8_t     sbyte;
  typedef  boost::uint8_t    ubyte;

  typedef  boost::int16_t    sword;
  typedef  boost::uint16_t   uword;

  typedef  boost::int32_t    sint;
  typedef  boost::uint32_t   uint;

  typedef  boost::int64_t    slong;
  typedef  boost::uint64_t   ulong;

#else

  typedef  signed __int8     sbyte;
  typedef  unsigned __int8   ubyte;

  typedef  signed __int16    sword;
  typedef  unsigned __int16  uword;

  typedef  signed __int32    sint;
  typedef  unsigned __int32  uint;

  typedef  signed __int64    slong;
  typedef  unsigned __int64  ulong;

#endif

#endif // __ELVEES_INTEGERS_H__

