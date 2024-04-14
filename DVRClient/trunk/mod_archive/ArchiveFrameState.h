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
#ifndef _ARCHIVE_FRAME_STATE_2474041537735283_
#define _ARCHIVE_FRAME_STATE_2474041537735283_

class ArchiveFrame;

//======================================================================================//
//                               class ArchiveFrameState                                //
//======================================================================================//

//! \brief Состояние архивного окна - проигрывание, пауза, остановка
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   27.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveFrameState
{
protected: 
	wxToolBar*		m_pVideoControlBar;

	enum ControlState
	{
		CSDisable,
		CSChecked,
		CSUnchecked
	};
	void ChangeControlState( ControlState csPlay, ControlState csPause, ControlState csStop );
private:
	void	ChangeControlState( const char* szName, ControlState cs  );
public:
	ArchiveFrameState(wxToolBar* pVideoControlBar);

//	virtual bool IsInvalidTime( time_t CurrentArchiveTime ) = 0;
	virtual ~ArchiveFrameState();
};

class ArchiveFrameIdle : public ArchiveFrameState
{
public:
	ArchiveFrameIdle(wxToolBar* pVideoControlBar);
};

class ArchiveFramePlay : public ArchiveFrameState
{
public:
	ArchiveFramePlay(wxToolBar* pVideoControlBar);
};

class ArchiveFramePause : public ArchiveFrameState
{
public:
	ArchiveFramePause(wxToolBar* pVideoControlBar);
};

class ArchiveFrameStop : public ArchiveFrameState
{
public:
	ArchiveFrameStop(wxToolBar* pVideoControlBar);
};

#endif // _ARCHIVE_FRAME_STATE_2474041537735283_