///////////////////////////////////////////////////////////////////////////////////////////////////
// region_extractor.cpp
// ---------------------
// begin     : 2006
// modified  : 26 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "region_extractor.h"
#include "mergeboxes.h"

namespace dvralgo
{

///------------------------------------------------------------------------------------------------
/// \brief Constructor.
///------------------------------------------------------------------------------------------------
RegionExtractor::RegionExtractor()
{
  boost::mutex::scoped_lock lock( m_dataLocker );
  m_curQueue.reserve( MAX_QUEUE_SIZE );
  m_curQueue.clear();
  m_totQueue.reserve( MAX_QUEUE_SIZE );
  m_totQueue.clear();
  m_dataLocker;
  m_pImg = 0;
  m_pOut = 0;
  m_boundRect.set_null();
  m_area = 0;
  m_scanPool.reserve(1<<16);
  m_scanPool.clear();
  m_regionPool.reserve(1<<10);
  m_regionPool.clear();
  m_bGood = true;
  m_bReady = true;
}


///------------------------------------------------------------------------------------------------
/// \brief Destructor.
///------------------------------------------------------------------------------------------------
RegionExtractor::~RegionExtractor()
{
  boost::mutex::scoped_lock lock( m_dataLocker );
  m_curQueue.clear();
  m_bReady = false;
}


///------------------------------------------------------------------------------------------------
/// \brief Function converts MotionRegion to MotionRect.
///------------------------------------------------------------------------------------------------
Elvees::ICameraMotionEvent::MotionRect RegionExtractor::GetMotionRect( const dvralgo::MotionRegion & rgn )
{
		int w = m_pImg->width();
		int h = m_pImg->height();
		Elvees::ICameraMotionEvent::MotionRect rct;
		rct.time = rgn.time / 1000;
		rct.x1 = (float)rgn.rect.x1 /	(float)w ;
		rct.y1 = (float)rgn.rect.y1 / (float)h;
		rct.x2 = (float)rgn.rect.x2 / (float)w;
		rct.y2 = (float)rgn.rect.y2 / (float)h;
		return rct;
}

///------------------------------------------------------------------------------------------------
/// \brief Function saves accumulated information into a binary buffer
///        and clears the queue of motion regions.
///
/// \param  data  destination binary data storage.
///------------------------------------------------------------------------------------------------
void RegionExtractor::SerializeMotionRegionsAndClearQueue( Elvees::ICameraMotionEvent::MotionRectArr & mra )
{
  boost::mutex::scoped_lock lock( m_dataLocker );
	if (!m_pImg) 
	{
		//Process was not called
		mra.clear();
		return;
	}

  // m_totQueue to be serialized
  // we will upscale motion rectanles back to normal size
  dvralgo::MotionRegionArr::iterator i = m_totQueue.begin();	
  mra.clear();
	while(i != m_totQueue.end())
	{
		mra.push_back( GetMotionRect( *i ) );
		i++;
  }
  m_curQueue.clear();
  m_totQueue.clear();
}


///------------------------------------------------------------------------------------------------
/// \brief Function recursively marks pixels of any uniform region by special flag (PASSED_POINT).
///
/// Function starts from any region's point and fills region line by line (lines are horizontal). 
///
/// \param  x         abscissa of start point
/// \param  y         ordinate of start point
/// \param  dir       direction of moving along Y-axis (-1 or +1)
/// \param  preLeft   X-coordinate of left end of previous segment
/// \param  preRight  X-coordinate of right end of previous segment
/// \return           the most right point of found segment in the current scan.
///------------------------------------------------------------------------------------------------
int RegionExtractor::LineFill( int x, int y, int dir, int preLeft, int preRight )
{
  ASSERT( m_pImg->inside( x, y ) && (abs( dir ) == 1) );

  int                     W = m_pImg->width();
  int                     H = m_pImg->height();
  int                     left, right;
  RichPixelImage::pointer row = m_pImg->row_begin( y );
  Scan                    scan;

  // Find and mark largest horizontal segment that contains start point (x,y).
#if 0

  for (left = x; (left >= 0) && ((row[left]).f == NORM_MOTION_POINT);)
  {
    (row[left--]).f |= PASSED_POINT;
  }
  ++left;

  for (right = x+1; (right < W) && ((row[right]).f == NORM_MOTION_POINT);)
  {
    (row[right++]).f |= PASSED_POINT;
  }
  --right;

  // Compute region's geometry.
  m_area += (right - left + 1);
  if (m_boundRect.x1 > left ) m_boundRect.x1 = left;
  if (m_boundRect.y1 > y    ) m_boundRect.y1 = y;
  if (m_boundRect.x2 < right) m_boundRect.x2 = right;
  if (m_boundRect.y2 < y    ) m_boundRect.y2 = y;
  scan.x1 = (sword)left;
  scan.x2 = (sword)right;
  scan.y  = y;
  m_scanPool.push_back( scan );

  // Find adjacent horizontal segments in the same direction.
  if (ALIB_IS_RANGE( y+dir, 0, H ))
  {
    row = m_pImg->row_begin( y+dir );
    for (x = left; x <= right; x++)
    {
      if ((row[x]).f == NORM_MOTION_POINT)
        x = LineFill( x, y+dir, dir, left, right );
    }
  }

  // Find adjacent horizontal segments in the opposite direction.
  if (ALIB_IS_RANGE( y-dir, 0, H ))
  {
    row = m_pImg->row_begin( y-dir );
    for (x = left; x < preLeft; x++)
    {
      if ((row[x]).f == NORM_MOTION_POINT)
        x = LineFill( x, y-dir, -dir, left, right );
    } 

    for (x = preRight+1; x <= right; x++)
    {
      if ((row[x]).f == NORM_MOTION_POINT)
        x = LineFill( x, y-dir, -dir, left, right );
    }
  }
  return right;

#else

  for (left = x; (left >= 0) && (((row[left]).f == NORM_MOTION_POINT) ||
                                 ((row[left]).f == WEAK_MOTION_POINT));)
  {
    (row[left--]).f |= PASSED_POINT;
  }
  ++left;

  for (right = x+1; (right < W) && (((row[right]).f == NORM_MOTION_POINT) ||
                                    ((row[right]).f == WEAK_MOTION_POINT));)
  {
    (row[right++]).f |= PASSED_POINT;
  }
  --right;

  // Compute region's geometry.
  m_area += (right - left + 1);
  if (m_boundRect.x1 > left ) m_boundRect.x1 = left;
  if (m_boundRect.y1 > y    ) m_boundRect.y1 = y;
  if (m_boundRect.x2 < right) m_boundRect.x2 = right;
  if (m_boundRect.y2 < y    ) m_boundRect.y2 = y;
  scan.x1 = (sword)left;
  scan.x2 = (sword)right;
  scan.y  = y;
  m_scanPool.push_back( scan );

  // Find adjacent horizontal segments in the same direction.
  if (ALIB_IS_RANGE( y+dir, 0, H ))
  {
    row = m_pImg->row_begin( y+dir );
    for (x = left; x <= right; x++)
    {
      if (((row[x]).f == NORM_MOTION_POINT) ||
          ((row[x]).f == WEAK_MOTION_POINT))
      {
        x = LineFill( x, y+dir, dir, left, right );
      }
    }
  }

  // Find adjacent horizontal segments in the opposite direction.
  if (ALIB_IS_RANGE( y-dir, 0, H ))
  {
    row = m_pImg->row_begin( y-dir );
    for (x = left; x < preLeft; x++)
    {
      if (((row[x]).f == NORM_MOTION_POINT) ||
          ((row[x]).f == WEAK_MOTION_POINT))
      {
        x = LineFill( x, y-dir, -dir, left, right );
      }
    } 

    for (x = preRight+1; x <= right; x++)
    {
      if (((row[x]).f == NORM_MOTION_POINT) ||
          ((row[x]).f == WEAK_MOTION_POINT))
      {
        x = LineFill( x, y-dir, -dir, left, right );
      }
    }
  }
  return right;

#endif
}


///------------------------------------------------------------------------------------------------
/// \brief Function traces and extracts the boundary of an object that lies inside specified image.
///
/// The image should contain exactly one object, otherwise the first found one will be traced.
///
/// \return        Ok = true.
///------------------------------------------------------------------------------------------------
bool RegionExtractor::Process( dvralgo::RichPixelImage & image,
                               Elvees::IMessage        * pOut,
                               float                     minArea,
                               ulong                     time,
                               Arr2ub                  * pDemoImg,
							   int                       maxRegionNumber)
{
  boost::mutex::scoped_lock lock( m_dataLocker );
  const ubyte               FLAGS = (PASSED_POINT | NORM_MOTION_POINT);

  if (!m_bReady)
    return true;   // do nothing

  try
  {
    sint   W = image.width();
    sint   H = image.height();
    IIPair regionScanSet(0,0);

    ELVEES_ASSERT( m_bGood );
    minArea /= (float)(dvralgo::DOWNSCALE * dvralgo::DOWNSCALE);

    m_pImg = &image;
    m_pOut = pOut;
    m_scanPool.clear();
    m_regionPool.clear();
    m_curQueue.clear();

    // Find non-painted motion points and recursively paints the regions.
    for (int y = 0; y < H; y++)
    {
      RichPixelImage::pointer row = image.row_begin( y );
      for (int x = 0; x < W; x++)
      {
        if (((row[x]).f & FLAGS) == NORM_MOTION_POINT)  // Is it a non-passed motion point?
        {
          m_boundRect.x1 = W;   m_boundRect.x2 = 0;
          m_boundRect.y1 = H;   m_boundRect.y2 = 0;
          m_area = 0;
          regionScanSet.first = (int)(m_scanPool.size());

          LineFill( x, y, 1, x, x );

          if (m_area >= minArea)
          {
            m_curQueue.push_back( dvralgo::MotionRegion() );
            MotionRegion & rgn = m_curQueue.back();
            rgn.rect = m_boundRect;
            rgn.area = m_area;
            rgn.time = time;
            regionScanSet.second = (int)(m_scanPool.size());
            m_regionPool.push_back( regionScanSet );
          }
        }
      }
    }

    // Refine regions.
 
	MergeBoxes(m_curQueue, maxRegionNumber);

	

    m_totQueue.insert( m_totQueue.end(), m_curQueue.begin(), m_curQueue.end() );

    // Draw extracted motion regions in demo image.
    if (pDemoImg != 0)
    {
      sint sW = W * dvralgo::DOWNSCALE;
      sint sH = H * dvralgo::DOWNSCALE;

      #if 1
      {
        for (int y = 0, mH = image.height(); y < mH; y++)
        {
          int _y = y * dvralgo::DOWNSCALE;
          for (int x = 0, mW = image.width(); x < mW; x++)
          {
            if ((image( x, y )).f & dvralgo::WEAK_MOTION_POINT)
            {
              int _x = x * dvralgo::DOWNSCALE;
              for (int v = 0; v < dvralgo::DOWNSCALE; v++)
              {
                for (int u = 0; u < dvralgo::DOWNSCALE; u++)
                {
                  (*pDemoImg)( _x+u, _y+v ) = alib::YELLOW;
                }
              }
            }
          }
        }
      }
      #endif

      // Draw regions.
      ELVEES_ASSERT( (pDemoImg->width() == sW) && (pDemoImg->height() == sH) );
      for (RegionPool::iterator rgnIt = m_regionPool.begin(); rgnIt != m_regionPool.end(); ++rgnIt)
      {
        for (int s = rgnIt->first; s < rgnIt->second; ++s)
        {
          const Scan & scan = m_scanPool[s];
          int          x1 = dvralgo::DOWNSCALE * (int)(scan.x1);
          int          x2 = dvralgo::DOWNSCALE * (int)(scan.x2);
          int          y  = dvralgo::DOWNSCALE * (int)(scan.y);

          for (int x = x1; x <= x2; x++)
          {
            for (int v = 0; v < dvralgo::DOWNSCALE; v++)
            {
              for (int u = 0; u < dvralgo::DOWNSCALE; u++)
              {
                (*pDemoImg)( x+u, y+v ) = alib::LIME;
              }
            }
          }
        }
      }

      // Draw bounding rectangles.
      for (MotionRegionArr::reverse_iterator it = m_curQueue.rbegin(); it != m_curQueue.rend(); ++it)
      {
        if (it->time == time)
        {
          int x1 = dvralgo::DOWNSCALE * it->rect.x1;
          int x2 = dvralgo::DOWNSCALE * it->rect.x2 + (dvralgo::DOWNSCALE-1);
          int y1 = dvralgo::DOWNSCALE * it->rect.y1;
          int y2 = dvralgo::DOWNSCALE * it->rect.y2 + (dvralgo::DOWNSCALE-1);

          for (int x = x1; x <= x2; x++)
            (*pDemoImg)( x, y1 ) = ((*pDemoImg)( x, y2 ) = alib::RED);

          for (int y = y1; y <= y2; y++)
            (*pDemoImg)( x1, y ) = ((*pDemoImg)( x2, y ) = alib::RED);
        }
      }
    }
  }
  catch ( std::runtime_error & e )
  {
    if (pOut != 0) pOut->Print( Elvees::IMessage::mt_error, e.what() );
    return false;
  }
  return true;
}

} // namespace dvralgo









