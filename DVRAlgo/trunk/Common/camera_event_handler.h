#ifndef __DVR_CAMERA_EVENT_HANDLER_H__
#define __DVR_CAMERA_EVENT_HANDLER_H__

#include "events.h"
#include "motion_region.h"

namespace dvralgo
{

///================================================================================================
/// \brief CameraEventHandler.
///================================================================================================
class CameraEventHandler
{
public:
  dvralgo::MotionRegionArr       m_motionRegions;
  dvralgo::CameraMotionEventArr  m_motionEvents;

  CameraEventHandler();
};

} // namespace dvralgo

#endif // __DVR_CAMERA_EVENT_HANDLER_H__

