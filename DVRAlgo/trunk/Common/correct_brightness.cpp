///////////////////////////////////////////////////////////////////////////////
//  CorrectBrightness.cpp
//  ---------------------
//  begin     : Aug 2004
//  date      : 21 Jul 2005
//  author(s) : Alexander Boltnev, Albert Akhriev
//  email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvr_common.h"

namespace dvralgo
{

const int MIN_INTENSITY = 1;

const int GRAY_LEVEL = (1<<10);

#define CORRECT_BRIGHTNESS_MACRO(OFFSET) \
  dvralgo::RichPixel & p = res[(OFFSET)]; \
  p.y = (GRAY_LEVEL*(DIMDIM*(p.y)     )) / my; \
  p.u = (GRAY_LEVEL*(DIMDIM*(p.u) - mu)) / my; \
  p.v = (GRAY_LEVEL*(DIMDIM*(p.v) - mv)) / my; \
	p.windowAverage = ( my + DIMDIMDOWNSCALEDOWNSCALE_2 ) / DIMDIMDOWNSCALEDOWNSCALE;

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function corrects brightness scaling image intensity by local mean value. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CorrectBrightness( const dvralgo::RichPixelImage * pSource, int winDim, dvralgo::RichPixelImage * pResult, UByteArr * pTempBuffer )
{
  typedef float color_type;
  struct PixelSumType
  {
    color_type y, u, v;
    PixelSumType(): y(0), u(0), v(0){}
  };

  const int MAX_WINDOW_DIM = 128;
  const int DIM = 2 * (winDim / 2) + 1;  // do 'DIM' odd!
  const int DIMDIM = DIM * DIM;
	const int DIMDIMDOWNSCALEDOWNSCALE = DIMDIM * DOWNSCALEDOWNSCALE;
	const int DIMDIMDOWNSCALEDOWNSCALE_2 = DIMDIMDOWNSCALEDOWNSCALE / 2;
	const int HALF = DIM / 2;
  const int MIN_SCALED_INTENSITY = MIN_INTENSITY * DIMDIM * DOWNSCALE * DOWNSCALE;

  if ((pSource == 0) || (pResult == 0) || (pTempBuffer == 0))
    throw std::exception("bad input");
  if ((pSource->width() < DIM) || (pSource->height() < DIM) || (DIM > MAX_WINDOW_DIM))
    throw std::exception("bad input dimensions");

  pResult->resize2( *pSource, false );

  PixelSumType * scan[MAX_WINDOW_DIM];
  PixelSumType * sumScan = 0;
  const int W = pSource->width();
  const int H = pSource->height();
  const int W_1 = (W - 1);
  const int H_1 = (H - 1);
  const int DIM_1 = (DIM - 1);
  const int middle_offset = (HALF + W*HALF);

  // Initialize scan pointers that point inside temporary buffer.
  {
    int tempBufferSize = (int)( W * (DIM + 1) * sizeof(PixelSumType) + sizeof(dvralgo::RichPixelImage::value_type) * W );
    if ((int)(pTempBuffer->size()) < tempBufferSize)
      pTempBuffer->resize( tempBufferSize );
    PixelSumType* ptrBegin = reinterpret_cast<PixelSumType*>( &pTempBuffer->front() );
    for (int i = 0; i < DIM; i++)
    {
      scan[i] = ptrBegin + i * W;
    }
    sumScan = scan[DIM-1] + W;
  }

  // Fill up the scan of vertical sums with zeros.
  std::fill( sumScan, sumScan + W, PixelSumType() );
  
  //copy last row of image into temporary buffer
  dvralgo::RichPixelImage::value_type * lastStrBufBegin = reinterpret_cast<dvralgo::RichPixelImage::value_type *>(&( (*pTempBuffer)[ W * (DIM + 1) * sizeof(PixelSumType)] ) );
  std::copy(pSource->row_begin(H_1), pSource->row_end(H_1), lastStrBufBegin );

  // Compute means and do convertion.
  for (int y = 0; y < H; y++)
  {
    dvralgo::RichPixelImage::value_type * res = pResult->row_begin( y );
    const dvralgo::RichPixelImage::value_type * src = pSource->row_begin( y );
    PixelSumType * curScan = scan[ y % DIM ];
    if ( y == H_1 )
      src = lastStrBufBegin;
    PixelSumType * preScan = scan[(y + 1) % DIM];
    PixelSumType   horzSum;
    horzSum.y = 0;
    horzSum.u = 0;
    horzSum.v = 0;

    // Compute sums inside sliding window along the current scan, and also compute sums across horizontal scans.
    for (int x = 0; x < W; x++)
    {
      // Add advanced value to horizontal sum.
      horzSum.y += src[x].y;
      horzSum.u += src[x].u;
      horzSum.v += src[x].v;

      if (x >= DIM_1)
      {
        // Store new horizontal sums in the current scanline and add them to the vertical sums.
        curScan[x].y = horzSum.y;
        curScan[x].u = horzSum.u;
        curScan[x].v = horzSum.v;

        sumScan[x].y += horzSum.y;
        sumScan[x].u += horzSum.u;
        sumScan[x].v += horzSum.v;

        // Subtract the oldest value from the horizontal sum.
        horzSum.y -= src[x - DIM_1].y;
        horzSum.u -= src[x - DIM_1].u;
        horzSum.v -= src[x - DIM_1].v;

        if (y >= DIM_1)
        {
          // Obtain local mean value of the source image.
          color_type my = sumScan[x].y;
          color_type mu = sumScan[x].u;
          color_type mv = sumScan[x].v;

          // Subtract the oldest horizontal sum from the vertical sum.
          sumScan[x].y -= preScan[x].y;
          sumScan[x].u -= preScan[x].u;
          sumScan[x].v -= preScan[x].v;

          my = std::max<color_type>( my, (color_type)MIN_SCALED_INTENSITY );

          // Now 'my' is calculated. Modifying the image.			
          if (y == DIM_1)
          {
            for (int cnty = 0; cnty < (HALF+1); cnty++)
            {									
              if (x == DIM_1)
              {
                for (int cntx = 0; cntx < (HALF+1); cntx++)
                {
                  CORRECT_BRIGHTNESS_MACRO(x - middle_offset - cntx - cnty*W);
                }
              } 
              else if (x == W_1)
              {
                for (int cntx = 0; cntx < (HALF+1); cntx++)
                {
                  CORRECT_BRIGHTNESS_MACRO(x - middle_offset + cntx - cnty*W);
                }
              }			
              else
              {
                CORRECT_BRIGHTNESS_MACRO(x - middle_offset - cnty*W);
              }
            }// for (int cnty = 1; cnty < (HALF+1); cnty++)
          }		
          else if (y == H_1)
          {
            for (int cnty = 0; cnty < (HALF+1); cnty++)
            {
              if (x == W_1)
              {
                for (int cntx = 0; cntx < (HALF+1); cntx++)
                {
                  CORRECT_BRIGHTNESS_MACRO(x - middle_offset + cntx + cnty*W);
                }
              }
              else if (x == DIM_1)
              {
                for (int cntx = 0; cntx < (HALF+1); cntx++)
                {
                  CORRECT_BRIGHTNESS_MACRO(x - middle_offset - cntx + cnty*W);	
                }
              }
              else
              {
                CORRECT_BRIGHTNESS_MACRO(x - middle_offset + cnty*W);
              }
            }
          } //	else if ( y == H_1)
          else
          {
            if (x == DIM_1)
            {
              for (int cntx = 0; cntx < (HALF+1); cntx++)
              {
                CORRECT_BRIGHTNESS_MACRO(x - middle_offset - cntx);
              }
            }
            else if (x == W_1)
            {
              for (int cntx = 0; cntx < (HALF+1); cntx++)
              {
                CORRECT_BRIGHTNESS_MACRO(x - middle_offset + cntx);
              }
            }
            else // inside inner rectangle 
            {
              CORRECT_BRIGHTNESS_MACRO(x  - middle_offset);
            }
          }// else
        }//	if (y >= DIM_1)
      } // if (x >= DIM_1)
    } // for (int x = 0; x < W; x++)
  } //for (int y = 0; y < H; y++)
}


void CorrectBrightness( dvralgo::RichPixelImage & image, const int DIM )
{
  dvralgo::RichPixelImage tmpImage = image;
  
  const int HALF = DIM / 2;
  const int DIMDIM = DIM * DIM;
  const int MIN_SCALED_INTENSITY = MIN_INTENSITY * DIMDIM * DOWNSCALE * DOWNSCALE;
  const int W = image.width();
  const int H = image.height();

  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      float my = 0;
      float mu = 0;
      float mv = 0;

      int u1 = x-HALF;  if (u1 < 0) u1 = 0; else if (u1 > W-DIM) u1 = W-DIM;
      int v1 = y-HALF;  if (v1 < 0) v1 = 0; else if (v1 > H-DIM) v1 = H-DIM;
      
      int u2 = u1 + DIM;
      int v2 = v1 + DIM;

      for (int v = v1; v < v2; v++)
      {
        for (int u = u1; u < u2; u++)
        {
          dvralgo::RichPixel & p = tmpImage( u, v );
          my += p.y;
          mu += p.u;
          mv += p.v;
        }
      }

      my = std::max<float>( my, (float)MIN_SCALED_INTENSITY );


      dvralgo::RichPixel & p = image( x, y );

      p.y = (GRAY_LEVEL*(DIMDIM*p.y     )) / my;
      p.u = (GRAY_LEVEL*(DIMDIM*p.u - mu)) / my;
      p.v = (GRAY_LEVEL*(DIMDIM*p.v - mv)) / my;
    }
  }

}


} // namespace dvralgo

