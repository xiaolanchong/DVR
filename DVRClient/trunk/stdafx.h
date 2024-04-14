// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// C RunTime Header Files
#include <windows.h>
#include <atlbase.h>

// TODO: reference additional headers your program requires here

// STL
#include <vector>
#include <memory>
#include <string>
#include <numeric>

// Boost
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/assign.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/regex.hpp>

#include <boost/program_options.hpp>

// wxWidget files

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/notebook.h>
#endif

#ifdef UNIX_RELEASE
#include <unistd.h>
#endif

#include <wx/intl.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/sysopt.h>
#include <wx/toolbar.h>
#include <wx/splitter.h>

#include <wx/fs_zip.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/spinbutt.h>
#include <wx/dcbuffer.h>
#include <wx/datectrl.h>
#include <wx/spinctrl.h>

#include <wx/html/htmlwin.h>
#include <wx/html/m_templ.h>



// OpenGL support
#include <GL/gl.h>
#include <wx/glcanvas.h>
// GLUT
//#include "../../3rd_party/glut-3.7/include/gl/glut.h"

#include "../../../../CommonLibs/HEAD/ExceptionHandler/trunk/ExceptionHandler.h"

// shared
#include "../../Shared/Interfaces/i_message.h"
#include "../../Shared/Common/ExceptionTemplate.h"

#include "../../DVRRender/trunk/DVRRender.hpp"
#include "../../DVRRender/trunk/IDVRGraphicsWrapper.h"
#include "../../DVRRender/trunk/OGLRender/Sources/DllMain.hpp"

#include "../../DVRControls/DateTimeControl.hpp"
#include "../../DVRControls/ScheduleControl.hpp"

#include "../../DVREngine/trunk/server/AlarmInfo.h"
#include "../../DVREngine/trunk/interface/IArchiveUserData.h"
#include "../../DVREngine/trunk/interface/IServer.h"
#include "../../DVREngine/trunk/interface/IClient.h"

// DBBridge::CreateVideoSystemConfig
#include "../../DVRDBBridge/trunk/DVRDBBridge.h"

// for server mode flags ( CHCS_MODE_* )
#include "../../chcsva/trunk/chcsva.h"
#include "../../chcsva/trunk/ext/IVideoSystemConfig.h"

#ifndef UNIX_RELEASE
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
#else
#define _ASSERTE(x) assert(x)
#endif

#define _ASSERTE_DESC(x, description) _ASSERTE( (x) && (description) )

inline void TraceDebug( const char* szText )
{
#ifdef WIN32
	OutputDebugStringA(szText);
	OutputDebugStringA("\n");
#endif
}

template <typename T1> void TraceDebug( const char* szFormatString, T1 t1 )
{
	std::string s = (boost::format( szFormatString ) % t1).str();
	TraceDebug(s.c_str());
}

template <typename T1, typename T2> void TraceDebug( const char* szFormatString, T1 t1, T2 t2 )
{
	std::string s = (boost::format( szFormatString ) % t1 % t2).str();
	TraceDebug(s.c_str());
}

template <typename T1, typename T2, typename T3> void TraceDebug( const char* szFormatString, T1 t1, T2 t2, T3 )
{
	std::string s = (boost::format( szFormatString ) % t1 % t2 % t3).str();
	TraceDebug(s.c_str());
}

bool localtimeWrap( time_t SrcTime, tm& DstTime );