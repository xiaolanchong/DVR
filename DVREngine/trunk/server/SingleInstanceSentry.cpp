//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: √арантирует единственность экземпл€ра сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.10.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "SingleInstanceSentry.h"
#include "../interface/IServer.h"

LPCTSTR const c_EventName = _T("Global\\{52E6876B-5AC8-4C54-B83D-7E0C225AAB17}");

bool	DVREngine::ServerLaunched()
{
	CHandle h ( CreateEvent( NULL, FALSE, FALSE, c_EventName ) );
	return ( GetLastError() == ERROR_ALREADY_EXISTS );
}

//======================================================================================//
//                              class SingleInstanceSentry                              //
//======================================================================================//

SingleInstanceSentry::SingleInstanceSentry():
	m_bAlreadyLaunched(false)
{
	HANDLE h = CreateEvent( NULL, FALSE, FALSE, c_EventName );
	m_ServerEvent.Attach( h );
	m_bAlreadyLaunched = ( GetLastError() == ERROR_ALREADY_EXISTS );
	if( m_bAlreadyLaunched )
	{
		throw DVREngine::IServer::ServerAlreadyLaunchedException("Server application has already launched");
	}
}

SingleInstanceSentry::~SingleInstanceSentry()
{
}