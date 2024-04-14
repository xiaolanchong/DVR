//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс для обработки сообщений между потоками

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

//! \brief Интерфейс для обработки сообщений между потоками
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMessageProcessor
{
public:
	virtual ~IMessageProcessor() {};

	//! обработать MsgCameraResponseData сообщение - данные от одной камеры к залу
	//! \param p указатель на сообщение
	virtual void	Process( MsgCameraResponseData*		p ) = 0;
	//! обработать MsgCameraSetHallData сообщение - данные для всех камер от зала
	//! \param p указатель на сообщение
	virtual void	Process( MsgCameraSetHallData*		p ) = 0;

	//! обработать MsgHallSetCameraData сообщение - данные от камеры залу
	//! \param p указатель на сообщение
	virtual void	Process( MsgHallSetCameraData*		p ) = 0;
	//! обработать MsgHallRequestAlarmData сообщение - запрос на рамки
	//! \param p указатель на сообщение
	virtual void	Process( MsgHallRequestAlarmData*		p ) = 0;
	//! обработать MsgHallResponseAlarmData сообщение - данные для камер + рамки объектов
	//! \param p указатель на сообщение
	virtual void	Process( MsgHallResponseAlarmData*		p ) = 0;
};

#endif // _I_MESSAGE_PROCESSOR_6934025625416804_