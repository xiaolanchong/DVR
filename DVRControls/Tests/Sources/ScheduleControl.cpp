/*
*	ScheduleControl(DVR)  
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ScheduleControl.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-12-20
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Schedule control implementation
*
*
*/
#include "StdAfx.h"
#include "ScheduleControl.hpp"
//////////////////////////////////////////////////////////////////////////
boost::regex gNavigationRX_Year( "^(\\d{4})$" );
boost::regex gNavigationRX_Day( "^(\\d{2})$" );
boost::regex gNavigationRX_YearMonthDay( "^(\\d{2,4})[-,/\\.](\\d{2})[-,/\\.](\\d{2})$" );
boost::regex gNavigationRX_MonthDay( "(\\d{2})\\.(\\d{2})" );

//////////////////////////////////////////////////////////////////////////
// Define a series of char arrays for short and long name strings 
const char* const gEnLongMonthNames[] = 
{
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December", "NAM" 
};

//////////////////////////////////////////////////////////////////////////
ScheduleControl::Bookmark::Bookmark( const tm& time, const std::string& message, Priority p ):
	mMessage( message ), 
	mPriority( p )
{
}

ScheduleControl::Bookmark::Bookmark( const boost::posix_time::ptime& time, const std::string& message, Priority p ):
	mMessage( message ),
	mPriority( p ),
	mTime( time )
{
}

const boost::posix_time::ptime& ScheduleControl::Bookmark::GetPTime() const
{
	return mTime;
}

const std::string& ScheduleControl::Bookmark::GetMessage() const
{
	return mMessage;
}

void ScheduleControl::Bookmark::SetMessage( const std::string& message )
{
	mMessage = message;
}

ScheduleControl::Bookmark::Priority ScheduleControl::Bookmark::GetPriority() const
{
	return mPriority;
}

void ScheduleControl::Bookmark::SetPriority( ScheduleControl::Bookmark::Priority p )
{
	mPriority = p;
}

bool ScheduleControl::Bookmark::operator==( const Bookmark& other ) const
{
	return ( mTime == other.mTime ) ? true : false;
}

bool ScheduleControl::Bookmark::operator!=( const Bookmark& other ) const
{
	return !operator==( other );
}

bool ScheduleControl::Bookmark::operator<( const Bookmark& other ) const
{
	return ( mTime < other.mTime ) ? true : false;
}

