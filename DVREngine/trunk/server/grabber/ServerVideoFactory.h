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
#ifndef _SERVER_VIDEO_FACTORY_7158850564915478_
#define _SERVER_VIDEO_FACTORY_7158850564915478_

#include "../MessageImpl.h"
#include "IVideoProvider.h"

//======================================================================================//
//                               class ServerVideoFactory                               //
//======================================================================================//

//! \brief Класс создания серверных видеопотоков, возможно продвижение до Abstract Factory
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerVideoFactory
{
	//! отладочный поток
	MessageImpl		m_Msg;
	//! менеджер видеопотоков CHCSVA
	boost::shared_ptr<CHCS::IStreamManager> m_pManager;
	//! отладочная процедура для CHCSVA (передается при инициализации и выводит в m_Msg)
	static void OutputDebugProc( DWORD_PTR, int, LPCWSTR );

	boost::shared_ptr<CHCS::IStreamManager> m_pClientManager;
public:

	MACRO_EXCEPTION(	ServerVideoException ,std::runtime_error );

	//! \param pMsg отладочный поток
	ServerVideoFactory( DWORD dwVideoModeFlags, boost::shared_ptr<Elvees::IMessage> pMsg );
	virtual ~ServerVideoFactory();

	//! создать видео поток для камеры
	//! \param nCameraID идентификатор камеры, для кот. требуется видео
	//! \return если не NULL, то содержить реализацию
	boost::shared_ptr<IVideoProvider>	CreateVideoReader( int nCameraID );
};

#endif // _SERVER_VIDEO_FACTORY_7158850564915478_