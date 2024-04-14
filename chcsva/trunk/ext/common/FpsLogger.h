//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� fps � ���������� ����� ����� ������ N ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FPS_LOGGER_6271540768279430_
#define _FPS_LOGGER_6271540768279430_

//#include "MilliSecTimer.h"

//======================================================================================//
//                                   class FpsLogger                                    //
//======================================================================================//

//! \brief ������������ fps ����� ������ N ������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class FpsLogger
{
	//! ������ ��� ��������
//	MilliSecTimer		m_Timer;
	//! ���������� ����� �������
	unsigned int				m_PrevTime;
	//! ������ ��������
	unsigned int				m_Period;
	//! ���-�� ������ �� �������
	unsigned int				m_FrameCounter;
public:
	//! \param nPeriod ���, ������ ��������
	FpsLogger( unsigned int nPeriod = 10 );
	//! ��������� ����
	//! \return fps, ���� �������� �����������
	boost::optional<double>	GetFPS();

	void	IncrementFrame();
};

#endif // _FPS_LOGGER_6271540768279430_