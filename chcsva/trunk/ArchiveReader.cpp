// ArchiveReader.cpp: implementation of the CArchiveReader class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
//#include "ArchiveServer.h"

#include <initguid.h>
#include "ArchiveReader.h"


#include "ext/archive/ArchiveDataExchange.h"
#include "ext/videofile/VideoFileInterface.h"

//#define DO_TIMEINGS

extern DWORD g_fccStreamDataType	;
extern DWORD g_fccStreamDataHandler ;

//////////////////////////////////////////////////////////////////////
// CArchiveReader
//////////////////////////////////////////////////////////////////////

CArchiveReader::CArchiveReader(IArchiveDataResponse* pResponseInterface,
								const std::tstring& sVideoBasePath)
	: m_pSocket(pResponseInterface)
	, m_lSourceID(-1)
	, m_sVideoBasePath(sVideoBasePath)
{
#if OLD
	m_pFormatPacket = NULL;
#endif
	m_timePos = 0;

	// avi file data
	m_cbCompressed = 0;
	m_pCompressed = NULL;
#if 1//OLD
	m_pVideoStream = NULL;
	ZeroMemory(&m_avisInfo, sizeof(m_avisInfo));
#endif
}

CArchiveReader::~CArchiveReader()
{
#if OLD
	if(m_pFormatPacket)
		m_pFormatPacket->Release();
#endif
	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
		TEXT("A[%ld] reader destroyed"), m_pSocket->GetSessionID()));

}
#if 0
double CArchiveReader::GetFPS() const
{

	return  m_pVideoFileReader.get() ?
		m_pVideoFileReader->GetFileInfo().m_fFPS :
	0;
}
#endif
void CArchiveReader::Shutdown()
{
	m_shutdownEvent.Set();

	if(IsStarted())
		Wait();
}

void CArchiveReader::ProcessCommand(LPCSTR stCommand, size_t nSize)
{
	if(strncmp(stCommand, "init", 4) == 0)
	{
		long lSourceID = 0;

		if(sscanf(stCommand, "init %ld", &lSourceID) == 1)
		{
			m_lSourceID = lSourceID;

			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] init id=%ld"),
				m_pSocket->GetSessionID(), m_lSourceID));
		}
	}
	else if(strncmp(stCommand, "quit", 4) == 0)
	{
		m_pSocket->Shutdown();
		return;
	}

	if(m_lSourceID < 0)
		return;

	//////////////////////////////////////////////////////////////////////////

	INT64 pos = 0;

	if(strncmp(stCommand, "seek", 4) == 0)
	{
		if(sscanf(stCommand, "seek %I64d", &pos) == 1 && pos < 0xF4862CCF)
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] seek %I64d = %s"),
				m_pSocket->GetSessionID(), pos, _tctime64(&pos)));

			if(!IsStarted())
				Start();

			m_timePos = pos;
			m_timeEvent.Set();
		}
		else
		{
			Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] seek notime ()"),
				m_pSocket->GetSessionID());
		}
	}
	else if(strncmp(stCommand, "getformat", 9) == 0)
	{
#if OLD
		bool bSended = false;
		Elvees::CIOBuffer* pFormat = m_pFormatPacket ? m_pFormatPacket->CreateCopy() : NULL;

		if(pFormat)
		{
			bSended = m_pSocket->Write(pFormat);
			pFormat->Release();
		}
#else
		bool bSended = SendFormat();
#endif

		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] getformat %s"),
			m_pSocket->GetSessionID(),
			bSended ? TEXT("sended") : TEXT("failed")));
	}
	else if(strncmp(stCommand, "pause", 5) == 0)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
			TEXT("A[%ld] pause"), m_pSocket->GetSessionID()));
	}
	else if(strncmp(stCommand, "resume", 6) == 0)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
			TEXT("A[%ld] resume"), m_pSocket->GetSessionID()));

		m_timeEvent.Set();
	}
}

//////////////////////////////////////////////////////////////////////////
// Main loop
//////////////////////////////////////////////////////////////////////////

