//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс диспетчера передачи сообщений

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_MESSAGE_DISPATCHER_3054817400887827_
#define _I_MESSAGE_DISPATCHER_3054817400887827_

class IMessageStream;
class IThreadMessage;

//======================================================================================//
//                               class IMessageDispatcher                               //
//======================================================================================//

//! \brief Интерфейс диспетчера передачи сообщений, посылает сообщения
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMessageDispatcher
{
public:
//	IMessageDispatcher();
	virtual ~IMessageDispatcher() {};

	virtual	 void	SendMessage( IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg ) = 0;
};

#endif // _I_MESSAGE_DISPATCHER_3054817400887827_