// VidDecoder.cpp: implementation of the CVidDecoder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VidDecoder.h"

#define MAX_FORMAT_SIZE		65536		// if junk format provided
#define DRIVER_ID			(HDRVR)0

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)

//////////////////////////////////////////////////////////////////////
// CVidDecoder
//////////////////////////////////////////////////////////////////////

CVidDecoder::CVidDecoder()
{
	m_hic = NULL;
	m_dwDriver = 0;
	m_hDriver = NULL;

	m_pbiInput   = NULL;
	m_pbiOutput  = NULL;

	m_lpOutBuffer = NULL;
	m_cbOutBuffer = 0;

	m_bInited  = false;
	m_bStarted = false;

	m_bWaitIFrame = true;

	m_uFrameSeq  = 0;
	m_uIFrameSeq = 0;
}

CVidDecoder::~CVidDecoder()
{
	FreeDecompressor();
}

bool CVidDecoder::InitDecompressor(const BITMAPINFO *pbiInput, const BITMAPINFO *pbiOutput, const funcDriverProc fDriverProc)
{
	if(!pbiInput || !pbiOutput)
		return false;

	if(m_bInited)
		FreeDecompressor();
	
	int cbSize;
	LRESULT lRes;

	do
	{
		if( pbiInput->bmiHeader.biSize < sizeof(BITMAPINFOHEADER) ||
			pbiOutput->bmiHeader.biSize < sizeof(BITMAPINFOHEADER))
		{
			Elvees::Output(Elvees::TError, TEXT("CVidDecoder::InitDecompressor() Unsupported format size"));
			break;
		}

		// Alloc output format
		cbSize = min(MAX_FORMAT_SIZE, pbiOutput->bmiHeader.biSize + pbiOutput->bmiHeader.biClrUsed*4);
		m_pbiOutput = (LPBITMAPINFO)calloc( max(cbSize, sizeof(BITMAPINFO)), 1);

		if(!m_pbiOutput)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Memory operation failed m_pbiOutput(%ld)"), cbSize);
			break;
		}

		// correct format structure up to application...
		CopyMemory(m_pbiOutput, pbiOutput, cbSize);

		// Alloc input format
		cbSize = min(MAX_FORMAT_SIZE, pbiInput->bmiHeader.biSize + pbiInput->bmiHeader.biClrUsed*4);
		m_pbiInput = (LPBITMAPINFO)calloc( max(cbSize, sizeof(BITMAPINFO)), 1 );

		if(!m_pbiInput)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Memory operation failed m_pbiInput(%ld)"), cbSize);
			break;
		}

		// ... HuffYUY codec store its settings in pbiInput
		CopyMemory(m_pbiInput, pbiInput, cbSize);

		// one extra line for swap operations
		m_cbOutBuffer = DIBWIDTHBYTES(m_pbiOutput->bmiHeader)*(labs(m_pbiOutput->bmiHeader.biHeight)+1);

		m_lpOutBuffer = (char*)malloc(m_cbOutBuffer);
		if(!m_lpOutBuffer)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Memory operation failed m_cbOutBuffer(%ld)"), m_cbOutBuffer);
			break;
		}

		// Initial zero buffer
		ZeroMemory(m_lpOutBuffer, m_cbOutBuffer);

		//////////////////////////////////////////////////////////////////////
		// Initialize decoder

		ZeroMemory(&m_icOpen, sizeof(m_icOpen));
		m_icOpen.dwSize = sizeof(m_icOpen);
		m_icOpen.fccType = ICTYPE_VIDEO;
		m_icOpen.fccHandler = m_pbiInput->bmiHeader.biCompression;
		m_icOpen.dwFlags = ICMODE_DECOMPRESS;

		if(fDriverProc)
		{
			// Testing function...

			__try
			{
				m_dwDriver = (DWORD)fDriverProc(0, DRIVER_ID, DRV_OPEN, 0, (LPARAM)&m_icOpen);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				Elvees::Output(Elvees::TError, TEXT("fDriverProc(DRV_OPEN) cause an unexpected exception"));
				break;
			}

			if(!m_dwDriver)
				break;

			lRes = ~ICERR_OK;

			__try
			{
				lRes = fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_QUERY,
					(LPARAM)(LONG_PTR)(m_pbiInput), (LPARAM)(LONG_PTR)m_pbiOutput);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				Elvees::Output(Elvees::TError, TEXT("fDriverProc(ICM_DECOMPRESS_QUERY) cause an unexpected exception"));
			}

			if(ICERR_OK == lRes)
			{
				lRes = ~ICERR_OK;

				__try
				{
					lRes = fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_BEGIN,
						(LPARAM)(LONG_PTR)(m_pbiInput), (LPARAM)(LONG_PTR)m_pbiOutput);

					if(ICERR_OK == lRes)
						fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_END, 0, 0);
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					lRes = ~ICERR_OK;
					Elvees::Output(Elvees::TError, TEXT("fDriverProc(ICM_DECOMPRESS_BEGIN/END) cause an unexpected exception"));
				}
			}

			if(ICERR_OK != lRes)
			{
				Elvees::Output(Elvees::TError, TEXT("Provided function fDriverProc can't decompress this data"));

				__try
				{
					fDriverProc(m_dwDriver, DRIVER_ID, DRV_CLOSE, 0, (LPARAM)&m_icOpen);
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					Elvees::Output(Elvees::TError, TEXT("fDriverProc(DRV_CLOSE) cause an unexpected exception"));
				}
				break;
			}
            			
			m_fDriverProc = fDriverProc;
		}
		else
		{
			__try
			{
				m_hic = ICOpen(ICTYPE_VIDEO, m_pbiInput->bmiHeader.biCompression, ICMODE_DECOMPRESS);

				if(!m_hic || ICERR_OK != ICDecompressQuery(m_hic, m_pbiInput, m_pbiOutput))
				{
					if(m_hic)
						ICClose(m_hic);

					m_hic = ICLocate(ICTYPE_VIDEO, NULL,
						&m_pbiInput->bmiHeader,	&m_pbiOutput->bmiHeader, ICMODE_DECOMPRESS);
				}

				if(!m_hic)
					break;
				
				if(ICERR_OK != ICDecompressQuery(m_hic, m_pbiInput, m_pbiOutput))
				{
					ICClose(m_hic);
					break;
				}

				if(ICERR_OK != ICDecompressBegin(m_hic, m_pbiInput, m_pbiOutput))
				{
					ICClose(m_hic);
					break;
				}

				ICDecompressEnd(m_hic);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				Elvees::Output(Elvees::TError, TEXT("An unexpected exception when open/locate decompressor"));
				break;
			}

			//////////////////////////////////////////////////////////////////////
			// Try to directly access to decoder.

			ICINFO info;

			ZeroMemory(&info, sizeof(info));
			info.dwSize = sizeof(info);

			ICGetInfo(m_hic, &info, sizeof(info));

			m_dwDriver = 0;
			m_hDriver = LoadLibraryW(info.szDriver);
			m_fDriverProc = m_hDriver ? (funcDriverProc)GetProcAddress(m_hDriver, "DriverProc") : NULL;

			if(m_fDriverProc)
			{
				bool bValid = false;

				__try
				{
					m_dwDriver = (DWORD)m_fDriverProc(0, DRIVER_ID, DRV_OPEN, 0, (LPARAM)&m_icOpen);

					if(m_dwDriver)
					{
						lRes = m_fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_QUERY,
							(LPARAM)(LONG_PTR)(m_pbiInput), (LPARAM)(LONG_PTR)m_pbiOutput);

						if(ICERR_OK == lRes)
						{
							lRes = m_fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_BEGIN,
								(LPARAM)(LONG_PTR)(m_pbiInput), (LPARAM)(LONG_PTR)m_pbiOutput);

							if(ICERR_OK == lRes)
							{
								m_fDriverProc(m_dwDriver, DRIVER_ID, ICM_DECOMPRESS_END, 0, 0);
								bValid = true;
							}
						}

						if(!bValid)
						{
							m_fDriverProc(m_dwDriver, DRIVER_ID, DRV_CLOSE, 0, (LPARAM)&m_icOpen);
							m_dwDriver = NULL;
						}
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					m_dwDriver = NULL;
				}
			}
			
			if(m_dwDriver)
			{
				ICClose(m_hic);
				m_hic = NULL;
			}
			else if(m_hDriver)
			{
				FreeLibrary(m_hDriver);
				m_hDriver = NULL;
			}
		}

		if(!m_hic && !m_dwDriver)
		{
			Elvees::Output(Elvees::TError, TEXT("Can't open/locate decompressor"));
			break;
		}

		m_bInited = true;
	}
	while(false);

	if(!m_bInited)
		FreeDecompressor();

	return m_bInited;
}

