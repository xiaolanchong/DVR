//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Базовый обработчик событий - игнорирование всех сообщений

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _BASE_MESSAGE_PROCESSOR_4128440386800518_
#define _BASE_MESSAGE_PROCESSOR_4128440386800518_

#include "message\IMessageProcessor.h"

//======================================================================================//
//                              class BaseMessageProcessor                              //
//======================================================================================//

//! \brief Базовый обработчик событий - игнорирование всех сообщений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class BaseMessageProcessor : public IMessageProcessor
{
	MessageImpl							m_BaseMessageProcessorMsg;
	std::string							m_sSystemDesc;
public:
	BaseMessageProcessor(boost::shared_ptr<Elvees::IMessage> pMsg, const std::string& sSystemDesc):
		m_BaseMessageProcessorMsg(pMsg),
		m_sSystemDesc(sSystemDesc)
	{

	}

	//virtual ~BaseMessageProcessor();

	virtual void	Process( MsgCameraResponseData*		/*p */)
	{
		//! такого сообщения быть не должно
		_ASSERTE(false);

		m_BaseMessageProcessorMsg.Print( Elvees::IMessage::mt_error, boost::format("[%s]MsgCameraResponseData-inadmissible message") % m_sSystemDesc  );
	}

	virtual void	Process( MsgCameraSetHallData*		/*p */ )
	{
		//! такого сообщения быть не должно
		_ASSERTE(false);

		m_BaseMessageProcessorMsg.Print( Elvees::IMessage::mt_error, boost::format("[%s]MsgCameraSetHallData-inadmissible message") % m_sSystemDesc   );
	}

	virtual void	Process( MsgHallSetCameraData*		/*p */ )
	{
		//! такого сообщения быть не должно
		_ASSERTE(false);

		m_BaseMessageProcessorMsg.Print( Elvees::IMessage::mt_error, boost::format("[%s]MsgHallSetCameraData-inadmissible message") % m_sSystemDesc   );
	}

	virtual void	Process( MsgHallRequestAlarmData*		/*p */ )
	{
		//! такого сообщения быть не должно
		_ASSERTE(false);

		m_BaseMessageProcessorMsg.Print( Elvees::IMessage::mt_error, boost::format("[%s]MsgHallRequestAlarmData-inadmissible message") % m_sSystemDesc   );
	}

	virtual void	Process( MsgHallResponseAlarmData*		/*p */ )
	{
		//! такого сообщения быть не должно
		_ASSERTE(false);

		m_BaseMessageProcessorMsg.Print( Elvees::IMessage::mt_error, boost::format("[%s]MsgHallResponseAlarmData-inadmissible message") % m_sSystemDesc   );
	}
};

#endif // _BASE_MESSAGE_PROCESSOR_4128440386800518_