//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Элемент вывода информации о ситуациях за промежуток времени

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "AlarmViewControl.h"
#include "MinuteTimeLineRender.h"
#include "HourLineRender.h"
#include "DayTimeLineRender.h"

#undef min
#undef max

using namespace boost::gregorian;
//======================================================================================//
//                                class AlarmViewControl                                //
//======================================================================================//

AlarmViewControl::AlarmViewControl( wxWindow* pParentWnd, boost::function1< void, time_t> fnOnTimeChanged ):
	wxScrolledWindow( pParentWnd, wxID_ANY ) ,
	m_bValidTime(false),
	m_fnOnTimeChanged( fnOnTimeChanged ) 
{
	SetScrollbars( 1, 1, 0, 0, 0, 0, false);
	SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );

	m_pTimeLineRender.reset( new HourTimeLineRender(4) );
}

AlarmViewControl::~AlarmViewControl()
{
}

BEGIN_EVENT_TABLE( AlarmViewControl, wxScrolledWindow )
	EVT_ERASE_BACKGROUND( AlarmViewControl:: OnEraseBkgrnd ) 
	EVT_LEFT_DOWN( AlarmViewControl::OnLButton )
END_EVENT_TABLE()

void	AlarmViewControl::OnLButton( wxMouseEvent& ev )
{
	wxSize sizeArea = GetVirtualSize();


	if( !m_CurrentTime.get_ptr() || !m_bValidTime )
	{
		return;
	}
	wxSize sizeOffsetFromTopLeft = GetStartOffset();
	wxSize sizeRealCoord ( ev.GetX(), ev.GetY() );
	sizeRealCoord -= sizeOffsetFromTopLeft;

	wxSize VisibleOffset;
	GetViewStart(&VisibleOffset.x, &VisibleOffset.y);
	sizeRealCoord += VisibleOffset;
	if( sizeRealCoord.GetY() < 0 )
	{
		return;
	}

	if( sizeRealCoord.GetY() <= sizeOffsetFromTopLeft.GetY() )
	{
		return;
	}

	double TimeRatio = sizeRealCoord.GetY() / double( sizeArea.GetY() - sizeOffsetFromTopLeft.GetY() );

	time_t NewTime = (m_EndTime - m_StartTime) * TimeRatio + m_StartTime;

	_ASSERTE( NewTime <= m_EndTime );
	_ASSERTE( NewTime >= m_StartTime );

	m_fnOnTimeChanged( NewTime );
}

void AlarmViewControl::OnDraw(wxDC& dc)
{
	wxRect z(GetMaxVisibleSize());
//	Draw( dc, z );

	wxBitmap bmp( z.GetWidth(), z.GetHeight() );
	wxMemoryDC MemDC;
	MemDC.SelectObject( bmp );
	Draw( MemDC, z );
	dc.Blit( wxPoint(0, 0), z.GetSize(), &MemDC, wxPoint(0, 0) );
}

void	AlarmViewControl::Draw(wxDC & dc, const wxRect& DrawArea)
{
	wxSize sizeClient ;
	GetVirtualSize( &sizeClient.x, &sizeClient.y );
	sizeClient.x = GetVirtualWidth();

	wxBrush brBackground( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE  ) );
	wxPen penBackground( *wxBLACK, 1, wxTRANSPARENT );

	dc.SetBrush( brBackground );
	dc.SetPen(penBackground);
	dc.DrawRectangle( DrawArea /*wxPoint(0, 0), GetMaxVisibleSize() */);

	if ( !m_bValidTime )
	{
		DrawControlWithoutData( dc );
		return;
	}

	int xoff = GetDatetimeAreaWidth();
	int yoff = GetCameraNumberAreaHeight();

	wxBrush br(  *wxBLACK, wxTRANSPARENT  );
	dc.SetBrush( br );

	wxPen penBound( *wxBLACK, 2 );
	wxPen penTimeLine( *wxBLACK, 1 );

	dc.SetFont( GetFont() );

	dc.SetPen( penBound );
	int TotalHeight = m_pTimeLineRender->GetOffsetInPixels( m_EndTime - m_StartTime );
	dc.DrawRectangle( xoff, yoff , sizeClient.GetX() - xoff , TotalHeight );

	wxRect DrawRect( xoff, yoff , sizeClient.GetX() - xoff, TotalHeight );

	DrawCurrentTime( dc, DrawRect);
	m_pTimeLineRender->DrawTimeLines( dc, DrawRect,	m_StartTime, m_EndTime );
	DrawCameraLines( dc, DrawRect );
}

