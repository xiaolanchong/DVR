//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Сборка/отправка TCP пакетов

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "NetUtil.h"
#include "NetPacketAssembler.h"

//======================================================================================//
//                               class NetPacketAssembler                               //
//======================================================================================//

void		NetPacketAssembler::ReceiveData( const void* pBytes, size_t nSize )
{
	StreamData* pData = &m_StreamData;
	std::vector<BYTE> & Arr = pData->Data;
	while( nSize )
	{

		if( pData->TotalValid )
		{
			ptrdiff_t nDiff = pData->Data.size() + nSize -  pData->TotalBytes;
			if( nDiff == 0)
			{
				Sys_Debug( "<-- + %u bytes", (unsigned int)nSize );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize );	
				Sys_Debug( "<-- Save message %u bytes", (unsigned int)Arr.size() );
				OnReceivePacket( pData->Data ) ;
				pData->Data.clear();
				pData->TotalValid = false;
				break;

			}
			else if( nDiff < 0 )
			{
				Sys_Debug( "<-- + %d bytes\n", nSize );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize);
				break;
			}
			else if( nDiff > 0 )
			{
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
					(const BYTE*)pBytes + nSize - nDiff );
				Sys_Debug( "<-- Save message %u bytes\n", (unsigned)Arr.size() );
				OnReceivePacket( pData->Data ) ;
				Arr.clear();
				pBytes = (const BYTE*)pBytes + nSize - nDiff;
				nSize = nDiff;
				pData->TotalValid = false;
				continue;
			}
		}
		else
		{
			size_t ForTotal = nSize + pData->Data.size();
			if( ForTotal >= sizeof(int) )
			{
				size_t OldSize = Arr.size() ;
				Arr.insert( pData->Data.end(), 
					(const BYTE*)pBytes, (const BYTE*)pBytes + sizeof(int) - Arr.size() );
				pData->TotalBytes = *(int*)&Arr[0];
				Sys_Debug( "<-- New packet ahead %d bytes\n", pData->TotalBytes  );
				Arr.clear();
				nSize -= sizeof(int) - Arr.size();
				pBytes = (const BYTE*)pBytes + sizeof(int) - Arr.size();
				pData->TotalValid = true;
				continue;
			}
			else
			{
				Sys_Debug( "<-- + for packet size %u bytes\n", (unsigned)nSize  );
				pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, (const BYTE*)pBytes + nSize);
				break;
			}
		}

	}
}

void	NetPacketAssembler::SendData( SenderFunc_t fn, const void* pBytes, size_t nSize )
{
	boost::uint32_t len = nSize;
/*	fn( &len, sizeof(len) );
	Sleep(300);
	fn( pBytes, nSize );*/

	/*
	Маркер необходимо передавать с данными за один вызов
	*/

	std::vector<BYTE>	NewPacket;
	NewPacket.reserve( sizeof(len) + nSize );
	NewPacket.insert( NewPacket.end(), (const BYTE*)&len,	(const BYTE*)&len +	sizeof(len)  );
	NewPacket.insert( NewPacket.end(), (const BYTE*)pBytes,	(const BYTE*)pBytes +	nSize  );

	fn( &NewPacket[0], NewPacket.size() );
}

void		NetPacketAssembler::ClearDataCache()
{
	m_StreamData.TotalBytes = 0;
	m_StreamData.TotalValid = false;
	m_StreamData.Data.clear();
}