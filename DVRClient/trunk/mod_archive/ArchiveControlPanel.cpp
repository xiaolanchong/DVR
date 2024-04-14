//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Окно задание времени архива + просмотр ситуаций

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   07.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ArchiveControlPanel.h"
#include "DateTimeSource.h"
#include "alarmcontrol/AlarmViewControl.h"
#include "CameraNumberStatic.h"

#include "ArchiveFrameState.h"

//======================================================================================//
//                              class ArchiveControlPanel                               //
//======================================================================================//

ArchiveControlPanel::ArchiveControlPanel(ArchiveControlPanelCallback* pCallback):
	m_pAlarmWnd ( 0 ),
	m_pCallback( pCallback )
{
	
}

ArchiveControlPanel::~ArchiveControlPanel()
{
}

BEGIN_EVENT_TABLE(ArchiveControlPanel, wxPanel)
	EVT_BUTTON(XRCID("Button_Start"),	 ArchiveControlPanel::OnStartArchive)
	EVT_TOOL( XRCID("ToolButton_VideoControl_Play"),	ArchiveControlPanel::OnPlayArchive )
	EVT_TOOL( XRCID("ToolButton_VideoControl_Pause"),	ArchiveControlPanel::OnPauseArchive )
	EVT_TOOL( XRCID("ToolButton_VideoControl_Stop"),	ArchiveControlPanel::OnStopArchive )
	EVT_COMBOBOX( XRCID("Combobox_TimeScale"), ArchiveControlPanel::OnTimeScaleChanged )
END_EVENT_TABLE()

void	ArchiveControlPanel::Init()
{
	bool res;

	m_pStartTimeWnd = std::auto_ptr<DateTimeSource>( new DateTimeSource( this, "DateCtrl_ArchiveStartTime", "TimeCtrl_ArchiveStartTime" ) );
	m_pEndTimeWnd	= std::auto_ptr<DateTimeSource>( new DateTimeSource( this, "DateCtrl_ArchiveEndTime", "TimeCtrl_ArchiveEndTime" ) );
	
	m_pAlarmWnd = new AlarmViewControl(this, boost::bind( &ArchiveControlPanel::OnTimeChanged, this, _1 )  ) ;
	res = wxXmlResource::Get()->AttachUnknownControl( wxT("ScheduleControl_Archive"), m_pAlarmWnd, this );
	_ASSERTE(res);

	m_pCameraNumberStatic = new CameraNumberStatic();
	
	res = wxXmlResource::Get()->AttachUnknownControl( wxT("Static_CameraNumber"), m_pCameraNumberStatic, this );
	_ASSERTE(res);

	m_pTimeScaleCombobox = XRCCTRL( *this, wxT("Combobox_TimeScale"), wxComboBox );
	m_pTimeScaleCombobox->SetSelection(1);
	m_pAlarmWnd->SetTimeMode( AlarmViewControl::Hours );

	m_pCameraNumberStatic->SetClientSize(0, 0);
	m_pCameraNumberStatic->Show(false);
	//////////////////////////////////////////////////////////////////////////
	
	m_pVideoControlBar = XRCCTRL( *this, wxT("ToolBar_Archive_VideoControl"), wxToolBar );
	_ASSERTE(m_pVideoControlBar);


	m_State = std::auto_ptr<ArchiveFrameState>( new ArchiveFrameIdle( m_pVideoControlBar ) );

	InitTimeControls();
}

//#define TEST_ALARM_CONTROL

