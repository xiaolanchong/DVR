/*
*	DVR 
*	Copyright(C) 2006 Elvees
*	All rights reserved.
*
*	$Filename: StdAfx.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-02-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Standart header
*
*
*/
#ifndef __STDAFX_HPP__
#define __STDAFX_HPP__

//Warning: String-routines defined as deprecated in VS2005
#pragma warning( disable: 4996 4251 )

//wxWidgets
#include <wx/wx.h>

//Windows
#include <windows.h>
#include <ocidl.h>

//STL
#include <exception>
#include <algorithm>
#include <list>
#include <vector>
#include <exception>
#include <deque>
#include <map>
#include <set>
#include <iostream>

//Boost
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/function/function1.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

//Elvees 
#include "../../../Shared/Interfaces/i_message.h"

#include "../ExceptionHandler/ExceptionHandler.h"

#endif //__STDAFX_HPP__
