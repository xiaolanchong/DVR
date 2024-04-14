///////////////////////////////////////////////////////////////////////////////////////////////////
// motion_region.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_MOTION_REGION_H__
#define __DVR_MOTION_REGION_H__

namespace dvralgo
{

///================================================================================================
/// \brief Region where motion was detected.
///================================================================================================
struct MotionRegion
{
  ARect rect;   //!<
  ulong time;   //!<
  uint  area;   //!< 

  ///----------------------------------------------------------------------------------------------
  /// \brief Constructor.
  ///----------------------------------------------------------------------------------------------
  MotionRegion() : rect(), time(0), area(0)
  {
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Constructor.
  ///----------------------------------------------------------------------------------------------
  MotionRegion( sint x1, sint y1, sint x2, sint y2, ulong timeStamp ) : rect( x1, y1, x2, y2 )
  {
    time = timeStamp;
    area = rect.area();
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Copy operator.
  ///----------------------------------------------------------------------------------------------
  MotionRegion & operator = ( const MotionRegion & rhs )
  {
    rect = rhs.rect;
    time = rhs.time;
    area = rhs.area;
    return (*this);
  }

};

inline bool operator > ( const MotionRegion & lhs, const MotionRegion & rhs )
{
  return (lhs.area > rhs.area);
}

inline bool operator < ( const MotionRegion & lhs, const MotionRegion & rhs )
{
  return (lhs.area < rhs.area);
}

struct MotionRegionTimeLessPredicate
	: std::binary_function <MotionRegion, MotionRegion, bool> 
{
	bool operator ()  ( const MotionRegion & lhs, const MotionRegion & rhs ) const
	{
		return ( lhs.time < rhs.time );
	}
};


typedef  std::vector<MotionRegion>  MotionRegionArr;

} // namespace dvralgo

#endif // __DVR_MOTION_REGION_H__

