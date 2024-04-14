///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_debug_macros.h
// ---------------------
// begin     : 1998
// modified  : 10 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DEFINE_ALIB_DEBUG_MACROS_H__
#define  __DEFINE_ALIB_DEBUG_MACROS_H__

namespace alib
{

// exception.cpp:
void ThrowStdException( const char * reason, const char * file, int line );

} // namespace alib

#define ALIB_THROW(reason) alib::ThrowStdException( reason, __FILE__, __LINE__ )

#define ALIB_VERIFY(e,reason) if (!(e)) alib::ThrowStdException( reason, __FILE__, __LINE__ )

#ifdef _DEBUG
#define ALIB_ASSERT(e) ASSERT(e)
#else
#define ALIB_ASSERT(e) if (!(e)) alib::ThrowStdException( #e, __FILE__, __LINE__ )
#endif

#endif // __DEFINE_ALIB_DEBUG_MACROS_H__

