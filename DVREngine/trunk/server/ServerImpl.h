//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ��������� �����

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_IMPL_6610914204503918_
#define _SERVER_IMPL_6610914204503918_

#include "../interface/IServer.h"
#include "BaseQueueThread.h"

#include "HallThread.h"
#include "CameraServer.h"

#include "CameraData.h"
#include "message/IMessageDispatcher.h"
#include "BackupImpl.h"

#include "AlarmInfo.h"
#include "SingleInstanceSentry.h"

class ServerThreadState;
class ActivityRecord;

//======================================================================================//
//                                   class ServerImpl                                   //
//======================================================================================//

//! \brief ���������� ��������� ����� - �������������� ������� ������-���
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 2.0
//! \bug 
//! \todo 

class ServerImpl :	public	DVREngine::IServer, 
					private	IMessageDispatcher,
					private	BaseQueueThread,
					boost::noncopyable
{
	//! ����������� ���������� ��������, ���������������� ������
	SingleInstanceSentry				m_ServerSentry;
	//! ������ �����
	std::auto_ptr<CameraServer>			m_CameraServer;
	//! ������ ����
	std::auto_ptr<HallThread>			m_HallServer;
	//! ��������� �������
	std::auto_ptr<ServerThreadState>		m_pState;
	//! ������ ��������������� ��������� �����
	std::vector<int>						m_CameraIDArr;

	//! ������ ������ ���������� �� ������
	std::auto_ptr<ActivityRecord>			m_FrameActivityStatistics;

	//! ������ �������� ������
	std::auto_ptr<BackupImpl>			m_pBackupWrapper;
	
	DVREngine::ServerStartSettings					m_ServerSettings;
public:
	//! ������ ������
	//! \param bLaunchAlgo	��������� ��������?
	//! \param pServerMsg	��������� ���� ��� �������
	//! \param pVideoMsg	��������� ���� ��� ���������������
	//! \param pBackupMsg	��������� ���� ��� ������� ������
	ServerImpl(	const DVREngine::ServerStartSettings& ss);
	virtual ~ServerImpl();

private:
	bool Handled_ServerThreadProc();
	//! ��������� ���������
	//! \return true-����������� ������, false-���
	bool			ThreadProc();
	//! ������� ������ ������� �� ����
	//! \param CamData ������ ��� ����� (�������� �������)
	void			SendHallData(const CameraData& CamData, const Frames_t& AlarmFrames);
	//! ���������� ����� ��� �������
	//! \param Alarm ������ �����
	void			SetFrames(const Frames_t& Alarms);

	//! �������� � �� ������ �����
	void			WriteToDatabase(const Frames_t& Alarms);
public:
	//! ������� ������ ����� � ���
	//! \param CamData ������ ��� ���� (�������� �������)
	void			SendCameraDataToHall( int nCameraID, const ByteArray_t& CamData );

	//! ��������� ������ �� ����
	void			SendRequestToHall();
	//! �������� ������ �� ����
	//! \param CamData ������ �� ���� ��� ���� �����
	//! \param Alarms ������ ����� ������ �������� 
	void			ReceiveHallData( const CameraData& CamData, const Frames_t& FramesForRender );
	//! ������� ���������� ��������� ��� ��������� m_pState
	//! \param ���������� ���������
	MessageImpl&						GetDebugOuput() { return m_Msg;		};
	//! ������� ������ �� ������� ���������� ��� ��������� m_pState
	//! \return ������ �� m_Queue
	MessageSyncQueue<ThreadMessage_t>&	GetQueue()		{ return m_Queue;	};
	//! ������� ������ ��������������� ���� ��������� �����
	//! \return  m_CameraIDArr
	const std::vector<int>&				GetCameraIDArr() const { return m_CameraIDArr; }

	void	LaunchThreads();

	void	DumpError( const std::string& ErrorDesc ); 
private:
	//! ������� ��������� pMsg � pSender
	//! \param pSender ����������� (this)
	//! \param pMsg ���������
	virtual void	SendMessage(IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg);
};


#endif // _SERVER_IMPL_6610914204503918_