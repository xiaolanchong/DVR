//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Timer classes

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   19.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _TIMER_2349643854834625_
#define _TIMER_2349643854834625_
//======================================================================================//
//                                     class Timer                                      //
//======================================================================================//

//! \brief ����� ���������� ������������������� ������� ����������� ���������� �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class Timer
{
	//! ������ ������
	std::time_t		m_nStartClock;
	//! ������ �������
	double		m_fPeriod;
public:
	//! \param fPeriod ������ ������� � ���
	Timer( double fPeriod ) : 
		m_nStartClock(  ),
		m_fPeriod(fPeriod) 
		{
			Reset();
		};
	~Timer() {};

	//! �������� ������ �������
	void	Reset()		
	{ 
		std::time( &m_nStartClock ) ;
	}

	//! ��� �������
	//! \return ������ � ��������� �������
	bool	Ticked()	
	{
		std::time_t CurTime;
		std::time( &CurTime) ; 
		double fPeriod = std::difftime( CurTime , m_nStartClock );
		if( fPeriod >= m_fPeriod )
		{
			Reset();
			return true;
		}
		else
			return false;
	}
};

#endif // _TIMER_2349643854834625_