int CArchiveReader::Run()
{
	LONG  shift;
	INT64 filetime;
	
	long lFrame, lSize;
	bool bKey, bSendIFrame;

	unsigned short m_uFrameSeq = 0;		// Frame counter
	unsigned short m_uIFrameSeq = 0;	// I-Frame counter

	HRESULT hr;
	TCHAR stCurFile[MAX_PATH];

	DWORD dwTimeout;
//	Elvees::CIOBuffer* pData;

	LARGE_INTEGER frmtime;

	CoInitialize(NULL);

	try
	{
		HANDLE handlesToWaitFor[2];

		handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
		handlesToWaitFor[1] = m_timeEvent.GetEvent();

		while(!m_shutdownEvent.Wait(0))
		{
			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

			if(waitResult == WAIT_OBJECT_0)
			{
				// Time to shutdown
				break;
			}
			else if(waitResult == WAIT_OBJECT_0 + 1)
			{
				// New time defined

				shift = 0;
				filetime = m_timePos;

				m_timeEvent.Reset();

				while(!m_shutdownEvent.Wait(0) && 
					FindFileByTime(&filetime, stCurFile, countof(stCurFile), &shift))
				{
					if(!OpenFile(stCurFile))
						break;

					if( !SendFormat() )
					{
						break;
					}
					bSendIFrame = true;

					lFrame = MulDiv(shift, m_avisInfo.dwRate, m_avisInfo.dwScale);
					lFrame = max(lFrame, (long)m_avisInfo.dwStart);

					dwTimeout = MulDiv(1000, m_avisInfo.dwScale, m_avisInfo.dwRate);

					while(!m_shutdownEvent.Wait(dwTimeout) && !m_timeEvent.Wait(0) &&
						m_pSocket->IsConnected(SD_SEND))
					{
						hr = m_pVideoStream->Read(
							lFrame,
							1,
							m_pCompressed,
							m_cbCompressed,
							&lSize,
							NULL);

						if(SUCCEEDED(hr) && lSize > 0)
						{
							bKey = m_pVideoStream->FindSample(lFrame, FIND_PREV | FIND_KEY) == lFrame;

							if(bKey)
							{
								m_uIFrameSeq++;
								m_uFrameSeq = 0;
							}
							else if(++m_uFrameSeq == 0)
							{
								m_uFrameSeq = 1;
							}

							if((bSendIFrame && bKey) || !bSendIFrame)
							{
								frmtime.QuadPart = filetime + MulDiv(lFrame, m_avisInfo.dwScale, m_avisInfo.dwRate);

								// debug assertion when a vector size is 0
								void* pUserBuffer = NULL;
								size_t UserDataSize = 0;
								if( m_pDataStream )
								{
									/*
									����� ����������� �������� ������ ����� 1�� �����
									*/
									LONG cbRead;

									pUserBuffer	= m_FrameUserDataBuffer.empty()? 0 : &m_FrameUserDataBuffer[0];
									const long CorrespondentDataFrame = lFrame/* - 1*/;
									const long FrameUserDataBufferAsLong = static_cast<long>(m_FrameUserDataBuffer.size());
									hr = m_pDataStream->Read( 
										CorrespondentDataFrame, 1, 
										pUserBuffer, FrameUserDataBufferAsLong, 
										&cbRead, 0 
										);
									if( S_OK == hr && cbRead )
									{
										UserDataSize = cbRead;
									}
									else
									{
										pUserBuffer		= NULL;
										UserDataSize	= 0;
									}	
								}
								bSendIFrame = !m_pSocket->SendFrameData
									( 
										m_pCompressed, lSize,
										pUserBuffer, UserDataSize,
										MAKELONG(m_uFrameSeq, m_uIFrameSeq), frmtime.LowPart
									 );
							}
						}

						if(bSendIFrame)
						{
							lFrame = m_pVideoStream->FindSample(lFrame, FIND_NEXT | FIND_KEY);
						}
						else
							lFrame++;

						if((long)m_avisInfo.dwLength - lFrame < 0)
						{
							filetime += MulDiv(m_avisInfo.dwLength, m_avisInfo.dwScale, m_avisInfo.dwRate) + 1;
							break;
						}

					} // end of while

					// Seek to new position (if needed)
					if(m_timeEvent.Wait(0))
					{
						shift = 0;
						filetime = m_timePos;

						m_timeEvent.Reset();
					}

					CloseFile();
				}
#if OLD
				// Send last - 0 size frame
				Elvees::CIOBuffer* pData = m_pServer->AllocateFrame();

				if(pData)
				{
					NetPacketHeader   header;
					NetStreamDataInfo datainfo;

					header.uSynchro = CHCS_NET_SYNCHRO;
					header.uPacket  = NET_PACKET_STREAMDATA;
					header.uTime    = 0;
					header.uLength  = sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo);
					header.uCheckSum = header.uPacket + header.uTime + header.uLength;

					datainfo.uStreamUID = GUID_NULL;
					datainfo.uFrameSize = 0;
					datainfo.uFrameSeq  = MAKELONG(m_uFrameSeq, m_uIFrameSeq);

					pData->AddData(&header, sizeof(header));
					pData->AddData(&datainfo, sizeof(datainfo));

					m_pSocket->Write(pData);
					pData->Release();
				}
#else
				m_pSocket->SendEndOfFormat( );
#endif
			}
			else
			{
				Elvees::OutputF(Elvees::TWarning, TEXT("CArchiveReader::Run() - WaitForMultipleObjects res=%d"), ::GetLastError());
			}


		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TCritical, TEXT("CArchiveReader::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TCritical, TEXT("CArchiveReader::Run() - Unexpected exception"));
	}

	CoUninitialize();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////

long CArchiveReader::GetFileLength(LPCTSTR stFile)
{
#if 1
	// Read from file
	BOOL bRet;
	HANDLE hFile;
	
	DWORD buf[0x24];
	DWORD dwReaded;

	hFile = CreateFile(stFile,
		FILE_READ_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;

	bRet = ReadFile(hFile, &buf, 0x90, &dwReaded, NULL);
	CloseHandle(hFile);

	if(!bRet || dwReaded != 0x90)
		return 0;

	if (buf[0x00] == MAKEFOURCC('R','I','F','F') &&
		buf[0x02] == MAKEFOURCC('A','V','I',' '))
	{
		return buf[0x21] ? MulDiv(buf[0x23], buf[0x20], buf[0x21]) : 0L;
	}
	
	return 0; // bad file
#else
	HRESULT    hr;
	PAVIFILE   pAVIFile = NULL;
	PAVISTREAM pAVIStream = NULL;
	AVISTREAMINFOW avisInfo;

	bool bDone = false;

	do
	{
		// use default handler, prevent change
		hr = AVIFileOpen(&pAVIFile, 
			stFile,		// file name
			OF_READ,	// mode to open file with
			const_cast<LPCLSID>(&defaultAviHandler));

		if(hr != AVIERR_OK || !pAVIFile)
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TError,
				TEXT("Fail to open file \"%s\" hr=0x%lX"), stFile, hr));
			break;
		}

		hr = pAVIFile->GetStream(&pAVIStream, streamtypeVIDEO, 0L);
		if(hr != AVIERR_OK || !pAVIStream)
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TError,
				TEXT("Fail get video stream hr=0x%lX"), hr));
			break;
		}

		hr = pAVIStream->Info(&avisInfo, sizeof(avisInfo));
		if(hr != AVIERR_OK)
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TError,
				TEXT("Fail get video stream info hr=0x%lX"), hr));
			break;
		}
		

		bDone = true;
	}
	while(0);

	if(pAVIFile)
		pAVIFile->Release();

	if(pAVIStream)
		pAVIStream->Release();

	return bDone ? MulDiv(avisInfo.dwLength, avisInfo.dwScale, avisInfo.dwRate) : 0L;
