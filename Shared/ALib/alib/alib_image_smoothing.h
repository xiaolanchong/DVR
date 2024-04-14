///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_image_smoothing.h
// ---------------------
// begin     : 1998
// modified  : 15 Mar 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ALIB_IMAGE_SMOOTHING_H__
#define __ALIB_IMAGE_SMOOTHING_H__

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

namespace alib
{

///------------------------------------------------------------------------------------------------
/// \brief Function repeatedly smoothes color image by applying (1 2 1)
///        mask in horizontal and vertical directions.
///------------------------------------------------------------------------------------------------
template< class IMAGE >
void RepeatedlySmoothColorImage121( IMAGE & image, int nRepeat, std::vector<unsigned char> & buffer )
{
  //#define  _GET_C0_(x)  ((precise_type)((x).##y))
  //#define  _GET_C1_(x)  ((precise_type)((x).##u))
  //#define  _GET_C2_(x)  ((precise_type)((x).##v))

  //#define  _SET_C0_(x,c)  ((x).##y = (value_type)(c))
  //#define  _SET_C1_(x,c)  ((x).##u = (value_type)(c))
  //#define  _SET_C2_(x,c)  ((x).##v = (value_type)(c))

  typedef  typename IMAGE::value_type         pixel_type;
  typedef  typename pixel_type::precise_type  precise_type;
  typedef  typename pixel_type::value_type    value_type;

  ASSERT( std::numeric_limits<precise_type>::is_specialized );
  ASSERT( std::numeric_limits<value_type>::is_specialized );
  ASSERT( std::numeric_limits<precise_type>::is_integer == std::numeric_limits<value_type>::is_integer );
  ASSERT( std::numeric_limits<precise_type>::is_signed == std::numeric_limits<value_type>::is_signed );
  ASSERT( sizeof(precise_type) >= sizeof(value_type) );
  ASSERT( sizeof(unsigned char) == 1 );

  int W = (int)(image.width());
  int H = (int)(image.height());

  if ((W < 2) || (H < 2) || (nRepeat < 1))
    return;
  buffer.resize( 3*3*W*sizeof(precise_type) );

  // Repeat smoothing several times.
  for (; nRepeat > 0; nRepeat--)
  {
    pixel_type   * img = image.begin();
    pixel_type   * row = img;
    precise_type * pre = reinterpret_cast<precise_type*>( &(buffer.front()) );
    precise_type * cur = pre + 3*W;
    precise_type * nxt = cur + 3*W;

    // Read and smooth the first row.
    for (int x = 1; x < (W-1); x++)
    {
      pixel_type & p = row[x-1];  // previous (left) pixel
      pixel_type & c = row[x];    // current (central) pixel
      pixel_type & n = row[x+1];  // next (right) pixel
      int          i = 3*x;

      cur[i+0] = (pre[i+0] = p.GetC0() + c.GetC0() + c.GetC0() + n.GetC0());
      cur[i+1] = (pre[i+1] = p.GetC1() + c.GetC1() + c.GetC1() + n.GetC1());
      cur[i+2] = (pre[i+2] = p.GetC2() + c.GetC2() + c.GetC2() + n.GetC2());
    }

    // Set leftmost pixel.
    {
      pixel_type & c = row[0];  // current (central) pixel
      pixel_type & n = row[1];  // next (right) pixel

      cur[0] = (pre[0] = c.GetC0() + c.GetC0() + c.GetC0() + n.GetC0());
      cur[1] = (pre[1] = c.GetC1() + c.GetC1() + c.GetC1() + n.GetC1());
      cur[2] = (pre[2] = c.GetC2() + c.GetC2() + c.GetC2() + n.GetC2());
    }

    // Set rightmost pixel.
    {
      pixel_type & p = row[W-2];  // previous (left) pixel
      pixel_type & c = row[W-1];  // current (central) pixel
      int          i = 3*(W-1);

      cur[i+0] = (pre[i+0] = p.GetC0() + c.GetC0() + c.GetC0() + c.GetC0());
      cur[i+1] = (pre[i+1] = p.GetC1() + c.GetC1() + c.GetC1() + c.GetC1());
      cur[i+2] = (pre[i+2] = p.GetC2() + c.GetC2() + c.GetC2() + c.GetC2());
    }

    // Go along the image row by row...
    for (int y = 0; y < H; y++)
    {
      // Read and smooth the next row.
      if ((y+1) < H)
      {
        row = (img+W);
        for (int x = 1; x < (W-1); x++)
        {
          pixel_type & pixel = img[x];
          pixel_type & p = row[x-1];  // previous (left) pixel
          pixel_type & c = row[x];    // current (central) pixel
          pixel_type & n = row[x+1];  // next (right) pixel
          int          i = 3*x;
          precise_type color[3];

          nxt[i+0] = p.GetC0() + c.GetC0() + c.GetC0() + n.GetC0();
          nxt[i+1] = p.GetC1() + c.GetC1() + c.GetC1() + n.GetC1();
          nxt[i+2] = p.GetC2() + c.GetC2() + c.GetC2() + n.GetC2();

          for (int q = 0; q < 3; q++)
          {
            color[q] = pre[i+q] + cur[i+q] + cur[i+q] + nxt[i+q];

            if (std::numeric_limits<value_type>::is_integer)
            {
              if (std::numeric_limits<value_type>::is_signed)
                color[q] = (precise_type)((color[q] + ((color[q] >= 0) ? 8 : -8)) / 16);
              else
                color[q] = (precise_type)((color[q] + 8) / 16);
            }
            else color[q] = (precise_type)(color[q] * (1.0f/16.0f));
          }

          pixel.SetC0( (value_type)(color[0]) );
          pixel.SetC1( (value_type)(color[1]) );
          pixel.SetC2( (value_type)(color[2]) );
        }

        // Set leftmost pixel.
        {
          pixel_type & pixel = img[0];
          pixel_type & c = row[0];  // current (central) pixel
          pixel_type & n = row[1];  // next (right) pixel
          precise_type color[3];

          nxt[0] = c.GetC0() + c.GetC0() + c.GetC0() + n.GetC0();
          nxt[1] = c.GetC1() + c.GetC1() + c.GetC1() + n.GetC1();
          nxt[2] = c.GetC2() + c.GetC2() + c.GetC2() + n.GetC2();

          for (int q = 0; q < 3; q++)
          {
            color[q] = pre[q] + cur[q] + cur[q] + nxt[q];

            if (std::numeric_limits<value_type>::is_integer)
            {
              if (std::numeric_limits<value_type>::is_signed)
                color[q] = (precise_type)((color[q] + ((color[q] >= 0) ? 8 : -8)) / 16);
              else
                color[q] = (precise_type)((color[q] + 8) / 16);
            }
            else color[q] = (precise_type)(color[q] * (1.0f/16.0f));
          }

          pixel.SetC0( (value_type)(color[0]) );
          pixel.SetC1( (value_type)(color[1]) );
          pixel.SetC2( (value_type)(color[2]) );
        }

        // Set rightmost pixel.
        {
          pixel_type & pixel = img[W-1];
          pixel_type & p = row[W-2];  // previous (left) pixel
          pixel_type & c = row[W-1];  // current (central) pixel
          int          i = 3*(W-1);
          precise_type color[3];

          nxt[i+0] = p.GetC0() + c.GetC0() + c.GetC0() + c.GetC0();
          nxt[i+1] = p.GetC1() + c.GetC1() + c.GetC1() + c.GetC1();
          nxt[i+2] = p.GetC2() + c.GetC2() + c.GetC2() + c.GetC2();

          for (int q = 0; q < 3; q++)
          {
            color[q] = pre[i+q] + cur[i+q] + cur[i+q] + nxt[i+q];

            if (std::numeric_limits<value_type>::is_integer)
            {
              if (std::numeric_limits<value_type>::is_signed)
                color[q] = (precise_type)((color[q] + ((color[q] >= 0) ? 8 : -8)) / 16);
              else
                color[q] = (precise_type)((color[q] + 8) / 16);
            }
            else color[q] = (precise_type)(color[q] * (1.0f/16.0f));
          }

          pixel.SetC0( (value_type)(color[0]) );
          pixel.SetC1( (value_type)(color[1]) );
          pixel.SetC2( (value_type)(color[2]) );
        }
      }
      else nxt = cur;

      // Cyclically swap scan pointers.
      precise_type * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }

  //#undef  _GET_C0_
  //#undef  _GET_C1_
  //#undef  _GET_C2_

  //#undef  _SET_C0_
  //#undef  _SET_C1_
  //#undef  _SET_C2_
}


#ifdef _DEBUG
template< class IMAGE >
bool TestRepeatedlySmoothColorImage121( IMAGE & image )
{
  typedef  typename IMAGE::value_type         pixel_type;
  typedef  typename pixel_type::precise_type  precise_type;
  typedef  typename pixel_type::value_type    value_type;

  ASSERT( std::numeric_limits<precise_type>::is_specialized );
  ASSERT( std::numeric_limits<value_type>::is_specialized );
  ASSERT( std::numeric_limits<precise_type>::is_integer == std::numeric_limits<value_type>::is_integer );
  ASSERT( std::numeric_limits<precise_type>::is_signed == std::numeric_limits<value_type>::is_signed );
  ASSERT( sizeof(precise_type) >= sizeof(value_type) );
  ASSERT( sizeof(unsigned char) == 1 );

  int                        W = (int)(image.width());
  int                        H = (int)(image.height());
  IMAGE                      temp = image;
  std::vector<unsigned char> buffer;
  
  const precise_type mask[9] = { 1, 2, 1,
                                 2, 4, 2,
                                 1, 2, 1 };

  const int xoff[9] = { -1, 0, 1,
                        -1, 0, 1,
                        -1, 0, 1 };

  const int yoff[9] = { -1, -1, -1,
                         0,  0,  0,
                         1,  1,  1 };

  if ((W < 2) || (H < 2))
    return false;

  // Repeat smoothing several times.
  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      precise_type sum[3] = {0,0,0};
      value_type   dst[3] = {0,0,0};

      for (int q = 0; q < 9; q++)
      {
        int u = ALIB_LIMIT( x+xoff[q], 0, W-1 );
        int v = ALIB_LIMIT( y+yoff[q], 0, H-1 );
        pixel_type & src = temp( u, v );
        sum[0] += (precise_type)(src.GetC0()) * mask[q];
        sum[1] += (precise_type)(src.GetC1()) * mask[q];
        sum[2] += (precise_type)(src.GetC2()) * mask[q];
      }

      for (int k = 0; k < 3; k++)
      {
        if (std::numeric_limits<value_type>::is_integer)
        {
          if (std::numeric_limits<value_type>::is_signed)
          {
            dst[k] = (value_type)((sum[k] + ((sum[k] >= 0) ? 8 : -8)) / 16);
          }
          else dst[k] = (value_type)((sum[k] + 8) >> 4);
        }
        else dst[k] = (value_type)(sum[k] * (1.0f/16.0f));
      }

      pixel_type & p = image( x, y );
      p.SetC0( dst[0] );
      p.SetC1( dst[1] );
      p.SetC2( dst[2] );
    }
  }

  // Smooth temporal image and compare results.
  RepeatedlySmoothColorImage121( temp, 1, buffer );
  int failCount = 0;
  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      pixel_type & p = image( x, y );
      pixel_type & q = temp( x, y );
      if ((p.GetC0() != q.GetC0()) ||
          (p.GetC1() != q.GetC1()) ||
          (p.GetC2() != q.GetC2()))
        ++failCount;
    }
  }
  std::cout << "fail count = " << failCount << std::endl;
  return (failCount == 0);
}
#endif // _DEBUG

} // namespace alib

#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif

#endif // __ALIB_IMAGE_SMOOTHING_H__



