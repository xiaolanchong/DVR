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
#ifndef _ARCHIVE_CONTROL_PANEL_5826262810537446_
#define _ARCHIVE_CONTROL_PANEL_5826262810537446_

class DateTimeSource;
class AlarmViewControl;
class CameraNumberStatic;
class ArchiveFrameState;

class ArchiveControlPanelCallback
{
public:
	virtual boost::shared_ptr<Frames_t> OnStartArchive( time_t StartTime, time_t EndTime ) = 0;
	virtual void						OnPlayArchive() = 0;
	virtual void						OnPauseArchive() = 0;
	virtual void						OnStopArchive() = 0;
	virtual void						OnArchiveTimeChanged( time_t NewArchiveTime ) = 0;
	virtual void						GetCurrentSelectedCameras(std::vector<int>& CameraIDArr) = 0;
};

//======================================================================================//
//                              class ArchiveControlPanel                               //
//======================================================================================//

//! \brief Окно задание времени архива + просмотр ситуаций
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   07.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveControlPanel : public wxPanel
{
	enum
	{
		ComboboxItem_Minutes = 0,
		ComboboxItem_Hours	= 1,
		ComboboxItem_Days	= 2
	};

	int		GetSuitableTimeMode( time_t StartTime, time_t EndTime ) const;
	void	SetTimeMode( int TimeMode );
public:
	ArchiveControlPanel(ArchiveControlPanelCallback* pCallback);
	virtual ~ArchiveControlPanel();

	void	Init();


	void	SetCameras( const std::vector<int>& Ids)
	{
		m_CameraIds = Ids;
	}

	void	SetCurrentArchiveTime( time_t NewTime );

	void	ReleaseArchiveData();

private:
	std::vector<int>	m_CameraIds;

	//! состояние окна просмотра архива
	std::auto_ptr<ArchiveFrameState>	m_State;

	ArchiveControlPanelCallback*		m_pCallback;

	//! элемент управления Date-time, начало периода работы с архивом
	std::auto_ptr<DateTimeSource>	m_pStartTimeWnd;
	//! элемент управления Date-time, конец периода работы с архивом
	std::auto_ptr<DateTimeSource>	m_pEndTimeWnd;
	//! элемент управления просмотра тревожных ситуаций
	AlarmViewControl*	m_pAlarmWnd;
	//! video seek bar, ползунок 

	CameraNumberStatic*	m_pCameraNumberStatic;

	//! поле для отображения текущего время архива ( в виде форматированной строки)
	wxStaticText*		m_pCurrentTimeText;

	wxToolBar*			m_pVideoControlBar;

	wxComboBox*			m_pTimeScaleCombobox;

	void	InitTimeControls();


	void	OnTimeChanged( time_t NewArchiveTime )
	{
		_ASSERT(m_pCallback);
		m_pCallback->OnArchiveTimeChanged( NewArchiveTime );
	}

	void StopArchive();

	//! загрузить данные в Schedule - контрол
	//! \param StartTime	начало периода времени, к кот. относятся данные
	//! \param EndTime		конец периода
	//! \param AlarmRects	рамки ситуаций
	void	LoadDataToSchedule(  time_t StartTime, time_t EndTime, const Frames_t& AlarmRects );

	DECLARE_EVENT_TABLE();

	//! обработчик нажатия кнопки 'Start'
	//! \param ev	инф-я о событии
	void OnStartArchive( wxCommandEvent& ev );

	void OnPlayArchive( wxCommandEvent& ev );
	void OnPauseArchive( wxCommandEvent& ev );
	void OnStopArchive( wxCommandEvent& ev );

	void OnTimeScaleChanged( wxCommandEvent& ev );
};

#endif // _ARCHIVE_CONTROL_PANEL_5826262810537446_