#endif
}

bool CArchiveReader::FindFileByTime(INT64* pTime, LPTSTR stFile, long cbFile, long* plShift)
{
	bool bFound, bFindNext, bSameDate;

	HANDLE hFindFile;
	HANDLE hFindFolder;
	WIN32_FIND_DATA FindFileData;
	WIN32_FIND_DATA FindFolderData;

	INT64 lFileTime;
	long  lShift;

	struct tm *ptmVideo;
	struct tm tmFile, tmFileLast;

	int   cbPath;
	TCHAR stPath[MAX_PATH];

	//////////////////////////////////////////////////////////////////////////

	if(!pTime)
		return false;

#ifdef DO_TIMEINGS
	Elvees::CTimer timer;
	timer.Start();
#endif

	// Format time

	if( (ptmVideo = _localtime64(pTime)) == NULL)
	{
		Elvees::Output(Elvees::TError, TEXT("Time conversion failed"));
		return false;
	}

	// Check base path

	cbPath = wsprintf(stPath, TEXT("%s\\%ld"),
			GetVideoBasePath(), m_lSourceID);
	
	if(!Elvees::IsFileExists(stPath))
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Invalid basepath \"%s\""), stPath));
		return false;
	}

	// Looking for such a day in archive...

	bFound = false;
	bFindNext = true;

	tmFile.tm_isdst = -1;
	tmFileLast.tm_year = -1;
	tmFileLast.tm_isdst = -1;

	lstrcat(&stPath[cbPath], TEXT("\\*"));

	hFindFolder = FindFirstFile(stPath, &FindFolderData);

	if(hFindFolder != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( FindFolderData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY &&
				_stscanf(FindFolderData.cFileName, TEXT("%02d-%02d-%02d"),
				&tmFile.tm_year, &tmFile.tm_mon, &tmFile.tm_mday) == 3)
			{
				// we will work only in 21th century
				// tmFile.tm_year is %100, tm_year is 1900 based
				tmFile.tm_year += 100;

				// tm_mon is 0 based
				tmFile.tm_mon--;

				bSameDate = false;

				// Find next day?
				bFindNext = 
					tmFile.tm_year < ptmVideo->tm_year ? true  : 
					tmFile.tm_year > ptmVideo->tm_year ? false : // equal year
					tmFile.tm_mon < ptmVideo->tm_mon   ? true  : 
					tmFile.tm_mon > ptmVideo->tm_mon   ? false : // equal year & month
					tmFile.tm_mday < ptmVideo->tm_mday ? true  : 
					(bSameDate = (tmFile.tm_mday == ptmVideo->tm_mday), false);

				DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("/%s"),
					FindFolderData.cFileName));

				// Looking for such a time in archive...

				if(!bFindNext)
				{
					wsprintf(&stPath[cbPath],
						TEXT("\\%s\\*.avi"), FindFolderData.cFileName);

					hFindFile = FindFirstFile(stPath, &FindFileData);

					if(hFindFile != INVALID_HANDLE_VALUE)
					{
						do
						{
							if(_stscanf(FindFileData.cFileName,	TEXT("%02d-%02d-%02d.avi"),
								&tmFile.tm_hour, &tmFile.tm_min, &tmFile.tm_sec) == 3)
							{
								// Find next time?
								bFindNext = !bSameDate ? false :
									tmFile.tm_hour < ptmVideo->tm_hour ? true  : 
									tmFile.tm_hour > ptmVideo->tm_hour ? false :	// same hour
									tmFile.tm_min < ptmVideo->tm_min ? true  : 
									tmFile.tm_min > ptmVideo->tm_min ? false :		// same minute
									tmFile.tm_sec < ptmVideo->tm_sec ? true  :
									tmFile.tm_sec > ptmVideo->tm_sec ? false :
									(tmFileLast.tm_year = -1, false);				// same datetime

								if(bFindNext)
								{
									DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT(".%s"),
										FindFileData.cFileName));

									tmFileLast = tmFile;
								}
								else
								{
									DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT(".%s done"),
										FindFileData.cFileName));

									bSameDate = true;

									// Check if position in last file
									if(tmFileLast.tm_year > 0)
									{
										wsprintf(&stPath[cbPath], 
											TEXT("\\%02d-%02d-%02d\\%02d-%02d-%02d.avi"),
											tmFileLast.tm_year%100, tmFileLast.tm_mon + 1, tmFileLast.tm_mday,
											tmFileLast.tm_hour, tmFileLast.tm_min, tmFileLast.tm_sec);

										lFileTime = _mktime64(&tmFileLast);
										lShift = GetFileLength(stPath);

										if(lFileTime > 0 && lFileTime + lShift > *pTime)
										{
											lShift = (long)(*pTime - lFileTime);
											*pTime = lFileTime;

											bSameDate = false;
										}
									}

									if(bSameDate)
									{
										wsprintf(&stPath[cbPath], TEXT("\\%s\\%s"),
											FindFolderData.cFileName, FindFileData.cFileName);

										lShift = 0L;
										*pTime = _mktime64(&tmFile);
									}

									lstrcpy(stFile, stPath);
									bFound = true;
								}
							}
						#ifdef _DEBUG
							else
							{
								Elvees::OutputF(Elvees::TTrace, TEXT(".%s invalid file format"),
									FindFileData.cFileName);
							}
						#endif
						}
						while(FindNextFile(hFindFile, &FindFileData) != 0 && bFindNext);

						FindClose(hFindFile);
					}
					else
					{
						// fail to find file
						bFindNext = true;
					}
				}
			}
		}
		while(FindNextFile(hFindFolder, &FindFolderData) != 0 && bFindNext);

		FindClose(hFindFolder);
	}

	if(bFound && plShift)
		*plShift = lShift;