bool ScheduleControl::Bookmark::operator>( const Bookmark& other ) const
{
	return ( mTime > other.mTime ) ? true : false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ScheduleControl::Duration::Duration( const boost::posix_time::time_period& period, wxColor color ):
	mPeriod( period ),
	mColor( color )
{
}

ScheduleControl::Duration::Duration( const boost::posix_time::ptime& begin, const boost::posix_time::ptime& end, wxColor color ):
	mPeriod( boost::posix_time::time_period( begin, end ) ),
	mColor( color )
{
}
//////////////////////////////////////////////////////////////////////////
int ScheduleControl::Camera::GetIndex() const
{
	return mCameraIndex;
}
//////////////////////////////////////////////////////////////////////////

const boost::posix_time::time_period& ScheduleControl::Duration::GetPeriod() const
{
	return mPeriod;
}

boost::posix_time::ptime ScheduleControl::Duration::GetPeriodStart() const
{
	return mPeriod.begin();
}

boost::posix_time::ptime ScheduleControl::Duration::GetPeriodEnd() const
{
	return mPeriod.end();
}

wxColor ScheduleControl::Duration::GetColor() const
{
	return mColor;
}

void ScheduleControl::Duration::SetColor( wxColor color )
{
	mColor = color;
}


bool ScheduleControl::Duration::operator==( const Duration& other ) const
{
	return ( mPeriod == other.mPeriod ) ? true : false;
}

bool ScheduleControl::Duration::operator!=( const Duration& other ) const
{
	return ( mPeriod != other.mPeriod ) ? true : false;
}

bool ScheduleControl::Duration::operator<( const Duration& other ) const
{
	return ( mPeriod < other.mPeriod ) ? true : false;
}

bool ScheduleControl::Duration::operator>( const Duration& other ) const
{
	return ( mPeriod > other.mPeriod ) ? true : false;
}

//////////////////////////////////////////////////////////////////////////
ScheduleControl::Duration ScheduleControl::Duration::Span( const Duration& other )
{
	return Duration( mPeriod.span( other.mPeriod ), other.mColor );
}

ScheduleControl::Duration ScheduleControl::Duration::Merge( const Duration& other )
{
	return Duration( mPeriod.merge( other.mPeriod ), other.mColor );
}

bool ScheduleControl::Duration::Intersects( const Duration& other ) const
{
	return mPeriod.intersects( other.mPeriod );
}

bool ScheduleControl::Duration::IsNull() const
{
	return mPeriod.is_null();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ScheduleControl::Camera::Camera( int index ):
	mCameraIndex( index ),
	mBookmarkItReseted( true ),
	mDurationItReseted( true )
{
}

void ScheduleControl::Camera::Add( const Bookmark& bookmark )
{
	mBookmarks.insert( bookmark );
}

void ScheduleControl::Camera::Remove( const Bookmark& bookmark )
{
	BookmarkSet::iterator it = mBookmarks.find( bookmark );
	if( it != mBookmarks.end()  ){
		mBookmarks.erase( it );
	}
}

void ScheduleControl::Camera::ClearBookmarks()
{
	mBookmarks.clear();
}


ScheduleControl::Bookmark& ScheduleControl::Camera::GetCurrentBookmark()
{
	return *mBookmarksIt;
}

bool ScheduleControl::Camera::MoveToNextBookmark()
{
	if( mBookmarks.empty() ){
		return false;
	}
	else if( mBookmarkItReseted )
	{
		mBookmarksIt = mBookmarks.begin();
		mBookmarkItReseted = false;
		return true;
	}
	else if( ++mBookmarksIt == mBookmarks.end() )
	{
		--mBookmarksIt;
		return false;
	}
	return true;
}

void ScheduleControl::Camera::ResetBookmarks()
{
	mBookmarkItReseted = true;
	mBookmarksIt = mBookmarks.end();
}

void ScheduleControl::Camera::ResetBookmarks( const boost::posix_time::ptime& time )
{
	ScheduleControl::Bookmark bookmark( time );
	mBookmarksIt = mBookmarks.find( bookmark );
	
	if( (mBookmarksIt != mBookmarks.end()) 
		&& (mBookmarksIt != mBookmarks.begin()) )
	{
		--mBookmarksIt;
	}
	else
	{
		mBookmarkItReseted = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void ScheduleControl::Camera::Add( const Duration& duration )
{
	//Search for intersections periods
	for( DurationSet::iterator it = mPeriods.begin(); it != mPeriods.end(); ++it  )
	{
		if( duration.Intersects( *it ) )
		{
			Duration newPeriod = it->Merge( duration );
			if( !newPeriod.IsNull() )
			{
				mPeriods.erase( it );
				mPeriods.insert( newPeriod );
				return;
			}
		}
	}

	mPeriods.insert( duration );
}

void ScheduleControl::Camera::AddToLast( const Duration& duration )
{
	//If periods map is empty, 
	//insert first period and return
	if( mPeriods.empty() )
	{
		mPeriods.insert( duration );
		return;
	}

	//Otherwise, 
	DurationSet::iterator it( mPeriods.rbegin().base() );
	Duration newPeriod = it->Span( duration );
	if( !newPeriod.IsNull() )
	{
		mPeriods.erase( it );
		mPeriods.insert( newPeriod );
	}
}

void ScheduleControl::Camera::Remove( const Duration& duration )
{
	DurationSet::iterator it 
		= mPeriods.find( duration );

	if( it != mPeriods.end() )
	{
		mPeriods.erase( it );
	}
}

void ScheduleControl::Camera::ClearPeriods()
{
	mPeriods.clear();
}

ScheduleControl::Duration& ScheduleControl::Camera::GetCurrentPeriod()
{
	return *mDurationIt;
}

bool ScheduleControl::Camera::MoveToNextPeriod()
{
	if( mPeriods.empty() ){
		return false;
	}
	else if( mDurationItReseted )
	{
		mDurationIt = mPeriods.begin();
		mDurationItReseted = false;
		return true;
	}
	else if( ++mDurationIt == mPeriods.end() )
	{
		--mDurationIt;
		return false;
	}
	return true;
}

void ScheduleControl::Camera::ResetPeriods()
{
	mDurationIt = mPeriods.end();	
	mDurationItReseted = true;
}

void ScheduleControl::Camera::ResetPeriods( const boost::posix_time::time_period& period )
{
	ScheduleControl::Duration duration( period );
	mDurationIt = mPeriods.find( duration );

	if( (mDurationIt != mPeriods.end()) 
		&& (mDurationIt != mPeriods.begin()) )
	{
		--mDurationIt;
	}
	else
	{
		mDurationItReseted = true;
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ScheduleControl::ScheduleControl(  wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size ):
	wxPanel( parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE | wxSIMPLE_BORDER ),
	mNavigateField( this, mEditControlID ),
	mDefaultTime( boost::gregorian::date( 1970,1,1 ), boost::posix_time::time_duration( 0,0,0 ) )
{
	boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();
	mCurrentTime =
		boost::posix_time::ptime( time.date(), boost::posix_time::time_duration( time.time_of_day().hours(), 0, 0 ) ) ;

	mDimensions.resize( SCD_DimensionsNum );
	mFonts.resize( SCF_FontsNum );
	mPens.resize( SCP_PensNum );
	mAreas.resize( SCA_AreasNum );

	//Default dimesion values
	mDimensions[SCD_HeaderHeight] = 20;
	mDimensions[SCD_ScrollBarWidth] = 14;
	mDimensions[SCD_ScrollBarHalfWidth] = mDimensions[SCD_ScrollBarWidth] / 2;
	mDimensions[SCD_DateTimeWidth] = mDimensions[SCD_ScrollBarWidth] * 4;
	mDimensions[SCD_StatusBarHeight] = 20;
	mDimensions[SCD_CamButtonWidth] = 28;
	mDimensions[SCD_HourScalePix] = 14;
	mDimensions[SCD_MinuteScalePix] = 14 / 60;
	mDimensions[SCD_ScrollBarBorderOffset] = 4;
	mDimensions[SCD_BookmarkRadius] = 4;
	mDimensions[SCD_NavigateFieldOffset] = 3;
	mDimensions[SCD_ZoomButtonWidth] = mDimensions[SCD_DateTimeWidth] / 2;
	mDimensions[SCD_ZoomButtonRadius] = 6;
	mDimensions[SCD_FirstCameraIDX] = 0;
	mDimensions[SCD_NavigationTextHandled] = 0;
	mDimensions[SCD_NavigationWidth] = 70;

	mDimensions[SCD_NavigationMinYear] = 1970;
	mDimensions[SCD_NavigationMaxYear] = 2055;

	mDimensions[SCD_DateTimeRectX0] = 0;
	mDimensions[SCD_DateTimeRectY0] = mDimensions[SCD_HeaderHeight];
	mDimensions[SCD_DateTimeRectWidth] = mDimensions[SCD_DateTimeWidth]; 
	mDimensions[SCD_DateTimeRectHeight] = 0;
	mDimensions[SCD_StatusWithScrollbarWidth] = mDimensions[SCD_StatusBarHeight] + mDimensions[SCD_ScrollBarWidth];

	//Fonts
	mFonts[SCF_DateTimeFont] = wxFont( 
		7, 
		wxFONTFAMILY_DEFAULT, 
		wxFONTSTYLE_NORMAL, 
		wxFONTWEIGHT_NORMAL,
		false, 
		"Tahoma",
		wxFONTENCODING_DEFAULT );

	mFonts[SCF_DateTimeFontBold] = wxFont( 
		7, 
		wxFONTFAMILY_DEFAULT, 
		wxFONTSTYLE_NORMAL, 
		wxFONTWEIGHT_BOLD,
		false, 
		"Tahoma",
		wxFONTENCODING_DEFAULT );

	mFonts[SCF_CamButtonFont] = wxFont( 
		7, 
		wxFONTFAMILY_DEFAULT, 
		wxFONTSTYLE_NORMAL, 
		wxFONTWEIGHT_BOLD,
		false, 
		"Tahoma",
		wxFONTENCODING_DEFAULT );

	//Pens
	mPens[SCP_TrackerPen] = wxPen( wxColour( 255, 0, 0 ) );

	//Start refresh timer
	mTimer.SetOwner( this, mTimerID );
	mTimer.Start( 1000 );

	//Navigate field control
	mNavigateField.SetFont( mFonts[SCF_DateTimeFont] );
	mNavigateField.SetWindowStyle( wxSIMPLE_BORDER );
	mNavigateField.SetBackgroundColour( wxColour( 192,192,192 ) );

	mNavigateField.Connect( mEditControlID,
		wxEVT_KEY_UP,
		wxKeyEventHandler(ScheduleControl::OnKeyUp ), NULL, this );

	mNavigateField.Connect( mEditControlID,
		wxEVT_KEY_DOWN,
		wxKeyEventHandler(ScheduleControl::OnKeyDown ), NULL, this );

#ifdef DEV_TEST_MODE
	DebugInit();
#endif

}

ScheduleControl::~ScheduleControl()
{

}

void ScheduleControl::DebugInit()
{
	AddCamera( 0 );
	AddCamera( 1 );
	AddCamera( 2 );
	AddCamera( 3 );
	AddCamera( 4 );
	AddCamera( 5 );
	AddCamera( 6 );
	AddCamera( 7 );
	AddCamera( 8 );
	AddCamera( 9 );
	AddCamera( 10 );


	mCameras[0].Add( Duration( 
		boost::posix_time::time_period( 
			boost::posix_time::second_clock::local_time(), 
			boost::posix_time::time_duration( 2,0,0 ) ),wxColour(255,0,0) ) );

	mCameras[1].Add( Duration( 
		boost::posix_time::time_period( 
			boost::posix_time::second_clock::local_time(),
			boost::posix_time::time_duration( 2,30,0 ) ), wxColour(0,255,0) ));

	mCameras[4].Add( Duration( 
		boost::posix_time::time_period( 
			boost::posix_time::second_clock::local_time(),
			boost::posix_time::time_duration( 5,30,0 ) ), wxColour(255,255,0) ) );

	//mCameras[6].second.Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 2,0,0 ) ), 80 );

	//mCameras[5].second.Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 2,30,0 ) ), 90 );

	//mCameras[1].second.Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 1,30,0 ) ), 100 );

	//mCameras[0].second.Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 200,0,0 ) ), 150 );

	mCameras[0].Add( Bookmark( 
		(boost::posix_time::second_clock::local_time()+boost::posix_time::hours(5) ),
		"This is simple bookmark" ) 
		);

	mCameras[1].Add( Bookmark( 
		(boost::posix_time::second_clock::local_time()+boost::posix_time::hours(5) ),
		"This is simple bookmark", Bookmark::BP_Low ) 
		);

	mCameras[2].Add( Bookmark( 
		(boost::posix_time::second_clock::local_time()+boost::posix_time::hours(5) ),
		"This is simple bookmark", Bookmark::BP_High ) 
		);


	//mCameras[5].Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 255,30,0 ) ), 200 );

	//mCameras[2].Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 1,10,0 ) ), 250 );

	//mCameras[0].Add( Camera::Duration( 
	//	boost::posix_time::time_duration( boost::posix_time::second_clock::local_time(),
	//	boost::posix_time::time_duration( 2,0,0 ) ), 250 );

	mCameras[5].Add( Duration( 
		boost::posix_time::time_period( 
			boost::posix_time::second_clock::local_time(),
			boost::posix_time::time_duration( 2000,30,0 ) ),
		wxColour(255,255,255) ));

	mCameras[5].Add( Bookmark(
		boost::posix_time::second_clock::local_time(), 
		"This is simple bookmark" ) );


}
//////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( ScheduleControl, wxPanel )
	EVT_PAINT( ScheduleControl::OnPaint )
	EVT_ERASE_BACKGROUND( ScheduleControl::OnEraseBackground )
	EVT_SIZE( ScheduleControl::OnSize )
	EVT_TEXT( mEditControlID, ScheduleControl::OnNavigateField )
	EVT_KEY_UP( ScheduleControl::OnKeyUp )
	EVT_KEY_DOWN( ScheduleControl::OnKeyDown )
	EVT_TIMER( mTimerID, ScheduleControl::OnTimer )
	EVT_LEFT_DOWN( ScheduleControl::OnLButtonDown )
	EVT_LEFT_UP( ScheduleControl::OnLButtonUp )
	EVT_RIGHT_DOWN( ScheduleControl::OnRButtonDown )
	EVT_RIGHT_UP( ScheduleControl::OnRButtonUp )
	EVT_MOUSEWHEEL( ScheduleControl::OnMouseWheel )
	EVT_MOTION( ScheduleControl::OnMouseMove )
	EVT_ENTER_WINDOW( ScheduleControl::OnMouseEnter )
	EVT_LEAVE_WINDOW( ScheduleControl::OnMouseLeave )
END_EVENT_TABLE()
//////////////////////////////////////////////////////////////////////////
void ScheduleControl::OnTimer( wxTimerEvent& event )
{
	//Update current time
	Refresh( false );
}

void ScheduleControl::OnEraseBackground( wxEraseEvent& event )
{
	//Keep it blank to avoid flickering
}

void ScheduleControl::OnPaint( wxPaintEvent& event )
{
	wxBufferedPaintDC dc(this);

	dc.BeginDrawing();

	dc.SetBackground( wxBrush( wxColour( 180,180,180 ) ) );
	dc.Clear();

	//Draw clock field
	DrawClock( dc );

	//Draw date time
	DrawDateTime( dc );

	//Camera buttons field
	DrawCamTrack( dc );

	//Draw statusbar pane
	DrawStatusbar( dc );

	//Draw scrollbars
	DrawScrollbars( dc );

	//Draw clock tracker
	DrawTracker( dc );

	if( mDimensions[SCD_NavigationTextHandled] == 1 ){
		DrawNavigationHint( dc, mNavigationHint );
	}

	dc.EndDrawing();
}

void ScheduleControl::OnSize( wxSizeEvent& event )
{
	wxSize size = event.GetSize();

	//Dimensions update
	mDimensions[SCD_ControlWidth] = size.x;
	mDimensions[SCD_ControlHeight] = size.y;
	mDimensions[SCD_CamTrackRectX0] = mDimensions[SCD_DateTimeWidth];
	mDimensions[SCD_CamTrackRectY0] = 0;
	mDimensions[SCD_CamTrackRectWidth] = mDimensions[SCD_ControlWidth] - mDimensions[SCD_DateTimeWidth];
	mDimensions[SCD_CamTrackRectHeight] = mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth];
	mDimensions[SCD_DateTimeRectHeight] = size.y - mDimensions[SCD_StatusBarHeight] - mDimensions[SCD_HeaderHeight];

	//Bottom sb
	mAreas[SCA_SB_BottomLeftArrow].first = wxRect(
		mDimensions[SCD_DateTimeWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth], 
		mDimensions[SCD_ScrollBarWidth], 
		mDimensions[SCD_ScrollBarWidth] );
	mAreas[SCA_SB_BottomLeftArrow].second = false;

	mAreas[SCA_SB_BottomRightArrow].first = wxRect(
		mDimensions[SCD_ControlWidth] - mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth], 
		mDimensions[SCD_ScrollBarWidth], 
		mDimensions[SCD_ScrollBarWidth] );
	mAreas[SCA_SB_BottomRightArrow].second = false;

	//Year sb
	mAreas[SCA_SB_DateTimeYearUpArrow].first = wxRect(
		0 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_HeaderHeight],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	mAreas[SCA_SB_DateTimeYearDownArrow].first = wxRect(
		0 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	//Month sb
	mAreas[SCA_SB_DateTimeMonthUpArrow].first = wxRect(
		1 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_HeaderHeight],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	mAreas[SCA_SB_DateTimeMonthDownArrow].first = wxRect(
		1 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	//Day sb
	mAreas[SCA_SB_DateTimeDayUpArrow].first = wxRect(
		2 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_HeaderHeight],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	mAreas[SCA_SB_DateTimeDayDownArrow].first = wxRect(
		2 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	//Hour sb
	mAreas[SCA_SB_DateTimeHourUpArrow].first = wxRect(
		3 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_HeaderHeight],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeYearUpArrow].second = false;

	mAreas[SCA_SB_DateTimeHourDownArrow].first = wxRect(
		3 * mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusWithScrollbarWidth],
		mDimensions[SCD_ScrollBarWidth],
		mDimensions[SCD_ScrollBarWidth]	);
	mAreas[SCA_SB_DateTimeHourUpArrow].second = false;

	//Navigate field
	mNavigateField.SetSize( 
		mDimensions[SCD_DateTimeWidth] + mDimensions[SCD_NavigateFieldOffset],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight] + mDimensions[SCD_NavigateFieldOffset],
		mDimensions[SCD_NavigationWidth],
		mDimensions[SCD_StatusBarHeight] - 2 * mDimensions[SCD_NavigateFieldOffset] ,
		wxSIZE_USE_EXISTING );

	mDimensions[SCD_HoursInCamTrack] = mDimensions[SCD_ControlHeight] / mDimensions[SCD_HourScalePix];
}

void ScheduleControl::OnMouseEnter( wxMouseEvent& event )
{
	for( size_t i = 0; i < mAreas.size(); ++i )
	{
		mAreas[i].second = false;
	}
}

void ScheduleControl::OnMouseLeave( wxMouseEvent& event )
{
	for( size_t i = 0; i < mAreas.size(); ++i )
	{
		mAreas[i].second = false;
	}
}

void ScheduleControl::OnMouseMove( wxMouseEvent& event )
{
}

void ScheduleControl::OnMouseWheel( wxMouseEvent& event )
{
	if( event.GetWheelRotation() > 0 )
		mCurrentTime += boost::posix_time::time_duration( 4, 0, 0 );
	else
		mCurrentTime -= boost::posix_time::time_duration( 4, 0, 0 );

	Refresh( false );
}

void ScheduleControl::OnRButtonDown( wxMouseEvent& event )
{

}

void ScheduleControl::OnRButtonUp( wxMouseEvent& event )
{

}

void ScheduleControl::OnLButtonUp( wxMouseEvent& event )
{
	bool updateScreen = false;
	wxPoint pt = event.GetPosition();

	for( size_t i = 0; i < mAreas.size(); ++i )
	{
		mAreas[i].second = false;
	}

	Refresh( false );
}

void ScheduleControl::OnLButtonDown( wxMouseEvent& event )
{
	bool isInside;
	wxPoint pt = event.GetPosition();

	for( size_t i = 0; i < mAreas.size(); ++i )
	{
		isInside = mAreas[i].first.Inside( pt );
		if( isInside )
		{
			mAreas[i].second = true;

			if( mAreas[i].second )
			{
				switch( i )
				{
				case SCA_SB_DateTimeHourUpArrow:
					{
						mCurrentTime += boost::posix_time::time_duration( 1, 0, 0 );
					}
					break;
				case SCA_SB_DateTimeHourDownArrow:
					{
						mCurrentTime -= boost::posix_time::time_duration( 1, 0, 0 );
					}
					break;
				case SCA_SB_DateTimeDayUpArrow:
					{
						mCurrentTime += boost::posix_time::time_duration( 24, 0, 0 );
					}
					break;

				case SCA_SB_DateTimeDayDownArrow:
					{
						mCurrentTime -= boost::posix_time::time_duration( 24, 0, 0 );
					}
					break;

				case SCA_SB_DateTimeMonthUpArrow:
					{
						boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
						boost::gregorian::month_iterator monthIt( mCurrentTime.date() );
						++monthIt;

						mCurrentTime = boost::posix_time::ptime( *monthIt, timeCurrent );
					}
					break;
				case SCA_SB_DateTimeMonthDownArrow:
					{
						boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
						boost::gregorian::month_iterator monthIt( mCurrentTime.date() );
						--monthIt;

						mCurrentTime = boost::posix_time::ptime( *monthIt, timeCurrent );
					}
					break;
				case SCA_SB_DateTimeYearUpArrow:
					{
						boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
						boost::gregorian::year_iterator yearIt( mCurrentTime.date() );
						++yearIt;

						mCurrentTime = boost::posix_time::ptime( *yearIt, timeCurrent );
					}
					break;
				case SCA_SB_DateTimeYearDownArrow:
					{
						boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
						boost::gregorian::year_iterator yearIt( mCurrentTime.date() );
						--yearIt;

						mCurrentTime = boost::posix_time::ptime( *yearIt, timeCurrent );
					}
					break;
				}
			}
		}
		else
		{
			mAreas[i].second = false;
		}
	}

	Refresh( false );
}

void ScheduleControl::OnKeyUp( wxKeyEvent& event )
{

}

void ScheduleControl::OnKeyDown( wxKeyEvent& event )
{
	int keyCode = event.GetKeyCode();
	bool skip = true;
	bool repaint = false;

	switch( keyCode )
	{
	case WXK_UP:
		{
			skip = false;
			repaint = true;
			if( event.ShiftDown() )
				mCurrentTime += boost::posix_time::time_duration( 24, 0, 0 );
			else
				mCurrentTime += boost::posix_time::time_duration( 1, 0, 0 );
		}
		break;
	case WXK_DOWN:
		{
			skip = false;
			repaint = true;
			if( event.ShiftDown() )
				mCurrentTime -= boost::posix_time::time_duration( 24, 0, 0 );
			else
				mCurrentTime -= boost::posix_time::time_duration( 1, 0, 0 );
		}
		break;
	case WXK_LEFT:
		{
			repaint = true;
			--mDimensions[SCD_FirstCameraIDX];
			if( mDimensions[SCD_FirstCameraIDX] < 0 ){
				mDimensions[SCD_FirstCameraIDX] = 0;
			}
		}
		break;
	case WXK_RIGHT:
		{
			repaint = true;
			++mDimensions[SCD_FirstCameraIDX];
			if( mDimensions[SCD_FirstCameraIDX] >= ((int)mCameras.size() - 1 )){
				mDimensions[SCD_FirstCameraIDX] = (int)mCameras.size() - 1;
			}

		}
		break;
	case WXK_RETURN:
		{
			if( mDimensions[SCD_NavigationTextHandled] == 1 )
			{
				repaint = true;
				mCurrentTime = mNavigateTime;
				mDimensions[SCD_NavigationTextHandled] = 0;
				mNavigateField.SetSelection( 0, mNavigateField.GetLastPosition() );

			}
		}
		break;
	}

	//Refresh
	if( repaint ){
		Refresh( false );
	}

	//Skip event
	event.Skip( skip );
}

void ScheduleControl::OnNavigateField(wxCommandEvent& event )
{
	std::string text = mNavigateField.GetValue();
	
	if( HandleNavigationText( text ) )
	{
		mNavigateField.SetBackgroundColour( wxColor( 78,130,185 ) );
	}
	else
	{
		mNavigateField.SetBackgroundColour( wxColor( 192,192,192 ) );
	}

	Refresh(false);
	mNavigateField.Refresh(false);

}

//////////////////////////////////////////////////////////////////////////
bool ScheduleControl::HandleNavigationText( const std::string& text )
{

	bool isHandleSuccess = false;
	boost::cmatch what;
	mDimensions[SCD_NavigationTextHandled] = 0;

	//boost::regex gNavigationRX_Year( "^(\\d{4})$" )
	if( boost::regex_search( text.c_str(),  what,  gNavigationRX_Year ) )
	{
		int year = atoi( what[1].str().c_str() );
		if( year >= mDimensions[SCD_NavigationMinYear] && year <= mDimensions[SCD_NavigationMaxYear] )
		{
			boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
			boost::gregorian::date dt = mCurrentTime.date();
			int yearDiff = year - dt.year();

			dt += boost::gregorian::years(yearDiff);
			mNavigateTime = boost::posix_time::ptime( dt, timeCurrent );
			mNavigationHint = ">year";

			mDimensions[SCD_NavigationTextHandled] = 1;
			isHandleSuccess = true;
		}
	}//boost::regex gNavigationRX_Day( "^\\d{2}$" )
	else if( boost::regex_search( text.c_str(),  what,  gNavigationRX_Day ) )
	{
		int day = atoi( what[1].str().c_str() );
		if( day > 0 && day <= mCurrentTime.date().end_of_month().day() )
		{
			boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
			boost::gregorian::date dt = mCurrentTime.date();
			int dayDiff = day - dt.day();

			dt += boost::gregorian::days(dayDiff);
			mNavigateTime = boost::posix_time::ptime( dt, timeCurrent );

			mNavigationHint = ">day";
			mDimensions[SCD_NavigationTextHandled] = 1;
			isHandleSuccess = true;
		}
	}//boost::regex gNavigationRX_YearMonthDay( "^(\\d{2})[-,/\\.](\\d{2})[-,/\\.](\\d{2})$" )
	else if( boost::regex_search( text.c_str(), what, gNavigationRX_YearMonthDay ) )
	{
		//Convet year from to digit to four digit format
		int year = atoi( what[1].str().c_str() ) + 2000;
		int month = atoi( what[2].str().c_str() );
		int day = atoi( what[3].str().c_str() );

		if( (year >= mDimensions[SCD_NavigationMinYear] && year <= mDimensions[SCD_NavigationMaxYear])
			&& ( month > 0 && month <= 12 )
			&& ( day > 0 && day <= boost::gregorian::date( year, month, 01 ).end_of_month().day() ) )
		{
			boost::posix_time::time_duration timeCurrent = mCurrentTime.time_of_day();
			boost::gregorian::date dt = mCurrentTime.date();

			dt = boost::gregorian::date( year, month, day );

			mNavigateTime = boost::posix_time::ptime( dt, timeCurrent );
			mNavigationHint = ">year:month:day";
			mDimensions[SCD_NavigationTextHandled] = 1;
			isHandleSuccess = true;
		}
	}

	return isHandleSuccess;
}

bool ScheduleControl::CalculatePositionOnTimeTrack( const boost::posix_time::ptime& time, int cameraId, int& x, int& y )
{
	boost::posix_time::time_period tp( mCurrentTime, time );
	boost::posix_time::time_duration td = tp.length();
	
	long hoursDiff = td.hours();
	long minutesDiff = td.minutes();

	int xOffset = mDimensions[SCD_DateTimeWidth] + 
		( cameraId - mDimensions[SCD_FirstCameraIDX] ) * mDimensions[SCD_CamButtonWidth];
		
	int yOffset = 
		mDimensions[SCD_HeaderHeight] 
		+ mDimensions[SCD_ScrollBarWidth]
		+ hoursDiff * mDimensions[SCD_HourScalePix]
		+ minutesDiff * mDimensions[SCD_HourScalePix] / 60;

	x = xOffset;
	y = yOffset;

	if(	   ( xOffset < ( mDimensions[SCD_CamTrackRectX0] ))
		|| ( xOffset > ( mDimensions[SCD_CamTrackRectX0] + mDimensions[SCD_CamTrackRectWidth] ))
		|| ( yOffset < ( mDimensions[SCD_HeaderHeight] + mDimensions[SCD_ScrollBarWidth] ))
		|| ( yOffset > ( mDimensions[SCD_CamTrackRectY0] + mDimensions[SCD_CamTrackRectHeight] )))
	{
		return false;
	}
	return true;
}

bool ScheduleControl::CalculateDurationOnTimeTrack( 
	const boost::posix_time::time_period& tp, 
	int cameraId, int& x, int& y, int& width, int& height )
{
	if( tp.end() < mCurrentTime || tp.begin() > ( mCurrentTime + boost::posix_time::hours(mDimensions[SCD_HoursInCamTrack])) )
		return false;

	boost::posix_time::time_period tpDiffer( mCurrentTime, tp.begin() );
	boost::posix_time::time_duration tdDiffer = tpDiffer.length();
	boost::posix_time::time_duration trackWidth = tp.length();


	long hoursDiff = tdDiffer.hours();
	long minutesDiff = tdDiffer.minutes();
	long hoursWidth = trackWidth.hours();
	long minutesWidth = trackWidth.minutes();  

	int xOffset = mDimensions[SCD_DateTimeWidth] + 
		( cameraId - mDimensions[SCD_FirstCameraIDX] ) * mDimensions[SCD_CamButtonWidth];

	int yOffset = 
		mDimensions[SCD_HeaderHeight] 
		+ mDimensions[SCD_ScrollBarWidth]
		+ hoursDiff * mDimensions[SCD_HourScalePix]
		+ minutesDiff * mDimensions[SCD_HourScalePix] / 60;

	x = xOffset;
	y = yOffset;
	
	width = mDimensions[SCD_CamButtonWidth];
	height = hoursWidth * mDimensions[SCD_HourScalePix]
	+ minutesWidth * mDimensions[SCD_HourScalePix] / 60;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void ScheduleControl::DrawClock( wxDC& dc )
{
	int textWidth, textHeight;
	int clockXOffset, clockYOffset;

	//Draw horizontal header line
	dc.DrawLine( 0, 
		mDimensions[SCD_HeaderHeight], 
		mDimensions[SCD_ControlWidth],
		mDimensions[SCD_HeaderHeight] );

	//Draw vertical line
	dc.DrawLine( mDimensions[SCD_DateTimeWidth], 0, mDimensions[SCD_DateTimeWidth], mDimensions[SCD_ControlHeight] );

	//Time part
	boost::posix_time::time_duration td = mCurrentTime.time_of_day();
	std::string timePart = boost::str( boost::format( "%02d:%02d" ) % td.hours() % td.minutes() );
	dc.SetFont( mFonts[SCF_DateTimeFont] );
	dc.GetTextExtent( timePart, &textWidth, &textHeight  );
	clockXOffset = ( mDimensions[SCD_DateTimeWidth] - textWidth ) / 2;
	clockYOffset = ( mDimensions[SCD_HeaderHeight] - textHeight ) / 2 - 4;
	dc.DrawText( timePart.c_str(), clockXOffset, clockYOffset  );

	//Date part
	std::string datePart = boost::gregorian::to_simple_string( mCurrentTime.date() );
	dc.GetTextExtent( datePart, &textWidth, &textHeight  );
	clockXOffset = ( mDimensions[SCD_DateTimeWidth] - textWidth ) / 2;
	clockYOffset = ( mDimensions[SCD_HeaderHeight] - textHeight ) / 2 + 4;
	dc.DrawText( datePart.c_str(), clockXOffset, clockYOffset  );
}

void ScheduleControl::DrawDateTime( wxDC& dc )
{
	//Clipping region
	dc.SetClippingRegion( 
		mDimensions[SCD_DateTimeRectX0], 
		mDimensions[SCD_DateTimeRectY0], 
		mDimensions[SCD_DateTimeRectWidth], 
		mDimensions[SCD_DateTimeRectHeight]
	);

	//>>Draw row and columns lines
	dc.SetPen( *wxGREY_PEN );
	for( int i = 1; i < 4; ++i )
	{
		dc.DrawLine( 
			i * mDimensions[SCD_ScrollBarWidth], 
			mDimensions[SCD_HeaderHeight], 
			i * mDimensions[SCD_ScrollBarWidth],
			mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight] );
	}
	dc.SetPen( *wxBLACK_PEN );

	//Draw year,month,day and hour labels
	int hourBarX =	3 * mDimensions[SCD_ScrollBarWidth] + (( mDimensions[SCD_ScrollBarWidth] - dc.GetCharWidth() * 2  ) / 2);
	int dayBarX =	2 * mDimensions[SCD_ScrollBarWidth];
	int monthBarX = 1 * mDimensions[SCD_ScrollBarWidth];
	int yearBarX =	0 * mDimensions[SCD_ScrollBarWidth];

	int yOffset = mDimensions[SCD_HeaderHeight] + mDimensions[SCD_ScrollBarWidth];
	
	boost::posix_time::time_duration tdHour = boost::posix_time::time_duration(1,0,0);
	for( boost::posix_time::ptime dateTime = mCurrentTime;
		yOffset < ( mDimensions[SCD_DateTimeRectHeight] );
		yOffset +=  mDimensions[SCD_HourScalePix], dateTime += tdHour )
	{

		bool lastDayOfMonth = dateTime.date().end_of_month() == dateTime.date() ? true : false;

		int hour = dateTime.time_of_day().hours();
		int dayBarBrightness = 128 + ( (24 - hour) * 64 / 24 );
		//int monthBarBrightness = day * 8;
		//int yearBarBrightness = month * 20;

		wxColor color( dayBarBrightness, dayBarBrightness, dayBarBrightness );
		dc.SetBrush( wxBrush( color ) );
		dc.SetPen( wxPen( color ) );
		dc.DrawRectangle( dayBarX, yOffset, mDimensions[SCD_ScrollBarWidth] + 1, mDimensions[SCD_HourScalePix] + 1 );

		//Does we need gradient bars for year and month?
		//dc.SetBrush( wxBrush( wxColour( monthBarBrightness, monthBarBrightness, monthBarBrightness ) ) );
		//dc.DrawRectangle( monthBarX, yOffset, mDimensions[SCD_ScrollBarWidth] + 1, mDimensions[SCD_ScrollBarWidth] + 1 );

		//dc.SetBrush( wxBrush( wxColour( yearBarBrightness, yearBarBrightness, yearBarBrightness ) ) );
		//dc.DrawRectangle( yearBarX, yOffset, mDimensions[SCD_ScrollBarWidth] + 1, mDimensions[SCD_ScrollBarWidth] + 1 );

		dc.SetPen( *wxBLACK_PEN );
		dc.DrawText( ( boost::str( boost::format( "%02d" ) % hour )).c_str(), hourBarX, yOffset+1 );

		//Draw notch on the clock face
		dc.DrawLine( 
			mDimensions[SCD_DateTimeWidth] - mDimensions[SCD_ScrollBarBorderOffset],
			yOffset + mDimensions[SCD_ScrollBarWidth],
			mDimensions[SCD_DateTimeWidth],
			yOffset + mDimensions[SCD_ScrollBarWidth]
			);
		
		dc.DrawLine( 
				mDimensions[SCD_DateTimeWidth] - 1,
				yOffset + mDimensions[SCD_ScrollBarHalfWidth],
				mDimensions[SCD_DateTimeWidth],
				yOffset + mDimensions[SCD_ScrollBarHalfWidth]
			);
	}
	
	//Draw current year
	int yearStrWidth = 4 * dc.GetCharWidth();
	dc.DrawRotatedText( (boost::str( boost::format( "%d" ) % mCurrentTime.date().year()).c_str() ), 
		yearBarX, 
		mDimensions[SCD_HeaderHeight] +( mDimensions[SCD_DateTimeRectHeight] + yearStrWidth ) / 2, 90 );

	//Draw current month
	std::string monthStr = gEnLongMonthNames[ mCurrentTime.date().month() - 1 ];
	int monthStrWidth = (int)monthStr.length() * dc.GetCharWidth();
	dc.DrawRotatedText( monthStr.c_str(), 
		monthBarX, 
		mDimensions[SCD_HeaderHeight] +( mDimensions[SCD_DateTimeRectHeight] + monthStrWidth ) / 2, 90 );

	//Disable clipping region
	dc.DestroyClippingRegion();

	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxBLACK_BRUSH );
}

void ScheduleControl::DrawTracker( wxDC& dc )
{
	//Draw datetime tracker
	dc.SetBrush( *wxRED_BRUSH );
	dc.SetPen( mPens[SCP_TrackerPen] );
	int trackerPos;
	int x,y;
	
	//>>Draw datetime tracker spots
	trackerPos = mDimensions[SCD_HeaderHeight] +  mDimensions[SCD_ScrollBarWidth];
	x = mDimensions[SCD_DateTimeWidth]; 
	for( int i = 0; 
		x < mDimensions[SCD_ControlWidth];
		x += mDimensions[SCD_CamButtonWidth] )
	{
		dc.DrawCircle( x, trackerPos, 2 );
	}

	dc.DrawLine( 
		mDimensions[SCD_DateTimeWidth], 
		trackerPos, 
		mDimensions[SCD_ControlWidth],
		trackerPos );

	//Draw current datetime
	trackerPos = mDimensions[SCD_HeaderHeight] + mDimensions[SCD_ScrollBarWidth];

	boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();
	if( CalculatePositionOnTimeTrack( time, mDimensions[SCD_FirstCameraIDX], x, y ) )
	{
		dc.SetPen( *wxCYAN_PEN );
		dc.DrawLine( 
			mDimensions[SCD_CamTrackRectX0],
			y, 
			mDimensions[SCD_CamTrackRectX0] + mDimensions[SCD_CamTrackRectWidth],
			y );
	}

	dc.SetPen( *wxBLACK_PEN );
}

void ScheduleControl::DrawCamTrack( wxDC& dc )
{
	int textWidth, textHeight;
	int texXOffset, textYOffset;

	//Clipping region
	dc.SetClippingRegion( 
		mDimensions[SCD_CamTrackRectX0], 
		mDimensions[SCD_CamTrackRectY0], 
		mDimensions[SCD_CamTrackRectWidth], 
		mDimensions[SCD_CamTrackRectHeight] );

	dc.SetFont( mFonts[SCF_DateTimeFont] );
	dc.GetTextExtent( "Cam", &textWidth, &textHeight  );
	texXOffset = ( mDimensions[SCD_CamButtonWidth] - textWidth ) / 2;
	textYOffset = ( mDimensions[SCD_HeaderHeight] - textHeight ) / 2 - 4;

	//Draw "Cam" word
	texXOffset += mDimensions[SCD_DateTimeWidth];
	for( size_t i = mDimensions[SCD_FirstCameraIDX]; i < mCameras.size(); ++i )
	{
		dc.DrawText( "Cam", texXOffset, textYOffset );
		texXOffset += mDimensions[SCD_CamButtonWidth];
	}

	//Draw camera numbers
	dc.GetTextExtent( "00", &textWidth, &textHeight  );
	texXOffset = ( mDimensions[SCD_CamButtonWidth] - textWidth ) / 2;
	textYOffset = ( mDimensions[SCD_HeaderHeight] - textHeight ) / 2 + 4;

	//Initial offset
	texXOffset += mDimensions[SCD_DateTimeWidth];
	for( size_t i = mDimensions[SCD_FirstCameraIDX]; i < mCameras.size(); ++i )
	{
		char buffer[16];
		
		sprintf( buffer, "%02d", mCameras[i].GetIndex() );
		dc.DrawText( &buffer[0], texXOffset, textYOffset );
		texXOffset += mDimensions[SCD_CamButtonWidth];
	}

	//Draw track grid
	int xOffset = mDimensions[SCD_DateTimeWidth];
	for( size_t i = 0; ( i <= mCameras.size() ) && ( xOffset < mDimensions[SCD_ControlWidth] ); ++i )
	{
		dc.DrawLine( xOffset, 0, xOffset, mDimensions[SCD_ControlHeight] );
		xOffset += mDimensions[SCD_CamButtonWidth];
	}

	//Disable clipping region
	dc.DestroyClippingRegion();

	//Clipping region
	dc.SetClippingRegion( 
		mDimensions[SCD_CamTrackRectX0], 
		mDimensions[SCD_HeaderHeight] + mDimensions[SCD_ScrollBarWidth], 
		mDimensions[SCD_CamTrackRectWidth], 
		mDimensions[SCD_CamTrackRectHeight] - mDimensions[SCD_StatusWithScrollbarWidth] );

	for( int idx = 0; idx < (int)mCameras.size(); ++idx )
	{
		//Duration
		for( mCameras[idx].ResetPeriods(); mCameras[idx].MoveToNextPeriod(); )
		{
			ScheduleControl::Duration& dt = mCameras[idx].GetCurrentPeriod();
			DrawDuration( dc, idx, dt );
		}

		//Bookmarks
		for( mCameras[idx].ResetBookmarks(); mCameras[idx].MoveToNextBookmark(); )
		{
			ScheduleControl::Bookmark& bm = mCameras[idx].GetCurrentBookmark();
			DrawBookmark( dc, idx, bm );
		}
	}

	dc.DestroyClippingRegion();

}

void ScheduleControl::DrawScrollbars( wxDC& dc )
{
	wxRect* rect;
	int hScrollbarPos = mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight] - mDimensions[SCD_ScrollBarWidth];

	dc.DrawLine( 0, hScrollbarPos, mDimensions[SCD_ControlWidth], hScrollbarPos );

	//Draw bottom scrollbar, left arrow
	rect = &(mAreas[SCA_SB_BottomLeftArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_LeftArrow, mAreas[SCA_SB_BottomLeftArrow].second );

	//Draw bottom scrollbar, right arrow
	rect = &(mAreas[SCA_SB_BottomRightArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_RightArrow, mAreas[SCA_SB_BottomRightArrow].second );

	//Draw date time scrollbar, year up arrow
	rect = &(mAreas[SCA_SB_DateTimeYearUpArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_UpArrow, mAreas[SCA_SB_DateTimeYearUpArrow].second );

	//Draw date time scrollbar, year down arrow
	rect = &(mAreas[SCA_SB_DateTimeYearDownArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_DownArrow, mAreas[SCA_SB_DateTimeYearDownArrow].second );

	//Draw date time scrollbar, month up arrow
	rect = &(mAreas[SCA_SB_DateTimeMonthUpArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_UpArrow, mAreas[SCA_SB_DateTimeMonthUpArrow].second );

	//Draw date time scrollbar, month down arrow
	rect = &(mAreas[SCA_SB_DateTimeMonthDownArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_DownArrow, mAreas[SCA_SB_DateTimeMonthDownArrow].second );

	//Draw date time scrollbar, day up arrow
	rect = &(mAreas[SCA_SB_DateTimeDayUpArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_UpArrow, mAreas[SCA_SB_DateTimeDayUpArrow].second );

	//Draw date time scrollbar, day down arrow
	rect = &(mAreas[SCA_SB_DateTimeDayDownArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_DownArrow, mAreas[SCA_SB_DateTimeDayDownArrow].second );

	//Draw date time scrollbar, hour up arrow
	rect = &(mAreas[SCA_SB_DateTimeHourUpArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_UpArrow, mAreas[SCA_SB_DateTimeHourUpArrow].second );

	//Draw date time scrollbar, hour down arrow
	rect = &(mAreas[SCA_SB_DateTimeHourDownArrow].first);
	DrawScrollArrow( dc, rect->x, rect->y, rect->width,	rect->height, SCE_DownArrow, mAreas[SCA_SB_DateTimeHourDownArrow].second );
}

void ScheduleControl::DrawStatusbar( wxDC& dc )
{
	int statusBarPos = mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight];
	dc.DrawLine( 0, statusBarPos, mDimensions[SCD_ControlWidth], statusBarPos );
}

void ScheduleControl::DrawDuration( wxDC& dc, int cameraId, const Duration& duration )
{

	int xOffset, yOffset;
	int width, height;


	if( CalculateDurationOnTimeTrack( duration.GetPeriod(), cameraId, xOffset, yOffset, width, height ) ) 	 
	{
		dc.SetBrush( wxBrush( duration.GetColor() ) );
		dc.DrawRectangle( 
			xOffset, yOffset,
			width + 1,
			height );
	}
}

void ScheduleControl::DrawBookmark( wxDC& dc, int cameraId, const Bookmark& bookmark )
{
	int xOffset, yOffset;
	if( CalculatePositionOnTimeTrack( bookmark.GetPTime(), cameraId, xOffset, yOffset ) )
	{
		switch( bookmark.GetPriority() )
		{
		case Bookmark::BP_Normal:
			dc.SetBrush( *wxGREEN_BRUSH );
			break;
		case Bookmark::BP_Low:
			dc.SetBrush( *wxGREY_BRUSH );
			break;
		case Bookmark::BP_High:
			dc.SetBrush( *wxRED_BRUSH );
			break;
		}
		dc.DrawCircle( 
			xOffset + mDimensions[SCD_CamButtonWidth] / 2,
			yOffset, 
			mDimensions[SCD_BookmarkRadius] );

		xOffset += ( mDimensions[SCD_CamButtonWidth] - dc.GetCharHeight() ) / 2;

		dc.DrawRotatedText( bookmark.GetMessage().c_str(), 
			xOffset, 
			yOffset - mDimensions[SCD_BookmarkRadius], 90 );
	}

	dc.SetBrush( *wxBLACK_BRUSH );
}

void ScheduleControl::DrawZoomer( wxDC& dc )
{

	dc.DrawLine( 
		mDimensions[SCD_ZoomButtonWidth],
		mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight],
		mDimensions[SCD_ZoomButtonWidth],
		mDimensions[SCD_ControlHeight] );

	int xOffset, yOffset;

	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxLIGHT_GREY_BRUSH );

	//Left button
	xOffset = mDimensions[SCD_ZoomButtonWidth] / 2;
	yOffset = mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight] / 2;

	dc.DrawCircle( xOffset, yOffset, mDimensions[SCD_ZoomButtonRadius] );
	dc.DrawLine( 
		xOffset - mDimensions[SCD_ZoomButtonRadius] / 2,
		yOffset, 
		xOffset + mDimensions[SCD_ZoomButtonRadius] / 2 + 1,
		yOffset );
	
	dc.DrawLine( 
		xOffset,
		yOffset - mDimensions[SCD_ZoomButtonRadius] / 2, 
		xOffset,
		yOffset + mDimensions[SCD_ZoomButtonRadius] / 2 + 1);

	//Right button
	xOffset = 3 * mDimensions[SCD_ZoomButtonWidth] / 2;
	yOffset = mDimensions[SCD_ControlHeight] - mDimensions[SCD_StatusBarHeight] / 2;

	dc.DrawCircle( xOffset, yOffset, mDimensions[SCD_ZoomButtonRadius] );
	dc.DrawLine( 
		xOffset - mDimensions[SCD_ZoomButtonRadius] / 2,
		yOffset, 
		xOffset + mDimensions[SCD_ZoomButtonRadius] / 2 + 1,
		yOffset);
}


void ScheduleControl::DrawScrollArrow( wxDC& dc, int x, int y, int width, int height, CustomElement arrow, bool selected )
{
	wxPoint points[3];
	int borderOffset = mDimensions[SCD_ScrollBarBorderOffset];

	if( selected )
	{
		dc.SetBrush( *wxBLACK_BRUSH );
		dc.DrawRectangle( x,y,width + 1,height + 1 );
		dc.SetBrush( *wxWHITE_BRUSH );
	}
	else
	{
		dc.SetBrush( *wxLIGHT_GREY_BRUSH );
		dc.DrawRectangle( x,y,width + 1,height + 1 );
		dc.SetBrush( *wxBLACK_BRUSH );
	}

	switch( arrow ) 
	{
	case SCE_UpArrow:
		{
			points[0] = wxPoint( x + borderOffset, y + height - borderOffset );
			points[1] = wxPoint( x + (width / 2), ( y + borderOffset ) );
			points[2] = wxPoint( x + width - borderOffset, y + height - borderOffset );
		}
		break;

	case SCE_DownArrow:
		{
			points[0] = wxPoint( x + borderOffset, y + borderOffset );
			points[1] = wxPoint( x + (width / 2), ( y + height - borderOffset  ) );
			points[2] = wxPoint( x + width - borderOffset, y + borderOffset );
		}
		break;

	case SCE_LeftArrow:
		{
			points[0] = wxPoint( x + width - borderOffset, y + borderOffset );
			points[1] = wxPoint( x + borderOffset, y + ( height / 2 ) );
			points[2] = wxPoint( x + width - borderOffset, y + height - borderOffset );			
		}
		break;

	case SCE_RightArrow:
		{
			points[0] = wxPoint( x + borderOffset, y + borderOffset );
			points[1] = wxPoint( x + width - borderOffset, y + ( height / 2 ) );
			points[2] = wxPoint( x + borderOffset, y + height - borderOffset );			
		}
		break;
	}
	dc.DrawPolygon( 3, points );
}

void ScheduleControl::DrawNavigationHint( wxDC& dc, const std::string& text )
{
	if( text.empty() )
		return;

	wxRect rect = mNavigateField.GetRect();

	int xPos = rect.x + rect.width + mDimensions[SCD_ScrollBarBorderOffset];
	int yPos = rect.y;
	dc.DrawText( text.c_str(), xPos, yPos );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ScheduleControl::AddCamera( int index )
{
	//Check if camera is present
	for( size_t i = 0; i < mCameras.size(); ++i )
	{
		if( mCameras[i].GetIndex() == index )
		{
			return false;
		}
	}

	//Add camera with the specified index
	mCameras.push_back( Camera( index ) );

	Refresh( false );
	return true;
}

void ScheduleControl::RemoveCamera( int index )
{
	for( std::vector<Camera>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		if( it->GetIndex() == index )
		{
			mCameras.erase( it );
			return;
		}
	}
	Refresh( false );
}

int ScheduleControl::GetCamerasNum() const
{
	return static_cast<int>( mCameras.size() );
}

//////////////////////////////////////////////////////////////////////////
ScheduleControl::Camera& ScheduleControl::operator[]( int idx )
{
	assert( ((idx > 0) && ( idx < (int)mCameras.size() )) && "Invalid camera index" );
	return mCameras[idx];
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////