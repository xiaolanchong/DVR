//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ��� ��������� ��������� ����� ��������

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_MESSAGE_PROCESSOR_6934025625416804_
#define _I_MESSAGE_PROCESSOR_6934025625416804_

class IThreadMessage;

class MsgCameraResponseData;
class MsgCameraSetHallData;

class MsgHallSetCameraData;
class MsgHallRequestAlarmData;
class MsgHallResponseAlarmData;

//======================================================================================//
//                               class IMessageProcessor                                //
//======================================================================================//

//! \brief ��������� ��� ��������� ��������� ����� ��������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMessageProcessor
{
public:
	virtual ~IMessageProcessor() {};

	//! ���������� MsgCameraResponseData ��������� - ������ �� ����� ������ � ����
	//! \param p ��������� �� ���������
	virtual void	Process( MsgCameraResponseData*		p ) = 0;
	//! ���������� MsgCameraSetHallData ��������� - ������ ��� ���� ����� �� ����
	//! \param p ��������� �� ���������
	virtual void	Process( MsgCameraSetHallData*		p ) = 0;

	//! ���������� MsgHallSetCameraData ��������� - ������ �� ������ ����
	//! \param p ��������� �� ���������
	virtual void	Process( MsgHallSetCameraData*		p ) = 0;
	//! ���������� MsgHallRequestAlarmData ��������� - ������ �� �����
	//! \param p ��������� �� ���������
	virtual void	Process( MsgHallRequestAlarmData*		p ) = 0;
	//! ���������� MsgHallResponseAlarmData ��������� - ������ ��� ����� + ����� ��������
	//! \param p ��������� �� ���������
	virtual void	Process( MsgHallResponseAlarmData*		p ) = 0;
};

#endif // _I_MESSAGE_PROCESSOR_6934025625416804_