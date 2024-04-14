//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ ÷åðåç âûçîâ DVRDBBridge íàïðÿìóþ

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "SystemSettingsBridgeImpl.h"

#include "../../resource.h"

//#pragma TODO("Uncomment EUGENE_ADD_ON")
#define EUGENE_ADD_ON

#ifdef EUGENE_ADD_ON
#include "../DBRegWrapper.h"
#include "../DatabaseWrapper.h"
#else
#include "DBAccessors.h"
#endif

//======================================================================================//
//                                 class SystemSettingsBridgeImpl                                 //
//======================================================================================//

SystemSettingsBridgeImpl::SystemSettingsBridgeImpl()
{
}

SystemSettingsBridgeImpl::~SystemSettingsBridgeImpl()
{
}

bool SystemSettingsBridgeImpl::LoadStorageSettings( StorageSystemSettings& param )
{
	LONG  lRes;
	DWORD dwValue;

	TCHAR m_stBasePath[MAX_PATH];	// Archive base path
	TCHAR m_stDBInitString[4096];	// DB Initialization string

	// Archive settings

	DWORD m_CodecFCC;				// Codec FOURCC

	long m_lVideoFPM;				// Video Frame per minute
	long m_lVideoMaxSize;			// Video max size in seconds

	long m_lCodecQuality;			//
	long m_lCodecKeyRate;			// Forced KeyRate
	long m_lCodecDataRate;			// Forced DataRate

	//	long  m_lCodecData;				// Codec state size
	//	void *m_pCodecData;				// Codec state
	std::vector<BYTE>				m_CodecStateData;

#ifdef EUGENE_ADD_ON
	CWrappedRegKey
#else
	CRegKey
#endif
		keyCinema;
	TCHAR stKey[MAX_PATH];

	// Load from registry

	if(::LoadString(_Module.GetResourceInstance(), 
		IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}

	lRes = keyCinema.Open(HKEY_LOCAL_MACHINE, stKey);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	dwValue = countof(m_stBasePath);
	lRes = keyCinema.QueryStringValue(TEXT("ArchivePath"), m_stBasePath, &dwValue);
	if(lRes != ERROR_SUCCESS)
		lstrcpy(m_stBasePath, TEXT(""));
#if 0
	dwValue = countof(m_stDBInitString);
	lRes = keyCinema.QueryStringValue(TEXT("DBConnectionString"), m_stDBInitString, &dwValue);
	if(lRes != ERROR_SUCCESS)
		lstrcpy(m_stDBInitString, TEXT(""));
#else
	lstrcpy(m_stDBInitString, TEXT(""));
#endif
	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
		TEXT("LoadSettings BasePath=\"%s\""), m_stBasePath));

	// Read archive settings
	//

	do
	{	
#ifdef EUGENE_ADD_ON
		CWrappedRegKey
#else
		CRegKey
#endif		
			keyStore;

		lRes = keyStore.Open(keyCinema, TEXT("Storage"));
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail open archive setting registry key"));
			break;
		}

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_FCC"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_CodecFCC = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_Quality"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecQuality = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_KeyRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecKeyRate = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_DataRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecDataRate = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_FMP"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoFPM = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_MaxSize"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoMaxSize = dwValue;

		// Load codec state

		//if(m_pCodecData)
		//	free(m_pCodecData);

		//m_lCodecData = 0;
		//m_pCodecData = NULL;

		dwValue = 0;

		lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), NULL, &dwValue);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));
			break;
		}

		if(dwValue > 0)
		{
			try
			{
				m_CodecStateData.resize( dwValue );
			}
			catch( std::bad_alloc& )
			{
				Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
				break;
			}
			lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), &m_CodecStateData[0], &dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));

				//free(m_pCodecData);

				//m_lCodecData = 0;
				//m_pCodecData = NULL;
				m_CodecStateData.clear();
			}
		}
	}
	while(false);

#ifdef _DEBUG
	BYTE bFCC[4];
	bFCC[0] = (BYTE)((m_CodecFCC & 0x000000FF));
	bFCC[1] = (BYTE)((m_CodecFCC & 0x0000FF00) >> 0x08);
	bFCC[2] = (BYTE)((m_CodecFCC & 0x00FF0000) >> 0x10);
	bFCC[3] = (BYTE)((m_CodecFCC & 0xFF000000) >> 0x18);

	if(!isprint(bFCC[0])) bFCC[0] = '?';
	if(!isprint(bFCC[1])) bFCC[1] = '?';
	if(!isprint(bFCC[2])) bFCC[2] = '?';
	if(!isprint(bFCC[3])) bFCC[3] = '?';

	Elvees::OutputF(Elvees::TTrace, TEXT("Store: FMP=%ld MS=%ld FCC=0x%08lX (%c%c%c%c) Q=%ld KR=%ld DR=%ld SS=%ld"),
		m_lVideoFPM,
		m_lVideoMaxSize,
		m_CodecFCC, bFCC[0], bFCC[1], bFCC[2], bFCC[3],
		m_lCodecQuality,
		m_lCodecKeyRate,
		m_lCodecDataRate,
		static_cast<DWORD>( m_CodecStateData.size() ) );
