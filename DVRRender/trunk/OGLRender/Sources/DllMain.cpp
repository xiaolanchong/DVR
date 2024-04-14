/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DllMain.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "StdAfx.h"
#include "Chcsva.h"	
#include "../../DVRRender.hpp"
#include "OGLStreamRender.hpp"
//#include "OGLArchiveRender.hpp"
#include "archive/ArchiveRender.h"
#include "OGLRenderManager.hpp"
#include "DllMain.hpp"

static unsigned short gVersionMajor = 1;
static unsigned short gVersionMinor = 0;

static OGLRenderManager* gRenderManager;

class wxDLLApp : public wxApp
{
	bool OnInit();
};

IMPLEMENT_APP_NO_MAIN(wxDLLApp) 

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

bool wxDLLApp::OnInit()
{
	return true;
}

extern "C"
{
	__declspec(dllexport) IDVRRenderManager* CreateRenderManager(
		boost::shared_ptr<CHCS::IVideoSystemConfig>		pVideoSystemConfig,
		boost::shared_ptr<Elvees::IMessage> pOutputDebug,
		wxWindow* parent )
	{
		gRenderManager = new OGLRenderManager( pVideoSystemConfig, pOutputDebug, parent );
		return static_cast<IDVRRenderManager*>( gRenderManager );
	}

	__declspec(dllexport) unsigned int GetRenderVersion()
	{
		struct{
			unsigned short major;
			unsigned short minor;
		} version;

		version.major = gVersionMajor;
		version.minor = gVersionMinor;
		return *(unsigned int*)(&version);
	}
}

OGLRenderManager* dllGetRenderManager()
{
	return gRenderManager;
}

OGLGraphicsWrapper* dllGetGraphicsWrapper()
{
	return gRenderManager ? (OGLGraphicsWrapper*)gRenderManager->GetWrapper() : 0;
}

OGLStreamRender* dllGetStreamRender()
{
	return gRenderManager ? (OGLStreamRender*)gRenderManager->GetStreamRender() : 0;
}

ArchiveRender* dllGetArchiveRender()
{
	return gRenderManager ? (ArchiveRender*)gRenderManager->GetArchiveRender() : 0;
}