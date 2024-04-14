// InstallHelper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "InstallHelper.h"
#include "interface/InstallerIml.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HINSTANCE g_hDLLHandler = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if( DLL_PROCESS_ATTACH == ul_reason_for_call )
	{
		g_hDLLHandler = hModule;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif



IInstaller*	CreateInstallInterface()
{
	return new InstallerIml;
}

STDAPI DllRegisterServer()
{
/*	IInstaller* p = CreateInstallInterface();
	bool res = p->Register();
	delete p;*/
	return /*res ? S_OK : */E_FAIL;
}

STDAPI DllUnregisterServer()
{
	std::auto_ptr<IInstaller> p ( CreateInstallInterface() );
	return p->Unregister() ? S_OK : E_FAIL;
}

class Singleton
{
	static std::auto_ptr<IInstaller>	s_pInstaller;
public:
	static IInstaller* GetInstance()
	{
		if( !s_pInstaller.get() )
		{
			s_pInstaller = std::auto_ptr<IInstaller>( new InstallerIml );
		}
		return s_pInstaller.get();
	}
	static void FreeInstance()
	{
		s_pInstaller.reset();
	}
};

std::auto_ptr<IInstaller>	Singleton::s_pInstaller;

bool	ConvertToUUID(const char* szUID, UUID& uid)
{
	USES_CONVERSION;
	CA2W cvt(szUID);
	char buf[255];
	strncpy( buf, szUID, 255 );
	return	UuidFromStringA( reinterpret_cast<unsigned char*>( buf ), &uid ) == S_OK  ||
			CLSIDFromString( cvt, &uid ) == S_OK ;
}

//////////////////////////////////////////////////////////////////////////

//static bool	g_UseBridgeForConfiguration = true;

bool  WINAPI	SetBridgeForConfigure( bool UseBridge )
{
	return Singleton::GetInstance()->SetBridgeForConfigure( UseBridge);
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI	Register(const char* ServerIPAddress, const char* szArchivePath)
{
	if( DVREngine::ServerLaunched() )
	{
		return Res_ServerLaunched;
	}

	bool res = Singleton::GetInstance()->Register(ServerIPAddress);
	if(!res)
	{
		return Res_Error;
	}
	if( szArchivePath && *szArchivePath )
	{
		return Singleton::GetInstance()->SetArchivePath( szArchivePath ) ? Res_Success : Res_Error;
	}
	else 
	{
		return Res_Success;
	}
}

DWORD WINAPI	Unregister()
{
	if( DVREngine::ServerLaunched() )
	{
		return Res_ServerLaunched;
	}

	return Singleton::GetInstance()->Unregister() ? Res_Success : Res_Error;
}

DWORD	WINAPI GetAvailableStreams( CAUUID* pAvailableStreams )
{
	return Singleton::GetInstance()->GetAvalableStreams( pAvailableStreams, 0 ) ? Res_Success : Res_Error;
}

DWORD	WINAPI AddStream( const char* szStreamUID)
{
	UUID uid;
	if( ConvertToUUID(szStreamUID, uid) )
	{
		return Singleton::GetInstance()->AddStream( &uid );
	}
	else
	{
		return false;
	}
}

DWORD WINAPI	RemoveStream( const char* szStreamUID )
{
	UUID uid;
	if( ConvertToUUID(szStreamUID, uid) )
	{
		return Singleton::GetInstance()->RemoveStream( &uid );
	}
	else
	{
		return false;
	}
}

DWORD WINAPI	ShowStream( HWND hWndForStreamRender, const char* szStreamUID )
{
	UUID uid;
	if( !szStreamUID || !*szStreamUID )
	{
		return Singleton::GetInstance()->ShowStream( 0, hWndForStreamRender ) ? Res_Success : Res_Error;
	}

	if( ConvertToUUID(szStreamUID, uid) )
	{
		return Singleton::GetInstance()->ShowStream( &uid, hWndForStreamRender ) ? Res_Success : Res_Error;
	}
	else
	{
		return Res_Error;
	}
}

char s_ArchivePath[ MAX_PATH + 1 ];

const char*	WINAPI GetArchivePath()
{
	std::string sArchivePath = Singleton::GetInstance()->GetArchivePath( );
	strncpy( s_ArchivePath, sArchivePath.c_str(), MAX_PATH + 1 );
	return s_ArchivePath;
}

DWORD	WINAPI SetArchivePath(const char* szArchivePath)
{
	return Singleton::GetInstance()->SetArchivePath( szArchivePath ) ? Res_Success : Res_Error;
}

void	WINAPI	FreeMemory()
{
	Singleton::FreeInstance();
}