wxSize	AlarmViewControl::GetMaxVisibleSize() const
{
	wxSize sizeClient ;
	GetVirtualSize( &sizeClient.x, &sizeClient.y );
	wxSize sizeRealClient = GetClientSize();

	sizeRealClient.x = std::max( sizeRealClient.x, sizeClient.x );
	sizeRealClient.y = std::max( sizeRealClient.y, sizeClient.y );

	return sizeRealClient;
}

void	AlarmViewControl::DrawControlWithoutData( wxDC& dc ) const
{


	wxBrush br(  *wxBLACK, wxTRANSPARENT  );
	dc.SetBrush( br );

	wxPen penBound( wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVEBORDER), 1 );

	wxSize sizeRealClient = GetMaxVisibleSize();

	dc.DrawRectangle( wxPoint(0, 0), sizeRealClient );
}

int		AlarmViewControl::GetDatetimeAreaWidth() const
{
	wxCoord x, y;
	GetTextExtent( "00.00", &x, &y );
	return x + 5;
}

int		AlarmViewControl::GetCameraNumberAreaHeight() const
{
	wxCoord x, y;
	GetTextExtent( "00", &x, &y );
	return y + 5;
}

int		AlarmViewControl::GetCameraNumberWidth() const
{
	wxCoord x, y;
	GetTextExtent( "00", &x, &y );
	return 2 * x;
}

void AlarmViewControl::SetPeriod( time_t StartTime, time_t EndTime, const CameraAlarmMap_t& CameraAlarmMap )
{
	_ASSERTE( StartTime <= EndTime );
	m_CurrentTime = boost::optional<time_t>();
	m_StartTime = StartTime;
	m_EndTime	= EndTime;
	m_bValidTime  = true;
	m_CameraMap	= CameraAlarmMap;

	AdjustScrollbarsForRender();
}

void	AlarmViewControl::AdjustScrollbarsForRender()
{
	if( m_bValidTime )
	{
		int y = m_pTimeLineRender->GetOffsetInPixels( m_EndTime - m_StartTime );
		wxSize off = GetStartOffset();
		int xoff = GetVirtualWidth();
		SetScrollbars( 1, 1, xoff, y + off.GetY() , 0, 0, false);
	}
	else
	{
		SetScrollbars( 1, 1, 0, 0, 0, 0, false);
	}
}

const int		c_MinCameraSpaceWidth = 15;

int AlarmViewControl::GetVirtualWidth() const
{
	
	const size_t	c_CameraNumber = m_CameraMap.size();
	wxSize sizeClient = GetClientSize();
	int Width = sizeClient.GetX() - GetDatetimeAreaWidth();
/*
	if( Width/c_CameraNumber < c_MinCameraSpaceWidth )
	{
		return ( c_CameraNumber * c_MinCameraSpaceWidth );
	}
	else
	{
		return sizeClient.GetX();
	}*/
	return int(c_CameraNumber) * c_MinCameraSpaceWidth + GetDatetimeAreaWidth();
}

void	AlarmViewControl::DrawAlarmPartitialRect( 
			wxDC& dc, 
			const wxRect& CurrentDrawRect, 
			int StartY, int  EndY, 
			const wxColour& cl ) const
{
	wxBrush brPastAlarm( cl );
	wxPen   penPastAlarm( cl, 1 );

	dc.SetPen(   penPastAlarm );
	dc.SetBrush( brPastAlarm );

	dc.DrawRectangle(
		CurrentDrawRect.GetLeft() + 1  , 
		CurrentDrawRect.GetTop() + StartY + 1,
		c_MinCameraSpaceWidth - 3 , 
		EndY - StartY - 1 );
}

void	AlarmViewControl::DrawAlarmRect( wxDC& dc, const wxRect& CurrentDrawRect, time_t StartAlarmTime, time_t EndAlarmTime ) const
{
	StartAlarmTime	= std::max( m_StartTime, StartAlarmTime );
	EndAlarmTime	= std::min( m_EndTime, EndAlarmTime );
	int StartOffset = m_pTimeLineRender->GetOffsetInPixels( StartAlarmTime - m_StartTime   )	;
	int EndOffset	= m_pTimeLineRender->GetOffsetInPixels( EndAlarmTime - m_StartTime  );

	if( !m_CurrentTime.get_ptr() || m_CurrentTime.get() <= StartAlarmTime )
	{
		// full future
		DrawAlarmPartitialRect( dc, CurrentDrawRect, StartOffset, EndOffset, *wxRED );
	}
	else if ( m_CurrentTime.get() >= EndAlarmTime )
	{
		// full past
		DrawAlarmPartitialRect( dc, CurrentDrawRect, StartOffset, EndOffset, wxColour( 255, 0, 255 ) );
	}
	else
	{
		int CurrentOffset = m_pTimeLineRender->GetOffsetInPixels( m_CurrentTime.get() - m_StartTime   );
		DrawAlarmPartitialRect( dc, CurrentDrawRect, StartOffset,       CurrentOffset, wxColour( 255, 0, 255 ) );
		DrawAlarmPartitialRect( dc, CurrentDrawRect, CurrentOffset - 1, EndOffset, *wxRED );
	}
}

