///////////////////////////////////////////////////////////////////////////////////////////////////
// camera_analyzer.cpp
// ---------------------
// begin     : 2006
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev, Alexander Boltnev
// email     : Albert.Akhriev@biones.com, Alexander.Boltnev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../lib.h"
#include "parameters.h"
#include "../../Common/parameter_functions.h"
#include "../../Common/dvr_common.h"
#include "../../Common/region_extractor.h"
#include "../../Common/are_images_equal.h"

#include "../../Common/delta_time_protection.h"

//#define _RESEARCH_MODE_

#ifdef _RESEARCH_MODE_

#include "../../common/add/dgl.h"

using namespace sl;
#define BGR(r,g,b) ((int)(((unsigned char)(r)|((short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))

void SaveHisto( float thrArr[256] )
{
	std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	std::string fname = std::string("d:\\temp\\pics\\testHisto.bmp");
	
	//prepare graph	
	std::vector< Point< float > > curve;
	float step = 0.1f;
	for( int n = 0; n < 256; n++ )
	{
		curve.push_back( MakePoint( step / 2 + n * step, thrArr[n]) );
	}
	sl::Rectangle<float> r(  MakePoint( 25.6f, -10.0f ), MakePoint( 0.0f, 0.1f ) );
	Sketch<float, int> s( r, 800 );
	s.DrawAxis( BGR(255,0,0) );
	s.DrawCurve( curve.begin(), curve.end(), BGR(0, 0, 255) );
	BitMapFileFormat::Save32bitFile( s.GetImage(), fname );

//GNU Plot
	std::fstream fs; 
	fs.open( L"d:\\temp\\pics\\testHisto.txt", std::ios_base::out );
	for( int bin = 0; bin < 256; bin++ )
		fs << bin << " " << thrArr[bin] << std::endl;
	fs.close();

}

#endif

namespace dvralgo
{
///================================================================================================
/// \brief Implementation of camera analyzer.
///================================================================================================
class CameraAnalyzer : public Elvees::ICameraAnalyzer
{
private:
  bool                     m_bFirstFrame;      //!< nonzero until first frame is processed
  ulong                    m_previousTime;     //!< 
  mutable boost::mutex     m_dataLocker;       //!< object locks/unlocks the data being transferred between threads
  dvralgo::RichPixelImage  m_image;            //!< 
  dvralgo::RegionExtractor m_rgnExtractor;     //!< 
  dvralgo::MotionRegionArr m_motionRegions;    //!< 
  UByteArr                 m_tempBuffer;       //!< 
  float                    m_meanDeviation[3]; //!< mean noise deviations of non-motion points of Y, U and V channels

