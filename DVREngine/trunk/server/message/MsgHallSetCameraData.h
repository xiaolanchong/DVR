//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача залу информации от камеры

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MSG_HALL_SET_CAMERA_DATA_5651000029321653_
#define _MSG_HALL_SET_CAMERA_DATA_5651000029321653_

#include "IThreadMessage.h"

//======================================================================================//
//                              class MsgHallSetCameraData                              //
//======================================================================================//

//! \brief Передача залу информации от камеры
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class MsgHallSetCameraData  : public IThreadMessage
{
	int				m_nCameraID;
	ByteArray_t		m_Data;
public:
	MsgHallSetCameraData(int nCameraID, const ByteArray_t& Data);
	virtual ~MsgHallSetCameraData();

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); }

	int					GetCameraID() const		{ return m_nCameraID;	}
	const ByteArray_t&	GetCameraData() const	{ return m_Data;		}
};

#endif // _MSG_HALL_SET_CAMERA_DATA_5651000029321653_