void	AlarmViewControl::DrawCameraLines( wxDC& dc, const wxRect& DrawRect ) const
{
	wxRect CurrentDrawRect(DrawRect);
	wxPen penTimeLine( *wxBLACK, 1 );

	boost::format fmtCameraName("%d");

	dc.SetTextForeground( *wxBLACK );

	CameraAlarmMap_t::const_iterator it = m_CameraMap.begin();
	for ( int off = 0 ; it != m_CameraMap.end() ; ++it)
	{
		const int CameraId = it->first; 
		const AlarmArr_t& al = it->second;

		dc.SetPen( penTimeLine );
		dc.DrawLine( CurrentDrawRect.GetLeft() , CurrentDrawRect.GetTop(), 
					 CurrentDrawRect.GetLeft() , CurrentDrawRect.GetBottom() );
		
		dc.DrawText( (fmtCameraName % CameraId).str().c_str(), CurrentDrawRect.GetLeft(), 0  );


		for ( size_t i = 0; i < al.size(); ++i )
		{
			time_t StartAlarmTime = al[i].first;
			time_t EndAlarmTime	 = al[i].second;
			if( StartAlarmTime	<	m_EndTime  &&
				EndAlarmTime	>	m_StartTime &&
				StartAlarmTime	<=	EndAlarmTime )
			{
				DrawAlarmRect( dc, CurrentDrawRect, StartAlarmTime, EndAlarmTime );
			}
		}
		CurrentDrawRect.Offset( c_MinCameraSpaceWidth, 0 );
	}
}

void	AlarmViewControl::DrawCurrentTime( wxDC& dc, const wxRect& DrawArea ) const
{
	if( !m_CurrentTime.get_ptr() || !m_bValidTime )
	{
		return;
	}
	
	time_t CurrentTime = m_CurrentTime.get();

	if( CurrentTime < m_StartTime ||
		CurrentTime > m_EndTime)
	{
		return;
	}

	int OffsetFromStartInPixels = m_pTimeLineRender->GetOffsetInPixels( difftime( CurrentTime, m_StartTime ) );

	wxPen	penLine( *wxGREEN, 1, wxTRANSPARENT );
	wxBrush	brAreaFill( wxSystemSettings::GetColour( /*wxSYS_COLOUR_INFOBK*/wxSYS_COLOUR_HIGHLIGHT ) );

	dc.SetPen(penLine);
	dc.SetBrush( brAreaFill );
	/*dc.DrawLine( DrawArea.GetLeft()+1,  DrawArea.GetTop() + OffsetFromStartInPixels,
				 DrawArea.GetRight()-1, DrawArea.GetTop() + OffsetFromStartInPixels );*/
	dc.DrawRectangle( DrawArea.GetLeft()+1,  DrawArea.GetTop() ,
					  DrawArea.GetWidth()-3, OffsetFromStartInPixels  );
}

void	AlarmViewControl::InvalidatePeriod()
{
//	m_StartTime = ptime();
//	m_EndTime	= ptime();
	m_bValidTime = false;
	m_CurrentTime = boost::optional<time_t>();
}

bool	AlarmViewControl::SetCurrentArchiveTime( time_t CurrentTime )
{
	bool res;
	if(  CurrentTime >= m_StartTime &&
		 CurrentTime <= m_EndTime )
	{
		m_CurrentTime = CurrentTime;
		res = true;
	}
	else
	{
		m_CurrentTime = boost::optional<time_t>();
		res = false;
	}
	Refresh();
	return res;
}

void	AlarmViewControl::SetTimeMode( TimeMode NewTimeMode )
{
	const size_t c_IntermediateMarkNumber = 4;
	switch(NewTimeMode) 
	{
	case  Minutes :
		m_pTimeLineRender.reset( new MinuteTimeLineRender(c_IntermediateMarkNumber) );
		break;
	case  Hours:
		m_pTimeLineRender.reset( new HourTimeLineRender(c_IntermediateMarkNumber) );
		break;
	case  Days:
		m_pTimeLineRender.reset( new DayTimeLineRender ( c_IntermediateMarkNumber )  );
		break;
	default:
		_ASSERTE(false);
	}
#if 0
	m_CurrentTime = ( m_EndTime + m_StartTime )/2;
#endif
	AdjustScrollbarsForRender();
	Refresh();
}

void AlarmViewControl::OnEraseBkgrnd(wxEraseEvent &ev)
{
//	ev.Veto();
}