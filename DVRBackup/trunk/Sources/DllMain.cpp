//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     DllMain.cpp		
//!	\date     2006-02-15		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    DLL Main

#include "StdAfx.hpp"
#include "DllMain.hpp"
#include "../interface/DVRBackup.hpp"
#include "DVRBackupImp.hpp"


//Hack: To prevent wxWidgets from glitching
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


	boost::shared_ptr<IDVRBackup> CreateBackupInterface( 
												boost::shared_ptr<Elvees::IMessage>	pLogInterface, 
												boost::shared_ptr<IBackupParam>		pParamInterface )
	{
		return boost::shared_ptr<IDVRBackup>( new DVRBackupImp( pLogInterface, pParamInterface ) ); 
	}
