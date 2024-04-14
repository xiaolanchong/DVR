//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс создания серверных видеопотоков

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerVideoFactory.h"
#include "ServerVideoReader.h"
#include "ServerFakeReader.h"

//#define FAKE_READER
#define LAUNCH_CHCSVA

//! NOTE: CAUUID requires this header, delete it after the next chcsva version will be available
#include <ocidl.h> 

#if !defined( FAKE_READER ) || defined (LAUNCH_CHCSVA)

#endif

//======================================================================================//
//                               class ServerVideoFactory                               //
//======================================================================================//

ServerVideoFactory::ServerVideoFactory(
									   DWORD dwVideoModeFlags,
									   boost::shared_ptr<Elvees::IMessage> pMsg
									   ):
	m_Msg(pMsg)
{
	std::auto_ptr<CHCS::IVideoSystemConfig> pVideoConfig( DBBridge::CreateServerVideoSystemConfig( ) ); 
#ifdef LAUNCH_CHCSVA
	CHCS::INITCHCS_EX in;
	//CHCS_MODE_SERVER
	const DWORD c_nFlags = dwVideoModeFlags;//CHCS_MODE_SERVER/*CHCS_MODE_NETSERVER | CHCS_MODE_DEVICE | CHCS_MODE_DATABASE | CHCS_MODE_GUI*/;

	in.dwMode		= c_nFlags;
	in.dwSize		= sizeof(CHCS::INITCHCS);
	in.dwUserData	= reinterpret_cast<DWORD_PTR>(this);
	in.procOutput	= &ServerVideoFactory::OutputDebugProc;
	// configurate with DVRDBBridge
	in.m_pVideoSystemConfig = pVideoConfig.get();

	CHCS::IStreamManager* pManager = CHCS::InitStreamManagerEx(&in);
	if (!pManager )
	{
		throw ServerVideoException("Failed to initialize chcsva.dll");
	}
	m_pManager = boost::shared_ptr<CHCS::IStreamManager>( pManager, ReleaseInterface<CHCS::IStreamManager>() );
#endif
}

ServerVideoFactory::~ServerVideoFactory()
{
#ifndef FAKE_READER
	m_pManager.reset();
#endif
}

void ServerVideoFactory::OutputDebugProc(DWORD_PTR pUserData, int nType, LPCWSTR szText)
{
	//! вызывается из chcsva, просто преобразуем в ANSI и положим в поток
	USES_CONVERSION;
	_ASSERTE( pUserData );
	_ASSERTE( szText );
	const size_t MaxStrLen = 1024;
	if( wcsnlen_s( szText, MaxStrLen + 1 ) >= MaxStrLen + 1 )
	{
		// too long
		return ;
	}
	ServerVideoFactory* pThis = reinterpret_cast<ServerVideoFactory*>(pUserData);
	pThis->m_Msg.Print( nType, W2CA(szText) );
}

boost::shared_ptr<IVideoProvider>	
	ServerVideoFactory::CreateVideoReader( int nCameraID )
try
{
	return boost::shared_ptr<IVideoProvider>( new 
#ifdef FAKE_READER
		ServerFakeReader
#else
		ServerVideoReader( m_pManager, nCameraID) 
#endif
		);
}
catch( Elvees::VideoReaderException& ex )
{
	throw ServerVideoException( ex.what() );
};