//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс приемщика сообщений

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_MESSAGE_STREAM_6248875289584826_
#define _I_MESSAGE_STREAM_6248875289584826_

class IThreadMessage;
class IMessageDispatcher;

//======================================================================================//
//                                 class IMessageStream                                 //
//======================================================================================//

//! \brief Интерфейс приемника сообщений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMessageStream
{
protected:
	//! указатель на отправителя
	IMessageDispatcher*		m_pDispatcher;
public:
	//! \param указатель на отправителя
	IMessageStream( IMessageDispatcher* pDispatcher ) : m_pDispatcher(pDispatcher) {};
	virtual ~IMessageStream() {};

	//! приянть сообщение
	//! \param pMsg сообщение
	virtual void OnMessage( boost::shared_ptr<IThreadMessage> pMsg ) = 0;
};

#endif // _I_MESSAGE_STREAM_6248875289584826_