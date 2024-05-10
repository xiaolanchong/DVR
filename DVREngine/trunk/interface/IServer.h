//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Серверный интерфейс DVREngine, через кот. реализуются задачи DVREngine

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_SERVER_9972059635508920_
#define _I_SERVER_9972059635508920_

#include <stdexcept>
#include "..\..\..\Shared\Common\ExceptionTemplate.h"
#include "..\..\..\Shared\Interfaces\i_message.h"

//======================================================================================//
//                                    class IServer                                     //
//======================================================================================//

//! содержит весь интерфейс DVREngine
namespace DVREngine
{
//! \brief Серверный интерфейс DVREngine, через кот. реализуются задачи DVREngine
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.2
//! \bug 
//! \todo 
class IServer
{
public:
	virtual ~IServer() {};

	//! базовое исключение для серверной части
	MACRO_EXCEPTION(ServerException, std::runtime_error)	  ;
	MACRO_EXCEPTION(ServerAlreadyLaunchedException, ServerException)	  ;
};

struct ServerStartSettings
{
	//! запустить алгоритм?
	bool								m_bLaunchAlgo;
	//! запустить бакап
	bool								m_bLaunchBackup;
	//! имя ключа защиты
	std::string							m_ProtectionKeyName;
	//! режим работы захвата
	unsigned int						m_VideoSystemMode;
	//! интерфейс отладки для самого сервера
	boost::shared_ptr<Elvees::IMessage> m_pServerMsg ;
	//! интерфейс отладки для видеоподсистемы (chcsva)
	boost::shared_ptr<Elvees::IMessage> m_pVideoMsg ;
	//! интерфейс отладки для удаления архива
	boost::shared_ptr<Elvees::IMessage> m_pBackupMsg;
};

//! создать интерфейс сервера, освободить через delete




//! \return указатель на реализацию, освобождается через delete
//! \exception ServerException, если инициализация не прошла

IServer*	CreateServerInterface(  const ServerStartSettings& ss );

bool	ServerLaunched();

}


#endif // _I_SERVER_9972059635508920_