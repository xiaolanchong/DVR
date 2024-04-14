//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ������� - ���� ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_THREAD_GATHERING_8060347484245166_
#define _SERVER_THREAD_GATHERING_8060347484245166_

#include "ServerThreadState.h"
#include "../../Common/Timer.h"
#include "../MilliSecTimer.h"
#include "../CameraData.h"

//======================================================================================//
//                             class ServerThreadGathering                              //
//======================================================================================//

//! \brief ��������� ������� - ����, �������� ������ � ���������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerThreadGathering : public ServerThreadState
{
	//! �������� ��������� , �� ������������
//	bool				m_bChangeState;
	//! ������ ��� �������� ����� ������
	MilliSecTimeoutTimer				m_Timer;
	//! ����� ������ ������ ��� ��������
//	CameraData			m_CameraData;

	virtual std::auto_ptr<ServerThreadState> PostProcess();
public:
	ServerThreadGathering(ServerImpl* p);
	virtual ~ServerThreadGathering();
private:
	//! ���������������� ��������� - ��������� ������ �� �����
	void Process( MsgCameraResponseData*		/*p*/ );

	//! ���������� ������� ������ �� ���� ������ m_CameraData
	//! \return true - ������ �� ���� ������� ��������
	bool	IsAllData() const;
};

#endif // _SERVER_THREAD_GATHERING_8060347484245166_