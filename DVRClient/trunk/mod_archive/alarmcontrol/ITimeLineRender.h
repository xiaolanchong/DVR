//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс для отображения сетки времени

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_TIME_LINE_RENDER_7693020547393758_
#define _I_TIME_LINE_RENDER_7693020547393758_
//======================================================================================//
//                                class ITimeLineRender                                 //
//======================================================================================//

//! \brief Интерфейс для отображения сетки времени
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ITimeLineRender
{
	const int		m_StepInSeconds ;
	const double	m_PixelsInSecond;

	std::pair<std::string, wxColour>	GetTextForTime( const tm& t ) const
	{
		if (t.tm_hour == 0 && t.tm_min == 0 )
		{
			std::string s = ( boost::format("%02u.%02u") % t.tm_mday % (t.tm_mon + 1) ).str() ;
			return std::make_pair( s, *wxBLUE );
		}
		else
		{
			std::string s = ( boost::format("%02u:%02u") % t.tm_hour % t.tm_min ).str();
			return std::make_pair( s, *wxBLACK  );
		}
	}

	int		GetPeriodInSeconds( const tm& from, const tm& to ) const
	{
		tm _from(from);
		tm _to(to);
		time_t tFrom	= mktime(&_from);
		time_t tTo		= mktime(&_to);
		return difftime( tTo, tFrom );
	}

	bool	IsMainTimeMark( time_t ttt ) const
	{
		//const tm *tt = localtime( &ttt );
		tm tt;
		bool res = localtimeWrap(ttt, tt) ;
		return  res? IsMainTimeMark( tt ): false;
	}
protected:
	static const int c_SecondsInMinute	= 60;
	static const int c_MinutesInHours	= 60;
	static const int c_HoursInDay		= 24;
	//void	DrawLine(  )	

#if 0
	virtual	std::pair<int, bool>		GetFirstOffset( tm& t ) const = 0;
	virtual std::pair<int, bool>		NextStep( tm& t) const = 0;
#else
	virtual tm		GetNearestTime( const tm& StartTime ) const = 0;
	virtual bool	IsMainTimeMark( const tm& AnyTime) const = 0;
protected:
	int				GetStepInSeconds() const 
	{
		return m_StepInSeconds;
	}

	double			GetPixelsInSecond() const
	{
		return m_PixelsInSecond;
	}
#endif	

protected:

	ITimeLineRender(int StepInSeconds, double PixelsInSecond );
public:
	virtual ~ITimeLineRender();

	int	GetOffsetInPixels( int nPeriodInSeconds ) const 
	{
		return static_cast<int>( GetPixelsInSecond() * nPeriodInSeconds );
	}
	
	void	DrawTimeLines( 
								wxDC& dc, const wxRect& DrawArea,
								time_t StartTime, time_t EndTime ) const ;
};

#endif // _I_TIME_LINE_RENDER_7693020547393758_