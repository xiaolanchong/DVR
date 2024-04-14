///////////////////////////////////////////////////////////////////////////////////////////////////
// i_camera_analyzer.h
// ---------------------
// begin     : 2006
// modified  : 19 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ELVEES_CAMERA_ANALYZER_INTERFACE_H__
#define __ELVEES_CAMERA_ANALYZER_INTERFACE_H__

#include "../Common/integers.h"
#include "i_query_info.h"

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Interface class of any camera analyzer. 
///
/// Working scenario:
/// <tt><pre>
/// CreateCameraAnalyzer(...);
/// SetData( ... any initialization data ... );
/// SetData( ... any initialization data ... );
/// while (...)
/// {
///   ProcessImage(...);
///   if (...) GetData( ... );
/// }
/// </pre></tt>
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICameraAnalyzer : public Elvees::IQueryInfo
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Virtual destructor ensures safe deallocation.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual ~ICameraAnalyzer() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function processes the current frame.
///
/// \param  pHeader    pointer to bitmap header.
/// \param  pBytes     pointer to bitmap contents.
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Process( const BITMAPINFOHEADER * pHeader,
                      const ubyte            * pBytes,
                      ulong                    timeStamp ) = 0;

};

typedef  boost::shared_ptr<ICameraAnalyzer>  ICameraAnalyzerPtr;

} // namespace Elvees

#endif // __ELVEES_CAMERA_ANALYZER_INTERFACE_H__

