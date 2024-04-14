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

#include "chcs.h"
#include "RemoteArchiveProvider.h"
#include "../NetProtocol.h"

//======================================================================================//
//                             class RemoteArchiveProvider                              //
//======================================================================================//

RemoteArchiveProvider::RemoteArchiveProvider(LPCTSTR stBaseArchivePath):
	m_stBaseArchivePath( stBaseArchivePath ),
		BasicNetServer( c_ArchiveServerPort, c_DefMaxSocketConnection, _T("Archive") )
{
}

RemoteArchiveProvider::~RemoteArchiveProvider()
{
}