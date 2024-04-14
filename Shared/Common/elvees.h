///////////////////////////////////////////////////////////////////////////////////////////////////
// elvees.h
// ---------------------
// begin     : 2006
// modified  : 30 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_H__
#define __DEFINE_ELVEES_H__

#ifdef _MSC_VER
  #ifndef _CRT_SECURE_NO_DEPRECATE
  #define _CRT_SECURE_NO_DEPRECATE 1
  #endif
#pragma warning( push, 0 )
#endif

#undef min
#undef max
#include <algorithm>
#include <numeric>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <valarray>
#include <vector>
#include <limits>
#include <stack>
#include <iomanip>
#include <deque>

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include "integers.h"
#include "types.h"
#include "debug_macros.h"
#include "abstract_menu.h"
#include "serialization.h"

#include "../Interfaces/i_camera_analyzer.h"
#include "../Interfaces/i_camera_grabber.h"
#include "../Interfaces/i_data_transfer.h"
#include "../Interfaces/i_hall_analyzer.h"
#include "../Interfaces/i_message.h"
#include "../Interfaces/i_query_info.h"
#include "../Interfaces/i_videofile_reader.h"
#include "../Interfaces/i_video_reader.h"

#endif // __DEFINE_ELVEES_H__

