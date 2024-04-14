//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Обработчик сообщений от окна рендере

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "RenderEventHandler.h"
#include "../res/Resource.h"

//======================================================================================//
//                               class RenderEventHandler                               //
//======================================================================================//

RenderEventHandler::RenderEventHandler(wxWindow* pRenderWnd):
	m_pRenderWnd(pRenderWnd)
{
	m_pRenderWnd->PushEventHandler( this );
}

RenderEventHandler::~RenderEventHandler()
{
	m_pRenderWnd->PopEventHandler( false );
}


BEGIN_EVENT_TABLE(RenderEventHandler, wxEvtHandler)
	EVT_SIZE(RenderEventHandler::OnSize)
	EVT_RIGHT_UP(RenderEventHandler::OnRButton)
	EVT_MENU_RANGE( ID_Menu_Camera_1, ID_Menu_Camera_1 + 99, RenderEventHandler::OnCameraMenu  )
END_EVENT_TABLE()


void RenderEventHandler::OnSize( wxSizeEvent& ev )
{
	if( m_ActiveHandler.get_ptr() )
	{
		_ASSERTE( m_ActiveHandler.get() < m_Handlers.size() );
		m_Handlers[ m_ActiveHandler.get() ]->OnSizeHandler( ev );
	}
}

void RenderEventHandler::OnRButton( wxMouseEvent& ev )
{
	if( m_ActiveHandler.get_ptr() )
	{
		_ASSERTE( m_ActiveHandler.get() < m_Handlers.size() );
		m_Handlers[ m_ActiveHandler.get() ]->OnRButtonHandler( ev );
	}
}

void RenderEventHandler::OnCameraMenu( wxCommandEvent& ev )
{
	if( m_ActiveHandler.get_ptr() )
	{
		_ASSERTE( m_ActiveHandler.get() < m_Handlers.size() );
		m_Handlers[ m_ActiveHandler.get() ]->OnMenuHandler( ev );
	}
}

void	RenderEventHandler::SetActive(size_t nActiveHandler)
{
	_ASSERTE( nActiveHandler < m_Handlers.size()  );
	m_ActiveHandler = nActiveHandler;
}

void	RenderEventHandler::AddHander(RenderEventCallback* pHanlder)
{
	m_Handlers.push_back( pHanlder );
}