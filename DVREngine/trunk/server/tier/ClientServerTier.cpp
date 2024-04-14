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

#include "stdafx.h"
#include "ClientServerTier.h"

//======================================================================================//
//                                class ClientServerTier                                //
//======================================================================================//

class ClientServerTier : public IClientServerTier
{
	//! синхронизация доступа к данным
	boost::mutex		m_Mutex;
	//! рамки обхектов
	Frames_t			m_Frames;
public:
	ClientServerTier();
	virtual ~ClientServerTier() ;
private:
	virtual void	SetCurrentFrames( const Frames_t& ca );
	virtual void	GetCurrentFrames( Frames_t& ca )	;
};


ClientServerTier::ClientServerTier()
{
}

ClientServerTier::~ClientServerTier()
{
}

void	ClientServerTier::SetCurrentFrames( const Frames_t& ca )	
{
	boost::mutex::scoped_lock l(m_Mutex);
	m_Frames = ca;
}

void	ClientServerTier::GetCurrentFrames( Frames_t& ca )
{
	//! получить и очистить
	boost::mutex::scoped_lock l(m_Mutex);
	ca = m_Frames;
	m_Frames.clear();
}

//////////////////////////////////////////////////////////////////////////

IClientServerTier*	GetClientServerTier()
{
	//! сам статический объект
	static ClientServerTier imp;
	return &imp;
}