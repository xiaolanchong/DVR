//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Базовый класс - основные операции для реализации интерфейса IStreamManager

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _BASE_MANAGER_3552607354717489_
#define _BASE_MANAGER_3552607354717489_

#include "StreamList.h"
//#include "../IVideoSystemConfig.h"

class ManagerWindow;
class IInputFramePin;
class SystemSettings;

//#define ARCHIVE_ONLY
#define VIDEO_MODE

//======================================================================================//
//                                  class BaseManager                                   //
//======================================================================================//

//! \brief Базовый класс - основные операции для реализации интерфейса IStreamManager
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class BaseManager : boost::noncopyable,
					Elvees::CThread
{
public:
	BaseManager( boost::shared_ptr<SystemSettings> pVideoConfig, bool bServerMode, DWORD WorkingMode );
	virtual ~BaseManager();

	// execution control
	bool Start();
	bool IsStarted();
	void WaitStarted();

	void ShutDown();

	// available streams
	CHCS::IStream* StreamByID(long lStreamID);
	CHCS::IStream* StreamByUID(UUID& StreamUID, long lPin = 0);

	bool GetStreams( std::vector<long>& StreamIDs );

private:
	// CThread overriding
	virtual int Run();

	boost::shared_ptr<SystemSettings>		m_pVideoConfig;
protected:

	TCHAR m_stBasePath[MAX_PATH];	// Archive base path

	StreamList	m_StreamList;
	std::vector< boost::shared_ptr<IInputFramePin> > m_IntermediateFilters;

	bool IsStreamAlreadyAttached(long StreamID);
	LPCTSTR GetVideoBasePath();
	// Mode settings

	template<DWORD mask> bool CheckMask() const
	{
		return (m_WorkingMode & mask) != 0;
	}

	bool	UseGUIForDebugPurpose() const
	{
		return CheckMask< CHCS_MODE_GUI >();
	}

	bool	UseCommonProcess() const
	{
		return CheckMask < CHCS_MODE_COMMON_PROCESS > ();
	}

	SystemSettings&	GetSystemSettings();
private:
	const DWORD	m_WorkingMode;

	Elvees::CManualResetEvent m_startedEvent;
	Elvees::CManualResetEvent m_shutdownEvent;


	// FIXME to archive server
	Elvees::CLibrary xvidcore;
	Elvees::CLibrary xvidvfw;

private:
	bool	m_bServerMode;
	bool	m_bUseGUIForDebugPurpose;

	bool	IsServer() const 
	{
		return m_bServerMode;
	}

	void	UpdateStreamListInWindow( ManagerWindow* pWindow);

	virtual void InitializeWork() = 0;
	virtual void UninitializeWork() = 0;
};

std::tstring UIDtoString( const UUID& uid );

#endif // _BASE_MANAGER_3552607354717489_