void CVidDecoder::FreeDecompressor()
{
	if(m_bStarted)
		StopDecompression();

	if(m_pbiInput)
		free(m_pbiInput);

	if(m_pbiOutput)
		free(m_pbiOutput);

	if(m_lpOutBuffer)
		free(m_lpOutBuffer);

	if(m_dwDriver)
		DriverProc(DRV_CLOSE, 0, (LPARAM)&m_icOpen);

	if(m_hDriver)
		FreeLibrary(m_hDriver);
	
	if(m_hic)
		ICClose(m_hic);

	m_hic = NULL;
	m_dwDriver = 0;
	m_hDriver = NULL;
	m_fDriverProc = NULL;

	m_pbiInput   = NULL;
	m_pbiOutput  = NULL;
	m_lpOutBuffer = NULL;

	m_bInited  = false;
}

bool CVidDecoder::IsFormatChanged(const BITMAPINFO *pbiFormat, long /*cbFormat*/)
{
	if(!m_bInited)
		return true;

	if( m_pbiInput->bmiHeader.biCompression != pbiFormat->bmiHeader.biCompression)
		return true;

	if( m_pbiInput->bmiHeader.biWidth != pbiFormat->bmiHeader.biWidth ||
		m_pbiInput->bmiHeader.biHeight != pbiFormat->bmiHeader.biHeight)
		return true;

	return false;
}

