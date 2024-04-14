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
/*
//wxWidgets
#include <wx/wx.h>
#include <wx/dynlib.h>

//Windows
#include <windows.h>
#include <ocidl.h>
*/
#include <conio.h>
//STL
#include <algorithm>
#include <list>
#include <vector>
#include <exception>
#include <deque>
#include <map>
#include <set>
#include <iostream>

//Boost
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

//Elvees 
#include "../../../../../shared/interfaces/i_message.h"

#endif //__STDAFX_HPP__
