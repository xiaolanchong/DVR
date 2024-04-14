//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача инф-и от сервера к клиенту (только в simple-версии)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   24.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CLIENT_SERVER_TIER_3001154973708852_
#define _CLIENT_SERVER_TIER_3001154973708852_

#include "../AlarmInfo.h"

//======================================================================================//
//                                class IClientServerTier                                //
//======================================================================================//

//! \brief Передача инф-и от сервера к клиенту (только в simple-версии), вся информация передается через
//!			глобальный статический объект, работаем в одном процессе
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class IClientServerTier
{
public:
	virtual ~IClientServerTier() {};

	//! установить для клиента рамки
	virtual void	SetCurrentFrames( const Frames_t& ca )	= 0;
	//! получить рамки в клиенте
	virtual void	GetCurrentFrames( Frames_t& ca )		= 0;
};

//! получить интерфейс обмена сервер-клиент
//! \return указатель на реализацию
IClientServerTier*	GetClientServerTier();

#endif // _CLIENT_SERVER_TIER_3001154973708852_