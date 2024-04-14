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
#include "stdafx.h"
#include "HourLineRender.h"
//======================================================================================//
//                              class HourTimeLineRender                                //
//======================================================================================//

HourTimeLineRender::HourTimeLineRender(size_t IntermediateMarkNumber):
	ITimeLineRender( 
					c_MinutesInHours * c_SecondsInMinute / IntermediateMarkNumber,	
					1.0f/ c_MinutesInHours  
					)
{
}

HourTimeLineRender::~HourTimeLineRender()
{
}