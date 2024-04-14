//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� ������ ���������� � ��������� �� ���������� �������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ALARM_VIEW_CONTROL_3428590154196546_
#define _ALARM_VIEW_CONTROL_3428590154196546_



class ITimeLineRender;
//======================================================================================//
//                                class AlarmViewControl                                //
//======================================================================================//

//! \brief ������� ������ ���������� � ��������� �� ���������� �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class AlarmViewControl : public wxScrolledWindow
{
public:
	//! ������ ������-����� �������
	typedef std::vector< std::pair<time_t, time_t> >	AlarmArr_t;
	//! ������� �� ���� �������
	typedef std::map<int, AlarmArr_t>	CameraAlarmMap_t;
private:
	//! ������ ����������, � ������� ��������������� ��������
	time_t m_StartTime;
	//! �� � �����
	time_t m_EndTime;
	//! ����� �����������?
	bool m_bValidTime;

	//! �������� ����� ��� seek
	boost::function1< void, time_t>		m_fnOnTimeChanged;

	//! �������� ����� � ��������
	int		GetDatetimeAreaWidth() const;
	//! �������� ����� � ���������
	int		GetCameraNumberAreaHeight() const;
	//! ������ � ��������
	int		GetCameraNumberWidth() const;

	//! �������� �������� �� ������ �������� ����, �� �������� ��������
	//! \return � �������� �������� �� �����������, ���������
	wxSize	GetStartOffset() const
	{
		return wxSize( GetDatetimeAreaWidth(), GetCameraNumberAreaHeight() );
	}

	//! ��������� ����������� ���������� ����� � ������� �������
	std::auto_ptr<ITimeLineRender>	m_pTimeLineRender;

	//! ��� �������
	CameraAlarmMap_t		m_CameraMap;
	//! ������� �����, ���� �����������
	boost::optional<time_t>	m_CurrentTime;

	//! ���������� ����� �������
	//! \param dc 
	//! \param DrawArea 
	void	DrawCameraLines( wxDC& dc, const wxRect& DrawArea ) const;

	//! ���������� ������� �������� �������
	//! \param dc 
	//! \param DrawArea 
	void	DrawCurrentTime( wxDC& dc, const wxRect& DrawArea  ) const;

	//! ����������, ���� ��� ������ (���������� �������, ����� �� ���������)
	//! \param dc 
	void	DrawControlWithoutData( wxDC& dc ) const; 

	//! �������� ������ ������ ���� ���� ��� ����������
	//! \return ����������� ������ ����, � ������� ����� ���������� ��� ����������
	int		GetVirtualWidth( ) const;

	//! ���������� �������� ���������
	void	AdjustScrollbarsForRender();

	wxSize	GetMaxVisibleSize() const;

//	void	DrawCurrentPartOfAlarmRect( wxDC& dc, const wxRect& CurrentDrawRect, time_t StartAlarmTime, time_t EndAlarmTime ) const;

	void	DrawAlarmRect( wxDC& dc, const wxRect& CurrentDrawRect, time_t StartAlarmTime, time_t EndAlarmTime ) const;

	void	DrawAlarmPartitialRect( wxDC& dc, const wxRect& CurrentDrawRect, int StartY, int  EndY, const wxColour& cl ) const;

	void	Draw(wxDC & dc, const wxRect& DrawArea);
public:
	//! �������
	//! \param pParentWnd ������������ ����
	//! \param fnOnTimeChanged �-� ��������� ������, ���� ������������ ������������ �����
	AlarmViewControl( wxWindow* pParentWnd, boost::function1< void, time_t> fnOnTimeChanged);
	virtual ~AlarmViewControl();

	//! ���������� ����� �������
	enum TimeMode
	{
		Days,
		Hours,
		Minutes
	};

	//! �������� ������ �������
	void	InvalidatePeriod();

	//! ������ ������
	//! \param StartTime ������ ����������
	//! \param EndTime ����� ����������
	//! \param CameraAlarmMap ��� �������
	void	SetPeriod( time_t StartTime, time_t EndTime, const CameraAlarmMap_t& CameraAlarmMap  ); 

	//! �������� ���������� ����� �������
	//! \param NewTimeMode ����� �������
	void	SetTimeMode( TimeMode NewTimeMode );

	//! ���������� ����� ������� ����� 
	//! \param CurrentTime 
	//! \return reset time period?
	bool	SetCurrentArchiveTime( time_t CurrentTime );
private:
	virtual void OnDraw(wxDC& dc);

	void	OnLButton( wxMouseEvent& ev ); 
	void	OnEraseBkgrnd( wxEraseEvent& ev ); 

	DECLARE_EVENT_TABLE()
};

#endif // _ALARM_VIEW_CONTROL_3428590154196546_