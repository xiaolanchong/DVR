///////////////////////////////////////////////////////////////////////////////////////////////////
// dvr_common.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_ALGO_COMMON_H__
#define __DVR_ALGO_COMMON_H__

#include "../../../Shared/Common/elvees.h"
#include "../../../Shared/ALib/utility.h"
#include "../Grabber/grabber.h"
#include "milli_timer.h"
#include "parameter_functions.h"
#include "pixel.h"
#include "exportable_functions.h"
#include "events.h"
#include "motion_region.h"
#include "transferable_types.h"

namespace dvralgo
{

// Direction of Y-axis (nonzero means downward direction).
const bool INVERT_AXIS_Y = true;

// The maximal number of simultaneously running camera threads.
const int MAX_CAMERA_NUMBER = 32;

// The coefficient of shrinkage of input image before processing (2 or 4).
const int DOWNSCALE = 4;

const int DOWNSCALEDOWNSCALE = DOWNSCALE * DOWNSCALE;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

// video_conversion.cpp:
void CopyYUY2ToGrayImage( const BITMAPINFOHEADER * pHeader,
                          const ubyte            * pBytes,
                          Arr2ub                 * pImage,
                          bool                     bInvertY,
                          bool                     bColor16 );

void CopyYUY2ToRGBImage( const BITMAPINFOHEADER * pHeader,
                         const ubyte            * pBytes,
                         QImage                 * pImage,
                         bool                     bInvertY );

void ShrinkYUY2Image( const BITMAPINFOHEADER * pHeader,
                      const ubyte            * pBytes,
                      RichPixelImage         * pImage,
                      bool                     bInvertY );


void CorrectBrightness( const dvralgo::RichPixelImage * pSource, 
												int winDim, dvralgo::RichPixelImage * pResult, 
												UByteArr * pTempBuffer );

void CorrectBrightness( dvralgo::RichPixelImage & image, const int winDim = 9 );

void CopyRichPixelImageToGreyFloatImage( const RichPixelImage & src, Arr2f & dst, int layer = 0 );

std::pair<float,float> CopyFloatImageToByteImage( const Arr2f & source, Arr2ub & result, int scale, bool bColor16 );

} // namespace dvralgo

#endif // __DVR_ALGO_COMMON_H__