  Elvees::IMessage *       m_pOut;             //!< pointer to the message handler
  std::fstream             m_log;              //!< log file stream
  Arr2ub                   m_demoImage[4];     //!< 
  Arr2f                    m_tmpFloatImg;      //!< 
  Elvees::AbstractMenu     m_menu;             //!< feedback menu used for debugging
  sint                     m_selectedInfoPage; //!< index of selected info page
  bool					   m_protectionInitialized; 

#ifdef _RESEARCH_MODE_
	long testCount;
	float thresholdsArr[256];
#endif

public:
  int                      m_cameraNo;         //!< 
  dvralgo::Parameters      m_parameters;       //!< parameter settings
  bool                     m_bGood;            //!< state flag: nonzero if surveillance is currently successful
  bool                     m_bReady;           //!< state flag: nonzero if this object is properly initialized

public:
///------------------------------------------------------------------------------------------------
/// \brief Constructor.
///------------------------------------------------------------------------------------------------
CameraAnalyzer( Elvees::IMessage * pOutput ) : m_parameters( dvralgo::FIRST_CONTROL_IDENTIFIER )
#ifdef _RESEARCH_MODE_
, testCount( 0 )
#endif
{
  m_bFirstFrame = true;
  m_previousTime = 0;
  m_dataLocker;
  m_image.clear();
  m_rgnExtractor;
  m_motionRegions;
  m_tempBuffer.clear();
  m_meanDeviation[0] = m_meanDeviation[1] = m_meanDeviation[2] = 1.0f;

  m_pOut = pOutput;
  m_log.close();
  m_demoImage;
  m_menu.Clear();
  m_selectedInfoPage = -1;

  m_cameraNo = -1;
  m_parameters;
  m_bGood = false;
  m_bReady = false;
  m_protectionInitialized = false;

#ifdef _RESEARCH_MODE_
  std::fill( thresholdsArr, thresholdsArr + 256, 0.0f );
#endif
}


///------------------------------------------------------------------------------------------------
/// \brief Destructor.
///------------------------------------------------------------------------------------------------
virtual ~CameraAnalyzer()
{
  m_bReady = false;
}


///------------------------------------------------------------------------------------------------
/// \brief Function processes the current frame.
///
/// \param  pHeader    pointer to bitmap header.
/// \param  pBytes     pointer to bitmap contents.
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///------------------------------------------------------------------------------------------------
virtual bool Process( const BITMAPINFOHEADER * pHeader, const ubyte * pBytes, ulong timeStamp )
{
  boost::mutex::scoped_lock lock( m_dataLocker );

#ifdef _RESEARCH_MODE_
	testCount++;
	std::cout << testCount << std::endl;
#endif

  if( !m_protectionInitialized )
	  return false;

  const float TAU = 1000.0f * m_parameters.s_algorithm.p_backgrUpdateTau();
  const float HOLE_TIMEOUT = 1000.0f * m_parameters.s_algorithm.p_holeTimeout();
  const float THRESHOLD_SCALE = m_parameters.s_algorithm.p_changeThreshold();

  if (!m_bReady)
    return true;   // do nothing

  try
  {
    ELVEES_ASSERT( m_bGood && (pHeader != 0) && (pBytes != 0) );

    ShrinkYUY2Image( pHeader, pBytes, &m_image, !INVERT_AXIS_Y );

    //alib::RepeatedlySmoothColorImage121( m_image, 2, m_tempBuffer );

    CorrectBrightness( &m_image, 9, &m_image, &m_tempBuffer );
  
	int protection = g_protection();

    // The first frame replaces the background.
    if (m_bFirstFrame)
    {
      for (int i = 0, n = m_image.size(); i < n; i++)
      {
        dvralgo::RichPixel & p = m_image[i];

        p.background[0] = (float)(p.old[0] = p.y);
        p.background[1] = (float)(p.old[1] = p.u);
        p.background[2] = (float)(p.old[2] = p.v);
				
				p.naturalBackground = p.windowAverage;

        p.threshold[0] = 5.0f;
        p.threshold[1] = 5.0f;
        p.threshold[2] = 5.0f;

        p.f = (p.f & HIDDEN_POINT);  // keep mask flag
        p.time = 0;
      }
      m_bFirstFrame = false;
      m_previousTime = timeStamp;
      return true;
    }

    ASSERT( m_previousTime <= timeStamp );

		const uint timeInc = (uint)(timeStamp - m_previousTime);
		//const uint timeInc = protection.SetTime( timeStamp );

    // Update background and detect motion.
    {
      const float beta = m_parameters.s_algorithm.p_backgrUpdateRate();
      const float alpha = 1.0f - beta;                                    //exp( -(float)timeInc/TAU );
      const float slowBeta = 0.5*beta;
      const float slowAlpha = 1.0f - slowBeta;
      float       pixel[3], diff[3];
      double      diff_sum[3] = {0,0,0};
      int         diff_num = 0;

      // For all image points...
      for (int i = 0, n = m_image.size(); i < n; i++)
      {
        RichPixel & p = m_image[i];

				//float THRESHOLD_SCALE = ThresholdFunction( p.background[0] );

        if (p.f & HIDDEN_POINT)
          continue;

        pixel[0] = (float)(p.y);
        pixel[1] = (float)(p.u);
        pixel[2] = (float)(p.v);

        bool bPrevMotion = ((p.f & NORM_MOTION_POINT) != 0);
        p.f &= ~MOTION_FLAGS;

        for (int c = 0; c < 3; c++)
        {
          diff[c] = //1.0f + 
fabs( p.background[c] - pixel[c] ) + protection;
	        if (diff[c] > (THRESHOLD_SCALE / 2) * ThresholdFunction( p.naturalBackground) * p.threshold[c])
          {
            p.f |= ((diff[c] > (ThresholdFunction( p.naturalBackground ) * THRESHOLD_SCALE * p.threshold[c])) ? NORM_MOTION_POINT : WEAK_MOTION_POINT);
          }
        }

#ifdef _RESEARCH_MODE_
				if( testCount > 500 && !(p.f & NORM_MOTION_POINT)	)
				{
					for (int c = 0; c < 3; c++)
					{
						int curColor = (int)p.naturalBackground;
						if( diff[c] > thresholdsArr[curColor] * p.threshold[c] )
						{
							thresholdsArr[curColor] = std::max( thresholdsArr[curColor], diff[c] / p.threshold[c] ) ;							
						}	
					}					
					if( testCount == 2000 )
					{
						SaveHisto( thresholdsArr );
						testCount = 2001;
					}
				}
#endif
        if (p.f & NORM_MOTION_POINT)
        {
          p.time += timeInc;
          if (p.time > HOLE_TIMEOUT)
          {
            for (int c = 0; c < 3; c++)
            {
              p.background[c] = pixel[c];
							p.naturalBackground = p.windowAverage;
              p.threshold[c] = m_meanDeviation[c];  // maybe 2*m_meanDeviation[c] would be better
            }
            p.time = 0;
            p.f &= ~MOTION_FLAGS;
          }
        }
        else // no normal motion
        {
          p.time = 0;
          if (!bPrevMotion)
          {
            for (int c = 0; c < 3; c++)
            {
							diff_sum[c] += fabs( p.old[c] - pixel[c] );
							p.old[c] = pixel[c];
            }
            diff_num++;
          }

          for (int c = 0; c < 3; c++)
          {
            p.background[c] = alpha * p.background[c] + beta * pixel[c];
						p.naturalBackground = alpha * p.naturalBackground + beta * p.windowAverage;

            if (diff[c] < p.threshold[c])
              p.threshold[c] = slowAlpha*p.threshold[c] + slowBeta*diff[c];
            else
              //p.threshold[c] = alpha*p.threshold[c]  + beta*diff[c];
							p.threshold[c] = std::min<float>( 3.0f * THRESHOLD_SCALE * ThresholdFunction( p.naturalBackground ) * m_meanDeviation[c], alpha*p.threshold[c]  + beta*diff[c] ); //< this variant works better

            if (p.threshold[c] < m_meanDeviation[c])
              p.threshold[c] = m_meanDeviation[c];
          }
        }
      }

      // Is there sufficient statistics?
      if ( diff_num > (1<<12) )
      {
        for (int c = 0; c < 3; c++)
          m_meanDeviation[c] = std::max<float>( 1.0f, (float)(diff_sum[c])/(float)(diff_num) );
      }
    }

    // Do motion regions analysis.
    if ( m_parameters.s_demo.p_bShowCurrentState() )
    {
      // prepare left image.
      dvralgo::CopyYUY2ToGrayImage( pHeader, pBytes, &m_demoImage[0], false, true );
      m_rgnExtractor.Process( m_image, m_pOut, m_parameters.s_algorithm.p_minRgnArea(),
                              timeStamp, &(m_demoImage[0]), m_parameters.s_algorithm.p_maxBoxes() );

      // prepare right image.
      dvralgo::CopyRichPixelImageToGreyFloatImage( m_image, m_tmpFloatImg );
      dvralgo::CopyFloatImageToByteImage( m_tmpFloatImg, m_demoImage[1], DOWNSCALE, true );

      // prepare left-down image.
      dvralgo::CopyRichPixelImageToGreyFloatImage( m_image, m_tmpFloatImg, 4 );
      dvralgo::CopyFloatImageToByteImage( m_tmpFloatImg, m_demoImage[2], DOWNSCALE, true );

      // prepare right-down image.
      dvralgo::CopyRichPixelImageToGreyFloatImage( m_image, m_tmpFloatImg, 5 );
      std::pair<float,float> res = dvralgo::CopyFloatImageToByteImage( m_tmpFloatImg, m_demoImage[3], DOWNSCALE, true );
//      std::cout << "mean thr = " << res.first << ",  dev thr = " << res.second << std::endl;
    }
    else
    {	
      m_rgnExtractor.Process( m_image, m_pOut, m_parameters.s_algorithm.p_minRgnArea(),
                              timeStamp, 0, m_parameters.s_algorithm.p_maxBoxes() );
    }

 
    // Process menu.
    //sint selectedId = (sint)(m_menu.GetSelectedItemId());
    //if (selectedId >= 0)
    //{
    //  m_selectedInfoPage = -1;
    //  Elvees::AbstractMenu::ItemArr::iterator i;
    //  for (i = m_menu.m_items.begin(); i != m_menu.m_items.end(); ++i)
    //  {
    //    i->m_bChecked = (ubyte)(selectedId == i->m_id);
    //    if (i->m_bChecked) m_selectedInfoPage = i->m_id;
    //  }
    //  m_menu.SetSelectedItemId(-1);
    //}

    m_previousTime = timeStamp;
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///------------------------------------------------------------------------------------------------
/// \brief Function uploads specified data to this object.
///
/// \param  pData  pointer to the input data.
/// \return        Ok = true.
///------------------------------------------------------------------------------------------------
virtual bool SetData( const Elvees::IDataType * pData )
{
  boost::mutex::scoped_lock lock( m_dataLocker );

  if (!m_bReady)
    return false;

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_FROM_HALL_TO_CAMERA:
      {
        const dvralgo::TFromHallToCamera * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( p->data.first == m_cameraNo );
      }
      break;

      case dvralgo::DATAID_SET_PARAMETERS:
      {
        const dvralgo::TSetParameters * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( Elvees::BinaryToParameters( &(p->data), &m_parameters, m_pOut ) );
      }
      break;
	  case dvralgo::DATAID_ALGO_SENSITIVITY:
	  {	
	     const dvralgo::TAlgoSensitivity * p = 0;
		 Elvees::DynamicCastPtr( pData, &p );
		 float sensitivity = p->data;
		 alib::Limit( sensitivity, 0.0f, 1.0f );
		 float minVal = m_parameters.s_algorithm.p_changeThreshold.min();
		 float maxVal = m_parameters.s_algorithm.p_changeThreshold.max();
		 m_parameters.s_algorithm.p_changeThreshold = maxVal + sensitivity * ( minVal - maxVal );
	  }
	  break;
	  case dvralgo::DATAID_PROTECTION_DEVICE:
	  {	
		  const dvralgo::TProtectionDeviceName * p = 0;
		  Elvees::DynamicCastPtr( pData, &p );
		  g_protection.Initialize( p->data );
		  m_protectionInitialized = true;
	  }
	  break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///------------------------------------------------------------------------------------------------
/// \brief Function unloads specified data from this object.
///
/// \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
/// \return        Ok = true.
///------------------------------------------------------------------------------------------------
virtual bool GetData( Elvees::IDataType * pData ) 
{
  boost::mutex::scoped_lock lock( m_dataLocker );

  if (!m_bReady)
    return false;

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_QIMAGE:
      {
        dvralgo::TQImage * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        QImage & image = p->data;
        image;
      }
      break;

      case dvralgo::DATAID_MENU_PTR:
      {
        dvralgo::TMenuPtr * p = 0;
        Elvees::DynamicCastPtr( pData, &p );

        // Create and fill menu.
        if (m_menu.m_items.empty())
        {
          p->data = &(m_menu);
        }
      }
      break;

      case dvralgo::DATAID_INFO_PAGE:
      {
        dvralgo::TInfoPage * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p->data.clear();
        if (m_selectedInfoPage >= 0)
        {
          p->data = "";
        }
      }
      break;

      case dvralgo::DATAID_BYTE_IMAGE_PTR:
      {
        dvralgo::TByteImagePtr * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p->data.second = 0;
        if (ALIB_IS_RANGE( p->data.first, 0, 4 ) && !((m_demoImage[p->data.first]).empty()))
          p->data.second = &(m_demoImage[p->data.first]);
      }
      break;

      case dvralgo::DATAID_FROM_CAMERA_TO_HALL:
      {
        dvralgo::TFromCameraToHall * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p->data.first = m_cameraNo;
        Elvees::ICameraMotionEvent::MotionRectArr rects;
        m_rgnExtractor.SerializeMotionRegionsAndClearQueue( rects );     
        Elvees::SerializeVector(rects, p->data.second );
	  }
      break;
      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///------------------------------------------------------------------------------------------------
/// \brief Function prints exception message.
/// 
/// \param  e  pointer to the exception object or 0 (unsupported exception).
///------------------------------------------------------------------------------------------------
void HandleException( const std::runtime_error & e ) const
{
  CameraAnalyzer * pThis = const_cast<CameraAnalyzer*>( this );
  pThis->m_bGood = false;
  if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << e.what() << std::endl << std::endl;
}


///------------------------------------------------------------------------------------------------
/// \brief Function creates and initializes a downscaled mask of visible points.
///
/// \param  pMask  pointer to user-defined mask of visible regions (nonzero pixel is visible).
///------------------------------------------------------------------------------------------------
void InitializeMask( const Arr2ub * pMask )
{
  m_image.release();
  if (pMask != 0)
  {
    const int SCALE = dvralgo::DOWNSCALE;
    const int HALF_SQUARE = (SCALE*SCALE)/2;
    const int W = (pMask->width()/SCALE)*SCALE;
    const int H = (pMask->height()/SCALE)*SCALE;

    // Create downscaled mask of visible points.
    m_image.resize( W/SCALE, H/SCALE );
    for (int y = 0; y < H; y++)
    {
      for (int x = 0; x < W; x++)
      {
        uint sum = 0;
        for (int v = 0; v < SCALE; v++)
        {
          for (int u = 0; u < SCALE; u++)
          {
            sum += (((*pMask)( x+u, y+v ) != 0) ? 1 : 0);
          }
        }
        (m_image( x/SCALE, y/SCALE )).f = ((sum >= HALF_SQUARE) ? 0 : HIDDEN_POINT);
      }
    }
  }
}

private:
	float ThresholdFunction( float background )	
	{
		background = std::max<float>( 1.0f, background );
		const float sigma = 100.0f;
		const float sigma3 = sigma*sigma*sigma;
		return 1.3f + 0.7f * sigma3 / ( background*background*background + sigma3 );
	}
}; // struct CameraAnalyzer

} // namespace dvralgo


namespace dvralgo
{

///------------------------------------------------------------------------------------------------
/// \brief Function creates an instance of camera analyzer.
///
/// \param  cameraNo     the index of a camera.
/// \param  pParameters  pointer to a binary storage of parameter settings.
/// \param  pMask        pointer to a mask of visible (nonzero) and non-visible (zero) points.
/// \param  pOutput      pointer to the message handler.
/// \return              Ok = pointer to the instance of created camera analyzer, otherwise 0.
///------------------------------------------------------------------------------------------------
Elvees::ICameraAnalyzer * CreateCameraAnalyzer( sint               cameraNo,
                                                const BinaryData * pParameters,
                                                const Arr2ub     * pMask,
                                                Elvees::IMessage * pOutput/*, HDC & */ )
{
  dvralgo::CameraAnalyzer * p = 0;

  try
  {
    p = new dvralgo::CameraAnalyzer( pOutput );
    ELVEES_ASSERT( p != 0 );

    p->m_cameraNo = cameraNo;

    // Initialize parameters.
    if (pParameters != 0)
      Elvees::BinaryToParameters( pParameters, &(p->m_parameters), pOutput );

    // Initialize the mask.
    p->InitializeMask( pMask );

    p->m_bGood = true;
    p->m_bReady = true;
  }
  catch (std::runtime_error & e)
  {
    if (pOutput != 0) pOutput->Print( Elvees::IMessage::mt_error, e.what() );
    delete p;
    p = 0;
  }
  return p;
}

} // namespace dvralgo

