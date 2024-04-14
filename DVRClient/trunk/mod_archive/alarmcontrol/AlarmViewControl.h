//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ёлемент вывода информации о ситуаци€х за промежуток времени

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

//! \brief Ёлемент вывода информации о ситуаци€х за промежуток времени
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class AlarmViewControl : public wxScrolledWindow
{
public:
	//! массив начало-конец тревоги
	typedef std::vector< std::pair<time_t, time_t> >	AlarmArr_t;
	//! словарь по всем камерам
	typedef std::map<int, AlarmArr_t>	CameraAlarmMap_t;
private:
	//! начало промежутка, в котором просматриваютс€ ситуации
	time_t m_StartTime;
	//! ну и конец
	time_t m_EndTime;
	//! врем€ установлено?
	bool m_bValidTime;

	//! обратный вызов при seek
	boost::function1< void, time_t>		m_fnOnTimeChanged;

	//! смещение слева в пиксел€х
	int		GetDatetimeAreaWidth() const;
	//! смещение снизу в писксел€х
	int		GetCameraNumberAreaHeight() const;
	//! нирина в пиксел€х
	int		GetCameraNumberWidth() const;

	//! получить смещение от левого верхнего угла, от которого рисовать
	//! \return в пиксел€х величина по горизонтали, вертикали
	wxSize	GetStartOffset() const
	{
		return wxSize( GetDatetimeAreaWidth(), GetCameraNumberAreaHeight() );
	}

	//! интерфейс отображени€ масштабной сетки с числами времени
	std::auto_ptr<ITimeLineRender>	m_pTimeLineRender;

	//! все тревоги
	CameraAlarmMap_t		m_CameraMap;
	//! текущее врем€, если установлено
	boost::optional<time_t>	m_CurrentTime;

	//! нарисовать сетку времени
	//! \param dc 
	//! \param DrawArea 
	void	DrawCameraLines( wxDC& dc, const wxRect& DrawArea ) const;

	//! нарисовать позицию текущего времени
	//! \param dc 
	//! \param DrawArea 
	void	DrawCurrentTime( wxDC& dc, const wxRect& DrawArea  ) const;

	//! нарисовать, если нет данных (промежутка времени, режим по умолчанию)
	//! \param dc 
	void	DrawControlWithoutData( wxDC& dc ) const; 

	//! получить ширину полной зоны окна дл€ скроллбара
	//! \return минимаоьна€ ширина окна, в которой можно отобразить его содержимое
	int		GetVirtualWidth( ) const;

	//! установить величины прокрутки
	void	AdjustScrollbarsForRender();

	wxSize	GetMaxVisibleSize() const;

//	void	DrawCurrentPartOfAlarmRect( wxDC& dc, const wxRect& CurrentDrawRect, time_t StartAlarmTime, time_t EndAlarmTime ) const;

	void	DrawAlarmRect( wxDC& dc, const wxRect& CurrentDrawRect, time_t StartAlarmTime, time_t EndAlarmTime ) const;

	void	DrawAlarmPartitialRect( wxDC& dc, const wxRect& CurrentDrawRect, int StartY, int  EndY, const wxColour& cl ) const;

	void	Draw(wxDC & dc, const wxRect& DrawArea);
public:
	//! создать
	//! \param pParentWnd родительское окно
	//! \param fnOnTimeChanged ф-€ обратного вызова, если пользователь прокручивает врем€
	AlarmViewControl( wxWindow* pParentWnd, boost::function1< void, time_t> fnOnTimeChanged);
	virtual ~AlarmViewControl();

	//! разрешение сетки времени
	enum TimeMode
	{
		Days,
		Hours,
		Minutes
	};

	//! обнулить период времени
	void	InvalidatePeriod();

	//! начать работу
	//! \param StartTime начало промежутка
	//! \param EndTime конец промежутка
	//! \param CameraAlarmMap все тревоги
	void	SetPeriod( time_t StartTime, time_t EndTime, const CameraAlarmMap_t& CameraAlarmMap  ); 

	//! помен€ть разрешение стеки времени
	//! \param NewTimeMode новый масштаб
	void	SetTimeMode( TimeMode NewTimeMode );

	//! установить новое текущее врем€ 
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