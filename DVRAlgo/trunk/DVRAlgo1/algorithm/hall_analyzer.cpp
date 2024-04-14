///////////////////////////////////////////////////////////////////////////////////////////////////
// hall_analyzer.cpp
// ---------------------
// begin     : 2006
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev, Alexander Boltnev
// email     : Albert.Akhriev@biones.com, Alexander.Boltnev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../lib.h"
#include "../../Common/events.h"

#include "../../Common/camera_event_handler.h"

namespace dvralgo
{

///================================================================================================
/// \brief CameraData.
///================================================================================================
class CameraData
{
public:
  Elvees::ICameraMotionEvent::MotionRectArr m_motionRects;
  dvralgo::CameraMotionEventArr  m_motionEvents;

  CameraData(){};
};

typedef  std::map<sint, CameraData>  CameraMap;


///================================================================================================
/// \brief Implementation of hall analyzer.
///================================================================================================
class HallAnalyzer : public Elvees::IHallAnalyzer
{
  boost::mutex m_dataLocker; //!< object locks/unlocks the data being transferred between threads
  CameraMap m_cameras;    //!< state data of all cameras
  std::fstream m_log;        //!< log file stream
  long m_curentEventNum;
	time_t m_startingTime;

public:
  bool m_bGood;      //!< state flag: non-zero if surveillance is currently successful 
  Elvees::IMessage *  m_pOut;       //!< pointer to the message handler
  dvralgo::Parameters	m_parameters; //!< parameter settings
///------------------------------------------------------------------------------------------------
/// \brief Constructor.
///------------------------------------------------------------------------------------------------
HallAnalyzer() 
	: m_parameters( dvralgo::FIRST_CONTROL_IDENTIFIER )
	, m_pOut ( 0 )
	, m_curentEventNum( 1 )
	, m_startingTime( time(0) )
{
}


///------------------------------------------------------------------------------------------------
/// \brief Destructor.
///------------------------------------------------------------------------------------------------
~HallAnalyzer()
{
}

// adds new motion event with timeEnd = timeBegin
void AddOneEmptyMotionEvent( dvralgo::CameraMotionEventArr & events,
					         ulong timeBegin,
					         sint camId  )
{
	dvralgo::CameraMotionEvent ev( camId );
	ev.SetBeginTime( timeBegin );
	ev.SetEndTime( timeBegin );
	ev.SetEventNum( m_curentEventNum);
	++m_curentEventNum;

	events.push_back( ev );
}

void ProcessOneCamera( CameraData & cd, 
					   ulong timeStamp,
					   sint camId )
{
	float timeOutBeetweenEvents = m_parameters.s_algorithm.p_timeOutBeetweenEvents(); // sec
	float timeOutEventLifetime = m_parameters.s_algorithm.p_maxEventLifeTime(); // sec

	//Create motion events

	
	Elvees::ICameraMotionEvent::MotionRectArr  & rects = cd.m_motionRects;
	dvralgo::CameraMotionEventArr & events = cd.m_motionEvents;

	if ( ! rects.empty() ) //sort regiions???
	{
		//analyze and create motion events
		Elvees::ICameraMotionEvent::MotionRectArr ::iterator cur = rects.begin(); // current position
		Elvees::ICameraMotionEvent::MotionRectArr ::iterator end = rects.end(); // end

		// decide, if it is needed to add new event
		if ( events.empty() )
		{
			AddOneEmptyMotionEvent( events, cur->time, camId );
		}

		dvralgo::CameraMotionEventArr::iterator curEvent;
		if ( ! events.empty() ) 
			curEvent = events.begin() + events.size() - 1;

		while ( cur != end )
		{
			if ( curEvent->IsClosed() )
			{
				AddOneEmptyMotionEvent( events, cur->time, camId );			
				curEvent = events.begin() + events.size() - 1;
			}
			if ( ((float)cur->time  - (float)curEvent->EndTime() ) > timeOutBeetweenEvents )
			{
				curEvent->Close();		
			}
			curEvent->PushBackRect( *cur  );
			curEvent->SetEndTime( cur->time );	
			cur++;
		}

		rects.clear();
	}
	
	//close events if timeout
	if ( ! events.empty() )
	{
		dvralgo::CameraMotionEventArr::iterator curEvent = events.begin();
		while ( curEvent != events.end() )
		{
			if ( ! curEvent->IsClosed() )
			{
				if ( ( (float)timeStamp / 1000.0f /* curEvent->EndTime() */ -  (float)curEvent->BeginTime() ) > timeOutEventLifetime )
				{
					curEvent->Close();
				}

				if ( ( (float)timeStamp / 1000.0f  /* curEvent->EndTime() */ -  (float)curEvent->EndTime()  ) > timeOutBeetweenEvents )
				{
					curEvent->Close();
				}
			}
			
			curEvent->SetEndTime( timeStamp / 1000 );
			curEvent++;
		}
	}

	if ( ! events.empty() )
	{
		dvralgo::CameraMotionEventArr::iterator curEvent = events.begin();
		while ( curEvent != events.end() )
		{	
			curEvent->RemoveSimilarBoxes();
			curEvent++;
		}
	}
}

///------------------------------------------------------------------------------------------------
/// \brief Function processes accumulated data obtained from all cameras.
///
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///------------------------------------------------------------------------------------------------
virtual bool Process( ulong timeStamp )
{
  boost::mutex::scoped_lock lock( m_dataLocker );

  try
  {
	  CameraMap::iterator camIt = m_cameras.begin();
	  while( camIt != m_cameras.end() )
	  {
		  ProcessOneCamera( camIt->second, timeStamp, camIt->first );
		  camIt++;
	  }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
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

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_FROM_CAMERA_TO_HALL:
      {
        const dvralgo::TFromCameraToHall * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( p->data.first >= 0 );
		if ( p->data.second.size() > 0 )
		{
			CameraData & cd = m_cameras[ p->data.first ];
			Elvees::DeserializeVector( cd.m_motionRects, p->data.second );
			//std::copy( p->data.motionRegions.begin(), p->data.motionRegions.end(), std::back_inserter(cd.m_motionRegions) );
			//std::cout << "recieved " << m_tempMotionRegions.size() << " rects from cam " <<  p->data.cameraNum << std::endl;
		}
      }
      break;
      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
    return false;
  }
  return true;
}

void CopyOrMoveCameraEvents( Elvees::IEventPtrArr & eventarr, CameraData & cd )
{
	dvralgo::CameraMotionEventArr::iterator it = cd.m_motionEvents.begin();
	while ( it != cd.m_motionEvents.end() )
	{
		if( ! ( it->IsShortFlash() )  ) 
		{			
			dvralgo::CameraMotionEvent * pOutEvent = new dvralgo::CameraMotionEvent( *it );
			pOutEvent->AddTime( m_startingTime );			
			eventarr.push_back( Elvees::IEventPtr( pOutEvent ) );
		}
		if ( it->IsClosed() || it->IsEmpty() )
		{
			it = cd.m_motionEvents.erase( it );
		}
		else
		{
			it++;
		}
	}
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

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_FROM_HALL_TO_CAMERA:
      {
		  //nothing to do, only checks
        dvralgo::TFromHallToCamera * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( p->data.first >= 0 );
      }
      break;

      case dvralgo::DATAID_EVENT_PTRS:
      {
		  //////////////////////////////////////////////////////////////////////////		  
		  dvralgo::TEventPtrs * p = 0;
		  Elvees::DynamicCastPtr( pData, &p );
		  Elvees::IEventPtrArr & eventarr = p->data;
		  //////////////////////////////////////////////////////////////////////////
		  eventarr.clear();	  
		  CameraMap::iterator it = m_cameras.begin();
		  while ( it != m_cameras.end() )
		  {
			  CopyOrMoveCameraEvents( eventarr, it->second );
			  it++;
		  }
	  }
      break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
    return false;
  }
  return true;
}


///------------------------------------------------------------------------------------------------
/// \brief Function prints exception message.
/// 
/// \param  e  pointer to the exception object or 0 (unsupported exception).
///------------------------------------------------------------------------------------------------
void PrintException( const std::runtime_error & e ) const
{
  HallAnalyzer * pThis = const_cast<HallAnalyzer*>( this );
  if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << e.what() << std::endl << std::endl;
}

}; // struct HallAnalyzer

} // namespace dvralgo


///////////////////////////////////////////////////////////////////////////////////////////////////
// Exported function(s).
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace dvralgo
{

///------------------------------------------------------------------------------------------------
/// \brief Function creates an instance of hall analyzer.
///
/// \param  pOutput   pointer to the message handler.
/// \return           Ok = pointer to the instance of created hall analyzer, otherwise 0.
///------------------------------------------------------------------------------------------------
Elvees::IHallAnalyzer * CreateHallAnalyzer( const BinaryData * pParameters,
                                            Elvees::IMessage * pOutput )
{
  dvralgo::HallAnalyzer * p = 0;

  try
  {
    p = new dvralgo::HallAnalyzer();
    ELVEES_ASSERT( p != 0 );

    // Initialize parameters.
    if (pParameters != 0)
      Elvees::BinaryToParameters( pParameters, &(p->m_parameters), pOutput );

    p->m_pOut = pOutput;
    p->m_bGood = true;
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

