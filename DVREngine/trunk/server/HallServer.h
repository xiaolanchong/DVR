//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Hall thread & queue server

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _HALL_SERVER_2481648139811209_
#define _HALL_SERVER_2481648139811209_

#include "HallThread.h"
#include "message/IMessageDispatcher.h"
#include "message/IMessageStream.h"

//======================================================================================//
//                                   class HallServer                                   //
//======================================================================================//

//! \brief Hall thread & queue server, соединяет сервер с потоком зала
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class HallServer	:	public	IMessageStream,
						public	IMessageDispatcher
{
	//! отладочный поток
	MessageImpl		m_Msg; // hall about 
	//! поток зала
	HallThread		m_Thread;

	virtual	 void	OnMessage( boost::shared_ptr<IThreadMessage> pMsg );

public:
	//! \param pMsg отладочный интерфейс
	//! \param pDsp интерфейс приемника сообщений от m_Thread
	HallServer( boost::shared_ptr<Elvees::IMessage> pMsg, 
				IMessageDispatcher* pDsp,
				bool bLaunchAlgo);
	virtual ~HallServer();

	virtual	 void	SendMessage( IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg );

	void		 StartThread( const std::vector<int>& CameraIDArr ) 
	{ 
	//	m_Thread.StartThread( CameraIDArr, bool bLaunchAlgo ); 
	}
};

#endif // _HALL_SERVER_2481648139811209_