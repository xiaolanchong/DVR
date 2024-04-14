///////////////////////////////////////////////////////////////////////////////////////////////////
// debug_macros.h
// ---------------------
// begin     : Nov 2005
// modified  : 29 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DEFINE_ELVEES_DEBUG_MACROS_H__
#define  __DEFINE_ELVEES_DEBUG_MACROS_H__

#include <stdio.h>
#include <exception>
#include <string>

namespace Elvees
{

// exception.cpp:
void ThrowStdException( const char * reason, const char * file, int line );

} // namespace Elvees

#define ELVEES_THROW(reason) Elvees::ThrowStdException( reason, __FILE__, __LINE__ )

#define ELVEES_VERIFY(e,reason) if (!(e)) Elvees::ThrowStdException( reason, __FILE__, __LINE__ )

#ifdef _DEBUG
#define ELVEES_ASSERT(e) ASSERT(e)
#else
#define ELVEES_ASSERT(e) if (!(e)) Elvees::ThrowStdException( #e, __FILE__, __LINE__ )
#endif

#ifdef _DEBUG
#define ELVEES_HRESULT_ASSERT(e) ASSERT(SUCCEEDED(e))
#else
#define ELVEES_HRESULT_ASSERT(e) if (FAILED(e)) Elvees::ThrowStdException( #e, __FILE__, __LINE__ )
#endif

#endif // __DEFINE_ELVEES_DEBUG_MACROS_H__