void	ArchiveControlPanel::OnStartArchive(wxCommandEvent& ev)
{

	tm tmStart	= m_pStartTimeWnd->GetCurrentDateTime();
	tm tmEnd	= m_pEndTimeWnd->GetCurrentDateTime();

	tmStart.tm_isdst	= 1;
	tmEnd.tm_isdst		= 1;
	time_t StartTime	= mktime( &tmStart );
	time_t EndTime		= mktime( &tmEnd );
	time_t CurrentTime	= time(0);

	if( EndTime <= StartTime )
	{
		wxMessageBox( _("The archive start time is greater or equal then the end one"), _("Wrong time period"), wxOK|wxICON_ERROR );
		return;
	}
	else if( EndTime > CurrentTime )
	{
		wxMessageBox( _("The archive end time is greater then the current one"), _("Wrong time period"), wxOK|wxICON_ERROR );
		return;
	}
	_ASSERTE(	StartTime	< EndTime		&& 
		StartTime	< CurrentTime &&
		EndTime		< CurrentTime );

#ifndef TEST_ALARM_CONTROL
	_ASSERTE(m_pCallback);
	boost::shared_ptr<Frames_t> AlarmRects =	m_pCallback->OnStartArchive( StartTime, EndTime );
#else
	boost::shared_ptr<Frames_t> AlarmRects( new Frames_t );
	std::vector<int> CurrentCameras;

	CurrentCameras.push_back( 1 );
	CurrentCameras.push_back( 2 );
	CurrentCameras.push_back( 3 );
	for ( size_t i =0; i < CurrentCameras.size(); ++i )
	{
		time_t CamStartTime =  StartTime + (EndTime - StartTime) * i/ CurrentCameras.size();
		time_t CamEndTime   =  StartTime + (EndTime - StartTime) * (i+1)/ CurrentCameras.size();
		AlarmRects->push_back( DBBridge::Frame( CurrentCameras[i], CamStartTime, CamEndTime, 
												DBBridge::Frame::Rect(0, 0, 0, 0) ) 
							  );
	}
#endif
	LoadDataToSchedule(StartTime, EndTime, *AlarmRects.get()  );

	int TimeMode = GetSuitableTimeMode( StartTime, EndTime );
	m_pTimeScaleCombobox->SetSelection( TimeMode );
	SetTimeMode( TimeMode );
	//	
	Layout();
}



void	ArchiveControlPanel::LoadDataToSchedule(  time_t StartTime, time_t EndTime, const Frames_t& AlarmRects )
{
	AlarmViewControl::CameraAlarmMap_t AlarmMap;
	std::vector<int> AllCameras;
#if 0 // load all cameras rather only current
	AllCameras = m_CameraIds;
	for (  size_t i = 0; i < AllCameras.size(); ++i )
	{
		AlarmMap.insert( std::make_pair( AllCameras[i] , AlarmViewControl::AlarmArr_t() ) );
	}
#endif
	std::vector<int> CurrentCameras;
	m_pCallback->GetCurrentSelectedCameras(CurrentCameras);

	for (  size_t i = 0; i < CurrentCameras.size(); ++i )
	{
		AlarmMap.insert( std::make_pair( CurrentCameras[i] , AlarmViewControl::AlarmArr_t() ) );
	}

	for ( size_t i =0; i < AlarmRects.size(); ++i )
	{
		AlarmViewControl::CameraAlarmMap_t::iterator it = AlarmMap.find( AlarmRects[i].GetCameraID() );
#ifndef TEST_ALARM_CONTROL
		if( it != AlarmMap.end() )
		{
			it->second.push_back(
				std::make_pair(
				AlarmRects[i].GetStartTime() ,
				AlarmRects[i].GetEndTime()  )
				);
		}
#else
		AlarmMap[ AlarmRects[i].GetCameraID() ].push_back(
			std::make_pair(
			AlarmRects[i].GetStartTime() ,
			AlarmRects[i].GetEndTime()  )
			); 	
#endif
	}

	m_pAlarmWnd->SetPeriod(  StartTime ,  EndTime , AlarmMap );

	m_State.reset( new ArchiveFramePlay( m_pVideoControlBar ) );
}

std::pair<tm, tm>	GetDebugTime()
{
	//int tm_sec;     /* seconds after the minute - [0,59] */
	//int tm_min;     /* minutes after the hour - [0,59] */
	//int tm_hour;    /* hours since midnight - [0,23] */
	//int tm_mday;    /* day of the month - [1,31] */
	//int tm_mon;     /* months since January - [0,11] */
	//int tm_year;    /* years since 1900 */
	//int tm_wday;    /* days since Sunday - [0,6] */
	//int tm_yday;    /* days since January 1 - [0,365] */
	//int tm_isdst;   /* daylight savings time flag */

	struct tm tmStartTime	= { 0, 00, 8, 28/*20*//*25*/, 6 - 1, 2006 - 1900, 0, 0, 1  };
	struct tm tmEndTime		= { 0, 30, 8, 28/*20*//*29*/, 6 - 1, 2006 - 1900, 0, 0, 1  };

	return std::make_pair( tmStartTime, tmEndTime );
}

