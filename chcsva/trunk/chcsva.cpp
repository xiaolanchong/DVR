// CHCSVA.cpp : Implementation of DLL Exports.

#include "chcs.h"

#include "resource.h"
#include "DlgConfigure.h"
#include "DlgShowStream.h"

#include "ext/manager/ServerManager.h"
#include "ext/manager/ClientManager.h"
#include "ext/manager/StreamManager.h"
// old interface
//#include "ext/config/SystemSettingsBridgeImpl.h"
#include "ext/config/SystemSettingsVideoConfig.h"

#include <initguid.h>

DWORD_PTR        procOutputData = 0;
CHCS::OutputProc procOutput     = NULL; 

#ifdef _DEBUG
#define WRITELOG
#endif

#ifdef WRITELOG
	#include <memory>
	#include <iostream>
	#include <fstream>

	#ifdef _UNICODE
	typedef std::wfstream _tfstream;
	#else
	typedef std::fstream _tfstream;
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// ElveesTools Output function
/////////////////////////////////////////////////////////////////////////////

namespace Elvees
{

#ifdef WRITELOG
	static _tfstream s_debugOut;
#endif

void Output(TOutput type, LPCTSTR stText)
{
#ifdef _DEBUG
	TCHAR stHeader[24];
	SYSTEMTIME stLocalTime;

	GetLocalTime(&stLocalTime);

	wsprintf(stHeader,
		TEXT("%02hd:%02hd:%02hd [%04ld] %c "),
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond,
		GetCurrentThreadId(),
		(type == TTrace)    ? TEXT('D') :
		(type == TWarning)  ? TEXT('W') :
		(type == TInfo)     ? TEXT('I') :
		(type == TError)    ? TEXT('E') :
		(type == TCritical) ? TEXT('C') : TEXT('U'));

	_tprintf(TEXT("%s%s\n"), stHeader, stText);
#else
	OutputDebugString(stText);
	OutputDebugString(TEXT("\r\n"));
#endif

#ifdef WRITELOG
	if(!s_debugOut.is_open())
	{
		s_debugOut.open("chcsva.log", std::ios_base::out | std::ios_base::app);

		s_debugOut << TEXT("****************New Log*****************") << std::endl;
	}

	s_debugOut << stHeader << stText << std::endl;
#endif

	if(!procOutput)
		return;

	int nMode = 
		(type == TInfo) ? CHCS_INFO :
		(type == TTrace) ? CHCS_TRACE :
		(type == TError) ? CHCS_ERROR : 
		(type == TWarning) ? CHCS_WARNING :
		(type == TCritical) ? CHCS_CRITICAL : CHCS_INFO;

	__try
	{
	#ifndef _UNICODE
		WCHAR szMsg[1024];
		MultiByteToWideChar(CP_ACP, 0, stText, lstrlen(stText) + 1,
			szMsg, countof(szMsg));

		procOutput(procOutputData, nMode, szMsg);
	#else
		procOutput(procOutputData, nMode, stText);
	#endif
	}
	__except(1)
	{
		OutputDebugString(TEXT("Unexpected exception in DebugOutput\n"));
	}
}

} // End of namespace Elvees

//////////////////////////////////////////////////////////////////////
// Namespace: CHCS

