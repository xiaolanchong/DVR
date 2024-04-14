//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ёлемент управлени€-ползунок дл€ управлени€ позицией архивного видео

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   17.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_SEEKER_2111791223619013_
#define _VIDEO_SEEKER_2111791223619013_

//using namespace boost::posix_time;

class VideoSeekerState;

//======================================================================================//
//                                  class VideoSeeker                                   //
//======================================================================================//

//! \brief Ёлемент управлени€-ползунок дл€ управлени€ архивным видео
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   27.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class VideoSeeker : public  wxGauge
{
	friend class WorkVideoSeekerState;

	//! ф-€ обратного вызова по нажатию мыши
	boost::function0<void>			m_Callback;
	//! состо€ние прокрутки
	std::auto_ptr<VideoSeekerState> m_State;
public:
	VideoSeeker(wxWindow* pParentWnd, boost::function0<void> cb);
	virtual ~VideoSeeker();

	//! задать период времени прокрутки
	void	SetPeriod(time_t Start, time_t End);
	//! обнулить текущуе врем€ прокрутки
	void	InvalidatePeriod();
	//! установить текущее врем€
	//! \param nCurrentTime	врем€ в диапазоне, установленном SetPeriod
	void	SetCurrentTime(time_t nCurrentTime);
	//! вернуть текущее врем€
	//! \return врем€ в диапазоне, установленном SetPeriod
	time_t	GetCurrentTime() const;
private:
	DECLARE_EVENT_TABLE();

	void OnLButton( wxMouseEvent& ev );
};

#endif // _VIDEO_SEEKER_2111791223619013_