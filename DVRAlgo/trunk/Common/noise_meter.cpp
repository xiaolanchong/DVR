///////////////////////////////////////////////////////////////////////////////////////////////////
// noise_meter.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 15 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvr_common.h"

namespace dvralgo
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function estimates noise deviation using one polynomial mask method.
///
/// \param  histogram   the noise histogram stores result of convolution with a polynomial mask.
/// \param  HISTO_SIZE  the size of histogram.
/// \param  SCALE       the multiplier used during histogram accumulation (=2 for 2x2 mask).
/// \return             estimated noise deviation.
///////////////////////////////////////////////////////////////////////////////////////////////////
float ProcessOnePolymaskNoiseHistogram( sint * histogram, const sint HISTO_SIZE, const float SCALE )
{
  const float MIN_NOISE_DEVIATION = 0.5;
  double      dev = 0.0, roughDev = 0.0;
  sint         histoSize = 0;

  ASSERT( histogram != 0 );

  // Compute the actual histogram's size and correct the first entry.
  {
    sint max = 0;

    for (sint i = 0; i < HISTO_SIZE; i++)
    {
      if (histogram[i] > 0)
        histoSize = i;
      if (histogram[i] > max)
        max = histogram[i];
    }
    histoSize = std::min( histoSize+1, HISTO_SIZE );

    // Sometimes, an image coded in block-based formats (MPEG, JPEG, YUV) has histogram with abnormal
    // value in the first entry. We fix situation by: histogram[0] = (histogram[1]+histogram[2]+1)/2.
    // But when histogram[0] is too large then the image consists of uniformly-colored regions (like
    // animated cartoons). In the latter case we do nothing.
    if (histogram[0] < 10*max)                               // !???????
      histogram[0] = (histogram[1]+histogram[2]+1)/2;
  }

  // Obtain initial estimation of noise deviation.
  {
    double I1 = 0.0;
    double I2 = 0.0;

    for (sint i = 0; i < histoSize; i++)
    {
      double h = histogram[i];

      I1 += h;
      I2 += h*h;
    }
    dev = (I2 > FLT_EPSILON) ? ((I1*I1)/(I2*sqrt( ALIB_PI ))) : 0.0;
    dev = (roughDev = std::max( dev, (double)(SCALE*MIN_NOISE_DEVIATION) ));
  }

  // Iteratively improve estimation using preconditioner.
  if (dev > ((SCALE+0.1)*MIN_NOISE_DEVIATION))
  {
    bool ok = false;
    sint  N = std::min( histoSize, (sint)(4.0*dev+1.0) );

    for (sint iter = 0; iter < 3; iter++)
    {
      double p = -0.5/(dev*dev), a = (0.5*histogram[0]), m = 0.0;

      for (sint i = 1; i < N; i++)
      {
        double t = exp( p*i*i ) * histogram[i];

        m += i*t;
        a += t;
      }

      ok = false;
      if (a > FLT_EPSILON)
      {
        m /= a;
        p = (2.0*dev*dev - ALIB_PI*m*m);
        if (p > FLT_EPSILON)
        {
          dev = dev * m * sqrt( ALIB_PI / p );
          ok = true;
        }
      }
    }

    // On failure use simplified estimation.
    if (!ok || (dev < (SCALE*MIN_NOISE_DEVIATION)))
      dev = roughDev;
  }

  return std::max( (float)(dev/SCALE), (float)MIN_NOISE_DEVIATION );
}

} // namespace dvralgo

