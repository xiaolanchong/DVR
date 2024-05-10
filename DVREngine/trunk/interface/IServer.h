//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ��������� DVREngine, ����� ���. ����������� ������ DVREngine

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

//! �������� ���� ��������� DVREngine
namespace DVREngine
{
//! \brief ��������� ��������� DVREngine, ����� ���. ����������� ������ DVREngine
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.2
//! \bug 
//! \todo 
class IServer
{
public:
	virtual ~IServer() {};

	//! ������� ���������� ��� ��������� �����
	MACRO_EXCEPTION(ServerException, std::runtime_error)	  ;
	MACRO_EXCEPTION(ServerAlreadyLaunchedException, ServerException)	  ;
};

struct ServerStartSettings
{
	//! ��������� ��������?
	bool								m_bLaunchAlgo;
	//! ��������� �����
	bool								m_bLaunchBackup;
	//! ��� ����� ������
	std::string							m_ProtectionKeyName;
	//! ����� ������ �������
	unsigned int						m_VideoSystemMode;
	//! ��������� ������� ��� ������ �������
	boost::shared_ptr<Elvees::IMessage> m_pServerMsg ;
	//! ��������� ������� ��� ��������������� (chcsva)
	boost::shared_ptr<Elvees::IMessage> m_pVideoMsg ;
	//! ��������� ������� ��� �������� ������
	boost::shared_ptr<Elvees::IMessage> m_pBackupMsg;
};

//! ������� ��������� �������, ���������� ����� delete




//! \return ��������� �� ����������, ������������� ����� delete
//! \exception ServerException, ���� ������������� �� ������

IServer*	CreateServerInterface(  const ServerStartSettings& ss );

bool	ServerLaunched();

}


#endif // _I_SERVER_9972059635508920_