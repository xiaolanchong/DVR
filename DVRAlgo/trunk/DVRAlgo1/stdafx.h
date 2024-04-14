#pragma once

//lint -w0
#pragma warning( push, 0 )

#if (defined(_WIN32) || defined(_WIN64))
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _T(x)
#define _T(x) x
#endif

#ifndef ASSERT
#define ASSERT(x) _ASSERT(x)
#endif

#include <conio.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>

#pragma warning( pop )
//lint -w2

