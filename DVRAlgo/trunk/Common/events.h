///////////////////////////////////////////////////////////////////////////////////////////////////
// events.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_EVENTS_H__
#define __DVR_EVENTS_H__

#include "../../../Shared/Interfaces/i_event.h"

namespace dvralgo
{

///================================================================================================
/// \brief Event.
///================================================================================================
class CameraMotionEvent : public Elvees::ICameraMotionEvent
{
public:
  CameraMotionEvent( int camId ) 
	  : rects_()
	  , timeBegin_(0)
	  , timeEnd_(0)
	  , cameraId_(camId)
	  , bClosed_(false)
	  , eventNum_(-1)
  {}

  CameraMotionEvent( const CameraMotionEvent & event ) 
	  : rects_( event.rects_ )
	  , timeBegin_( event.timeBegin_ )
	  , timeEnd_( event.timeEnd_ )
	  , cameraId_( event.cameraId_ )
	  , bClosed_( event.bClosed_ )
	  , eventNum_( event.eventNum_ )
  {}

  virtual time_t BeginTime() const { return timeBegin_; }

  virtual time_t EndTime() const { return timeEnd_; }

  virtual sint SensorId() const { return cameraId_; }

	virtual bool IsClosed() const 
	{ 
		return bClosed_; 
	}

  virtual const MotionRectArr & GetMotionRectangles() const { return rects_; }

  bool IsEmpty() const 
	{ 
		return (rects_.empty()); 
	}

	bool IsShortFlash() const 
	{ 
		if (IsEmpty()) return true;
		// it is not guaranteed that rects with less time go first
		time_t minTime = rects_.front().time;
		time_t maxTime = rects_.front().time;
		MotionRectArr::const_iterator i = rects_.begin();
		while ( i != rects_.end() )
		{	
			if  ( i->time < minTime ) minTime = i->time;
			if  ( i->time > maxTime ) maxTime = i->time;
			i++;
		}
		return ( minTime == maxTime ) && bClosed_; 
	}

  void Close( ) 
  {
	  bClosed_ = true;
  }

  void SetBeginTime( time_t time )
  {
	  timeBegin_ = time;
  }
  void SetEndTime( time_t time )
  {
	  timeEnd_ = time;
  }
  void PushBackRect( MotionRect & rect )
  {
	  rects_.push_back( rect);
  }

  void SetEventNum( long num )
  {
	  eventNum_ = num;
  }
  
  long GetEventNum()
  {
	 return eventNum_;
  }

	void AddTime( time_t delta )
	{
		for( int i = 0; i < rects_.size(); i++ ) 
		{
			rects_[i].time += delta;
		}		
	}

	void RemoveSimilarBoxes()
	{
		MergeLatestWithPreviousBoxes( rects_, 0.5f );
	}
	
	virtual const MotionRectArr & GetLastSecondMotionRectangles()
	{
		rectsLastSecond_.clear();
		if ( !rects_.empty() )
		{
			std::sort( rects_.begin(), rects_.end(), MotionRectTimeLess() );
			time_t lastTime = rects_.back().time;			
			MotionRectArr::reverse_iterator i = rects_.rbegin();
			while ( i != rects_.rend() )
			{
				if( i->time != lastTime) break;
				rectsLastSecond_.push_back( *i );				
				i++;
			}			
		}		
		return rectsLastSecond_;
	}

private:
	MotionRectArr rects_;      //!< 
	MotionRectArr rectsLastSecond_;      //!< 
	time_t timeBegin_;  //!< 
	time_t timeEnd_;    //!< 
	int cameraId_;   //!< 
	bool bClosed_;    //!< 
	long eventNum_;

	struct MotionRectAreaGreater
		: std::binary_function
				<
					const ICameraMotionEvent::MotionRect,	
					const ICameraMotionEvent::MotionRect, 
					bool
				>
	{
		inline bool operator () (const ICameraMotionEvent::MotionRect & lhs,
														const ICameraMotionEvent::MotionRect & rhs)
		{
			float lhsArea = std::abs( (lhs.x1 - lhs.x2) * (lhs.y1 - lhs.y2) );
			float rhsArea = std::abs( (rhs.x1 - rhs.x2) * (rhs.y1 - rhs.y2) );
			return (lhsArea > rhsArea);
		}
	};
	
	struct MotionRectTimeGreater
		: std::binary_function
		<
		const ICameraMotionEvent::MotionRect,	
		const ICameraMotionEvent::MotionRect, 
		bool
		>
	{
		inline bool operator () (const ICameraMotionEvent::MotionRect & lhs,
			const ICameraMotionEvent::MotionRect & rhs) const
		{		
			return ( lhs.time > rhs.time );
		}
	};

