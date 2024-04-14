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

#include "stdafx.h"
#include "VideoSeeker.h"

//! класс состо€ни€ прокрутки в зависимости от заданного периода
class VideoSeekerState
{
protected:
	VideoSeeker&	m_Seeker;
	VideoSeekerState( VideoSeeker& sk ): m_Seeker(sk){}

public:
	//! событие - установка текущего времни
	virtual void	SetCurrentTime(time_t nCurrentTime) = 0;
	//! событие - нажатие левой кнопки в окне
	//! \param XMouseCoord x-координата позиции нажати€
	virtual void	OnLButton( int XMouseCoord ) = 0;
	//! вернуть текущее врем€
	virtual time_t	GetCurrentTime() const = 0;
};

//! состо€ние с неверными данными (по умолчанию)
class InvalidVideoSeekerState : public VideoSeekerState
{
public:
	InvalidVideoSeekerState( VideoSeeker& sk ): VideoSeekerState(sk)
	{
		sk.Enable(false);
		sk.SetRange(0);
	}
private:
	virtual void	SetCurrentTime(time_t nCurrentTime)
	{
	}

	virtual void OnLButton( int XMouseCoord )
	{

	}

	virtual time_t	GetCurrentTime() const
	{
		return 0;
	}
};

//! состо€ние с рабочими данными
class WorkVideoSeekerState : public VideoSeekerState
{
	void	UpdateCurrentPosition()
	{
		int nCurrentPos = m_CurrentTime - m_TimePeriod.first;
		m_Seeker.SetValue( nCurrentPos );
	}
public:
	WorkVideoSeekerState( VideoSeeker& sk, time_t Start, time_t End ): 
		VideoSeekerState(sk),
		m_TimePeriod(Start, End),
		m_CurrentTime(Start)
	{
		_ASSERTE( End > Start );
		sk.Enable(true);
		sk.SetRange( End - Start );
		UpdateCurrentPosition();
	}
private:
	virtual void	SetCurrentTime(time_t nCurrentTime)
	{
		_ASSERTE( m_TimePeriod.second > m_TimePeriod.first );
		_ASSERTE( m_TimePeriod.first <= nCurrentTime );
		_ASSERTE( m_TimePeriod.second >= nCurrentTime );
		m_CurrentTime = nCurrentTime;
		UpdateCurrentPosition();
	}

	virtual void OnLButton( int XMouseCoord )
	{
		_ASSERTE( m_TimePeriod.second > m_TimePeriod.first  );
		wxSize sizeWindow = m_Seeker.GetClientSize();
		int tdAll	= m_TimePeriod.second - m_TimePeriod.first;
		int tdClick	= tdAll * XMouseCoord / sizeWindow.GetX();
		m_CurrentTime = m_TimePeriod.first + tdClick;
		UpdateCurrentPosition();
		if( !m_Seeker.m_Callback.empty() )
		{
			m_Seeker.m_Callback();
		}
	}

	virtual time_t	GetCurrentTime() const
	{
		_ASSERTE( m_TimePeriod.second > m_TimePeriod.first );
		return m_CurrentTime;
	}

	//! заданный период времени, в котором прокручиваем
	std::pair<time_t, time_t>		m_TimePeriod;
	//! текущее врем€
	time_t							m_CurrentTime;
};

//======================================================================================//
//                                  class VideoSeeker                                   //
//======================================================================================//

VideoSeeker::VideoSeeker(wxWindow* pParentWnd, boost::function0<void> cb):
	wxGauge( pParentWnd, wxID_ANY, 0 , wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH ) ,
	m_Callback(cb)
{
	m_State = std::auto_ptr<VideoSeekerState>( new InvalidVideoSeekerState(*this) );
}

VideoSeeker::~VideoSeeker()
{
}

BEGIN_EVENT_TABLE(VideoSeeker, wxGauge)
	EVT_LEFT_DOWN ( VideoSeeker::OnLButton )
END_EVENT_TABLE()

void	VideoSeeker::SetPeriod(time_t Start, time_t End)
{
	if( Start >= End )
	{
		m_State = std::auto_ptr<VideoSeekerState>( new InvalidVideoSeekerState(*this) );
	}
	else
	{
		m_State = std::auto_ptr<VideoSeekerState>( new WorkVideoSeekerState(*this, Start, End ) );
	}
}

time_t	VideoSeeker::GetCurrentTime() const
{
	return m_State->GetCurrentTime();
}

void	VideoSeeker::InvalidatePeriod()
{
	m_State = std::auto_ptr<VideoSeekerState>( new InvalidVideoSeekerState(*this) );
}

void	VideoSeeker::SetCurrentTime(time_t nCurrentTime)
{
	m_State->SetCurrentTime(nCurrentTime);
	Refresh();
}

void	VideoSeeker::OnLButton( wxMouseEvent& ev )
{
	m_State->OnLButton( ev.GetX() );
	Refresh();
}