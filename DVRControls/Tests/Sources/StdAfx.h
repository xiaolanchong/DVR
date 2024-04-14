/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: StdAfx.h
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#ifndef __STDAFX_HPP__
#define __STDAFX_HPP__

//Disable depricated functions warning 
#pragma warning(disable : 4996)

//wxWidgets
#include <wx/wx.h>
#include <wx/spinbutt.h>
#include <wx/dcbuffer.h>

#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <map>

//Just shutup the cl.exe
#pragma warning(disable : 4267 4244)
#include <boost/format.hpp>
#pragma warning(default : 4267 4244)

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/regex.hpp>


#endif //__STDAFX_HPP__
