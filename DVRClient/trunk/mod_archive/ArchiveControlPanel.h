//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���� ������� ������� ������ + �������� ��������

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

//! \brief ���� ������� ������� ������ + �������� ��������
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

	//! ��������� ���� ��������� ������
	std::auto_ptr<ArchiveFrameState>	m_State;

	ArchiveControlPanelCallback*		m_pCallback;

	//! ������� ���������� Date-time, ������ ������� ������ � �������
	std::auto_ptr<DateTimeSource>	m_pStartTimeWnd;
	//! ������� ���������� Date-time, ����� ������� ������ � �������
	std::auto_ptr<DateTimeSource>	m_pEndTimeWnd;
	//! ������� ���������� ��������� ��������� ��������
	AlarmViewControl*	m_pAlarmWnd;
	//! video seek bar, �������� 

	CameraNumberStatic*	m_pCameraNumberStatic;

	//! ���� ��� ����������� �������� ����� ������ ( � ���� ��������������� ������)
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

	//! ��������� ������ � Schedule - �������
	//! \param StartTime	������ ������� �������, � ���. ��������� ������
	//! \param EndTime		����� �������
	//! \param AlarmRects	����� ��������
	void	LoadDataToSchedule(  time_t StartTime, time_t EndTime, const Frames_t& AlarmRects );

	DECLARE_EVENT_TABLE();

	//! ���������� ������� ������ 'Start'
	//! \param ev	���-� � �������
	void OnStartArchive( wxCommandEvent& ev );

	void OnPlayArchive( wxCommandEvent& ev );
	void OnPauseArchive( wxCommandEvent& ev );
	void OnStopArchive( wxCommandEvent& ev );

	void OnTimeScaleChanged( wxCommandEvent& ev );
};

#endif // _ARCHIVE_CONTROL_PANEL_5826262810537446_