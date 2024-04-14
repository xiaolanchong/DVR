//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображает сетку времени с разрешением в 1 мин

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MINUTE_TIME_LINE_RENDER_4145711417996625_
#define _MINUTE_TIME_LINE_RENDER_4145711417996625_

#include "ITimeLineRender.h"

//======================================================================================//
//                              class MinuteTimeLineRender                              //
//======================================================================================//

//! \brief Отображает сетку времени с разрешением в 1 мин
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class MinuteTimeLineRender : public ITimeLineRender
{
	virtual tm		GetNearestTime( const tm& StartTime ) const
	{
		tm NewTime( StartTime );

		const int OneUnit = GetStepInSeconds();
		if( NewTime.tm_sec )
		{
			NewTime.tm_sec = (StartTime.tm_sec / OneUnit + 1) * OneUnit;
		}
		return NewTime;
	}

	virtual bool	IsMainTimeMark( const tm& AnyTime) const 
	{
		return AnyTime.tm_sec == 0;
	}
public:
	MinuteTimeLineRender(size_t IntermediateMarkNumber);
	virtual ~MinuteTimeLineRender();

};

#endif // _MINUTE_TIME_LINE_RENDER_4145711417996625_