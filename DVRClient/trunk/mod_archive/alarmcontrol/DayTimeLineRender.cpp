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

#include "stdafx.h"
#include "DayTimeLineRender.h"

//======================================================================================//
//                               class DayTimeLineRender                                //
//======================================================================================//

DayTimeLineRender::DayTimeLineRender(unsigned int IntermediateMarkNumber):
	ITimeLineRender( 
				c_MinutesInHours * c_SecondsInMinute * c_HoursInDay / IntermediateMarkNumber,	
				1.0f/ (c_MinutesInHours * c_HoursInDay )
				)
{
}

DayTimeLineRender::~DayTimeLineRender()
{
}