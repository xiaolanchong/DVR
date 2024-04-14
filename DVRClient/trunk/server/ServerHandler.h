//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����� �������������� � ��������, ������ �������, �������� ������� �� �����

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

//! \brief ����� �������������� � ��������, ������ �������, �������� ������� �� �����
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerHandler
{
	//! ��������� ����� DVREngine, ����� ���� � � ��������� ����������
	std::auto_ptr<DVREngine::IServer>	m_pServer;
	//! ���������� ����� DVREngine
	std::auto_ptr<DVREngine::IClient>	m_pClient;
public:
	MACRO_EXCEPTION(ServerHandlerException, std::runtime_error);

	ServerHandler(	
		const DVREngine::ServerStartSettings& ss, 
		boost::shared_ptr<Elvees::IMessage> pClientMsg,
		bool bStartServer);
	virtual ~ServerHandler();

	//! ��������� �� ���������� ����� ���-� � ���������
	//! \param Frames ���-� � ��������� ���������
	void	GetCurrentFrames( Frames_t& Frames) const		;
	//! �������� ��������� ����������� (����������) 
	//! \param	sName	IP-����� ���������� N.N.N.N
	//! \return ��������� 
	int		GetComputerState(const std::string& sName) const	;
	//! �������� ��������� ������ (����������) 
	//! \param	nID	������������� ������
	//! \return ��������� ������
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