bool CVidDecoder::StartDecompression()
{
	if(!m_bInited)
		return false;

	if(m_bStarted)
		StopDecompression();

	if(ICERR_OK != DriverProc(ICM_DECOMPRESS_BEGIN, (LPARAM)m_pbiInput, (LPARAM)m_pbiOutput))
		return false;

	m_bStarted = true;
	return true;
}

void CVidDecoder::StopDecompression()
{
	if(m_bStarted)
		DriverProc(ICM_DECOMPRESS_END, 0, 0);

	m_bStarted = false;
}

void* CVidDecoder::DecompressData(const void* pData, long lSize, unsigned uFrameSeq)
{
	// Check frames sequence
	bool bKeyframe = !(uFrameSeq & 0x0000FFFF);

	if(bKeyframe)
	{
		m_bWaitIFrame = false;
		m_uFrameSeq  = 0;
		m_uIFrameSeq = HIWORD(uFrameSeq);
	}
	else if(!m_bWaitIFrame)
	{
		if(HIWORD(uFrameSeq) == m_uIFrameSeq)
		{
			if(++m_uFrameSeq == 0)
				m_uFrameSeq = 1;

			if(LOWORD(uFrameSeq) != m_uFrameSeq)
			{
				m_bWaitIFrame = true;  // Lost frame
			}
		}
		else
		{
			m_bWaitIFrame = true;      // Lost I-Frame
		}
	}
	
	return m_bWaitIFrame ? NULL : DecompressData(pData, lSize, bKeyframe);
}

