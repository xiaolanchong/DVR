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

#include "chcs.h"
#include "ArchiveServerConnection.h"

//======================================================================================//
//                            class ArchiveServerConnection                             //
//======================================================================================//

ArchiveServerConnection::ArchiveServerConnection(
		SOCKET s, 
		const std::tstring& stBaseArchivePath
	):
	NetTCPServerConnection(s)
{
	m_pArchiveReader = std::auto_ptr<CArchiveReader>( new CArchiveReader( this, stBaseArchivePath ) ); 
	m_pArchiveReader->Start();
}

ArchiveServerConnection::~ArchiveServerConnection()
{
//	m_pArchiveReader->Shutdown();
}

void	ArchiveServerConnection::OnReceivePacket( const std::vector<BYTE>& Data)
try
{
	if( Data.empty() )
	{
		return;
	}
	ArchivePacketValidator v;
	v.ValidateArchiveServerPacket( &Data[0], Data.size(), this );
}
catch( std::runtime_error&  )
{

}

bool	ArchiveServerConnection::SendEndOfFormat()
try
{
	m_StreamFormat = boost::optional<BITMAPINFOHEADER>();

	NPacker_ArchiveCloseDecompressor packet;
	SendData(packet.GetData(), packet.GetDataSize());
	return true;
}
catch(NetException& )
{
	ForceShutdown();
	return true;
};

bool	ArchiveServerConnection::SendFrameData(
							  const void* pImageData, size_t ImageDataSize, 
							  const void* pUserData,  size_t UserDataSize, 
							  unsigned uFrameSeq, unsigned uTime 
							  ) 
try
{
	if( !m_StreamFormat.get_ptr() )
	{
		// wait for format
		return true;
	}
	NPacker_VideoImage packet(
			-1,
			m_StreamFormat.get_ptr(), 
			pImageData, ImageDataSize,
			pUserData, UserDataSize,
			uFrameSeq, uTime
		);
	SendData( packet.GetData(), packet.GetDataSize() );
	return true;
}
catch ( NetException&  ) 
{
	ForceShutdown();
	return false;
};

void	ArchiveServerConnection::OnCommand(LPCSTR szCommand, size_t TextSize)
{
	m_pArchiveReader->ProcessCommand( szCommand, TextSize );
}

void	ArchiveServerConnection::OnPostCloseConnection()
{
	NetTCPServerConnection::OnPostCloseConnection();
	Elvees::OutputF( Elvees::TInfo, _T("Close archive stream") );
	m_pArchiveReader->Shutdown();
	m_pArchiveReader.reset();
}