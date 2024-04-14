// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <rpc.h>
#include <ocidl.h>
// elvees tools
#include <mmsystem.h>
#include <vfw.h>



#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlconv.h>
//#include <atl>

#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

// TODO: reference additional headers your program requires here

#include <map>
#include <vector>
#include <algorithm>


#include "../../DVRDBBridge/trunk/DVRDBBridge.h"
#include "../../../../CommonLibs/head/ElveesTools/trunk/VidDecoder.h"

#include "../../chcsva/trunk/chcsva.h"
#include "../../chcsva/trunk/ext/IVideoSystemConfig.h"
#include "../../chcsva/trunk/ext/UuidUtility.h"

#include "../../DVREngine/trunk/interface/IServer.h"

// DIBWIDTH
#include <dshow.h>

#undef SubclassWindow