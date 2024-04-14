// DlgConfigure.cpp: implementation of the CDlgConfigure class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "resource.h"

#include "DlgConfigure.h"
#include "DlgCompressor.h"

//#pragma TODO("Uncomment EUGENE_ADD_ON")
// 2006-09-18 at the disabled , wrappre is required
//#define EUGENE_ADD_ON

#include <shlobj.h>

CDlgConfigure::CDlgConfigure(
							 const CHCS::IVideoSystemConfig::CodecSettings& param, 
							 const std::tstring& ArchivePath
							 ):
	m_CodecSettings(param),
	m_ArchivePath(ArchivePath)
{
	ZeroMemory(&m_Codec, sizeof(COMPVARS));
	m_Codec.cbSize = sizeof(COMPVARS);
	m_Codec.fccType  = ICTYPE_VIDEO;
	m_Codec.dwFlags &= ~ICMF_COMPVARS_VALID;
	m_Codec.lQ = 10000;
#if 0
	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	lstrcpy(m_stBasePath, TEXT(""));
	lstrcpy(m_stDBInitString, TEXT(""));
#endif
	m_bUpdateBasePath = false;
	m_bUpdateDBString = false;
	m_bUpdateCodec = false;
}

CDlgConfigure::~CDlgConfigure()
{
	FreeCompressor();
}

LRESULT CDlgConfigure::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	// center the dialog on the screen
	CenterWindow(GetParent());

	if(LoadSettings())
		InitCompressor();

#if OLD
	SetDlgItemText(IDC_DATABASE,		m_stDBInitString);
#else
	SetDlgItemText(IDC_DATABASE,		_T("<This setting is not used at the moment>"));
#endif
	SetDlgItemText(IDC_BASEPATH,		m_ArchivePath.c_str() );

	SetDlgItemInt(IDC_VIDEO_FPM,		m_CodecSettings.m_lVideoFPM);
	SetDlgItemInt(IDC_VIDEO_MAXRATE,	m_CodecSettings.m_lVideoMaxSize);

	return TRUE;
};

LRESULT CDlgConfigure::OnSelectCodec(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDlgCompressor dlg;
	
	if(dlg.DoModal(m_hWnd, (LPARAM)&m_Codec) == IDOK)
	{
		UpdateCodecName();
		m_bUpdateCodec = true;
	}

	return 0;
}

void	LoadCodecSettings(  const CHCS::IVideoSystemConfig::CodecSettings& cs, COMPVARS m_Codec  )
{
	m_Codec.fccHandler	= cs.m_CodecFCC;
	m_Codec.lQ			= cs.m_lCodecQuality;
	m_Codec.lKey		= cs.m_lCodecKeyRate;
	m_Codec.lDataRate	= cs.m_lCodecDataRate;
	LONG m_lVideoFPM			= cs.m_lVideoFPM;
	LONG m_lVideoMaxSize		= cs.m_lVideoMaxSize;

	if(m_Codec.lpState)
		free(m_Codec.lpState);

	m_Codec.cbState = 0;
	m_Codec.lpState = NULL;

	if( !cs.m_CodecStateData.empty() )
	{
		m_Codec.lpState = malloc( cs.m_CodecStateData.size() );
		if(!m_Codec.lpState)
		{
			Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
		}

		m_Codec.cbState = static_cast<LONG>( cs.m_CodecStateData.size() );

		memcpy( m_Codec.lpState,  &cs.m_CodecStateData[0], cs.m_CodecStateData.size() );
	}
#ifdef _DEBUG
	BYTE bFCC[4];
	bFCC[0] = (BYTE)( m_Codec.fccHandler & 0x000000FF);
	bFCC[1] = (BYTE)((m_Codec.fccHandler & 0x0000FF00) >> 0x08);
	bFCC[2] = (BYTE)((m_Codec.fccHandler & 0x00FF0000) >> 0x10);
	bFCC[3] = (BYTE)((m_Codec.fccHandler & 0xFF000000) >> 0x18);

	if(!isprint(bFCC[0])) bFCC[0] = '?';
	if(!isprint(bFCC[1])) bFCC[1] = '?';
	if(!isprint(bFCC[2])) bFCC[2] = '?';
	if(!isprint(bFCC[3])) bFCC[3] = '?';

	Elvees::OutputF(Elvees::TTrace, TEXT("Configure: FMP=%ld MS=%ld FCC=0x%08lX (%c%c%c%c) Q=%ld KR=%ld DR=%ld SS=%ld"),
		m_lVideoFPM,
		m_lVideoMaxSize,
		m_Codec.fccHandler, bFCC[0], bFCC[1], bFCC[2], bFCC[3],
		m_Codec.lQ,
		m_Codec.lKey,
		m_Codec.lDataRate,
		m_Codec.cbState);
#endif
}

