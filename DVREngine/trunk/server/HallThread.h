//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Hall analyzer thread actions

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _HALL_THREAD_9494093906572220_
#define _HALL_THREAD_9494093906572220_

#include "BaseQueueThread.h"
#include "message\IMessageStream.h"
#include "message\BaseMessageProcessor.h"
#include "AlarmInfo.h"
#include "CameraData.h"

//======================================================================================//
//                                   class HallThread                                   //
//======================================================================================//

//! \brief Запускает алгоритм на уровне зала
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 2.0
//! \bug 
//! \todo 

class HallThread :	public	IMessageStream,
					public	BaseMessageProcessor,
					private BaseQueueThread
{
	//! указатель на реализацию алгоримов зала 
	std::auto_ptr<Elvees::IHallAnalyzer>	m_pAnalyzer;

	std::vector<int>						m_CameraIDArr;
private:
	bool DoAction();

	//! обработать очередь сообщений BaseQueueThread
	void ProcessQueue();

	virtual void	Process( MsgHallSetCameraData*		p );
	virtual void	Process( MsgHallRequestAlarmData*	p );

	//! вернуть из IHallAnalyzer рамки объектов
	//! \param frm массив рамок
	void	GetFrames( Frames_t& framesForDatabase,  Frames_t& framesForRender );
	//! вернуть из IHallAnalyzer данные для камер
	//! 
	void	GetCameraData( const std::vector<int>& CamArr, CameraData& cd  );

	bool	Handled_ThreadProc();
public:
	HallThread(	boost::shared_ptr<Elvees::IMessage> pMsg, 
				IMessageDispatcher* pDsp);
	virtual ~HallThread();

	virtual void OnMessage( boost::shared_ptr<IThreadMessage> pMsg ) ;

	void		 StartThread( const std::vector<int>& CameraIDArr, bool bLaunchAlgo ) ;
};

#endif // _HALL_THREAD_9494093906572220_