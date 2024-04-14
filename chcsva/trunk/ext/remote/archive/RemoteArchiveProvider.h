//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Архивный сетевой сервер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   04.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _REMOTE_ARCHIVE_PROVIDER_2819692928119365_
#define _REMOTE_ARCHIVE_PROVIDER_2819692928119365_

#include "../ipc/NetUtil.h"
#include "../BasicNetExchange.h"
#include "ArchiveServerConnection.h"

//======================================================================================//
//                             class RemoteArchiveProvider                              //
//======================================================================================//

//! \brief Архивный сетевой сервер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   04.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class RemoteArchiveProvider : BasicNetServer<ArchiveServerConnection>
{
	const std::tstring	m_stBaseArchivePath;

	virtual ArchiveServerConnection* GetServerConnection( SOCKET s)
	{
		return new ArchiveServerConnection( s, m_stBaseArchivePath );
	}
public:
	RemoteArchiveProvider(LPCTSTR stBaseArchivePath);
	virtual ~RemoteArchiveProvider();

	void	Start()
	{
		BasicNetServer<ArchiveServerConnection>::Start();
	}

	void	Stop()
	{
		BasicNetServer<ArchiveServerConnection>::Stop();
	}
};

#endif // _REMOTE_ARCHIVE_PROVIDER_2819692928119365_