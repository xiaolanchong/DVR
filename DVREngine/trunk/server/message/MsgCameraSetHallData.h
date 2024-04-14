//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных зала камерам, сервер->камера

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MSG_CAMERA_SET_HALL_DATA_9048417479561736_
#define _MSG_CAMERA_SET_HALL_DATA_9048417479561736_

#include "IThreadMessage.h"
#include "../CameraData.h"
#include "../AlarmInfo.h"

//======================================================================================//
//                              class MsgCameraSetHallData                              //
//======================================================================================//

//! \brief Передача данных зала камерам, сервер->камера
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class MsgCameraSetHallData : public IThreadMessage
{
	//! данные то зала для всех камер
	CameraData		m_DataArr;

	Frames_t		m_AlarmFrames;
public:
	MsgCameraSetHallData(const CameraData& d, const Frames_t& AlarmFrames) ;
	virtual ~MsgCameraSetHallData() ;

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); };

	const CameraData&	GetHallData() const	{ return m_DataArr;	}
	const Frames_t&		GetFrames() const { return m_AlarmFrames; }
};

#endif // _MSG_CAMERA_SET_HALL_DATA_9048417479561736_