#endif

	param.m_stBasePath		= m_stBasePath;
	param.m_stDBInitString	= m_stDBInitString;

	// Archive settings
	param. m_CodecFCC = m_CodecFCC;				// Codec FOURCC

	param. m_lVideoFPM = m_lVideoFPM;				// Video Frame per minute
	param. m_lVideoMaxSize = m_lVideoMaxSize;			// Video max size in seconds

	param. m_lCodecQuality = m_lCodecQuality;			//
	param. m_lCodecKeyRate = m_lCodecKeyRate;			// Forced KeyRate
	param. m_lCodecDataRate = m_lCodecDataRate;			// Forced DataRate

	param. m_CodecStateData = m_CodecStateData;

	return true;
}

void SystemSettingsBridgeImpl::SaveStorageSettings( LPCTSTR stBasePath, LPCTSTR stDBInitString  )
{
	LPCTSTR m_stBasePath		= stBasePath;
	LPCTSTR m_stDBInitString	= stDBInitString;

	LONG  lRes;
	TCHAR stKey[MAX_PATH];
#ifdef EUGENE_ADD_ON
	CWrappedRegKey
#else
	CRegKey 
#endif
		keyCinema;	

	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return;
	}

	lRes = keyCinema.Open(HKEY_LOCAL_MACHINE, stKey);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return;
	}

	lRes = keyCinema.SetStringValue(TEXT("ArchivePath"), m_stBasePath);
	if(lRes != ERROR_SUCCESS)
		Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update ArchivePath parameter lRes=%ld"), lRes);
#if 0
	lRes = keyCinema.SetStringValue(TEXT("DBConnectionString"), m_stDBInitString);
	if(lRes != ERROR_SUCCESS)
		Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update DBConnectionString parameter lRes=%ld"), lRes);
#endif
}

bool	SystemSettingsBridgeImpl::LoadLocalDeviceSettings(std::vector<DeviceSettings >& param)
{
	LONG  lRes;
	DWORD dwIndex, dwValue;
	TCHAR stKeyName[MAX_PATH];

	// Stream settings
	UUID streamID;
	bool bBWMode;
	long lWidth, lHeight, lFrameRate, lInput, lVStandart;
	TCHAR stMoniker[MAX_PATH];

#ifdef EUGENE_ADD_ON
	CWrappedDevicesRegKey
#else
	CRegKey
#endif	
		keyDevices;



	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKeyName, countof(stKeyName)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}

	lstrcat(stKeyName, TEXT("\\Devices"));

	lRes = keyDevices.Open(HKEY_LOCAL_MACHINE, stKeyName);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	dwIndex = 0;

	while(RegEnumKey(keyDevices, dwIndex++, stKeyName, countof(stKeyName)) ==  ERROR_SUCCESS)
	{
		// Get camera UIDs
		//

		streamID = GUID_NULL;
		lstrcpy(stMoniker, TEXT(""));

		// Default settings
		//

		lWidth  = 0;
		lHeight = 0;
		lInput  = 0;
		lFrameRate = 0;
		lVStandart = 0;
		bBWMode = false;

		if(!StringToUuid(stKeyName, &streamID))
		{
			Elvees::OutputF(Elvees::TWarning, TEXT("Invalid stream UID = \"%s\""), stKeyName);
			continue;
		}

		// Read device parameters
		//

		do
		{
#ifdef EUGENE_ADD_ON
			CWrappedDeviceRegKey
#else
			CRegKey
#endif
				keyDevice;

			lRes = keyDevice.Open(keyDevices, stKeyName);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open registry key \"%s\""), stKeyName);
				break;
			}

			dwValue = countof(stMoniker);

			lRes = keyDevice.QueryStringValue(TEXT("Device_DisplayName"), stMoniker, &dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to read moniker for device \"%s\""), stKeyName);
				break;
			}

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Width"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default width for device \"%s\""), stKeyName);
			else
				lWidth = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Height"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default height for device \"%s\""), stKeyName);
			else
				lHeight = dwValue;
#if 0
			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Input"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default input for device \"%s\""), stKeyName);
			else
				lInput = dwValue;
