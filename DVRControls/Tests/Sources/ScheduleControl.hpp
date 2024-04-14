/*
*	ScheduleControl(DVR) 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ScheduleControl.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-12-20
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Schedule control header
*
*
*/
#ifndef __SCHEDULECONTROL_HPP__
#define __SCHEDULECONTROL_HPP__

/////////////////////////////////////////////////////////////////////////////
class ScheduleControl : public wxPanel
{
	static const int mTimerID = 1661;
	static const int mEditControlID = 1662;
public: 
	//////////////////////////////////////////////////////////////////////////
	//Camera bookmark class
	class Bookmark
	{
	public:
		enum Priority
		{
			BP_Normal = 0,
			BP_High,
			BP_Low,
		};
	public:
		Bookmark( const tm& time, const std::string& message = "", Priority p = BP_Normal );
		Bookmark( const boost::posix_time::ptime& time, const std::string& message = "", Priority p = BP_Normal );
	public:
		/*
		*	Gets bookmark date
		*/
		const boost::posix_time::ptime& GetPTime() const;

		/*
		*	Message accessors
		*/
		const std::string& GetMessage() const;
		void SetMessage( const std::string& message );

		/*
		*	Priority accessors
		*/
		Priority GetPriority() const;
		void SetPriority( Priority p );
	public:
		/*
		*	Comparsion operators
		*/
		bool operator==( const Bookmark& other ) const;
		bool operator!=( const Bookmark& other ) const;
		bool operator<( const Bookmark& other ) const;
		bool operator>( const Bookmark& other ) const;
	private:
		boost::posix_time::ptime mTime;
		std::string mMessage;
		Priority mPriority;
	};
	//////////////////////////////////////////////////////////////////////////
	//Duration class
	class Duration
	{
	public:
		Duration( const boost::posix_time::time_period& period, wxColor = wxColor( 0,0,0 ) );
		Duration( const boost::posix_time::ptime& begin, const boost::posix_time::ptime& end, wxColor = wxColor(0,0,0) );
	public:
		/*
		*	Returns period
		*/
		const boost::posix_time::time_period& GetPeriod() const;

		/*
		*	Returns period start time
		*/
		boost::posix_time::ptime GetPeriodStart() const;

		/*
		*	Returns period end time
		*/
		boost::posix_time::ptime GetPeriodEnd() const;

		/*
		*	Gets duration track color
		*/
		wxColor GetColor() const;

		/*
		*	Sets duration track color
		*/
		void SetColor( wxColor );

	public:
		bool operator==( const Duration& other ) const;
		bool operator!=( const Duration& other ) const;
		bool operator<( const Duration& other ) const;
		bool operator>( const Duration& other ) const;
	public:
		Duration Span( const Duration& other );
		Duration Merge( const Duration& other );
		bool Intersects( const Duration& other ) const;
		bool IsNull() const;
	private:
		wxColor mColor;
		boost::posix_time::time_period mPeriod;
	};
public:
	//////////////////////////////////////////////////////////////////////////
	//Camera class
	class Camera
	{
		typedef std::set<Bookmark> BookmarkSet;
		typedef std::set<Duration> DurationSet;
	public:
		/*
		*	Creates camera with specified index
		*/
		Camera( int idx );
	public:
		/*
		*	Gets camera index
		*/
		int GetIndex() const;

		/*
		*	Adds bookmark
		*/
		void Add( const Bookmark& bookmark );

		/*
		*	Removes bookmark with specified bookmark
		*/
		void Remove( const Bookmark& bookmark );

		/*
		*	Removes all bookmarks
		*/
		void ClearBookmarks();
	
	public:
		/*
		*	Gets the current element in the collection
		*/
		Bookmark& GetCurrentBookmark();

		/*
		*	Advances the enumerator to the next element of the collection
		*/
		bool MoveToNextBookmark();

		/*
		*	Sets the enumerator to its initial position,
		*	which is before the first element in the collection
		*/
		void ResetBookmarks();

		/*
		*	Sets the enumerator to its initial position,
		*	which is before the element with time in the collection
		*/
		void ResetBookmarks( const boost::posix_time::ptime& time );

	public:
		/*
		*	Adds period 
		*/
		void Add( const Duration& duration );

		/*
		*	Adds period and merges it with the previous
		*/
		void AddToLast( const Duration& duration );

		/*
		*	Removes period
		*/
		void Remove( const Duration& duration );

		/*
		*	Removes all periods
		*/
		void ClearPeriods();
	
	public:
		/*
		*	Gets the current element in the collection
		*/
		Duration& GetCurrentPeriod();

		/*
		*	Advances the enumerator to the next element of the collection
		*/
		bool MoveToNextPeriod();

		/*
		*	Sets the enumerator to its initial position,
		*	which is before the first element in the collection
		*/
		void ResetPeriods();

		/*
		*	Sets the enumerator to its initial position,
		*	which is before the element with time in the collection
		*/
		void ResetPeriods( const boost::posix_time::time_period& period );

	private:
		int mCameraIndex;
		bool mBookmarkItReseted;
		bool mDurationItReseted;
		BookmarkSet mBookmarks;
		DurationSet mPeriods;
		DurationSet::iterator mDurationIt;
		BookmarkSet::iterator mBookmarksIt;
		
    };
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
public:
	enum CustomElement
	{
		SCE_LeftArrow = 0,
		SCE_RightArrow,
		SCE_UpArrow,
		SCE_DownArrow
	};
public:
	ScheduleControl(  wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size );
	virtual ~ScheduleControl();
	void DebugInit();
public:
	/*
	*	Adds camera with specified index
	*/
	bool AddCamera( int idx );