///------------------------------------------------------------------------------------------------//
///// \brief Function .
///------------------------------------------------------------------------------------------------//
//void RegionExtractor::AnalyzeMotionRegion()
//{
//  ASSERT( vislib::IsEnclosedCurve( m_curve.begin(), m_curve.end() ) );
//  ASSERT( vislib::IsValidCurve( m_curve.begin(), m_curve.end() ) );
//
//  if (vislib::CurveOrientation( m_curve.begin(), m_curve.end() ) > 0 )
//  {
//    double perimeter = vislib::CurveLength( m_curve.begin(), m_curve.end(), true );
//    if (perimeter >= m_minPerimeter)
//    {
//      double area = vislib::CurveArea( m_curve.begin(), m_curve.end() );
//      if (area >= m_minArea)
//      {
//        m_curQueue.push_back( dvralgo::MotionRegion() );
//        MotionRegion & rgn = m_curQueue.back();
//        rgn.rect = vislib::GetBoundRect( m_curve.begin(), m_curve.end(), int(), false );
//        rgn.strength = (float)m_difference;
//        rgn.area = (uint)(area + 0.5);
//        rgn.perimeter = (uint)(perimeter + 0.5);
//        rgn.time = m_time;
//        rgn.contour;
//
//        if ((m_pDemoImg != 0) && !(m_curve.empty()))
//        {
//          GoAlongLine gal;
//          Vec2i       curPt, prePt = m_curve.back() * dvralgo::DOWNSCALE;
//
//          prePt.x += dvralgo::DOWNSCALE/2;
//          prePt.y += dvralgo::DOWNSCALE/2;
//
//          for (int i = 0, n = (int)(m_curve.size()); i < n; i++)
//          {
//            curPt = m_curve[i] * dvralgo::DOWNSCALE;
//            curPt.x += dvralgo::DOWNSCALE/2;
//            curPt.y += dvralgo::DOWNSCALE/2;
//
//            for (const Vec2i * p = gal.first( prePt, curPt ); p != 0; p = gal.next())
//            {
//              m_pDemoImg->set_if_in( *p, alib::RED );
//            }
//            prePt = curPt;
//          }
//        }
//      }
//    }
//  }
//}
//
//
///------------------------------------------------------------------------------------------------//
///// \brief Function traces motion regions, extracts their outline contours and analyzes them.
///------------------------------------------------------------------------------------------------//
//void RegionExtractor::TraceAllMotionRegions()
//{
//  const ubyte FLAGS = (PASSED_POINT | NORM_MOTION_POINT);
//  int         W = m_pImg->width();
//  int         H = m_pImg->height();
//  Vec2i       vec, start;
//
//  m_curve.clear();
//
//  // Scan image for any motion pixel.
//  for (int y = 0; y < H; y++)
//  {
//    ubyte prevFlags = 0;
//
//    for (int x = 0; x < W; x++)
//    {
//      ubyte currFlags = ((*m_pImg)( x, y )).f;
//
//      if (!(prevFlags & FLAGS) && (currFlags == FLAGS))
//      {
//        int k = 0, n = 4;
//
//        m_curve.clear();
//        start = vec.set( x, y );
//
//        do 
//        {
//          bool bContinue = false;
//
//          // Search the neighborhood of the current point.
//          for (k = 0; k < ALIB_LengOf(ALIB_NEIBR8); k++, n=(n+1)&7)
//          {
//            int                     u = vec.x + ALIB_NEIBR8[n].x;
//            int                     v = vec.y + ALIB_NEIBR8[n].y;
//            RichPixelImage::pointer i = m_pImg->get_if_in( u, v );
//
//            if (i != 0)
//            {
//              if (i->f & RichPixel::TRACED_POINT)
//              {
//                break;
//              }
//              else if (i->f == FLAGS)
//              {
//                bContinue = true;
//                break;
//              }
//            }
//          }
//
//          // Is isolated point or a traced point has been found in the neighbourhood?
//          if (!bContinue)
//          {
//            m_curve.clear();
//            break;
//          }
//
//          m_curve.push_back( vec );
//          vec += ALIB_NEIBR8[n];
//          n = (n+5)&7;
//        }
//        while (vec != start);
//
//        for (Vec2iArr::iterator it = m_curve.begin(); it != m_curve.end(); ++it)
//          ((*m_pImg)[*it]).f |= RichPixel::TRACED_POINT;
//
//        if (!(m_curve.empty()))
//          AnalyzeMotionRegion();
//      }
//
//      prevFlags = currFlags;
//    }
//  }
//}