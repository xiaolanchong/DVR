// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#ifdef _MSC_VER
//#pragma warning( disable : 4996 )		// function is declared deprecated
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <ctime>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
//thread
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include <boost/lambda/lambda.hpp>
//time
#pragma warning( push )	
#pragma warning( disable : 4996 )		// function is declared deprecated (localtime, gmtime)
#include <boost/date_time/posix_time/posix_time_types.hpp>
#pragma warning( pop )

#pragma warning( push )	
#pragma warning( disable : 4996 )		// function is declared deprecated
#include <boost/format.hpp>
#pragma warning( pop )

#include <boost/integer.hpp>
#pragma warning( push )		
#pragma warning( disable : 4127 4701 )		// conditional express is constant, potentially uninitialized variable
#include <boost/lexical_cast.hpp>
#pragma warning( pop )

#include "../../shared/Common/ExceptionTemplate.h"
#include "../../shared/Common/integers.h"
#include "../../shared/Common/types.h"
#include "../../shared/Interfaces/i_video_reader.h"
#if 0
#include "../../../../CommonLibs/head/ExceptionHandler/trunk/ExceptionHandler.h"
#endif
#ifndef ASSERT
#define ASSERT(x) _ASSERTE(x)
#endif
/*
#ifndef _T
#define _T(x) (x)
#endif
*/
//for registry
#include <atlbase.h>

#include "../../chcsva/trunk/ext/IVideoSystemConfig.h"
#include "../../chcsva/trunk/chcsva.h"
#include "../../DVRAlgo/trunk/Common/exports.h"
#include "../../DVRDBBridge/trunk/DVRDBBridge.h"
#include "../../DVRDBBridge/trunk/interface/IDBServer.h"
#include "../../DVRDBBridge/trunk/interface/IVideoConfig.h"
#include "../../DVRBackup/trunk/interface/DVRBackup.hpp"
#include "../../DVRBackup/trunk/interface/BackupParam.h"

typedef std::vector<boost::uint8_t>		ByteArray_t;

//! DUPLICATE
struct Extract1st
{
	template<class T, class U> T operator() ( const std::pair<T, U>& p ) const 
	{ 
		return p.first; 
	}
};

struct Extract2nd
{
	template<class T, class U> U operator() ( const std::pair<T, U>& p ) const 
	{ 
		return p.second; 
	}
};

#ifndef UNIX_RELEASE
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
#else
#define _ASSERTE(x) assert(x)
#endif