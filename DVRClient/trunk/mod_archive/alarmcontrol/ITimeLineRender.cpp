//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс для отображения сетки времени

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.06.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ITimeLineRender.h"
//======================================================================================//
//                                class ITimeLineRender                                 //
//======================================================================================//

ITimeLineRender::ITimeLineRender(int StepInSeconds, double PixelsInSecond ):
	m_StepInSeconds(StepInSeconds),
	m_PixelsInSecond(PixelsInSecond)
{
}

ITimeLineRender::~ITimeLineRender()
{
}

void	ITimeLineRender::DrawTimeLines( 
							wxDC& dc, const wxRect& DrawArea,
							time_t StartTime, time_t EndTime ) const
{
	wxPen penMainTimeLine( *wxBLACK, 1 );
	wxPen penIntermediateTimeLine( wxColour(128, 128, 128), 1 );

	tm tStartTime; //=	*localtime(&StartTime);
	localtimeWrap( StartTime, tStartTime );
	tm tCurTime = GetNearestTime( tStartTime );

	int nStartOffset = GetPeriodInSeconds( tStartTime, tCurTime ) * GetPixelsInSecond();
	nStartOffset += DrawArea.GetTop();

	time_t tmEndTime	= EndTime;

	time_t tmCurTime = mktime( &tCurTime );
	wxPen penLine(  1 );
	for ( int CurOff = nStartOffset;tmCurTime <= tmEndTime;   )
	{
		bool bMainMark = IsMainTimeMark( tmCurTime );
		if( bMainMark )
		{
			tm tCurTime;// = *localtime( &tmCurTime );
			localtimeWrap( tmCurTime, tCurTime );
			std::pair<std::string, wxColour> s = GetTextForTime( tCurTime );
			dc.SetTextForeground( s.second );
			dc.DrawText( s.first.c_str(), 1, CurOff );
			dc.SetPen( penMainTimeLine );
		}
		else
		{
			dc.SetPen( penIntermediateTimeLine );
		}
		dc.DrawLine( DrawArea.GetLeft(), CurOff, DrawArea.GetRight(), CurOff );

		tmCurTime += GetStepInSeconds();
		CurOff += GetStepInSeconds() * GetPixelsInSecond();
	}
}