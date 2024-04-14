//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Клиентский интерфейс DVREngine, основные действия для клиента

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_CLIENT_9552629000407592_
#define _I_CLIENT_9552629000407592_

#include "..\..\..\Shared\Common\ExceptionTemplate.h"
#include "..\server\AlarmInfo.h"
#include "..\..\..\Shared\Interfaces\i_message.h"
#include "EngineSettings.h"

//======================================================================================//
//                                    class IClient                                     //
//======================================================================================//

namespace DVREngine
{
//! \brief Клиентский интерфейс DVREngine, основные действия для клиента DVRClient
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.04.2006
//! \version 2.0
//! \bug 
//! \todo 
class IClient : public IEngineSettingsReader, 
				public IEngineSettingsWriter
{
public:
	virtual ~IClient() {};

	//! получить текущие рамки
	//! \param CurrentFrames массив рамок по всем камерам за текущий период (время начала отстоит порядка секунды от текущего)
	virtual void	GetCurrentFrames(Frames_t& CurrentFrames)			= 0;
/*	//! получить состояние компьютера
	//! \param sName IP адрес компьютера
	//! \return пока неопределено, какая-то константа состояния
	virtual int		GetComputerState(const std::string& sName) const	= 0;
	//! получить состояние камеры
	//! \param nID идентификатор камеры
	//! \return пока неопределено, какая-то константа состояния
	virtual int		GetCameraState(int nID) const						= 0;*/


};

//! создать реализацию с интерфейсом IClient
//! \param  pMsg отладочный поток
//! \return указатель на реализацию, освобождается через delete
//! \exception ServerException, если создание реализации провалилось
IClient*	CreateClientInterface( boost::shared_ptr<Elvees::IMessage> pMsg );

}

#endif // _I_CLIENT_9552629000407592_