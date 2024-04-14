//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображает сетку времени с разрешением в 1 час

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DAY_TIME_LINE_RENDER_8038406860283687_
#define _DAY_TIME_LINE_RENDER_8038406860283687_

#include "ITimeLineRender.h"

//======================================================================================//
//                               class DayTimeLineRender                                //
//======================================================================================//

//! \brief Отображает сетку времени с разрешением в 1 час
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class DayTimeLineRender : public ITimeLineRender
{
	virtual tm		GetNearestTime( const tm& StartTime ) const
	{
		tm NewTime( StartTime );
		NewTime.tm_sec = 0;
		NewTime.tm_min = 0;

		const int OneUnit = ( GetStepInSeconds()/ (c_SecondsInMinute * c_MinutesInHours ));
		if( NewTime.tm_hour )
		{
			NewTime.tm_hour = (StartTime.tm_hour / OneUnit + 1) * OneUnit;
		}
		return NewTime;
	}

	virtual bool	IsMainTimeMark( const tm& AnyTime) const 
	{
		return AnyTime.tm_hour == 0;
	}

public:
	DayTimeLineRender(unsigned int IntermediateMarkNumber);
	virtual ~DayTimeLineRender();
};

#endif // _DAY_TIME_LINE_RENDER_8038406860283687_