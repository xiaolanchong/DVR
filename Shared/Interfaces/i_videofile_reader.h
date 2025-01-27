///////////////////////////////////////////////////////////////////////////////////////////////////
// i_videofile_reader.h
// ---------------------
// begin     : Nov 2005
// modified  : 23 Nov 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ELVEES_VIDEOFILE_READER_INTERFACE_H__
#define __ELVEES_VIDEOFILE_READER_INTERFACE_H__

#include "../Common/integers.h"
#include "i_video_reader.h"

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Base class of any video file reader.
///
/// It is assumed that any member function can throw 'VideoReaderException' on failure.
///////////////////////////////////////////////////////////////////////////////////////////////////
class IVideoFileReader : public Elvees::IBaseVideoReader
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function moves internal pointer to the beginning of specified frame.
///
/// \param  frameNo  the index of desired frame.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual void Seek( uint frameNo ) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero if the end of a file has been reached.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool IsEnd() const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the value of frame-per-seconds rate.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual float GetFPS() const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the number of frames in a video file.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual uint GetNumberOfFrames() const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the index of the last read frame.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual uint GetCurrentFrameIndex() const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns some description of the currenly opened video file.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual const char * FileDescription() const = 0;

};

} // namespace Elvees

#endif // __ELVEES_VIDEOFILE_READER_INTERFACE_H__

