///////////////////////////////////////////////////////////////////////////////////////////////////
// i_event.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_EVENT_INTERFACE_H__
#define __DVR_EVENT_INTERFACE_H__

namespace Elvees
{

///================================================================================================
/// \brief IEvent.
///================================================================================================
class IEvent
{
public:
  virtual ~IEvent() {}
  virtual bool IsEmpty() const = 0;
};

typedef  boost::shared_ptr<IEvent>  IEventPtr;
typedef  std::vector<IEventPtr>     IEventPtrArr;


///================================================================================================
/// \brief ISensorEvent.
///================================================================================================
class ISensorEvent : public IEvent
{
public:
  virtual time_t BeginTime() const = 0;
  virtual time_t EndTime() const = 0;
  virtual sint   SensorId() const = 0;
  virtual bool   IsClosed() const = 0;
};


///================================================================================================
/// \brief ICameraMotionEvent.
///================================================================================================
class ICameraMotionEvent : public ISensorEvent
{
public:
  struct MotionRect
  {
    float x1, y1, x2, y2;   //!< bounding rectangle of motion region
    time_t time;             //!< time of motion detection
  };

  typedef  std::vector<MotionRect>  MotionRectArr;
  
	virtual const MotionRectArr & GetLastSecondMotionRectangles() = 0;
  virtual const MotionRectArr & GetMotionRectangles() const = 0;
};

} // namespace Elvees

#endif // __DVR_EVENT_INTERFACE_H__

