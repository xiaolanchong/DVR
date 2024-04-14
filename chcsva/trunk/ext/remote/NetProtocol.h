//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Форматы пакетов обмена данных сервер-клиент

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_PROTOCOL_8835930506626854_
#define _NET_PROTOCOL_8835930506626854_

const unsigned short c_StreamServerPort = 6015;
const unsigned short c_ArchiveServerPort = 6016;

#pragma pack(push)
#pragma pack(1)

struct NP_Generic
{
	boost::uint32_t	m_ProtoUID;
	boost::uint8_t	m_Command;
};

enum	
{
	// from server
	NPCommand_VideoImage		= 1,

	// from server
	NPCommand_ArchiveCloseDecompressor		= 128,
	// from client 
	NPCommand_ArchiveCommand				= 129
};

struct NP_VideoImage : NP_Generic
{
	boost::int32_t		m_StreamID;
	boost::uint32_t		m_ImageDataOffset;
	boost::uint32_t		m_UserDataOffset;
	boost::uint64_t		m_FrameTime;
	boost::uint32_t		m_RelativeFrameTime;
};

struct NP_ArchiveCommand : NP_Generic
{
	boost::int32_t		m_StreamID;
};

struct NP_ArchiveCloseDecompressor : NP_Generic
{
};

class NPacker_Generic
{
	std::vector<BYTE>	m_Data;
protected:
	template<typename T> void	Pack( T v)
	{
		m_Data.insert( m_Data.end(), (const BYTE*)&v, (const BYTE*)&v + sizeof(v)  );
	}
	void	Pack( const void* pData, size_t DataSize)
	{
		m_Data.insert( m_Data.end(), (const BYTE*)pData, (const BYTE*)pData + DataSize  );
	}
public:
	const void*	GetData() const
	{
		return m_Data.size() ? &m_Data[0] : 0;
	}
	size_t		GetDataSize() const
	{
		return m_Data.size();
	}
};

const boost::uint32_t	c_VideoFourCC	= MAKEFOURCC('v','i','d','1');
const boost::uint32_t	c_ArchiveFourCC = MAKEFOURCC('a','r','c','1');

class NPacker_VideoImage : public NPacker_Generic
{
public:
	NPacker_VideoImage( 
			long StreamID, 
			const BITMAPINFOHEADER* pbih, 
			const void* pImageData, size_t ImageDataSize,
			const void* pUserData, size_t UserDataSize,
			boost::uint64_t	FrameTime, boost::uint32_t RelativeFrameTime
			)
	{
		NP_VideoImage packet;
		packet.m_ProtoUID			= c_VideoFourCC;
		packet.m_Command			= NPCommand_VideoImage ;
		packet.m_StreamID			= StreamID;
		packet.m_ImageDataOffset	= sizeof(BITMAPINFOHEADER);
		packet.m_UserDataOffset		= packet.m_ImageDataOffset + ImageDataSize;
		packet.m_FrameTime			= FrameTime;
		packet.m_RelativeFrameTime	= RelativeFrameTime;

		Pack(&packet, sizeof( packet ) );
		Pack( pbih, sizeof(BITMAPINFOHEADER) );
		Pack( pImageData, ImageDataSize );
		Pack( pUserData, UserDataSize );
	}
};

//////////////////////////////////////////////////////////////////////////

class	NPacker_ArchiveCommand : public NPacker_Generic
{
public:
	NPacker_ArchiveCommand( long StreamID, const std::string& CommandText)
	{
		NP_ArchiveCommand packet;
		packet.m_ProtoUID			= c_ArchiveFourCC;
		packet.m_Command			= NPCommand_ArchiveCommand ;
		packet.m_StreamID			= StreamID;
		
		Pack( &packet, sizeof(packet) );
		Pack( CommandText.c_str(), CommandText.length() );
	}
};

class NPacker_ArchiveCloseDecompressor : public NPacker_Generic
{
public:
	NPacker_ArchiveCloseDecompressor()
	{
		NP_ArchiveCloseDecompressor packet;
		packet.m_ProtoUID			= c_ArchiveFourCC;
		packet.m_Command			= NPCommand_ArchiveCloseDecompressor ;
	}
};

#pragma pack(pop)

class IArchiveServerPacketProcessor
{
public:
	virtual void	OnCommand(LPCSTR szCommand, size_t TextSize) = 0;
};

class IArchiveClientPacketProcessor
{
public:
	virtual void	OnImageData(
		long StreamID, 
		const BITMAPINFOHEADER* pbih, 
		const void* pImageData, size_t ImageDataSize,
		const void* pUserData, size_t UserDataSize,
		boost::uint64_t	FrameTime, boost::uint32_t RelativeFrameTime
		) = 0;

	virtual void	OnCloseDecompressor() = 0;
};

class ArchivePacketValidator
{
	bool GenericCheck( const void* pPacketData, size_t PacketSize );
public:
//! разобрать сетевой пакет серверу
//! \param pPacketData данные пакета
//! \param PacketSize размер данных
//! \param pProtocolHandler	интерфейс обработки пакетов
//! \exception	std::runtime_error произошла ошибка формата
void	ValidateArchiveServerPacket( 
				const void* pPacketData, size_t PacketSize,
				IArchiveServerPacketProcessor* pProtocolHandler 
				); 

//! разобрать сетевой пакет клиенту
//! \param pPacketData данные пакета
//! \param PacketSize размер данных
//! \param pProtocolHandler интерфейс обработки пакетов
//! \exception	std::runtime_error произошла ошибка формата
void	ValidateArchiveClientPacket( 
									const void* pPacketData, size_t PacketSize,
									IArchiveClientPacketProcessor* pProtocolHandler
									); 

};

#endif // _NET_PROTOCOL_8835930506626854_