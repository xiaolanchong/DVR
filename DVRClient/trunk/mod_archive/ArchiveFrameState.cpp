//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архивного окна - проигрывание, пауза, остановка

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   27.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ArchiveFrameState.h"
#include "ArchiveFrame.h"

//======================================================================================//
//                               class ArchiveFrameState                                //
//======================================================================================//

ArchiveFrameState::ArchiveFrameState(wxToolBar* pVideoControlBar ) : 
	m_pVideoControlBar(pVideoControlBar)
{
}

ArchiveFrameState::~ArchiveFrameState()
{
}

void	ArchiveFrameState::ChangeControlState( const char* szName, ControlState cs  )
{
	switch( cs ) 
	{
	case CSDisable:
		m_pVideoControlBar->EnableTool( XRCID(szName),	false );
		m_pVideoControlBar->ToggleTool( XRCID(szName),	false );
		break;
	case CSUnchecked:
		m_pVideoControlBar->EnableTool( XRCID(szName),	true );
		m_pVideoControlBar->ToggleTool( XRCID(szName),	false );
		break;
	case CSChecked:
		m_pVideoControlBar->EnableTool( XRCID(szName),	true );
		m_pVideoControlBar->ToggleTool( XRCID(szName),	true );
		break;
	default:_ASSERTE(false);
	}
}

void	ArchiveFrameState::ChangeControlState( ControlState csPlay, ControlState csPause, ControlState csStop )
{
	ChangeControlState( "ToolButton_VideoControl_Play",		csPlay );
	ChangeControlState( "ToolButton_VideoControl_Pause",	csPause );
	ChangeControlState( "ToolButton_VideoControl_Stop",		csStop );
}


//======================================================================================//
//                               class ArchiveFrameIdle                                 //
//======================================================================================//

ArchiveFrameIdle::ArchiveFrameIdle(wxToolBar* pVideoControlBar ) : 
	ArchiveFrameState(pVideoControlBar)
{
	ChangeControlState( CSDisable, CSDisable, CSDisable );
}

//======================================================================================//
//                               class ArchiveFramePlay                                 //
//======================================================================================//

ArchiveFramePlay::ArchiveFramePlay(wxToolBar* pVideoControlBar) : 
	ArchiveFrameState(pVideoControlBar)
{
	ChangeControlState( CSChecked ,	CSUnchecked , CSUnchecked );
}

//======================================================================================//
//                               class ArchiveFramePause                                //
//======================================================================================//

ArchiveFramePause::ArchiveFramePause(wxToolBar* pVideoControlBar) : 
	ArchiveFrameState(pVideoControlBar)
{
	ChangeControlState( CSUnchecked , CSChecked , CSUnchecked );
}

//======================================================================================//
//                               class ArchiveFrameStop                                 //
//======================================================================================//

ArchiveFrameStop::ArchiveFrameStop(wxToolBar* pVideoControlBar) : 
	ArchiveFrameState(pVideoControlBar)
{
	ChangeControlState( CSUnchecked , CSDisable , CSChecked );
}