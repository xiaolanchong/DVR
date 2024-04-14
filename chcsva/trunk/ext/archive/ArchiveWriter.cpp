//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Запись файлового архива сжатых видеокадров + данные к ним

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "ArchiveWriter.h"
#include "../videofile/VideoFileInterface.h"

//======================================================================================//
//                                 class ArchiveWriter                                  //
//======================================================================================//

ArchiveWriter::ArchiveWriter(
							 LPCTSTR szBaseArchivePath, 
							 long nCameraID, 
							 const Settings& ArchiveSettings
							 ):
	m_ArchiveSettings(ArchiveSettings)
{
	int cbFilePath;
	TCHAR stFilePath[MAX_PATH];

	cbFilePath = wsprintf(stFilePath, TEXT("%s\\%ld"), szBaseArchivePath, nCameraID);
	m_sBaseCameraArchivePath = stFilePath;

	m_lVideoSize = 0;
	m_lStartFrame = -1;

	PrepareArchive( false, szBaseArchivePath );
	// Settings
}

ArchiveWriter::~ArchiveWriter()
{
//	SAFE_RELEASE(m_pArchive);
	// not required (auto)
	ReleaseStreams();
}

bool ArchiveWriter::PrepareArchive(bool bUseDefaultOnFailed, LPCTSTR szBaseArchivePath)
{
	::SetLastError(0);

	if(!::CreateDirectory(szBaseArchivePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if(!bUseDefaultOnFailed)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive base directory"));
			return false;
		}
/*
		//  Create directory in dll directory
		::GetModuleFileName(_Module.GetModuleInstance(), szBaseArchivePath, countof(m_stBasePath));

		int i=lstrlen(m_stBasePath);
		for(; m_stBasePath[i] != '\\'; i--);

		m_stBasePath[i+1] = 0;
		lstrcat(m_stBasePath, TEXT("Archive"));

		::SetLastError(0);

		if(!::CreateDirectory(szBaseArchivePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive directory"));
			return false;
		}*/
	}

	return true;
}

std::tstring	ArchiveWriter::PrepareArchiveFile(__time64_t ArchiveStartTime) const
{
	struct tm* tmFrame = _localtime64(&ArchiveStartTime);

	if(!tmFrame)
		throw std::runtime_error("Invalid frame time") ;

	size_t cbFilePath;
	TCHAR stFilePath[MAX_PATH];
	lstrcpyn( stFilePath, m_sBaseCameraArchivePath.c_str(), MAX_PATH );
	cbFilePath = m_sBaseCameraArchivePath.length();

	if(!::CreateDirectory(stFilePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		Elvees::OutputF(Elvees::TError, TEXT("Fail to create \"%s\" directory"), stFilePath);
		throw std::runtime_error("Fail to create directory");
	}

	cbFilePath += wsprintf(&stFilePath[cbFilePath], TEXT("\\%02d-%02d-%02d"),
		tmFrame->tm_year%100, tmFrame->tm_mon + 1, tmFrame->tm_mday);

	if(!::CreateDirectory(stFilePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		Elvees::OutputF(Elvees::TError, TEXT("Fail to create \"%s\" directory"), stFilePath);
		throw std::runtime_error("Fail to create directory");
	}

	cbFilePath += wsprintf(&stFilePath[cbFilePath], TEXT("\\%02d-%02d-%02d.avi"),
		tmFrame->tm_hour, tmFrame->tm_min, tmFrame->tm_sec);

	if(Elvees::IsFileExists(stFilePath))
	{
		Elvees::OutputF(Elvees::TError, TEXT("AVIFileOpen file \"%s\" already exists"), stFilePath);
		throw std::runtime_error("AVIFileOpen : file already exists");
	}

	return std::tstring( stFilePath );
}

bool	ArchiveWriter::CreateVideoFile(__time64_t FrameTime )
{
	if( m_pVideoFileWriter.get() )
	{
		// already created
		return false;
	}

	bool bOpenFile   = false;
	bool bDeleteFile = false;

	IAVIFile *pAVIFile = NULL;
	std::tstring stFilePath;
	do 
	{	
		try
		{
			stFilePath = PrepareArchiveFile( FrameTime );
		}
		catch (std::runtime_error&) 
		{
			break;
		}
		// Open the movie file for writing....
		// I know that i must call AviFileInit but it only call CoInitilize()

		bDeleteFile = true;

	
		try
		{
			VideoFileSettings FileWriterSettings  =
			{
				m_ArchiveSettings.m_bihVideoFormat,
				m_ArchiveSettings.m_EncoderFourCC,
				m_ArchiveSettings.m_FramesPerMinute/60.0,
				m_ArchiveSettings.m_VideoMaxSize,
				m_ArchiveSettings.m_UserDataMaxSize
			};
			m_pVideoFileWriter = GetVideoFileFactory()->CreateVideoFileWriter( stFilePath.c_str(), FileWriterSettings );
		}
		catch(std::runtime_error&)
		{
			Elvees::OutputF(Elvees::TError, TEXT("AVIFileOpen failed. Check filename \"%s\""), stFilePath);
			break;
		}

		
		bOpenFile = true;
	}
	while(false);

	// IAVIFile don't need anymore...
	SAFE_RELEASE(pAVIFile);

	if(!bOpenFile)
	{
		if(bDeleteFile)
			::DeleteFile(stFilePath.c_str());

		m_lVideoSize  = 0;
		m_lStartFrame = -1;

		ReleaseStreams();
	}
	return bOpenFile;
}

bool	ArchiveWriter::WriteFrame( 
								  const void* pVideoFrameData, size_t VideoFrameDataSize, 
								  const void* pUserFrameData, size_t UserFrameDataSize,
								  __time64_t FrameTime, bool bKeyFrame, long lFrameNumber )
{
	long lVideoFrame = m_lStartFrame < 0 ? 0 : lFrameNumber - m_lStartFrame;

	// Segment archive check by time or by file size
	//

	if(( MulDiv(lVideoFrame, 60, m_ArchiveSettings.m_FramesPerMinute) >= m_ArchiveSettings.m_VideoMaxSize || 
		m_lVideoSize >= 0x2BC00000L )	&& 
		bKeyFrame)
	{
		lVideoFrame   = 0;

		m_lVideoSize  = 0;
		m_lStartFrame = -1;

		ReleaseStreams();
	}

	if( bKeyFrame && CreateVideoFile(FrameTime) )
	{
		m_lStartFrame = lFrameNumber;
	}

	if( m_pVideoFileWriter.get())
	{
		m_lVideoSize += static_cast<long>(VideoFrameDataSize);
#if 0
		char buf[1];
		if( pUserFrameData )
		{
			INT64 tt = *(INT64*)(( const BYTE*)pUserFrameData + 4);
			int z = 1;
		}
		else
		{
			pUserFrameData = buf;
			UserFrameDataSize = 1;
		}
#endif

		bool res = m_pVideoFileWriter->WriteFrame(
						pVideoFrameData, VideoFrameDataSize,
						pUserFrameData, UserFrameDataSize,
						bKeyFrame, lVideoFrame
						);

		if(!res)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail write frame to avi size=%ld"), m_lVideoSize);

			m_lVideoSize  = 0;
			m_lStartFrame = -1;

			ReleaseStreams();
			return false;
		}
	}
	return true;
}

void		ArchiveWriter::ReleaseStreams()
{
	m_pVideoFileWriter.reset();
}