#ifdef DO_TIMEINGS
	Elvees::OutputF(Elvees::TTrace, TEXT("FindFileByTime(%ld, %02d-%02d-%02d, %02d-%02d-%02d) (%u ms)"),
		m_lSourceID,
		ptmVideo->tm_year%100, ptmVideo->tm_mon + 1, ptmVideo->tm_mday,
		ptmVideo->tm_hour, ptmVideo->tm_min, ptmVideo->tm_sec,
		timer.Stop(), bFound ? TEXT("ok") : TEXT("failed"));
#endif

	return bFound;
}

bool CArchiveReader::OpenFile(LPCTSTR stFilename)
{
	HRESULT  hr;
	PAVIFILE pAVIFile = NULL;

	long        cbFormat  = 0;
	//BITMAPINFO *pbiFormat = NULL;

	//////////////////////////////////////////////////////////////////////////

	CloseFile();

#ifdef DO_TIMEINGS
	Elvees::CTimer timer;
	timer.Start();
#endif

	bool bOpened = false;

	do
	{	
		// Use default handler, prevent change
		hr = AVIFileOpen(&pAVIFile,
			stFilename,		// file name
			OF_READ,		// mode to open file with
			const_cast<LPCLSID>(&defaultAviHandler));

		if(hr != AVIERR_OK || !pAVIFile)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Fail to open file \"%s\" hr=0x%lX"), stFilename, hr);
			break;
		}

		hr = pAVIFile->GetStream(&m_pVideoStream, streamtypeVIDEO, 0L);
		if(hr != AVIERR_OK)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Fail get video stream hr=0x%lX"), hr);
			break;
		}

		hr = m_pVideoStream->ReadFormat(0L, NULL, &cbFormat);
		if(FAILED(hr) && cbFormat <= 0)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Fail to read format hr=0x%lX, Size=%ld"), hr, cbFormat);
			break;
		}
		m_FormatBuffer.resize( cbFormat );
		m_pbiFormat = reinterpret_cast<BITMAPINFO*>(&m_FormatBuffer[0]);
		hr = m_pVideoStream->ReadFormat(0L, &m_FormatBuffer[0], &cbFormat);
		if(hr != AVIERR_OK)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Fail to read format hr=0x%lX"), hr);
			break;
		}

		hr = m_pVideoStream->Info(&m_avisInfo, sizeof(m_avisInfo));
		if(hr != AVIERR_OK)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Fail get video stream info hr=0x%lX"), hr);
			break;
		}

		if(!GetDataStream( pAVIFile ))
		{
			Elvees::OutputF(Elvees::TWarning,
				TEXT("Fail get data stream info hr=0x%lX"), hr);
		}

		m_cbCompressed = max(0x10000, m_avisInfo.dwSuggestedBufferSize);
		m_pCompressed = malloc(m_cbCompressed);
		if(!m_pCompressed)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("Memory operation failed. Buffer Size = %ld"), m_cbCompressed);
			break;
		}