namespace CHCS 
{

bool __stdcall FreeStreamManager(BaseManager* _Manager);

CHCSVA_API IStreamManager* __stdcall InitStreamManager(LPINITCHCS lpInitCHCS)
{
	if( !lpInitCHCS )
	{
		return NULL;
	}

	INITCHCS_EX	init;
	init.m_pVideoSystemConfig	= 0;

	init.dwSize		= sizeof(INITCHCS_EX);     // size of this structure
	init.dwMode		= lpInitCHCS->dwMode;     // operation mode
	init.dwReserved = lpInitCHCS->dwReserved; //
	init.dwUserData = lpInitCHCS->dwUserData; // output user data
	init.procOutput = lpInitCHCS->procOutput; // output message function

	return InitStreamManagerEx( &init );
}

CHCSVA_API IStreamManager* __stdcall InitStreamManagerEx(INITCHCS_EX* lpInitCHCS)
{
	DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("=InitStreamManager")));

	// Check input parameter
	//

	bool bOK = false;
	
	do
	{
		if(!lpInitCHCS)
			break;
	
		if(IsBadReadPtr(lpInitCHCS, sizeof(INITCHCS)))
			break;

		if(lpInitCHCS->dwSize < sizeof(INITCHCS))
			break;

		// Test output proc...

		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Mode=%ld"), lpInitCHCS->dwMode));

		if( !procOutput )
		{
		
		procOutput = lpInitCHCS->procOutput;
		procOutputData = lpInitCHCS->dwUserData;
		}

		if(lpInitCHCS->procOutput)
		{
			try
			{
				lpInitCHCS->procOutput(procOutputData, CHCS_INFO, L"InitManager: testing output");
			}
			catch(...)
			{
				procOutput = NULL;
				procOutputData = 0;

				Elvees::Output(Elvees::TError, TEXT("InitManager invalid output function"));
				break;
			}
		}

		bOK = true;
	}
	while(false);

	if(!bOK)
	{
		Elvees::Output(Elvees::TError, TEXT("InitManager failed. Invalid parameter"));
		return false;
	}

	// Create and start manager
	//
	BaseManager* _Manager = NULL;
	try
	{
		boost::shared_ptr<IVideoSystemConfigEx>	pStubConfig;
		IVideoSystemConfig* pRealConfig;

		if( lpInitCHCS->m_pVideoSystemConfig )
		{
			pRealConfig = lpInitCHCS->m_pVideoSystemConfig;
		}
		else
		{
			pStubConfig  = boost::shared_ptr<IVideoSystemConfigEx>( CHCS::CreateVideoSystemConfig() );
			pRealConfig	 = pStubConfig.get();
		}

		boost::shared_ptr<SystemSettings> ss ;
		try
		{
			ss.reset ( new SystemSettingsVideoConfig(pRealConfig) );
		}
		catch ( std::exception& ex ) 
		{
			Elvees::OutputF(Elvees::TError, TEXT("IVideoSystemConfigEx has thrown an exception: %hs"), ex.what());
			return NULL;
		}
		if( lpInitCHCS->dwMode & CHCS_MODE_DEVICENEW )
		{
			Elvees::Output(Elvees::TError, TEXT("Invalid parameter: CHCS_MODE_DEVICENEW not implemented. InitManager failed."));
			return NULL;
		}
		else if( lpInitCHCS->dwMode & CHCS_MODE_DEVICE )
		{
			_Manager = new ServerManager(ss, lpInitCHCS->dwMode);
				
			if(_Manager)
			{
				_Manager->Start();
				_Manager->WaitStarted();

				return new StreamManagerServerImpl( _Manager );
			}
		}
		else
		{
			ClientManager *ClManager = new ClientManager(ss, lpInitCHCS->dwMode);
			_Manager = ClManager;

			if(_Manager)
			{
				_Manager->Start();
				_Manager->WaitStarted();

				return new StreamManagerClientImpl( ClManager );
			}
		}
	}
	catch(const Elvees::CWin32Exception& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("InitManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("InitManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("InitManager - Unexpected exception"));
	}

	CHCS::FreeStreamManager(_Manager);

	return NULL;
}

bool __stdcall FreeStreamManager(BaseManager* _Manager)
{
	DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("=FreeStreamManager")));

	if(!_Manager)
		return false;

	try
	{
		_Manager->ShutDown();

		delete _Manager;
	}
	catch(const Elvees::CWin32Exception& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("FreeStreamManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("FreeStreamManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("FreeStreamManager - Unexpected exception"));
	}

	_Manager = NULL;

	return true;
}

CHCSVA_API void WINAPI Configure(HWND hwnd, HINSTANCE hinst, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;
	hr = CoInitialize(NULL);
	if(FAILED(hr))
	{
		::MessageBox(NULL, TEXT("CoInitialize failed"),
			TEXT("Configure"), MB_OK | MB_ICONSTOP);
		return;
	}

	CHCS::IVideoSystemConfig::CodecSettings cs;
	InitCodecState(cs);
	std::tstring ArchivePath;

	CDlgConfigure dlg(cs, ArchivePath);
	dlg.DoModal();

	SaveCodecState( dlg.GetCodecSettings() );

	CoUninitialize();
}

} // End of namespace CHCS

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(hInstance);
		_Module.Init(NULL, hInstance);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		_Module.Term();
	}

	return TRUE;
}

STDAPI DllRegisterServer(void)
{
	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	return S_OK;
}