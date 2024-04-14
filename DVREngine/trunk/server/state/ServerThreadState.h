//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ��������� �������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_THREAD_STATE_6131551782488200_
#define _SERVER_THREAD_STATE_6131551782488200_

#include "../message/IMessageProcessor.h"

class ServerImpl;

//======================================================================================//
//                               class ServerThreadState                                //
//======================================================================================//

//! \brief ��������� ��������� �������, �������� �������� �� ��������� ���������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerThreadState : public  IMessageProcessor
{
protected:
	//! ��������� �� �������� ���������
	ServerImpl*		m_pImpl;

public:
	ServerThreadState(ServerImpl* p);
	virtual ~ServerThreadState();

	//! ���������, �������� ���������
	//! \return NULL - ��������� �����������, ����� ����� ���������
	std::auto_ptr<ServerThreadState> Process();

	//! �������� ��������� ���������
	//! \param p ��������� �� ������ ������
	static std::auto_ptr<ServerThreadState> GetStartState(ServerImpl* p);

private:
	//! �-� ��� �������� - ������������� ���������, Template ������
	//! \return NULL - ��������� �����������, ����� ����� ���������
	virtual std::auto_ptr<ServerThreadState>	PostProcess() = 0;

	virtual void	Process( MsgCameraResponseData*		p );
	virtual void	Process( MsgCameraSetHallData*		p );
	virtual void	Process( MsgHallRequestAlarmData*		p );
	virtual void	Process( MsgHallResponseAlarmData*		p );
	virtual void	Process( MsgHallSetCameraData*		p );
};

#endif // _SERVER_THREAD_STATE_6131551782488200_