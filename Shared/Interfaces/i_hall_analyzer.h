///////////////////////////////////////////////////////////////////////////////////////////////////
// i_hall_analyzer.h
// ---------------------
// begin     : 2006
// modified  : 19 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ELVEES_HALL_ANALYZER_INTERFACE_H__
#define __ELVEES_HALL_ANALYZER_INTERFACE_H__

#include "../Common/integers.h"
#include "i_query_info.h"

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Interface class of any hall analyzer.
///
/// Working scenario:
/// <tt><pre>
/// IHallAnalyzer * pHall = CreateHallAnalyzer(...);
/// pHall->SetData( ... any initialization data ... );
/// pHall->SetData( ... any initialization data ... );
/// while(...)
/// {
///   for (all cameras)
///   {
///     pCamera->GetData( ... from camera analyzer ... );
///     pHall->SetData( ... to hall analyzer ... );
///   }
///
///   Process(...);
///
///   for (all cameras)
///   {
///     pHall->GetData( ... from hall analyzer ... );
///     pCamera->SetData( ... to camera analyzer ... );
///   }
///
///   pHall->GetData( ... get overall result ... );
/// }
/// </pre></tt> */
///////////////////////////////////////////////////////////////////////////////////////////////////
class IHallAnalyzer : public Elvees::IQueryInfo
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Virtual destructor ensures safe deallocation.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual ~IHallAnalyzer() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function processes accumulated data obtained from all cameras.
///
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Process( ulong timeStamp ) = 0;

};

typedef  boost::shared_ptr<IHallAnalyzer>  IHallAnalyzerPtr;

} // namespace Elvees

#endif // __ELVEES_HALL_ANALYZER_INTERFACE_H__