void* CVidDecoder::DecompressData(const void* pData, long lSize, bool bKeyframe)
{
	if(!m_bStarted && !StartDecompression())
		return NULL;

	ICDECOMPRESS icDecompress;
	icDecompress.ckid = 0;
	icDecompress.dwFlags = bKeyframe ? 0 : ICDECOMPRESS_NOTKEYFRAME;
	icDecompress.lpbiInput  = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput);
	icDecompress.lpbiOutput = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput);
	icDecompress.lpInput  = const_cast<void*>(pData);
	icDecompress.lpOutput = m_lpOutBuffer;
	m_pbiInput->bmiHeader.biSizeImage = lSize;

	if(ICERR_OK != DriverProc(ICM_DECOMPRESS, (LPARAM)&icDecompress, sizeof(icDecompress)))
	{
		m_bWaitIFrame = true; // Wait for I-Frame if decompression failed
		return NULL;
	}

	return m_lpOutBuffer;
}

long CVidDecoder::GetInFormat(BITMAPINFO* pbi)
{
	if(m_pbiInput == NULL)
		return -1;

	int cbSize = m_pbiInput->bmiHeader.biSize + \
		m_pbiInput->bmiHeader.biClrUsed*4;

	if(pbi == NULL)
		return cbSize;

	__try
	{
		CopyMemory(pbi, m_pbiInput, cbSize);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

long CVidDecoder::GetOutFormat(BITMAPINFO* pbi)
{
	if(m_pbiOutput == NULL)
		return -1;

	int cbSize = m_pbiOutput->bmiHeader.biSize + \
		m_pbiOutput->bmiHeader.biClrUsed*4;

	if(pbi == NULL)
		return cbSize;

	__try
	{
		CopyMemory(pbi, m_pbiOutput, cbSize);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

long CVidDecoder::GetBufferSize() const
{
	return m_cbOutBuffer;
}

long CVidDecoder::GetDecompressedPitch()
{
	if(!m_pbiOutput)
		return 0;

	return DIBWIDTHBYTES(m_pbiOutput->bmiHeader);
}

//////////////////////////////////////////////////////////////////////
// DriverProc - spy on codec and protect from exceptions
// usefully for debug
//////////////////////////////////////////////////////////////////////

LRESULT CVidDecoder::DriverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = ICERR_ERROR;

	if(m_dwDriver)
	{
		__try
		{
			lRes = m_fDriverProc(m_dwDriver, DRIVER_ID, uMsg, lParam1, lParam2);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	else
	{
		__try
		{
			lRes = ICSendMessage(m_hic, uMsg, lParam1, lParam2);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	if(uMsg != ICM_DECOMPRESS)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Decoder(0x%X,?,%s,%u,%u)=0x%X"),
			m_dwDriver, GetMessageDescription(uMsg), lParam1, lParam2, lRes));
	}
	else if(lRes == ICERR_ERROR)
	{
		Elvees::OutputF(Elvees::TWarning, TEXT("Restarting decoder [%08X] after failure on frame [F %d, I %d]"),
			this, m_uFrameSeq, m_uIFrameSeq);

		StopDecompression();
		StartDecompression();
	}

	return lRes;
}

LPCTSTR CVidDecoder::GetMessageDescription(UINT uMsg)
{
	struct MessageNames
	{
		UINT	uMsg;
		LPCTSTR	stName;
	};

	MessageNames MessageMap[] =
	{
		{DRV_OPEN, TEXT("DRV_OPEN")},
		{DRV_CLOSE, TEXT("DRV_CLOSE")},
		{ICM_DECOMPRESS_BEGIN, TEXT("ICM_DECOMPRESS_BEGIN")},
		{ICM_DECOMPRESS_END, TEXT("ICM_DECOMPRESS_END")},
		{ICM_DECOMPRESS, TEXT("ICM_DECOMPRESS")}
	};

	for(int i = 0; i < countof(MessageMap); i++)
		if(uMsg == MessageMap[i].uMsg)
			return MessageMap[i].stName;

	return TEXT("Unknown");
}
