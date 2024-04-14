//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса клиента

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   24.01.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ClientImpl.h"
#include "../server/tier/ClientServerTier.h"
#include "../common/EngineSettingsImpl.h"

namespace DVREngine
{

	IClient* CreateClientInterface(boost::shared_ptr<Elvees::IMessage> pMsg)
	{
		return new ClientImpl( pMsg );
	}

}

//======================================================================================//
//                                   class ClientImpl                                   //
//======================================================================================//

ClientImpl::ClientImpl(boost::shared_ptr<Elvees::IMessage> pMsg):
	m_Msg(pMsg)
{
}

ClientImpl::~ClientImpl()
{
}

void	ClientImpl::GetCurrentFrames(Frames_t& CurrentFrames) 
{
	GetClientServerTier()->GetCurrentFrames( CurrentFrames );
}
/*
int		ClientImpl::GetComputerState(const std::string& sName) const
{
	UNREFERENCED_PARAMETER(sName);
	return 0;
}

int		ClientImpl::GetCameraState(int nID) const
{
	UNREFERENCED_PARAMETER(nID);
	return 0;
}*/

void					ClientImpl::GetArchiveStorageParameters( DVREngine::ArchiveStorageParam& NewParam )
{
	EngineSettingsImpl impl(false);
	impl.GetArchiveStorageParameters( NewParam );
}

unsigned int				ClientImpl::GetAlgorithmSensitivity(  ) 
{
	EngineSettingsImpl impl(false);
	return impl.GetAlgorithmSensitivity();
}

DVREngine::ArchiveWriteCondition	ClientImpl::GetArchiveWriteCondition( ) 
{
	EngineSettingsImpl impl(false);
	return impl.GetArchiveWriteCondition();
}

void	ClientImpl::SetArchiveStorageParameters( const DVREngine::ArchiveStorageParam& NewParam )
{
	EngineSettingsImpl impl(false);
	impl.SetArchiveStorageParameters(NewParam);
}

void	ClientImpl::SetAlgorithmSensitivity( unsigned int Sensitivity )
{
	EngineSettingsImpl impl(false);
	impl.SetAlgorithmSensitivity( Sensitivity );
}

void	ClientImpl::SetArchiveWriteCondition( DVREngine::ArchiveWriteCondition NewCondition ) 
{
	EngineSettingsImpl impl(false);
	impl.SetArchiveWriteCondition( NewCondition );
}