	/*
	*	Removes camera with the specified index
	*/
	void RemoveCamera( int idx );

	/*
	*	Gets camera number
	*/
	int GetCamerasNum() const;

	/*
	*	Returns reference to the camera with specified index
	*/
	Camera& operator[]( int idx );

protected:
	//Event handlers...
	void OnKeyUp( wxKeyEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnTimer( wxTimerEvent& event );
	void OnKeyDown( wxKeyEvent& event );
	void OnPaint( wxPaintEvent& event );
	void OnLButtonUp( wxMouseEvent& event );
	void OnRButtonUp( wxMouseEvent& event );
	void OnMouseMove( wxMouseEvent& event );
	void OnMouseWheel( wxMouseEvent& event );
	void OnMouseEnter( wxMouseEvent& event );
	void OnMouseLeave( wxMouseEvent& event );
	void OnLButtonDown( wxMouseEvent& event );
	void OnRButtonDown( wxMouseEvent& event );
	void OnEraseBackground( wxEraseEvent& event );
	void OnNavigateField( wxCommandEvent& event );
	DECLARE_EVENT_TABLE()
protected:
	//Internal drawing functions
	virtual void DrawClock( wxDC& dc );
	virtual void DrawTracker( wxDC& dc );
	virtual void DrawDateTime( wxDC& dc );
	virtual void DrawCamTrack( wxDC& dc );
	virtual void DrawStatusbar( wxDC& dc );
	virtual void DrawScrollbars( wxDC& dc );
	virtual void DrawZoomer( wxDC& dc );
protected:
	virtual void DrawDuration( wxDC& dc, int cameraId, const Duration& duration );
	virtual void DrawBookmark( wxDC& dc, int cameraId, const Bookmark& bookmark );
	virtual void DrawScrollArrow( wxDC& dc, int x, int y, int width, int height, CustomElement arrow, bool selected = false  );
	virtual void DrawNavigationHint( wxDC& dc, const  std::string& text );
protected:
	virtual bool HandleNavigationText( const std::string& text );

	//Calcualtes position on the time track for the specified time
	virtual bool CalculatePositionOnTimeTrack( const boost::posix_time::ptime& time, int cameraId,  int& x, int& y );
	virtual bool CalculateDurationOnTimeTrack( const boost::posix_time::time_period& tp, int cameraId, int& x, int& y, int& width, int& height );
private:
	enum Dimension
	{
		SCD_ControlWidth = 0,
		SCD_ControlHeight,
		SCD_HeaderHeight,
		SCD_StatusBarHeight,
		SCD_ScrollBarWidth,
		SCD_ScrollBarHalfWidth,
		SCD_ScrollBarBorderOffset,
		SCD_DateTimeWidth,
		SCD_CamButtonWidth,
		SCD_CamTrackRectX0,
		SCD_CamTrackRectY0,
		SCD_CamTrackRectWidth,
		SCD_CamTrackRectHeight,
		SCD_DateTimeRectX0,
		SCD_DateTimeRectY0,
		SCD_DateTimeRectWidth,
		SCD_DateTimeRectHeight,
		SCD_StatusWithScrollbarWidth,
		SCD_NavigateFieldOffset,
		SCD_HourScalePix,
		SCD_MinuteScalePix,
		SCD_BookmarkRadius,
		SCD_ZoomButtonRadius,
		SCD_ZoomButtonWidth,
		SCD_FirstCameraIDX,
		SCD_NavigationTextHandled,
		SCD_NavigationWidth,
		SCD_NavigationMaxYear,
		SCD_NavigationMinYear,
		SCD_HoursInCamTrack,
		SCD_DimensionsNum
	};

	enum Area
	{
		SCA_SB_BottomLeftArrow,
		SCA_SB_BottomRightArrow,
		SCA_SB_DateTimeYearUpArrow,
		SCA_SB_DateTimeYearDownArrow,
		SCA_SB_DateTimeMonthUpArrow,
		SCA_SB_DateTimeMonthDownArrow,
		SCA_SB_DateTimeDayUpArrow,
		SCA_SB_DateTimeDayDownArrow,
		SCA_SB_DateTimeHourUpArrow,
		SCA_SB_DateTimeHourDownArrow,
		SCA_BT_ZoomPlus,
		SCA_BT_ZoomMinus,
		SCA_AreasNum
	};

	enum Font
	{
		SCF_DateTimeFont = 0,
		SCF_DateTimeFontBold,
		SCF_CamButtonFont,
		SCF_FontsNum
	};

	enum Pen
	{
		SCP_TrackerPen = 0,
		SCP_CTPen,
		SCP_PensNum
	};

	enum Brush
	{

	};

private:
	boost::posix_time::ptime mDefaultTime;
	boost::posix_time::ptime mCurrentTime;
	boost::posix_time::ptime mTrackerTime;
	boost::posix_time::ptime mNavigateTime;

private:
	wxTimer mTimer;
	wxTextCtrl mNavigateField;
	std::string mNavigationHint;
private:
	std::vector<wxPen> mPens;
	std::vector<wxFont> mFonts;
	std::vector<Camera> mCameras;
	std::vector<int> mDimensions;
	std::vector<std::pair<wxRect,bool> > mAreas;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif //__SCHEDULECONTROL_HPP__
