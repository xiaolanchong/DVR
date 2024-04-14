//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Управлением окном отображения списка потоков

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MANAGER_WINDOW_4783080141099620_
#define _MANAGER_WINDOW_4783080141099620_

#define WM_TRAYICON				WM_USER+1
#define WM_UPDATE_STREAM_LIST	WM_USER+2

struct StreamInfo
{
	long			m_StreamID;
	UUID			m_StreamUID;
	std::tstring	m_stMoniker;
	bool			m_bIsLocal;
	long			m_DevicePin;
	CHCS::IStream*	m_pStream;			
};

//======================================================================================//
//                                 class ManagerWindow                                  //
//======================================================================================//

//! \brief Управлением окном отображения списка потоков
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ManagerWindow
{
	bool						m_bServerMode;
	Elvees::CCriticalSection	m_listSection;
	std::vector< StreamInfo >	m_StreamArr;
	HWND						m_hServerWnd;

	bool	IsServerWindow() const
	{
		return m_bServerMode;
	}

	CHCS::IStream*	StreamByUID(const UUID& streamUID, long lPin) const;

	static BOOL CALLBACK ManagerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	ManagerWindow( bool bServerMode );
	virtual ~ManagerWindow();

	void SetStreams( const std::vector< StreamInfo >& StreamArr );

	bool IsDialogMessage(MSG* pmsg) const
	{
		return ::IsDialogMessage( m_hServerWnd, pmsg) != FALSE;
	}

	void	Destroy();
};

#endif // _MANAGER_WINDOW_4783080141099620_