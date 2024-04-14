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
#ifndef _CLIENT_IMPL_8761221967163416_
#define _CLIENT_IMPL_8761221967163416_

#include "../interface/IClient.h"
#include "../server/MessageImpl.h"

//======================================================================================//
//                                   class ClientImpl                                   //
//======================================================================================//

//! \brief Реализация интерфейса клиента
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ClientImpl : public DVREngine::IClient
{
	MessageImpl							m_Msg;
public:
	ClientImpl(boost::shared_ptr<Elvees::IMessage> pMsg);
	virtual ~ClientImpl();

	virtual void	GetCurrentFrames(Frames_t& CurrentFrames) 		;
/*	virtual int		GetComputerState(const std::string& sName) const	;
	virtual int		GetCameraState(int nID) const						;*/

	virtual void					GetArchiveStorageParameters( DVREngine::ArchiveStorageParam& NewParam );
	virtual unsigned int				GetAlgorithmSensitivity(  ) ;
	virtual DVREngine::ArchiveWriteCondition	GetArchiveWriteCondition( ) ;
	virtual void	SetArchiveStorageParameters( const DVREngine::ArchiveStorageParam& NewParam ) ;
	virtual void	SetAlgorithmSensitivity( unsigned int Sensitivity ) ;
	virtual void	SetArchiveWriteCondition( DVREngine::ArchiveWriteCondition NewCondition ) ;
};

#endif // _CLIENT_IMPL_8761221967163416_