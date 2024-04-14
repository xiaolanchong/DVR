#pragma once

#include "../../../Shared/Interfaces/i_camera_grabber.h"
#include "../../../Shared/Interfaces/i_message.h"

namespace Elvees
{

extern "C"
{
  Elvees::ICameraGrabber * CreateChcsvaCameraGrabber( sint cameraNo, Elvees::IMessage * pOutput );

	void DeleteChcsvaCameraGrabber();
	
  sint GetNumberOfAvailablesCameras();
}

} // namespace Elvees

