///////////////////////////////////////////////////////////////////////////////////////////////////
// video_conversion.cpp
// ---------------------
// begin     : 2006
// modified  : 14 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvr_common.h"

namespace dvralgo
{

///------------------------------------------------------------------------------------------------
/// \brief Function copies an image in YUY2 format to grayscaled one.
///
/// \param  pHeader   pointer to the source image header.
/// \param  pBytes    pointer to the source image content.
/// \param  pImage    pointer to destination image.
/// \param  bInvertY  inverts axis Y if nonzero.
/// \param  bColor16  reserve 16 first intensity levels for system colors if nonzero.
///------------------------------------------------------------------------------------------------
void CopyYUY2ToGrayImage( const BITMAPINFOHEADER * pHeader,
                          const ubyte            * pBytes,
                          Arr2ub                 * pImage,
                          bool                     bInvertY,
                          bool                     bColor16 )
{
  ELVEES_ASSERT( (pHeader != 0) && (pBytes != 0) && (pImage != 0) );
  ELVEES_ASSERT( (sint)(pHeader->biCompression) == alib::MakeFourCC('Y','U','Y','2') );
  ELVEES_ASSERT( pHeader->biBitCount == 2*8 );

  int W = pHeader->biWidth;
  int H = pHeader->biHeight;
  int scanW = 2*W;

  if ((pImage->width() != W) || (pImage->height() != H))
  {
    pImage->resize( W, H, Arr2ub::value_type(), false );
    pImage->fast_zero();
  }

  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pBytes + (bInvertY ? (H-1-y) : y) * scanW;
    ubyte       * dst = pImage->row_begin( y );

    if (bColor16)
    {
      for (int x = 0; x < W; x++)
        dst[x] = std::max( src[x+x], (ubyte)16 );
    }
    else
    {
      for (int x = 0; x < W; x++)
        dst[x] = src[x+x];
    }
  }
}


///------------------------------------------------------------------------------------------------
/// \brief Function copies an image in YUY2 format to RGB one.
///
/// <pre><tt>
/// Convertion used (http://www.fourcc.org/fccyvrgb.php):
/// B = 1.164(Y - 16)                  + 2.018(U - 128)
/// G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
/// R = 1.164(Y - 16) + 1.596(V - 128)
/// </tt></pre>
///
/// \param  pHeader   pointer to the source image header.
/// \param  pBytes    pointer to the source image content.
/// \param  pImage    pointer to destination image.
/// \param  bInvertY  inverts axis Y if nonzero.
///------------------------------------------------------------------------------------------------
void CopyYUY2ToRGBImage( const BITMAPINFOHEADER * pHeader,
                         const ubyte            * pBytes,
                         QImage                 * pImage,
                         bool                     bInvertY )
{
  ELVEES_ASSERT( (pHeader != 0) && (pBytes != 0) && (pImage != 0) );
  ELVEES_ASSERT( (sint)(pHeader->biCompression) == alib::MakeFourCC('Y','U','Y','2') );
  ELVEES_ASSERT( pHeader->biBitCount == 2*8 );

  int W = pHeader->biWidth;
  int H = pHeader->biHeight;
  int scanW = 2*W;

  if ((pImage->width() != W) || (pImage->height() != H))
  {
    pImage->resize( W, H, QImage::value_type(), false );
    pImage->fast_zero();
  }

  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pBytes + (bInvertY ? (H-1-y) : y) * scanW;
    RGBQUAD     * dst = pImage->row_begin( y );

    for (int x = 0, i = 0; x < W; x+=2, i+=4)
    {
      RGBQUAD * c = dst + x;
      sint      y0 = (sint)(src[i+0]);
      sint      u  = (sint)(src[i+1]);
      sint      y1 = (sint)(src[i+2]);
      sint      v  = (sint)(src[i+3]);
      sint      R, G, B;

      y0 -= 16;
      y1 -= 16;
      u -= 128;
      v -= 128;

      B = ((1220542*y0             + 2116026*u + (1<<19)) >> 20);
      G = ((1220542*y0 -  852492*v -  409993*u + (1<<19)) >> 20);
      R = ((1220542*y0 + 1673527*v             + (1<<19)) >> 20);

      c->rgbBlue  = (ubyte)ALIB_LIMIT( B, 0, 0x0FF );
      c->rgbGreen = (ubyte)ALIB_LIMIT( G, 0, 0x0FF );
      c->rgbRed   = (ubyte)ALIB_LIMIT( R, 0, 0x0FF );

      ++c;

      B = ((1220542*y1             + 2116026*u + (1<<19)) >> 20);
      G = ((1220542*y1 -  852492*v -  409993*u + (1<<19)) >> 20);
      R = ((1220542*y1 + 1673527*v             + (1<<19)) >> 20);

      c->rgbBlue  = (ubyte)ALIB_LIMIT( B, 0, 0x0FF );
      c->rgbGreen = (ubyte)ALIB_LIMIT( G, 0, 0x0FF );
      c->rgbRed   = (ubyte)ALIB_LIMIT( R, 0, 0x0FF );
    }
  }
}