bool CDlgConfigure::LoadSettings( )
{
	// Set default settings
	//

	FreeCompressor();
	LoadCodecSettings( m_CodecSettings, m_Codec );
	return true;
}

void	SaveSettings( CHCS::IVideoSystemConfig::CodecSettings& cs, COMPVARS m_Codec )
{
	cs.m_CodecFCC		= m_Codec.fccHandler ;
	cs.m_lCodecQuality	= m_Codec.lQ;
	cs.m_lCodecKeyRate  = m_Codec.lKey;
	cs.m_lCodecDataRate = m_Codec.lDataRate;
//	cs.m_lVideoFPM		= m_lVideoFPM;
//	cs.m_lVideoMaxSize  = m_lVideoMaxSize;

	cs.m_CodecStateData.assign( static_cast<const BYTE*>(m_Codec.lpState),
								static_cast<const BYTE*>(m_Codec.lpState) + m_Codec.cbState 
								);
}

bool CDlgConfigure::SaveSettings()
{
	// Set default settings
	//

	Elvees::Output(Elvees::TInfo, TEXT("Saving params"));
	
	::SaveSettings( m_CodecSettings, m_Codec );
	return true;
}

void CDlgConfigure::InitCompressor()
{
	LRESULT	lRes;
	ICINFO info;
	ICCOMPRESSFRAMES icf;

	const LONG m_lVideoFPM = m_CodecSettings.m_lVideoFPM;

	do
	{
		if(m_Codec.fccHandler == 0)
		{
			Elvees::Output(Elvees::TError, TEXT("Codec not selected"));
			break;
		}

		m_Codec.hic = ICOpen(ICTYPE_VIDEO, m_Codec.fccHandler, ICMODE_COMPRESS);
		if(!m_Codec.hic)
		{
			Elvees::Output(Elvees::TError, TEXT("Cant open compressor"));
			break;
		}

		if(m_Codec.lpState && m_Codec.cbState > 0)
		{
			lRes = ICSetState(m_Codec.hic, m_Codec.lpState, m_Codec.cbState);
			if(lRes != ICERR_OK && lRes != m_Codec.cbState)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Cant set compressor state lRes=%ld"), lRes);
				break;
			}
		}

		// Retrieve compressor information.
		//

		ZeroMemory(&info, sizeof(info));
		info.dwSize = sizeof(info);

		lRes = ICGetInfo(m_Codec.hic, &info, sizeof(info));
		if(!lRes)
		{
			Elvees::Output(Elvees::TError, TEXT("Unable to get codec info"));
			break;
		}

		// Set quality
		if( !(info.dwFlags & VIDCF_QUALITY))
			m_Codec.lQ = 0;

		// Set DataRate
		ZeroMemory(&icf, sizeof(icf));

		icf.dwFlags		= (DWORD)(LONG_PTR)&icf.lKeyRate;
		icf.lStartFrame = 0;
		icf.lFrameCount = MAXLONG;
		icf.lQuality	= m_Codec.lQ;
		icf.lDataRate	= m_Codec.lDataRate;
		icf.lKeyRate	= m_Codec.lKey;
		icf.dwRate		= 1000000;
		icf.dwScale		= m_lVideoFPM > 0 ? 60000000/m_lVideoFPM : 60000000;

		ICSendMessage(m_Codec.hic, ICM_COMPRESS_FRAMES_INFO, (LPARAM)(LPVOID)&icf, sizeof(ICCOMPRESSFRAMES));
		
		info.szDescription[127] = 0;
		UpdateCodecName(info.szDescription);

		m_Codec.dwFlags = info.dwFlags | ICMF_COMPVARS_VALID;
	}
	while(false);

	if(!(m_Codec.dwFlags & ICMF_COMPVARS_VALID))
	{
		FreeCompressor();
		UpdateCodecName();
	}
}

