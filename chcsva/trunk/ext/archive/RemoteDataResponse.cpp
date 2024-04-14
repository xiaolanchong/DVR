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
#include "chcs.h"
#include "RemoteDataResponse.h"

#include "../../NetProtocol.h"
//======================================================================================//
//                               class RemoteDataResponse                               //
//======================================================================================//

RemoteDataResponse::RemoteDataResponse(CArchiveServer* pServer, Elvees::CSocketServer::Socket *pSocket):
	m_pSocket(pSocket),
	m_pServer(pServer)
{
}

RemoteDataResponse::~RemoteDataResponse()
{
}

bool	RemoteDataResponse::SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput ) 
{
#if OLD
	Elvees::CIOBuffer* pFormat = m_pServer->AllocateFrame();
	if(pFormat)
	{
		NetPacketHeader header;
		NetStreamInfo   datainfo;

		datainfo.uStreamUID  = GUID_NULL;
		datainfo.uFOURCC     = pbiFormat->bmiHeader.biCompression;
		datainfo.uFormatSize = cbFormat;

		header.uSynchro = CHCS_NET_SYNCHRO;
		header.uPacket  = NET_PACKET_STREAMINFO;
		header.uTime    = 0;
		header.uLength  = sizeof(NetPacketHeader) + sizeof(NetStreamInfo) + cbFormat;
		header.uCheckSum = header.uPacket + header.uTime + header.uLength;

		pFormat->AddData(&header, sizeof(header));
		pFormat->AddData(&datainfo, sizeof(datainfo));
		pFormat->AddData(pbiFormat, cbFormat);

		// Store copy for future use
		if(m_pFormatPacket)
			m_pFormatPacket->Release();

		m_pFormatPacket = pFormat;
		

	}

#endif

	return true;
}

bool	RemoteDataResponse::SendData(const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime)	
{
#if OLD
	pData = m_pServer->AllocateFrame();

	if(pData)
	{
#if 0
		char stPacket[128];
		wsprintfA(stPacket, "\t%c \t%ld \t%u [F %d, I %d]\r\n",
			bKey ? '*' : '-', lSize,
			MAKELONG(m_uFrameSeq, m_uIFrameSeq),
			m_uFrameSeq,
			m_uIFrameSeq);

		lSize = (long)strlen(stPacket);
		memcpy(m_pCompressed, stPacket, lSize);
#endif

		if(lSize + sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo)
			<= pData->GetSize())
		{
			NetPacketHeader   header;
			NetStreamDataInfo datainfo;

			frmtime.QuadPart = filetime + MulDiv(lFrame, m_avisInfo.dwScale, m_avisInfo.dwRate);

			header.uSynchro = CHCS_NET_SYNCHRO;
			header.uPacket  = NET_PACKET_STREAMDATA;
			header.uTime    = frmtime.LowPart;
			header.uLength  = sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo) + lSize;
			header.uCheckSum = header.uPacket + header.uTime + header.uLength;

			datainfo.uStreamUID = GUID_NULL;
			datainfo.uFrameSize = lSize;
			datainfo.uFrameSeq  = MAKELONG(m_uFrameSeq, m_uIFrameSeq);
			// offset video data from custom data
			//										datainfo.uFrameVideoOffset = 0;

			pData->AddData(&header, sizeof(header));
			pData->AddData(&datainfo, sizeof(datainfo));
			pData->AddData(m_pCompressed, lSize);

			bSendIFrame = m_pSocket->GetWriteCount() > 5 ? true : !m_pSocket->Write(pData);
		}
		else
			bSendIFrame = true;

		pData->Release();
	}
	else
		bSendIFrame = true;
#endif
	return true;
}