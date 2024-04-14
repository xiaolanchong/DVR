/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DllMain.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: DLL-main 
*
*
*/
#ifndef __DLLMAIN_HPP__
#define __DLLMAIN_HPP__

class IDVRRender;
class IDVRArchiveRender;
class IDVRRenderManager;
class OGLRenderManager;
class OGLGraphicsWrapper;
class OGLStreamRender;
class ArchiveRender;

namespace Elvees
{
	class IMessage;
}

namespace CHCS
{
	class IVideoSystemConfig;
}

//Internal functions 
OGLRenderManager* dllGetRenderManager();
OGLGraphicsWrapper* dllGetGraphicsWrapper();

OGLStreamRender* dllGetStreamRender();
ArchiveRender* dllGetArchiveRender();


extern "C"
{
	//Creates oglRenderManager instance
	__declspec(dllexport) IDVRRenderManager* CreateRenderManager( 
		boost::shared_ptr<CHCS::IVideoSystemConfig>		pVideoSystemConfig,
		boost::shared_ptr<Elvees::IMessage> pOutputDebug, 
		wxWindow* parent );
	__declspec(dllexport) unsigned int GetRenderVersion();
}

#endif //__DLLMAIN_HPP__