#if OLD

#else
		bOpened = SendFormat();
#endif
	}
	while(0);

	if(pAVIFile)
		pAVIFile->Release();
#if OLD
	if(pbiFormat)
		free(pbiFormat);
#endif
	if(!bOpened)
		CloseFile();

#ifdef DO_TIMEINGS
	if(bOpened)
		Elvees::OutputF(Elvees::TTrace, TEXT("OpenFile (%u ms)"), timer.Stop());
#endif

	return bOpened;
}

void CArchiveReader::CloseFile()
{
	m_pDataStream.Release();

	if(m_pVideoStream)
		m_pVideoStream->Release();
		
	if(m_pCompressed)
		free(m_pCompressed);

	m_cbCompressed = 0;
	m_pCompressed = NULL;

	m_pVideoStream = NULL;
	ZeroMemory(&m_avisInfo, sizeof(m_avisInfo));	
}

bool	CArchiveReader::SendFormat()
{
	return m_pSocket->SendStreamFormat( m_avisInfo.fccHandler, m_pbiFormat );
}

bool	CArchiveReader::GetDataStream( IAVIFile* pAVIFile )
{
	HRESULT hr;
	m_pDataStream.Release();
	hr = pAVIFile->GetStream(&m_pDataStream, g_fccStreamDataType, 0L);
	if(hr != AVIERR_OK)
	{
		Elvees::OutputF(Elvees::TError,
			TEXT("Fail get data stream hr=0x%lX"), hr);
		return false;
	}
/*
	hr = m_pVideoStream->ReadFormat(0L, NULL, &cbFormat);
	if(FAILED(hr) && cbFormat <= 0)
	{
		Elvees::OutputF(Elvees::TError,
			TEXT("Fail to read format hr=0x%lX, Size=%ld"), hr, cbFormat);
		break;
	}*/

	AVISTREAMINFOW info;
	hr = m_pDataStream->Info(&info, sizeof(info));
	if(hr != AVIERR_OK)
	{
		Elvees::OutputF(Elvees::TError,
			TEXT("Fail get data stream info hr=0x%lX"), hr);
		return false;
	}

	// FIXME max frame size 5 MB
	m_FrameUserDataBuffer.resize( std::max<size_t>( 5 * 1024 * 1024, info.dwSuggestedBufferSize ) );

	m_DataFPS = info.dwRate/double( info.dwScale );
	return true;
}