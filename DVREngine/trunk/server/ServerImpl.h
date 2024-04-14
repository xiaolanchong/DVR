//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация серверной части

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

//! \brief реализация серверной части - взаимодействия уровней камера-зал
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
	//! обнаружение запущенных серверов, инициализируется первый
	SingleInstanceSentry				m_ServerSentry;
	//! сервер камер
	std::auto_ptr<CameraServer>			m_CameraServer;
	//! сервер зала
	std::auto_ptr<HallThread>			m_HallServer;
	//! состояние сервера
	std::auto_ptr<ServerThreadState>		m_pState;
	//! массив идентификаторов локальных камер
	std::vector<int>						m_CameraIDArr;

	//! сервер записи статистики по рамкам
	std::auto_ptr<ActivityRecord>			m_FrameActivityStatistics;

	//! сервер удаления архива
	std::auto_ptr<BackupImpl>			m_pBackupWrapper;
	
	DVREngine::ServerStartSettings					m_ServerSettings;
public:
	//! начать работу
	//! \param bLaunchAlgo	запустить алгоритм?
	//! \param pServerMsg	интерфейс лога для сервера
	//! \param pVideoMsg	интерфейс лога для видеоподсистемы
	//! \param pBackupMsg	интерфейс лога для очистки архива
	ServerImpl(	const DVREngine::ServerStartSettings& ss);
	virtual ~ServerImpl();

private:
	bool Handled_ServerThreadProc();
	//! потоковая процедура
	//! \return true-звавершение потока, false-нет
	bool			ThreadProc();
	//! послать данные камерам от зала
	//! \param CamData данные для камер (двоичные массивы)
	void			SendHallData(const CameraData& CamData, const Frames_t& AlarmFrames);
	//! установить рамки для клиента
	//! \param Alarm массив рамок
	void			SetFrames(const Frames_t& Alarms);

	//! записать в БД массив рамок
	void			WriteToDatabase(const Frames_t& Alarms);
public:
	//! послать данные камер в зал
	//! \param CamData данные для зала (двоичные массивы)
	void			SendCameraDataToHall( int nCameraID, const ByteArray_t& CamData );

	//! запросить данные от зала
	void			SendRequestToHall();
	//! получить данные от зала
	//! \param CamData данные от зала для всех камер
	//! \param Alarms массив рамок вокруг объектов 
	void			ReceiveHallData( const CameraData& CamData, const Frames_t& FramesForRender );
	//! вернуть отладочный интерфейс для состояния m_pState
	//! \param отладочный интерфейс
	MessageImpl&						GetDebugOuput() { return m_Msg;		};
	//! вернуть ссылку на очередь сообщенией для состояния m_pState
	//! \return ссылка на m_Queue
	MessageSyncQueue<ThreadMessage_t>&	GetQueue()		{ return m_Queue;	};
	//! вернуть массив идентификаторов всех локальных камер
	//! \return  m_CameraIDArr
	const std::vector<int>&				GetCameraIDArr() const { return m_CameraIDArr; }

	void	LaunchThreads();

	void	DumpError( const std::string& ErrorDesc ); 
private:
	//! послать сообщение pMsg в pSender
	//! \param pSender отправитель (this)
	//! \param pMsg сообщение
	virtual void	SendMessage(IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg);
};


#endif // _SERVER_IMPL_6610914204503918_