#else
			lInput = 0;
#endif
			lRes = keyDevice.QueryDWORDValue(TEXT("Device_BWMode"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default BWMode for device \"%s\""), stKeyName);
			else
				bBWMode = (dwValue == 0) ? false : true;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Standard"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default VStandard for device \"%s\""), stKeyName);
			else
				lVStandart = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_FrameRate"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default FrameRate for device \"%s\""), stKeyName);
			else
				lFrameRate = dwValue;


			DeviceSettings CamParam;

			CamParam.streamID	= streamID;
			CamParam.stMoniker	= stMoniker;
			CamParam.lWidth		= lWidth;
			CamParam.lHeight	= lHeight;
			CamParam.lFrmRate	= lFrameRate;
			CamParam.lInput		= lInput;
			CamParam.lVStandard	= lVStandart;
			CamParam.bBWMode	= bBWMode;
			CamParam.stKeyName	= stKeyName;

			param.push_back( CamParam );
		}
		while(false);
	}
	return true;
}

bool SystemSettingsBridgeImpl::RegisterLocalDevice( const UUID& streamID, LPCTSTR stMoniker )
{
	UUID MutableStreamID = streamID;
	TCHAR stDevice[64];
	UuidString pszUuid = 0;

	if(::UuidToString(&MutableStreamID, &pszUuid) == RPC_S_OK && pszUuid)
	{
		wsprintf(stDevice, TEXT("{%s}"), pszUuid);
		RpcStringFree(&pszUuid);
	}
	else
	{
		Elvees::Output(Elvees::TError, TEXT("UUID conversion failed"));
		return false;
	}

	LONG  lRes;
	TCHAR stKeyName[MAX_PATH];


#ifdef EUGENE_ADD_ON
	CWrappedDevicesRegKey
#else
	CRegKey
#endif
		keyDevices;

	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKeyName, countof(stKeyName)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}

	lstrcat(stKeyName, TEXT("\\Devices"));

	lRes = keyDevices.Open(HKEY_LOCAL_MACHINE, stKeyName);
	if(lRes != ERROR_SUCCESS)
		lRes = keyDevices.Create(HKEY_LOCAL_MACHINE, stKeyName);

	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	do
	{
#ifdef EUGENE_ADD_ON
		CWrappedDeviceRegKey 
#else
		CRegKey
#endif
			keyDevice;

		lRes = keyDevice.Create(keyDevices, stDevice);
		if(lRes != ERROR_SUCCESS)
			break;

		keyDevice.SetDWORDValue(TEXT("Device_BWMode"), 1);
#if 0
		keyDevice.SetDWORDValue(TEXT("Device_Input"), 2);
#endif
		keyDevice.SetDWORDValue(TEXT("Device_Width"), 352);
		keyDevice.SetDWORDValue(TEXT("Device_Height"), 288);
		keyDevice.SetDWORDValue(TEXT("Device_Standard"), 16);
		keyDevice.SetDWORDValue(TEXT("Device_FrameRate"), 25);

		lRes = keyDevice.SetStringValue(TEXT("Device_DisplayName"), stMoniker);
		if(lRes != ERROR_SUCCESS)
			break;

		return true;
	}
	while(false);

	keyDevices.DeleteSubKey(stDevice);
	return false;
}

