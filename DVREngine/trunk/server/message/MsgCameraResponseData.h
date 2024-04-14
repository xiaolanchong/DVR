//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: �������� ������ ��� ����, ������->������

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

//! \brief �������� ������ ��� ����, ������->������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class MsgCameraResponseData : public IThreadMessage
{
	//! ������������� ������, �� ������� ���������� ������
	int				m_nCameraID;
	//! ������-�������� ������ �� ������
	ByteArray_t		m_CameraArr;
public:
	//! \param nCameraID	������������� ������, �� ������� ���������� ������
	//! \param Arr			������-�������� ������ �� ������
	MsgCameraResponseData( int nCameraID, const ByteArray_t& Arr);
	virtual ~MsgCameraResponseData();

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); };

	//! �������� �����-� ������, ����������� ���������
	int					GetCameraID() const		{ return m_nCameraID;	}
	//! �������� ������
	const ByteArray_t&	GetCameraData() const	{ return m_CameraArr;	}
};

#endif // _MSG_CAMERA_SET_DATA_2565727618417086_