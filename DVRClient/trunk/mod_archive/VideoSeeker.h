//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� ����������-�������� ��� ���������� �������� ��������� �����

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

//! \brief ������� ����������-�������� ��� ���������� �������� �����
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   27.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class VideoSeeker : public  wxGauge
{
	friend class WorkVideoSeekerState;

	//! �-� ��������� ������ �� ������� ����
	boost::function0<void>			m_Callback;
	//! ��������� ���������
	std::auto_ptr<VideoSeekerState> m_State;
public:
	VideoSeeker(wxWindow* pParentWnd, boost::function0<void> cb);
	virtual ~VideoSeeker();

	//! ������ ������ ������� ���������
	void	SetPeriod(time_t Start, time_t End);
	//! �������� ������� ����� ���������
	void	InvalidatePeriod();
	//! ���������� ������� �����
	//! \param nCurrentTime	����� � ���������, ������������� SetPeriod
	void	SetCurrentTime(time_t nCurrentTime);
	//! ������� ������� �����
	//! \return ����� � ���������, ������������� SetPeriod
	time_t	GetCurrentTime() const;
private:
	DECLARE_EVENT_TABLE();

	void OnLButton( wxMouseEvent& ev );
};

#endif // _VIDEO_SEEKER_2111791223619013_