///------------------------------------------------------------------------------------------------
/// \brief  Function copies the source YUY2 bitmap into the destination image of smaller size
///         using the global constant dvralgo::DOWNSCALE.
///
/// \param  pHeader   pointer to the source image header.
/// \param  pBytes    pointer to the source image content.
/// \param  pImage    pointer to destination image.
/// \param  bInvertY  inverts axis Y if nonzero.
///------------------------------------------------------------------------------------------------
void ShrinkYUY2Image( const BITMAPINFOHEADER * pHeader,
                      const ubyte            * pBytes,
                      RichPixelImage         * pImage,
                      bool                     bInvertY )
{
  const int BPP = 2;                                             // bytes per pixel
  const int SCALE = dvralgo::DOWNSCALE;

  ELVEES_ASSERT( (pHeader != 0) && (pBytes != 0) && (pImage != 0) );
  ELVEES_ASSERT( (sint)(pHeader->biCompression) == alib::MakeFourCC('Y','U','Y','2') );
  ELVEES_ASSERT( pHeader->biBitCount == BPP*8 );
  ELVEES_ASSERT( (SCALE == 2) || (SCALE == 4) ); // || (SCALE == 8) || (SCALE == 16) );

  int           W = pHeader->biWidth / SCALE;
  int           H = pHeader->biHeight / SCALE;
  int           scanW = BPP * pHeader->biWidth;
  const ubyte * source[SCALE];

  if ((pImage->width() != W) || (pImage->height() != H))
  {
    pImage->resize( W, H );
    pImage->fast_zero();
  }

  for (int y = 0; y < H; y++)
  {
    dvralgo::RichPixel * destinationBegin = pImage->row_begin( y );
    dvralgo::RichPixel * destinationEnd = pImage->row_end( y );
    dvralgo::RichPixel * destination = destinationBegin;

    for (int i = 0; i < SCALE; i++)
    {
      source[i] = pBytes + (bInvertY ? (pHeader->biHeight-1-(y*SCALE+i)) : (y*SCALE+i)) * scanW;
    }

    for (; destination < destinationEnd; ++destination)
    {
      uint ysum = 0;
      uint usum = 0;
      uint vsum = 0;

      for (int i = 0; i < SCALE; i++)
      {
        for (int k = 0; k < (SCALE/2); k++)
        {
          ysum += (uint)(source[i][0]);
          usum += (uint)(source[i][1]);
          ysum += (uint)(source[i][2]);
          vsum += (uint)(source[i][3]);
          source[i] += 2*BPP;               // move forward
        }
      }

      destination->y = (RichPixel::value_type)ysum; 
      destination->u = (RichPixel::value_type)usum;
      destination->v = (RichPixel::value_type)vsum;
      destination->f = (destination->f & STATE_FLAGS);
    }
  }

  // Check correctness.
#if 0
  if (((pHeader->biWidth * BPP) & 0x03) == 0) // has 4-byte alignment?
  {
    Arr2uw source;
    int    W = pHeader->biWidth;
    int    H = pHeader->biHeight;
    void * p = source.wrap( W, H, (void*)pBytes, (void*)(pBytes+W*H*BPP), Arr2uw::value_type(), false );

    ELVEES_ASSERT( p != 0 );
    for (int y = 0; y < H; y += SCALE)
    {
      for (int x = 0; x < W; x += SCALE)
      {
        sint ysum = 0;
        sint usum = 0;
        sint vsum = 0;
        for (int v = 0; v < SCALE; v++)
        {
          for (int u = 0; u < SCALE; u+=2)
          {
            uword yu = source( x+u+0, (bInvertY ? (H-1-(y+v)) : (y+v)) );
            uword yv = source( x+u+1, (bInvertY ? (H-1-(y+v)) : (y+v)) );

            sint y0 = (sint)(yu & 255); 
            sint uu = (sint)(yu >> 8);  
            sint y1 = (sint)(yv & 255); 
            sint vv = (sint)(yv >> 8);

            ysum += y0;
            ysum += y1;
            usum += uu;
            vsum += vv;
          }
        }
        dvralgo::YUVF c = (*pImage)( x/SCALE, y/SCALE );
        ELVEES_ASSERT( c.y == (ubyte)((ysum + (SUM_SCALE/2)) / (SUM_SCALE/1)) );
        ELVEES_ASSERT( c.u == (ubyte)((usum + (SUM_SCALE/4)) / (SUM_SCALE/2)) );
        ELVEES_ASSERT( c.v == (ubyte)((vsum + (SUM_SCALE/4)) / (SUM_SCALE/2)) );
      }
    }
  }
#endif
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
void CopyRichPixelImageToGreyFloatImage( const RichPixelImage & src, Arr2f & dst, int layer )
{
  dst.resize( src.width(), src.height() );
  RichPixelImage::const_iterator iSrc = src.begin();
  Arr2f::iterator iDst = dst.begin();
  switch (layer)
  {
    case 0:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->y; } break;
    case 1:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->u; } break;
    case 2:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->v; } break;
    case 3:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->background[0]; } break;
    case 4:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->threshold[0]; } break;
		case 5:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->naturalBackground; } break;
		case 6:  for (; iSrc != src.end(); ++iSrc) { (*iDst++) = iSrc->windowAverage; } break;

    default: for (; iSrc != src.end(); ++iSrc) { (*iDst++) = 0; } break;
  }
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
std::pair<float,float> CopyFloatImageToByteImage( const Arr2f & source,
                                                  Arr2ub      & result,
                                                  int           scale,
                                                  bool          bColor16 )
{
  const double         MULTIPLIER = 2.0;
  const int            MIN_VALUE = (bColor16 ? 16 : 0);
  MeanVarAccum<double> accum;
  double               deviation = 0.0, mean = 0.0, multiplier = 0.0;

  // Compute intensity scale factor.
  for (int i = 0, n = source.size(); i < n; i++) { accum += source[i]; }
  accum.statistics( &mean, 0, &deviation );
  multiplier = (deviation > FLT_EPSILON) ? (127.0/(MULTIPLIER*deviation)) : 0.0;

  // Adjust image entries.
  scale = alib::Limit( scale, 1, 100 );
  if (scale == 1)
  {
    result.resize2( source, false );
    for (int i = 0, n = source.size(); i < n; i++)
    {
      result[i] = (ubyte)(alib::Limit( alib::Round( (source[i]-mean)*multiplier+127.0 ), MIN_VALUE, 255 ));
    }
  }
  else
  {
    int W = source.width();
    int H = source.height();

    result.resize( W*scale, H*scale, 0, false );
    for (int y = 0; y < H; y++)
    {
      const float * src = source.row_begin( y );
      ubyte       * res = result.row_begin( y*scale );

      for (int x = 0, k = 0; x < W; x++, k += scale)
      {
        ubyte t = (ubyte)(alib::Limit( alib::Round( (src[x]-mean)*multiplier+127.0 ), MIN_VALUE, 255 ));
        std::fill( res+k, res+(k+scale), t );
      }

      for (int i = 1; i < scale; i++)
        std::copy( res, res+(scale*W), res+(i*scale*W) );
    }
  }
  return std::make_pair( mean, deviation );
}

} // namespace dvralgo

