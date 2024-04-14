//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных для алгоритма зала, зал->сервер

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MSG_HALL_SET_DATA_9052622092277540_
#define _MSG_HALL_SET_DATA_9052622092277540_

#include "IThreadMessage.h"
#include "../AlarmInfo.h"
#include "../CameraData.h"

//======================================================================================//
//                                 class MsgHallResponseAlarmData                       //
//======================================================================================//

//! \brief Передача данных для алгоритма зала, зал->сервер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class MsgHallResponseAlarmData : public IThreadMessage
{
	//! данные для всех камер
	CameraData		m_DataForCamera;
	//! рамки объектов
	Frames_t		m_FramesForDatabase;

	//! рамки объектов
	Frames_t		m_FramesForRender;

public:
	MsgHallResponseAlarmData( const CameraData& DataForCamera, 
							  const Frames_t& FramesForRender,
							  const Frames_t& FramesForDatabase ) ;
	virtual ~MsgHallResponseAlarmData() ;

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); };

	const CameraData&		GetCameraData() const		{ return m_DataForCamera;	}
	const Frames_t&			GetFramesForRender()	const	{ return m_FramesForRender;	}
	const Frames_t&			GetFramesForDatabase()	const	{ return m_FramesForDatabase;	}
};

#endif // _MSG_HALL_SET_DATA_9052622092277540_