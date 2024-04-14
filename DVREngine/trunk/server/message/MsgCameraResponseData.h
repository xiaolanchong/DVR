//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных для зала, камера->сервер

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MSG_CAMERA_SET_DATA_2565727618417086_
#define _MSG_CAMERA_SET_DATA_2565727618417086_

#include "IThreadMessage.h"

//======================================================================================//
//                                class MsgCameraSetData                                //
//======================================================================================//

//! \brief Передача данных для зала, камера->сервер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class MsgCameraResponseData : public IThreadMessage
{
	//! идентификатор камеры, от которой передаются данные
	int				m_nCameraID;
	//! данные-двоичный массив от камеры
	ByteArray_t		m_CameraArr;
public:
	//! \param nCameraID	идентификатор камеры, от которой передаются данные
	//! \param Arr			данные-двоичный массив от камеры
	MsgCameraResponseData( int nCameraID, const ByteArray_t& Arr);
	virtual ~MsgCameraResponseData();

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); };

	//! получить идент-р камеры, отправившей сообщение
	int					GetCameraID() const		{ return m_nCameraID;	}
	//! получить данные
	const ByteArray_t&	GetCameraData() const	{ return m_CameraArr;	}
};

#endif // _MSG_CAMERA_SET_DATA_2565727618417086_