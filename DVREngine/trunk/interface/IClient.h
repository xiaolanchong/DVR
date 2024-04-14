//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ��������� DVREngine, �������� �������� ��� �������

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
//! \brief ���������� ��������� DVREngine, �������� �������� ��� ������� DVRClient
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

	//! �������� ������� �����
	//! \param CurrentFrames ������ ����� �� ���� ������� �� ������� ������ (����� ������ ������� ������� ������� �� ��������)
	virtual void	GetCurrentFrames(Frames_t& CurrentFrames)			= 0;
/*	//! �������� ��������� ����������
	//! \param sName IP ����� ����������
	//! \return ���� ������������, �����-�� ��������� ���������
	virtual int		GetComputerState(const std::string& sName) const	= 0;
	//! �������� ��������� ������
	//! \param nID ������������� ������
	//! \return ���� ������������, �����-�� ��������� ���������
	virtual int		GetCameraState(int nID) const						= 0;*/


};

//! ������� ���������� � ����������� IClient
//! \param  pMsg ���������� �����
//! \return ��������� �� ����������, ������������� ����� delete
//! \exception ServerException, ���� �������� ���������� �����������
IClient*	CreateClientInterface( boost::shared_ptr<Elvees::IMessage> pMsg );

}

#endif // _I_CLIENT_9552629000407592_