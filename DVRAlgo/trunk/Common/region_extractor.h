///////////////////////////////////////////////////////////////////////////////////////////////////
// region_extractor.h
// ---------------------
// begin     : 2006
// modified  : 26 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_DVRALGO_REGION_EXTRACTOR_H__
#define __DEFINE_DVRALGO_REGION_EXTRACTOR_H__

#include "dvr_common.h"

namespace dvralgo
{

///================================================================================================
/// \class RegionExtractor.
/// \brief RegionExtractor.
///================================================================================================
class RegionExtractor
{
public:
  ///==========================
  /// \brief Horizontal line.
  ///==========================
  struct Scan
  {
    sword x1, x2;  //!< left and right positions of a scan, closed interval [x1,x2]
    sint  y;       //!< ordinate of a horizontal scan line
  }; 

  typedef  std::vector<Scan>                  ScanPool;
  typedef  std::pair<int,int>                 IIPair;
  typedef  std::vector<IIPair>                RegionPool;
  typedef  std::deque<dvralgo::MotionRegion>  MotionRegionQueue; 

  static const int MAX_QUEUE_SIZE = (1<<14);  //!< the maximal size of a queue of motion regions

private:
  MotionRegionArr    m_curQueue;   //!< queue of motion regions of the current frame
  MotionRegionArr    m_totQueue;   //!< queue of motion regions of several last frames
  boost::mutex       m_dataLocker; //!< object locks/unlocks the data being transferred between threads
  RichPixelImage   * m_pImg;       //!< pointer to the rich-pixel image
  Elvees::IMessage * m_pOut;       //!< pointer to the message handler
  ARect              m_boundRect;  //!< temporal: bounding rectangle of a motion region
  uint               m_area;       //!< temporal: area of a motion region
  ScanPool           m_scanPool;   //!< pool of scans of all regions
  RegionPool         m_regionPool; //!< pool of motion regions (indices of the first and the last+1 scans)
  bool               m_bGood;      //!< state flag is non-zero if this object has been properly initialized
  bool               m_bReady;     //!< state flag is non-zero if this object is ready to run

private:
  int LineFill( int x, int y, int dir, int preLeft, int preRight );
	Elvees::ICameraMotionEvent::MotionRect GetMotionRect( const dvralgo::MotionRegion & rgn );

public:
  RegionExtractor();
  virtual ~RegionExtractor();
	void SerializeMotionRegionsAndClearQueue( Elvees::ICameraMotionEvent::MotionRectArr & mra );
  bool Process( dvralgo::RichPixelImage & image, Elvees::IMessage * pOut,
                float minArea, ulong time, Arr2ub * pDemoImg, int maxRegionNumber );

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns the number of reliable motion regions.
  /// The value is valid between successive calls of function <i>Process(..)</i>.
  ///----------------------------------------------------------------------------------------------
  sint GetNumberOfRegions() const
  {
    return (sint)(m_regionPool.size());
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns pointer to the array of scans of specified region.
  /// Returned values are valid between successive calls of function <i>Process(..)</i>.
  /// \param  regionNo  index of a region of interest.
  /// \param  pScans    reference to the pointer that receives address of the first scan.
  /// \param  size      reference to the storage that receives the number of scans of the region.
  ///----------------------------------------------------------------------------------------------
  void GetRegion( sint regionNo, const Scan *& pScans, sint & size ) const
  {
    const IIPair & p = m_regionPool[ regionNo ];
    pScans = &(m_scanPool[ p.first ]);
    size = (sint)(p.second - p.first);
  }
};

} // namespace dvralgo

#endif // __DEFINE_DVRALGO_REGION_EXTRACTOR_H__

