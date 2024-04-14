//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображения сетки времени с интервалом в час

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   07.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _HOUR_LINE_RENDER_8389577608489390_
#define _HOUR_LINE_RENDER_8389577608489390_

#include "ITimeLineRender.h"

//======================================================================================//
//                                 class HourTimeLineRender                             //
//======================================================================================//

//! \brief Отображения сетки времени с интервалом в час
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   07.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class HourTimeLineRender : public ITimeLineRender
{
	
#if OLD	
	virtual	std::pair<int, bool>		GetFirstOffset( tm& tmStartTime ) const 
	{
		const int c_OneUnit = (c_MinutesInHours/int(m_IntermediateMarkNumber) );
		int nOffset = 0;
		
		bool bMainMark = false;
		if( tmStartTime.tm_min )
		{
			int OldMinNumber = tmStartTime.tm_min;
			tmStartTime.tm_min = (tmStartTime.tm_min / c_OneUnit + 1) * c_OneUnit;
			nOffset = 	(tmStartTime.tm_min - OldMinNumber) * 1;
			bMainMark = (c_MinutesInHours - tmStartTime.tm_min)  < c_OneUnit;
		}
		
		tmStartTime.tm_sec = 0;
		return std::make_pair( nOffset, bMainMark );
	}
#endif
	virtual tm		GetNearestTime( const tm& StartTime ) const
	{
		tm NewTime( StartTime );
		NewTime.tm_sec = 0;

		const int OneUnit = ( GetStepInSeconds()/c_SecondsInMinute);
		if( NewTime.tm_min )
		{
			NewTime.tm_min = (StartTime.tm_min / OneUnit + 1) * OneUnit;
		}
		return NewTime;
	}

	virtual bool	IsMainTimeMark( const tm& AnyTime) const 
	{
		return AnyTime.tm_sec == 0 && AnyTime.tm_min == 0;
	}
public:
	HourTimeLineRender(size_t IntermediateMarkNumber);
	virtual ~HourTimeLineRender();
};

#endif // _HOUR_LINE_RENDER_8389577608489390_