void CDlgConfigure::FreeCompressor()
{
	if(m_Codec.hic)
	{
		ICClose(m_Codec.hic);
		m_Codec.hic = NULL;
	}

	if(m_Codec.lpState)
	{
		free(m_Codec.lpState);

		m_Codec.lpState = NULL;
		m_Codec.cbState = 0;
	}

	ZeroMemory(&m_Codec, sizeof(COMPVARS));
	m_Codec.cbSize = sizeof(COMPVARS);
	m_Codec.fccType = ICTYPE_VIDEO;
	m_Codec.dwFlags &= ~ICMF_COMPVARS_VALID;
	m_Codec.lQ = 10000;
}

void CDlgConfigure::UpdateCodecName(LPCWSTR szDriverName)
{
	ICINFO info;
	TCHAR stCodecName[128];

	if(szDriverName)
	{
		wsprintf(stCodecName, TEXT("%lS"), szDriverName);
	}
	else
	{
		lstrcpy(stCodecName, TEXT("Not selected"));

		if(m_Codec.dwFlags & ICMF_COMPVARS_VALID)
		{
			if(ICGetInfo(m_Codec.hic, &info, sizeof(info)))
			{
				info.szDescription[127] = 0;
				wsprintf(stCodecName, TEXT("%lS"), info.szDescription);
			}
		}
	}

	if(lstrlen(stCodecName) == 0)
		lstrcpy(stCodecName, TEXT("Unknown"));

	SetDlgItemText(IDC_DRIVER_NAME, stCodecName);
}

