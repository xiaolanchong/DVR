//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: �������� ������ ��� ��������� ����, ���->������

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

//! \brief �������� ������ ��� ��������� ����, ���->������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class MsgHallResponseAlarmData : public IThreadMessage
{
	//! ������ ��� ���� �����
	CameraData		m_DataForCamera;
	//! ����� ��������
	Frames_t		m_FramesForDatabase;

	//! ����� ��������
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