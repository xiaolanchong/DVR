///////////////////////////////////////////////////////////////////////////////////////////////////
// utility.h
// ---------------------
// begin     : 2006
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_UTILITY_H__
#define __DEFINE_ELVEES_UTILITY_H__

#include "../Common/elvees.h"
#include "alib/alib.h"
#include "functional.h"
typedef  Arr2D<RGBQUAD,RGBQUAD,true>  QImage;
typedef  Arr2D<RGBQUAD,RGBQUAD,true>  AImage;
#include "ini_file_parser.h"
#include "demo/i_console_painter.h"

#ifndef  ALIB_ASSERT
#define  ALIB_ASSERT  ELVEES_ASSERT
#endif
/*
namespace Elvees
{

void CopyYUY2ToGrayImage( const BITMAPINFOHEADER * pHeader, const ubyte * pBytes,
                          Arr2ub * pImage, bool bInvertY );

void CopyYUY2ToRGBImage( const BITMAPINFOHEADER * pHeader, const ubyte * pBytes,
                         QImage * pImage, bool bInvertY );

} // namespace Elvees
*/
#endif // __DEFINE_ELVEES_UTILITY_H__