std::pair<tm, tm>	GetInitialTime()
{
	std::pair<tm, tm> InitialTime;
	const __time64_t timeEnd		= time(0) - 60 * 5;
	const __time64_t timeStart	= timeEnd - 1 * 60 * 60;

	localtime_s( &InitialTime.first,  &timeStart );
	localtime_s( &InitialTime.second, &timeEnd );
	return InitialTime;
}

void	ArchiveControlPanel::InitTimeControls()
{
	std::pair<tm, tm>	InitialTime = GetInitialTime();

//	__time64_t StubStartTime = _mktime64( & tmStartTime );
//	__time64_t StubEndTime	 = _mktime64( & tmEndTime );

	m_pStartTimeWnd->SetCurrentDateTime( InitialTime.first );
	m_pEndTimeWnd->SetCurrentDateTime( InitialTime.second );
}

//////////////////////////////////////////////////////////////////////////

void ArchiveControlPanel::OnPlayArchive( wxCommandEvent& ev )
{
	_ASSERTE(m_pCallback);
	m_State.reset( new ArchiveFramePlay( m_pVideoControlBar ) );
	m_pCallback->OnPlayArchive();
}

void ArchiveControlPanel::OnPauseArchive( wxCommandEvent& ev )
{
	_ASSERTE(m_pCallback);
	m_State.reset( new ArchiveFramePause(m_pVideoControlBar) );
	m_pCallback->OnPauseArchive();
}

void ArchiveControlPanel::OnStopArchive( wxCommandEvent& ev )
{
	StopArchive();
}

void ArchiveControlPanel::StopArchive()
{
	_ASSERTE(m_pCallback);
	m_State.reset( new ArchiveFrameStop(m_pVideoControlBar) );
	m_pCallback->OnStopArchive();
}

void ArchiveControlPanel::OnTimeScaleChanged( wxCommandEvent& ev )
{
	int nIndex = m_pTimeScaleCombobox->GetSelection();
	_ASSERTE( nIndex != wxNOT_FOUND  );

	SetTimeMode( nIndex );
}

void	ArchiveControlPanel::SetTimeMode( int TimeMode )
{
	switch( TimeMode ) 
	{
	case ComboboxItem_Minutes :
		m_pAlarmWnd->SetTimeMode( AlarmViewControl::Minutes );
		break;
	case ComboboxItem_Hours :
		m_pAlarmWnd->SetTimeMode( AlarmViewControl::Hours );
		break;
	case ComboboxItem_Days :
		m_pAlarmWnd->SetTimeMode( AlarmViewControl::Days );
		break;
	default:
		_ASSERTE(false); // no such mode
	}
}

void	ArchiveControlPanel::SetCurrentArchiveTime( time_t NewTime )
{
	bool bPlayArchive = m_pAlarmWnd->SetCurrentArchiveTime( NewTime );
	if( !bPlayArchive )
	{
		StopArchive();
	}
}

int	ArchiveControlPanel::GetSuitableTimeMode( time_t StartTime, time_t EndTime ) const
{
	int nTimePeriod = EndTime - StartTime;

	time_duration td ( time_period( from_time_t( StartTime ), from_time_t(EndTime) ).length() );
	if( td.hours() < 6  )
	{
		return ComboboxItem_Minutes;
	}
	else if( td.hours() < 24*6 )
	{
		return ComboboxItem_Hours;
	}
	else 
	{
		return ComboboxItem_Days;
	}
}

void ArchiveControlPanel::ReleaseArchiveData()
{
	m_pAlarmWnd->InvalidatePeriod();
	m_State.reset( new ArchiveFrameIdle(m_pVideoControlBar) );
}