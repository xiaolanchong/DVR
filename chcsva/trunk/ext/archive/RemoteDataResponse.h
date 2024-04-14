//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Обработка запросов архивного сервера между компьютерами

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _REMOTE_DATA_RESPONSE_7116588856587880_
#define _REMOTE_DATA_RESPONSE_7116588856587880_

#include "ArchiveDataExchange.h"

//======================================================================================//
//                               class RemoteDataResponse                               //
//======================================================================================//

//! \brief Обработка запросов архивного сервера между компьютерами
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.06.2006
//! \version 1.0
//! \bug 
//! \todo 
#if 0
class RemoteDataResponse : public IArchiveDataResponse
{
public:
	RemoteDataResponse(CArchiveServer* pServer, Elvees::CSocketServer::Socket *pSocket );
	virtual ~RemoteDataResponse();

	virtual bool	SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput ) ;
	virtual bool	SendData(const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime)	;

private:

	Elvees::CSocketServer::Socket *m_pSocket;		// Connected socket
	Elvees::CIOBuffer             *m_pFormatPacket;	// Format packet
	CArchiveServer *m_pServer;					// Server
};
#endif
#endif // _REMOTE_DATA_RESPONSE_7116588856587880_