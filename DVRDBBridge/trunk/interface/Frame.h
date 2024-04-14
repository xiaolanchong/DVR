//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Информация о рамках обнаруженных объектов на видеоизображении

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   28.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_1273197843846346_
#define _FRAME_1273197843846346_

//======================================================================================//
//                                     class Frame                                      //
//======================================================================================//

//! \brief Информация о рамках обнаруженных объектов на видеоизображении
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.03.2006
//! \version 1.0
//! \bug 
//! \todo 

namespace DBBridge
{

class Frame
{
public:
	struct Rect
	{
		float x;		//!< x-coordinate of the top left point
		float y;		//!< y-coordinate of the top left point
		float w;		//!< rectangle width
		float h;		//!< rectangle height
		Rect(float _x, float _y, float _w, float _h): x(_x), y(_y), w(_w), h(_h){}
		Rect(){}
	};

	time_t				GetStartTime()		const { return m_StartTime;		}
	time_t				GetEndTime()		const { return m_EndTime;		}
	int					GetCameraID()		const { return m_CameraID;	}
	const Frame::Rect&	GetRect()			const { return m_Rect;		}	

	Frame( int nCameraID, time_t StartTime, time_t EndTime, const Rect& rc ) : 
			m_CameraID(nCameraID), 
			m_StartTime(StartTime), 
			m_EndTime(EndTime), 
			m_Rect(rc){}
private:
	//! время начала действия рамки
	time_t	m_StartTime;
	//! время конца действия рамки
	time_t	m_EndTime;
	//! камера, где была рамка
	int		m_CameraID;
	//! прямоугольная зона на изображении
	Rect	m_Rect;
};

}

#endif // _FRAME_1273197843846346_