LRESULT CDlgConfigure::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CDlgConfigure::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LONG m_lVideoFPM = GetDlgItemInt(IDC_VIDEO_FPM, NULL, FALSE);

	if(m_lVideoFPM < 10 || m_lVideoFPM > 60*30)
	{
		MessageBox(TEXT("Set valid frame rate"), TEXT("Invalid frame rate"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_FPM));
		return 0;
	}

	LONG m_lVideoMaxSize = GetDlgItemInt(IDC_VIDEO_MAXRATE, NULL, FALSE);

	if( m_lVideoMaxSize < 10)
	{
		MessageBox(TEXT("Set valid video part size"), TEXT("Invalid video size"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_MAXRATE));
		return 0;
	}

	if(!(m_Codec.dwFlags & ICMF_COMPVARS_VALID) || !m_Codec.hic)
	{
		MessageBox(TEXT("Select video compressor"), TEXT("No codec selected"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_MAXRATE));
		return 0;
	}

	// All params properly setted, save to registry
	//

	if(!SaveSettings())
	{
		MessageBox(TEXT("Fail to save settings"), TEXT("Save settings"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	EndDialog(wID);

	m_CodecSettings.m_lVideoFPM		= m_lVideoFPM;
	m_CodecSettings.m_lVideoMaxSize = m_lVideoMaxSize;
	return 0;
}

#if 0

LRESULT CDlgConfigure::OnBrowseDatabase(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr, hrOpen;
	CDataSource dbsource;

	hrOpen = dbsource.Open();
	if(FAILED(hrOpen))
	{
		if(hrOpen != DB_E_CANCELED)
		{
			TCHAR stMessage[1024];
			int   nMessageLen = 0;

			ULONG i, cRecords;
			CComBSTR bstrDesc;
			CDBErrorInfo ErrorInfo;
			
			hr = ErrorInfo.GetErrorRecords(&cRecords);
			if(FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
			{
				wsprintf(stMessage,	TEXT("Unknown error hr=0x%x"), hrOpen);
			}
			else
			{
				LCID lcLocale = GetSystemDefaultLCID();

				for(i = 0; i < cRecords; i++)
				{
					hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc);
					
					if(FAILED(hr))
					{
						wsprintf(&stMessage[nMessageLen], 
							TEXT("OLE DB Error Record dump retrieval failed hr=0x%x"), hr);
						break;
					}

					wsprintf(&stMessage[nMessageLen], TEXT("%lS\n"), bstrDesc);
					nMessageLen = lstrlen(stMessage);

					bstrDesc.Empty();
				}
			}
		
			MessageBox(stMessage, TEXT("Select database"), MB_OK | MB_ICONSTOP);
		}

		return 0;
	}

	BSTR bstrDBInit;
	hr = dbsource.GetInitializationString(&bstrDBInit, true);

	if(FAILED(hr))
	{
		MessageBox(TEXT("GetInitializationString failed"),
			TEXT("Select database"), MB_OK | MB_ICONSTOP);
		return 0;
	}

#ifdef _UNICODE
	lstrcpy(m_stDBInitString, bstrDBInit);
#else
	WideCharToMultiByte(CP_ACP, 0, bstrDBInit, (int)wcslen(bstrDBInit) + 1,
		m_stDBInitString, sizeof(m_stDBInitString), NULL, NULL);
#endif
	SysFreeString(bstrDBInit);

	SetDlgItemText(IDC_DATABASE, m_stDBInitString);
	m_bUpdateDBString = true;

	return 0;
}

#endif

LRESULT CDlgConfigure::OnBrowseBasepath(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr;
	ITEMIDLIST *pItemList;
	BROWSEINFO browseinfo;
	TCHAR stPath[MAX_PATH];

	IMalloc *pMalloc = NULL;
	hr = SHGetMalloc(&pMalloc);
	if(FAILED(hr))
	{
		MessageBox(TEXT("Can't retrieve system's IMalloc interface"),
			TEXT("Browse basepath"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	ZeroMemory(&browseinfo, sizeof(BROWSEINFO));
	
	lstrcpy(stPath, m_ArchivePath.c_str() );

	browseinfo.hwndOwner = m_hWnd;
	browseinfo.pszDisplayName = stPath;
	browseinfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	browseinfo.lpszTitle = TEXT("Select BasePath");

	pItemList = ::SHBrowseForFolder(&browseinfo);
	if(pItemList)
	{
		TCHAR PathBuffer[ MAX_PATH ];
		::SHGetPathFromIDList(pItemList, PathBuffer);
		pMalloc->Free(pItemList);

		SetDlgItemText(IDC_BASEPATH, PathBuffer);
		m_bUpdateBasePath = true;
		m_ArchivePath = PathBuffer;
	}

	pMalloc->Release();
	pMalloc = NULL;

	return 0;
}

void	InitCodecState( CHCS::IVideoSystemConfig::CodecSettings& param )
{
	param.m_lVideoFPM		= 300L;
	param.m_lVideoMaxSize	= 300L;
	param.m_CodecFCC		= mmioFOURCC( 'x', 'v', 'i', 'd' );
	param.m_lCodecDataRate	= 0;
	param.m_lCodecKeyRate	= 0;
	param.m_lCodecQuality	= 10000;
	param.m_CodecStateData.clear();
}

void	SaveCodecState( const CHCS::IVideoSystemConfig::CodecSettings& param )
{
	if( param.m_CodecStateData.empty() )
	{
		return;
	}

	std::ofstream CodecDump;
	CodecDump.open( "CodecState.bin", std::ios::ate | std::ios::binary ) ;
	CodecDump.write( 
		reinterpret_cast<const char*>(&param.m_CodecStateData[0]), 
		static_cast<std::streamsize>( param.m_CodecStateData.size() ) 
		);
}