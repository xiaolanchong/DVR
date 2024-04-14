//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Хранит по камерам произошедшие события для архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ALARM_RECT_DATA_1438854645977401_
#define _ALARM_RECT_DATA_1438854645977401_

//======================================================================================//
//                                 class AlarmRectData                                  //
//======================================================================================//

//! \brief Хранит по камерам произошедшие события для архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class AlarmRectData
{
	Frames_t	m_AllAlarms;
public:
	AlarmRectData( const Frames_t& AllAlarmData );

	AlarmRectData( );

	virtual ~AlarmRectData();

	//! отдает массив рамок для камеры в заданное время
	//! \param CameraID		идентификатор камеры
	//! \param ArchiveTime	прошедшее время 
	//! \param AlarmRectForTimeAndCamera массив координат рамок за эту секунду
	void GetData( 
		int CameraID, 
		time_t  ArchiveTime, 
		Frames_t& AlarmRectForTimeAndCamera 
		) const;
};

#endif // _ALARM_RECT_DATA_1438854645977401_