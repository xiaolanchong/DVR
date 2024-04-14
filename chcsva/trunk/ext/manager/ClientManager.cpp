//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Главный класс реализации интерфейса IStreamManager для клиента

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "ClientManager.h"
#include "../config/SystemSettings.h"

#include "../../Stream.h"
#include "../localvideo/LocalClientStream.h"

#include "../remote/video/RemoteVideoConsumer.h"
#include "../remote/NetProtocol.h"
#include "../remote/ipc/NetUtil.h"

#include "../ImageGraph/DecompressFilter.h"

#include "../archive/IArchiveStreamFactory.h"

//======================================================================================//
//                                 class ClientManager                                  //
//======================================================================================//

ClientManager::ClientManager(boost::shared_ptr<SystemSettings> pVideoConfig, DWORD WorkingMode):
	BaseManager( pVideoConfig, false, WorkingMode )
{
}

ClientManager::~ClientManager()
{
}

void ClientManager::InitializeWork() 
{
#ifndef ARCHIVE_ONLY
	CreateVideoConsumer();
#endif
	StorageSystemSettings param;

	SystemSettings& ss = GetSystemSettings();
	bool res = ss.LoadStorageSettings( param );

	if(! res )
	{
		return ;
	}

	lstrcpyn( m_stBasePath,		param.m_stBasePath.c_str(), MAX_PATH);	// Archive base path

#ifndef ARCHIVE_ONLY
	ReadStreamsFromDB( true );
#endif
}

void ClientManager::UninitializeWork()
{
	m_VideoConsumer.reset();
	m_StreamList.Clear();
}

bool ClientManager::ReadStreamsFromDB(bool bShowDialogOnFailed)
{
	Elvees::Output(Elvees::TInfo, TEXT("> Read client streams from database"));

	//////////////////////////////////////////////////////////////////////////
	//

	PHOSTENT pHost = gethostbyname(NULL);

	if(pHost)
	{
		Elvees::Output(Elvees::TInfo, TEXT("Local address(es):"));

		int nAddr = 0;

		while(pHost->h_addr_list[nAddr])
		{
			Elvees::OutputF(Elvees::TInfo, TEXT("\t%hS"),
				inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[nAddr])); 
			nAddr++;
		}
	}

	SystemSettings& ss = GetSystemSettings();
	std::vector<CameraSettings> CamParams;
	bool res = ss.LoadCameraSettings( bShowDialogOnFailed, CamParams );

	if( !res )
	{
		return false;
	}

	for ( size_t i = 0; i < CamParams.size(); ++i  )
	{
		long	lCameraID	 = CamParams[i].m_lCameraID;
		long	lDevicePin	 = CamParams[i].m_lDevicePin;
		LPCTSTR stCameraIP	 = CamParams[i].m_stCameraIP.c_str();

	//	hostent * pHostInfo = gethostbyaddr( stCameraIP, lstrlen(stCameraIP), AF_INET );

		unsigned long IPAddr = inet_addr( stCameraIP  );
		if( IPAddr == INADDR_NONE )
		{
			Elvees::OutputF(Elvees::TError, TEXT("Invalid/unknown IP address = \"%hs\""), stCameraIP);
			continue;
		}
		bool bLocalIP = SystemSettings::IsLocalIPAddress( IPAddr );

		TCHAR stCameraUID[1024];
		lstrcpyn( stCameraUID, CamParams[i].m_stCameraUID.c_str(), 1024 );

		UUID camUID;
		if(!StringToUuid(stCameraUID, &camUID))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Invalid stream UID = \"%s\""), stCameraUID);
			break;
		}

		if( IsStreamAlreadyAttached(lCameraID) )
		{
			Elvees::OutputF(Elvees::TError, TEXT("Stream %ld is already attached, ignore"), lCameraID);
			continue;
		}

		CHCS::IStream * pStream = 0;
		if( bLocalIP && UseCommonProcess() )
		{
			pStream = LocalClientStream::CreateStream(lCameraID, camUID);
		}
		else
		{
			pStream = CreateRemoteStream(lCameraID, stCameraIP);
		}

		if(pStream)
		{
			StreamList::StreamInfo info;
			info.m_bIsLocal		= false;
			info.m_DevicePin	= lDevicePin;
			info.m_pStream		= boost::shared_ptr<CHCS::IStream>( pStream );
			info.m_stMoniker	= stCameraIP;
			info.m_StreamID		= lCameraID;
			info.m_StreamUID	= camUID;
			m_StreamList.Push(info );
			Elvees::OutputF(Elvees::TInfo, TEXT(" %c%ld %s (%ld) \"%s\""),
				bLocalIP ? TEXT('~') : TEXT('*'), lCameraID,
				stCameraIP, lDevicePin, stCameraUID);

		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////


CHCS::IStream* ClientManager:: CreateArchiveStream( long lSourceID, INT64 startPos)
{
	SystemSettings& ss = GetSystemSettings();
	ULONG sourceIP = ss.GetIPAddressOfStreamServer( lSourceID );
	if(INADDR_NONE == sourceIP)
	{
		return 0;
	}

	return GetArchiveStreamFactory()->CreateArchiveStream( 
		lSourceID, startPos, 0, sourceIP, GetVideoBasePath() 
		);
}

void ClientManager::CreateVideoConsumer()
try
{
	m_VideoConsumer = std::auto_ptr<RemoteVideoConsumer>( new RemoteVideoConsumer );
	Elvees::OutputF(Elvees::TInfo, 
		TEXT("RemoteVideoConsumer created"));
}
catch ( NetException& ex ) 
{
	Elvees::OutputF(Elvees::TError, 
		TEXT("Failed to create RemoteVideoConsumer : %hs"), ex.what());
};

CHCS::IStream* ClientManager::CreateRemoteStream(long StreamID, LPCTSTR stCameraIP)
try
{
	DecompressFilter* p = new DecompressFilter(StreamID);
	m_VideoConsumer->ConnectPin(p, StreamID, InternetAddress( stCameraIP, c_StreamServerPort ));
	Elvees::OutputF(Elvees::TInfo, TEXT("DecompressFilter created for %hs"), stCameraIP);
	return p;
}
catch ( ... ) 
{
	Elvees::OutputF(Elvees::TError, 
		TEXT("Failed to create DecompressFilter for %hs"), stCameraIP);
	return 0;
};