///////////////////////////////////////////////////////////////////////////////////////////////////
//  alib.h - the main header file of my library.
// ---------------------
// begin     : 1998
// modified  : 16 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define  ALIB_VERSION  "2.0.0"

///////////////////////////////////////////////////////////////////////////////////////////////////
// WIN32
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32

  #pragma message( "Alik's Library, version: " ALIB_VERSION )

  #ifdef _DEBUG
    #define ALIB_CHECK_MEMORY_LEAKAGE
    #pragma message( "Memory leakage detection is enabled" )
  #endif // _DEBUG

#endif // WIN32

///////////////////////////////////////////////////////////////////////////////////////////////////
// System inclusions.
///////////////////////////////////////////////////////////////////////////////////////////////////

#if (defined(_WIN32) || defined(_WIN64))
#include <conio.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif // _WIN32 || _WIN64

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// STL and BOOST inclusions.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "alib_stl.h"
#if 0
#include <boost/cstdint.hpp>
#else
#include <cstdint>
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
// Alib inclusions.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "alib_integers.h"
#include "alib_types.h"
#include "alib_constants.h"
#include "alib_debug_macros.h"
#include "alib_base2d.h"
#include "alib_rect.h"
#include "alib_numeric.h"
#include "alib_textual.h"
#include "alib_clear.h"
#include "alib_compare.h"
#include "alib_util.h"
#include "alib_load_save.h"
#include "alib_fourcc.h"
#include "alib_macros.h"
#include "alib_color.h"
#include "alib_file.h"
#include "alib_statistics.h"
#include "alib_parameter.h"
#include "alib_vec2d.h"
#include "alib_go_along_line.h"
#include "alib_allocator.h"
#include "alib_image_smoothing.h"

#include "vl_curve2d_functions.h"

/*
#include "alib_macros.h"
#include "alib_types_const.h"
#include "alib_exception.h"
#include "alib_message.h"
#include "alib_allocator.h"
#include "alib_rand_inc_iter.h"
#include "alib_vec2d.h"
#include "alib_rect.h"
#include "alib_arr3d.h"
#include "alib_b_rgb.h"
#include "alib_f_rgb.h"
#include "alib_i_rgb.h"
#include "alib_small_sort.h"

#include "alib_file.h"
#include "alib_img_smooth.h"
#include "alib_img_lib.h"

#include "alib_util.h"
#include "alib_algo.h"
#include "alib_sort.h"
#include "alib_go_along_line.h"
#include "alib_timer.h"
#include "alib_vicinity.h"
#include "alib_h_scan.h"
#include "alib_functions.h"
#include "alib_filter1d.h"
#include "alib_graph.h"
#include "alib_graph_ex.h"
#include "alib_graph_copy.h"
#include "alib_spline.h"
#include "alib_alg_eq.h"
#include "alib_random.h"
#include "alib_result_html.h"
#include "alib_histogram.h"
#include "alib_statistics.h"
#include "alib_uint_color_accum.h"
#include "alib_kdim_tree.h"
#include "alib_partition_tree2d.h"
#include "alib_part_tree3d.h"

typedef  std::vector<Pnt2s>  Pnt2sArr;
typedef  std::vector<Pnt2i>  Pnt2iArr;
typedef  std::vector<Pnt2f>  Pnt2fArr;
typedef  std::vector<Pnt2d>  Pnt2dArr;

typedef  std::list<Pnt2s>    Pnt2sLst;
typedef  std::list<Pnt2i>    Pnt2iLst;
typedef  std::list<Pnt2f>    Pnt2fLst;
typedef  std::list<Pnt2d>    Pnt2dLst;

typedef  Arr2D<Vec2i, Vec2i, true>  Vec2iImg;
typedef  Arr2D<Vec2f, Vec2f, true>  Vec2fImg;
typedef  Arr2D<Vec2d, Vec2d, true>  Vec2dImg;
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations of global library functions.
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace alib
{

/*
// alib_dir.cpp
void         RemoveTrailingBackSlash( std::string & dir );
bool         IsDirectoryExist( const char * dir );
void         MakeDirectory( const char * dir );
std::string  SetCurrentDirectory( const char * dir );

// alib_io_image.cpp
bool LoadImage( const char * fname, Arr2f & image, bool bInvertY = ALIB_INVERT_Y );
bool LoadImage( const char * fname, Arr2ub & image, bool bInvertY = ALIB_INVERT_Y );
bool LoadImage( const char * fname, AImage & image, bool * bColor = 0, bool bInvertY = ALIB_INVERT_Y );
bool LoadImage( const char * fname, FImage & image, bool bInvertY = ALIB_INVERT_Y );
bool SaveImage( const char * fname, const AImage & image, bool bInvertY = ALIB_INVERT_Y );
bool SaveImage( const char * fname, const Arr2ub & image, const bRGBArr * pPalette = 0, bool bInvertY = ALIB_INVERT_Y );
bool SaveImage( const char * fname, const FImage & image, bool bInvertY = ALIB_INVERT_Y );
*/

} // namespace alib

