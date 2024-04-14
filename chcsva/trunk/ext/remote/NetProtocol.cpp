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

#include "chcs.h"
#include "NetProtocol.h"

bool ArchivePacketValidator::GenericCheck( const void* pPacketData, size_t PacketSize )
{
	if( !pPacketData || !PacketSize  )
	{
		return false;
	}
	if( sizeof( NP_Generic ) > PacketSize )
	{
		throw std::runtime_error( "ValidateArchiveServerPacket : too small packet size" );
	}	
	return true;
}

void	ArchivePacketValidator::ValidateArchiveServerPacket(
									const void* pPacketData, size_t PacketSize,
									IArchiveServerPacketProcessor* p 
									)
{
	if( !GenericCheck( pPacketData, PacketSize ) || !p )
	{
		return;
	}
	const NP_Generic* packet = reinterpret_cast<const NP_Generic*>( pPacketData );
	switch( packet->m_Command ) 
	{
	case  NPCommand_ArchiveCommand :
		p->OnCommand( reinterpret_cast<const char*>(packet) + sizeof(NP_ArchiveCommand),
					  PacketSize - 	sizeof(NP_ArchiveCommand));
		break;
	default:
		throw std::runtime_error( "ValidateArchiveServerPacket : unknown command" );
	}
}

void	ArchivePacketValidator::ValidateArchiveClientPacket(
		const void* pPacketData, size_t PacketSize,
		IArchiveClientPacketProcessor* p 
	)
{
	if( !GenericCheck( pPacketData, PacketSize ) || !p )
	{
		return;
	}
	const NP_Generic* packet = reinterpret_cast<const NP_Generic*>( pPacketData );
	switch( packet->m_Command ) 
	{
	case  NPCommand_VideoImage :
		{
			const NP_VideoImage* pImagePacket = reinterpret_cast<const NP_VideoImage*>( pPacketData );

			const BYTE* pHeader		= (const BYTE*)pPacketData + sizeof(NP_VideoImage);
			const BYTE* pImage		= pHeader + pImagePacket->m_ImageDataOffset;
			const BYTE* pUserData	= pHeader + pImagePacket->m_UserDataOffset;
			p->OnImageData( 
				pImagePacket->m_StreamID, 
				reinterpret_cast<const BITMAPINFOHEADER*>( pHeader ),
				pImage,		pImagePacket->m_UserDataOffset - pImagePacket->m_ImageDataOffset,
				pUserData,	PacketSize - pImagePacket->m_UserDataOffset - sizeof( NP_VideoImage ),
				pImagePacket->m_FrameTime, pImagePacket->m_RelativeFrameTime
				);
		}
		break;
	case NPCommand_ArchiveCloseDecompressor:
		p->OnCloseDecompressor();
		break;
	default:
		throw std::runtime_error( "ValidateArchiveServerPacket : unknown command" );
	}
}