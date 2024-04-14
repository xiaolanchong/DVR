//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс взаимодействия с сервером, запуск сервера, поставка событий от камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   28.02.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_HANDLER_4684911399675724_
#define _SERVER_HANDLER_4684911399675724_

namespace DVREngine
{
	class IServer;
	class IClient;
}

//======================================================================================//
//                                 class ServerHandler                                  //
//======================================================================================//

//! \brief Класс взаимодействия с сервером, запуск сервера, поставка событий от камер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerHandler
{
	//! серверная часть DVREngine, может быть и в отдельном приложении
	std::auto_ptr<DVREngine::IServer>	m_pServer;
	//! клиентская часть DVREngine
	std::auto_ptr<DVREngine::IClient>	m_pClient;
public:
	MACRO_EXCEPTION(ServerHandlerException, std::runtime_error);

	ServerHandler(	
		const DVREngine::ServerStartSettings& ss, 
		boost::shared_ptr<Elvees::IMessage> pClientMsg,
		bool bStartServer);
	virtual ~ServerHandler();

	//! прочитать из клиентской части инф-ю о ситуациях
	//! \param Frames инф-я о тревожных ситуациях
	void	GetCurrentFrames( Frames_t& Frames) const		;
	//! получить состояния компьютеров (телеметрия) 
	//! \param	sName	IP-адрес компьютера N.N.N.N
	//! \return состояние 
	int		GetComputerState(const std::string& sName) const	;
	//! получить состояния камеры (телеметрия) 
	//! \param	nID	идентификатор камеры
	//! \return состояние камеры
	int		GetCameraState(int nID) const						;

	DVREngine::IEngineSettingsReader*	GetParamReader()
	{
		return m_pClient.get();
	}
	DVREngine::IEngineSettingsWriter*	GetParamWriter()
	{
		return m_pClient.get();
	}
};

#endif // _SERVER_HANDLER_4684911399675724_