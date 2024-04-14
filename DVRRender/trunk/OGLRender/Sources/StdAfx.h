/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: StdAfx.h
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/


#ifndef __STDAFX_HPP__
#define __STDAFX_HPP__

//Warning: String-routines defined as deprecated in VS2005
#pragma warning( disable: 4996 4251 )

//wxWidget
#include <wx/glcanvas.h>
#include <wx/wx.h>
#include <wx/dynlib.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/rawbmp.h>
#include <ctime>


//Windows
#include <windows.h>
#include <ocidl.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

//STL
#include <algorithm>
#include <functional>
#include <numeric>
#include <list>
#include <vector>
#include <exception>
#include <deque>
#include <map>
#include <set>

//OpenGL
//NOTE: glut 3.7 используется только для отображения текста, по умолчанию он не может отображать
//      символы русского алфавита (нет шрифтов), на 2006-09-19 он не используется

//#define USE_GLUT
#ifdef USE_GLUT
#include <gl/gl.h>
#include "glut/glut.h"
#endif

#include <gl/gl.h>
//NOTE: for gluScaleImage
#include <gl/glu.h>
//NOTE: for  GL_TEXTURE_RECTANGLE_ARB extension
#include "../External/gl/glext.h"

//Boost
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/function/function0.hpp>
#include <boost/function/function1.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/lambda/lambda.hpp>

#include "chcsva.h"

#include "../../shared/Interfaces/i_message.h"

#endif //__STDAFX_HPP__
