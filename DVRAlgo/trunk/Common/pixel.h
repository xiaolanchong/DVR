///////////////////////////////////////////////////////////////////////////////////////////////////
// pixel.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_PIXEL_H__
#define __DVR_PIXEL_H__

namespace dvralgo
{

///================================================================================================
/// \struct YUVF.
/// \brief  YUVF.
///================================================================================================
struct YUVF
{
  typedef  float   value_type;
  typedef  double  precise_type;
  typedef  uint    flag_type;

  value_type y;    //!< Y component
  value_type u;    //!< U component
  value_type v;    //!< V component
  flag_type  f;    //!< usually reserved for flags

  value_type GetC0() const { return y; }
  value_type GetC1() const { return u; }
  value_type GetC2() const { return v; }

  void SetC0( value_type value ) { y = value; }
  void SetC1( value_type value ) { u = value; }
  void SetC2( value_type value ) { v = value; }
};


const YUVF::flag_type HIDDEN_POINT      = (1<<0); //!< the flag of masked point
const YUVF::flag_type NORM_MOTION_POINT = (1<<1); //!< normal motion was detected
const YUVF::flag_type WEAK_MOTION_POINT = (1<<2); //!< weak motion was detected
const YUVF::flag_type PASSED_POINT      = (1<<3); //!< point has been passed during region extraction

// All motion flags taken together.
const YUVF::flag_type MOTION_FLAGS = (NORM_MOTION_POINT | WEAK_MOTION_POINT);

// One must preserve these flags when obtain a new frame.
const YUVF::flag_type STATE_FLAGS = (HIDDEN_POINT | NORM_MOTION_POINT);


///================================================================================================
/// \struct RichPixel.
/// \brief  RichPixel.
///================================================================================================
struct RichPixel : public YUVF
{
  float difference;     //!< difference between the background and the current frame
  float background[3];  //!< accumulated background of Y,U and V components
  float threshold[3];   //!< thresholds on Y, U and V components
  float old[3];         //!< previous Y, U and V values
	float windowAverage;
	float naturalBackground; //!< non normalized background
  uint  time;           //!< 
};

typedef  Arr2D<RichPixel,RichPixel,true>  RichPixelImage;

inline bool operator == (const RichPixel & p1, const RichPixel & p2 )
{
  return ((p1.y    == p2.y) && 
          (p1.u    == p2.u) &&
          (p1.v    == p2.v) &&
          (p1.f    == p2.f) &&
          (p1.time == p2.time));
}

} // namespace dvralgo

#endif // __DVR_PIXEL_H__