bool SystemSettingsBridgeImpl::LoadCameraSettings(	bool bShowDialogOnFailed, std::vector<CameraSettings>& param )
{
	//////////////////////////////////////////////////////////////////////////

	HRESULT hr;
#ifdef EUGENE_ADD_ON
	CWrapDataSource dbsource;
	CWrapSession    dbsession;
	CWrapAllCameraCommand	cmdCameras;
#else
	CDataSource dbsource;
	CSession    dbsession;
	CCommand<CAccessor<CAccAllCameras> > cmdCameras;
#endif	

	// Open and read database

	bool bDone = false;


	do
	{	
		// Open database

		TCHAR	stDBInitString[1024] = {0};
		size_t	DBInitStringSize = 1024;
		//size_t  
#ifdef _UNICODE
		hr = dbsource.OpenFromInitializationString(stDBInitString);
#else
		WCHAR szDBInitString[4096];

		MultiByteToWideChar(CP_ACP, 0, stDBInitString, lstrlen(stDBInitString) + 1,
			szDBInitString, countof(szDBInitString));

		hr = dbsource.OpenFromInitializationString(szDBInitString);
#endif
		if(FAILED(hr))
		{
			if(!bShowDialogOnFailed)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
				break;
			}

			hr = dbsource.Open();
			if(FAILED(hr))
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
				break;
			}

			BSTR bstrDBInit;
			hr = dbsource.GetInitializationString(&bstrDBInit);

			if(FAILED(hr))
			{
				Elvees::OutputF(Elvees::TError, TEXT("GetInitializationString failed hr=0x%08lX"), hr);
				break;
			}

#ifdef _UNICODE
			lstrcpy(m_stDBInitString, bstrDBInit);
#else
			WideCharToMultiByte(CP_ACP, 0, bstrDBInit, (int)wcslen(bstrDBInit) + 1,
				stDBInitString, DBInitStringSize, NULL, NULL);
#endif
			SysFreeString(bstrDBInit);

			//			m_bUpdateSettings = true;
		}

		// Open session

		hr = dbsession.Open(dbsource);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database session hr=0x%08lX"), hr);
			break;
		}

		// Save all streams to list

		hr = cmdCameras.Open(dbsession);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open table hr=0x%08lX"), hr);
			break;
		}

		hr = cmdCameras.MoveFirst();

		UUID camUID;
		unsigned long camIP;

		while(hr != DB_S_ENDOFROWSET)
		{
			do
			{
				// Check UID

				if(!StringToUuid(cmdCameras.m_stCameraUID, &camUID))
				{
					Elvees::OutputF(Elvees::TError, TEXT("Invalid stream UID = \"%s\""), cmdCameras.m_stCameraUID);
					break;
				}

				// Check IP

#ifndef _UNICODE
				camIP = inet_addr(cmdCameras.m_stCameraIP);
#else
				char stCameraIP[56];

				WideCharToMultiByte(CP_ACP, 0, cmdCameras.m_stCameraIP,
					(int)wcslen(cmdCameras.m_stCameraIP) + 1,
					stCameraIP, sizeof(stCameraIP), NULL, NULL);

				camIP = inet_addr(stCameraIP);
#endif

				if(INADDR_NONE == camIP)
				{
					Elvees::OutputF(Elvees::TError, TEXT("Invalid stream IP = \"%s\""), cmdCameras.m_stCameraIP);
					break;
				}

				CameraSettings CamParam;
				CamParam.m_lCameraID	= cmdCameras.m_lCameraID;
				CamParam.m_lDevicePin	= cmdCameras.m_lDevicePin;
				CamParam.m_stCameraUID	= cmdCameras.m_stCameraUID;
				CamParam.m_stCameraIP	= cmdCameras.m_stCameraIP;

				param.push_back( CamParam );
			}
			while(false);

			hr = cmdCameras.MoveNext();
		}

		bDone = true;
	}
	while(false);

	return bDone;
}

ULONG	SystemSettingsBridgeImpl::GetIPAddressOfStreamServer( LPCTSTR stDBInitString, long StreamID )
{
	HRESULT hr;
#ifdef EUGENE_ADD_ON
	CWrapDataSource dbsource;
	CWrapSession    dbsession;
	CWrapCameraInRoomCommand cmdCamera;
#else
	CDataSource dbsource;
	CSession    dbsession;
	CCommand<CAccessor<CAccCameraID> > cmdCamera;
#endif	

	// Open and read database
	ULONG sourceIP = INADDR_NONE;


	bool bDone = false;

	do
	{	
		// Open database

#ifdef _UNICODE
		hr = dbsource.OpenFromInitializationString(m_stDBInitString);
#else
		WCHAR szDBInitString[4096];

		MultiByteToWideChar(CP_ACP, 0, stDBInitString, lstrlen(stDBInitString) + 1,
			szDBInitString, countof(szDBInitString));

		hr = dbsource.OpenFromInitializationString(szDBInitString);
#endif
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
			break;
		}

		// Open session

		hr = dbsession.Open(dbsource);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database session hr=0x%08lX"), hr);
			break;
		}

		// Save all streams to list
		cmdCamera.m_lCameraID = StreamID;

		hr = cmdCamera.Open(dbsession);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open table hr=0x%08lX"), hr);
			break;
		}

		if(cmdCamera.MoveFirst() == S_OK)
		{
#ifndef _UNICODE
			sourceIP = inet_addr(cmdCamera.m_stCameraIP);
#else
			char stSourceIP[56];

			WideCharToMultiByte(CP_ACP, 0, cmdCamera.m_stCameraIP,
				(int)wcslen(cmdCamera.m_stCameraIP) + 1,
				stSourceIP, sizeof(stSourceIP), NULL, NULL);

			sourceIP = inet_addr(stSourceIP);
#endif

			if(INADDR_NONE == sourceIP)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Invalid stream IP = \"%s\""), cmdCamera.m_stCameraIP);
				break;
			}
		}
	}
	while(false);
	return sourceIP;
}

