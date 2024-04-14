///////////////////////////////////////////////////////////////////////////////////////////////////
// i_camera_grabber.h
// ---------------------
// begin     : 2006
// modified  : 19 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ELVEES_CAMERA_GRABBER_INTERFACE_H__
#define __ELVEES_CAMERA_GRABBER_INTERFACE_H__

#include "../Common/integers.h"
#include "i_video_reader.h"

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Interface class of any camera analyzer. 
///
/// Working scenario:
/// <tt><pre>
/// ICameraGrabber * pGrab = CreateCameraGrabber(...);
/// while (...)
/// {
///   pGrab->GetFrame(...);
///   ... do processing ...
/// }
/// </pre></tt>
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICameraGrabber : public Elvees::IBaseVideoReader
{
public:
};

typedef  boost::shared_ptr<ICameraGrabber>  ICameraGrabberPtr;

} // namespace Elvees

#endif // __ELVEES_CAMERA_GRABBER_INTERFACE_H__

