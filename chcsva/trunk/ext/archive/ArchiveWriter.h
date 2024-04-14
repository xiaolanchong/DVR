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
#ifndef _ARCHIVE_WRITER_3727401917052036_
#define _ARCHIVE_WRITER_3727401917052036_

class IVideoFileWriter;

//======================================================================================//
//                                 class ArchiveWriter                                  //
//======================================================================================//

//! \brief Запись файлового архива сжатых видеокадров + данные к ним
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveWriter
{
	std::tstring	m_sBaseCameraArchivePath;
	long			m_lVideoSize;
	long			m_lStartFrame;

	bool					PrepareArchive(bool bUseDefaultOnFailed, LPCTSTR szBaseArchivePath);
	std::tstring			PrepareArchiveFile( __time64_t ArchiveStartTime) const;
	bool					CreateVideoFile(__time64_t FrameTime);

	std::auto_ptr<IVideoFileWriter> m_pVideoFileWriter;
	void					ReleaseStreams();
public:
	//! настройка записи архива
	struct Settings
	{
		//! выходной формат видеокодека
		BITMAPINFOHEADER	m_bihVideoFormat;
		//! fourcc видеокодека
		DWORD				m_EncoderFourCC;
		//! макс. размер сжатого видеокадра в байтах
		long				m_MaxPackedSize;
		//! скорость записи, кадров в минуту
		long				m_FramesPerMinute;
		//! максимальный размер видеофайла, в секундах
		long				m_VideoMaxSize;
		
		long				m_UserDataMaxSize;
	};

	ArchiveWriter( 
		LPCTSTR szBaseArchivePath, 
		long nCameraID, 
		const Settings& ArchiveSettings
		);
	virtual ~ArchiveWriter();

	bool	WriteFrame( const void* pVideoFrameData, size_t VideoFrameDataSize, 
						const void* pUserFrameData, size_t UserFrameDataSize,
						__time64_t FrameTime, bool bKeyFrame, long lFrameNumber );
public:
	const Settings m_ArchiveSettings;
};

#endif // _ARCHIVE_WRITER_3727401917052036_