	struct MotionRectTimeLess
		: std::binary_function
		<
		const ICameraMotionEvent::MotionRect,	
		const ICameraMotionEvent::MotionRect, 
		bool
		>
	{
		inline bool operator () (const ICameraMotionEvent::MotionRect & lhs,
			const ICameraMotionEvent::MotionRect & rhs) const
		{		
			return ( lhs.time < rhs.time );
		}
	};
	void MergeLatestWithPreviousBoxes( MotionRectArr & boxes, float areaPercent = 0.99f )
	{
		const int IMAG_X = 1024; // imaginary picture width
		const int IMAG_Y = 1024; // imaginary picture width
		const int MIMINAL_AREA = 64; 
		//unite every big box with all small boxes intersecting it
		std::sort(boxes.begin(), boxes.end(), MotionRectAreaGreater() );
		
		MotionRectArr::iterator crntBox = boxes.begin();
		MotionRectArr::iterator crntInnerBox = boxes.begin();

		while ( crntBox != boxes.end() )
		{	
			crntInnerBox = crntBox + 1;
			while ( crntInnerBox != boxes.end() )
			{
				ARect cur( (int)(crntBox->x1 * IMAG_X), (int)(crntBox->y1 * IMAG_Y), (int)(crntBox->x2 * IMAG_X), (int)(crntBox->y2 * IMAG_Y));
				ARect inr( (int)(crntInnerBox->x1 * IMAG_X), (int)(crntInnerBox->y1 * IMAG_Y), (int)(crntInnerBox->x2 * IMAG_X), (int)(crntInnerBox->y2 * IMAG_Y));
				ARect intersection = cur & inr;
		
				// intersection area is greater than areaPercent of big rectangle area
				if ( intersection.area() > areaPercent * cur.area() && (crntBox->time == crntInnerBox->time) )
				{
					//time_t tmpTime = std::max<time_t>( crntBox->time, crntInnerBox->time );
					*crntBox = *crntInnerBox;
					//crntBox->time = tmpTime;
					crntInnerBox = boxes.erase( crntInnerBox );
				}
				// small rectangle is almost in intersection or too small
				else if ( (  (intersection.area() >= areaPercent * inr.area()) || ( inr.area() < MIMINAL_AREA )) &&  (crntBox->time == crntInnerBox->time) )
				{		
					crntInnerBox = boxes.erase( crntInnerBox );	
				}
				else
				{
					crntInnerBox++;
				}
			}
			if ( crntBox == boxes.end())
				break;
			crntBox++;
		}
	}
};

typedef  std::vector<CameraMotionEvent>  CameraMotionEventArr;


/////================================================================================================
///// \struct Event.
///// \brief  Event.
/////================================================================================================
//struct Event
//{
//  static const ulong NO_EVENT      = (ulong(0));
//  static const ulong NEW_EVENT     = (ulong(1) << 0);
//  static const ulong FEEBLE_MOTION = (ulong(1) << 1);
//  static const ulong NORMAL_MOTION = (ulong(1) << 2);
//  static const ulong STRONG_MOTION = (ulong(1) << 3);
//
//  static const ulong MOTION_FLAGS = (FEEBLE_MOTION | NORMAL_MOTION | STRONG_MOTION);
//
//  time_t timeBegin; //!<
//  time_t timeEnd;   //!< 
//  
//
//  ulong  type;      //!< 
//
//  ///----------------------------------------------------------------------------------------------
//  /// \brief .
//  ///----------------------------------------------------------------------------------------------
//  Event() : timeBegin(0), timeEnd(0), type(NO_EVENT)
//  {
//  }
//
//  ///----------------------------------------------------------------------------------------------
//  /// \brief .
//  ///----------------------------------------------------------------------------------------------
//  bool is_flag( ulong f ) const
//  {
//    return ((type & f) != 0);
//  }
//
//  ///----------------------------------------------------------------------------------------------
//  /// \brief .
//  ///----------------------------------------------------------------------------------------------
//  void set_flag( ulong f )
//  {
//    type |= f;
//  }
//
//  ///----------------------------------------------------------------------------------------------
//  /// \brief .
//  ///----------------------------------------------------------------------------------------------
//  void reset_flag( ulong f )
//  {
//    type &= ~f;
//  }
//};

} // namespace dvralgo

#endif // __DVR_EVENTS_H__

