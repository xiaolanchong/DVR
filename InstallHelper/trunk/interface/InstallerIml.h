//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _INSTALLER_IML_3974289482974550_
#define _INSTALLER_IML_3974289482974550_

#include "IInstaller.h"
#include "StreamWindow.h"

//! уникальный идент-р для камеры (пина устройства)
//! строковый идент-р устройства, куда подключена камера
//! идент-р камеры (1..)
//! номер пина (0-3)
typedef boost::tuple<UUID, CString, int, int>	CameraInfo_t;

class DynamicLibrary : boost::noncopyable
{
	HMODULE m_hDLLInstance;
protected:
	DynamicLibrary(LPCTSTR szDllFileName):
		 m_hDLLInstance( LoadLibrary(szDllFileName )  )
	{
			if ( !m_hDLLInstance )
			{
				throw std::runtime_error( "Failed to load library" );
			}
	}

		 ~DynamicLibrary()
		 {
			 if( m_hDLLInstance )
			 {
				 try
			  {
				  FreeLibrary(m_hDLLInstance);
			  }
				 catch(...)
			  {
			  }
			 }
		 }

	 HMODULE	GetHandle() const
	 {
		 return m_hDLLInstance;
	 }

	 template<typename ProcType>	void GetProcAddress( LPCSTR szProcName, ProcType& ProcPtr)
	 {
		 ProcPtr = reinterpret_cast<ProcType>( m_hDLLInstance, ::GetProcAddress( GetHandle(), szProcName ) );
		 if( !ProcPtr )
		 {
			 throw std::runtime_error( "Failed to get the procedure address" );
		 }
	 }
};

typedef CHCS::IVideoSystemConfigEx* (WINAPI* CreateVideoSystemConfig_t)(LPCWSTR);

class VideoLibary : DynamicLibrary
{
	typedef CHCS::IStreamManager* (WINAPI *  InitStreamManagerEx_t )(CHCS::INITCHCS_EX*);
	typedef bool				  (WINAPI *  RegisterDevices_t )(CHCS::IDeviceRegistrar*);
	

	InitStreamManagerEx_t		m_fnInitStreamManagerEx;
	RegisterDevices_t			m_fnRegisterDevices;
	CreateVideoSystemConfig_t	m_fnCreateVideoSystemConfig;
public:
	VideoLibary(): DynamicLibrary(_T("chcsva.dll")) 
	  {
		GetProcAddress( "InitStreamManagerEx",		m_fnInitStreamManagerEx		);
		GetProcAddress( "RegisterDevices",			m_fnRegisterDevices			);
		GetProcAddress( "CreateVideoSystemConfig",	m_fnCreateVideoSystemConfig );
	  }

	  CHCS::IStreamManager* InitStreamManagerEx(CHCS::INITCHCS_EX* in) const
	  {
		  return m_fnInitStreamManagerEx( in );
	  }

	  bool					RegisterDevices( CHCS::IDeviceRegistrar* p ) const
	  {
		  return m_fnRegisterDevices( p );
	  }

	  CHCS::IVideoSystemConfigEx*	CreateVideoSystemConfig( LPCWSTR szFileName) const
	  {
		  return m_fnCreateVideoSystemConfig( szFileName );
	  }
};

class BridgeLibrary : DynamicLibrary
{
	CreateVideoSystemConfig_t	m_fnCreateVideoSystemConfig;
public:
	BridgeLibrary(): DynamicLibrary(_T("DVRDBBridge.dll"))
	{
		GetProcAddress( "CreateVideoSystemConfig",	m_fnCreateVideoSystemConfig );
	}

	CHCS::IVideoSystemConfigEx*	CreateVideoSystemConfig( LPCWSTR szFileName) const
	{
		return m_fnCreateVideoSystemConfig( szFileName );
	}
};

//======================================================================================//
//                                  class InstallerIml                                  //
//======================================================================================//

//! \brief Реализация интерфейса
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   26.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class InstallerIml : public IInstaller
{
	HRESULT m_hrCoinitResult;
	bool	m_bUseBridge;
	bool	m_bUseBridgeOption;
	
	std::vector< CameraInfo_t >	m_CameraArr;

	boost::shared_ptr<CHCS::IStreamManager>	m_pManager;
	boost::shared_ptr<CHCS::IStream>		m_pCurrentStream;

	StreamWindow							m_wndStream;
	std::auto_ptr<VideoLibary>				m_pVideoDynamicLibrary;
	std::auto_ptr<BridgeLibrary>				m_pBridgeDynamicLibrary;

	std::wstring							m_ServerIPAddress;

	boost::shared_ptr<CHCS::IVideoSystemConfigEx>	CreateVideoSystemConfig();

	void	DeleteAllDevicesAndCameras();
public:
	InstallerIml();
	virtual ~InstallerIml();
private:
	virtual bool SetBridgeForConfigure(bool UseBridge)
	{
		m_bUseBridgeOption = UseBridge;
		return true;
	}

	virtual bool Register(const char* ServerIPAddress);
	virtual bool Unregister();

	virtual bool GetAvalableStreams( CAUUID* pAvailableStreams, CAUUID* pInstalledStreams )	; 

	virtual bool AddStream		( const UUID* pStreamUID ) ;

	virtual bool RemoveStream	( const UUID* pStreamUID ) ;

	virtual bool ShowStream		( const UUID* pStreamUID, HWND hWndForRender ) ;

	virtual std::string GetArchivePath();

	virtual bool		 SetArchivePath( const std::string& sArchivePath );
};

#endif // _INSTALLER_IML_3974289482974550_