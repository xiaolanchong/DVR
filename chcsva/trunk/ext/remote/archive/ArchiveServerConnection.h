//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Соединение на стороне сетевого сервера архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_SERVER_CONNECTION_7541606018939932_
#define _ARCHIVE_SERVER_CONNECTION_7541606018939932_

#include "../NetProtocol.h"
#include "../ipc/NetUtil.h"
#include "../ipc/NetTCPConnection.h"
#include "../../archive/ArchiveDataExchange.h"
#include "../../../ArchiveReader.h"

class CArchiveReader;
//======================================================================================//
//                            class ArchiveServerConnection                             //
//======================================================================================//

//! \brief Соединение на стороне сетевого сервера архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveServerConnection : public NetTCPServerConnection,
								public IArchiveDataResponse,
								private IArchiveServerPacketProcessor
{
	virtual void	OnReceivePacket( const std::vector<BYTE>& Data);
	virtual bool	OnSocketError( const char* ErrorDesc )
	{
		return true;
	}
private:	
	std::auto_ptr<CArchiveReader>		m_pArchiveReader;
	boost::optional<BITMAPINFOHEADER>	m_StreamFormat;	

	virtual bool	SendFrameData(
		const void* pImageData, size_t ImageDataSize, 
		const void* pUserData,  size_t UserDataSize, 
		unsigned uFrameSeq, unsigned uTime 
		) ;

	virtual bool	SendEndOfFormat();

	virtual bool	SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput )
	{
		m_StreamFormat = pbiInput->bmiHeader;
		return true;
	}

	virtual  void	Shutdown()
	{
		ForceShutdown();
	}
	virtual void	OnPostCloseConnection();
	virtual  bool IsConnected(int ) 
	{
		return m_Socket.IsValid() && IsWorking();
	}
private:
	virtual void	OnCommand(LPCSTR szCommand, size_t TextSize);
public:
	ArchiveServerConnection(SOCKET s, const std::tstring& stBaseArchivePath);
	virtual ~ArchiveServerConnection();
};

#endif // _ARCHIVE_SERVER_CONNECTION_7541606018939932_