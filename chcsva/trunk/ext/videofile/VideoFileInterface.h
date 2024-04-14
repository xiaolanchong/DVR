//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейсы чтения/записи видеофайлов в произвольном формате

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_FILE_INTERFACE_5908590709651876_
#define _VIDEO_FILE_INTERFACE_5908590709651876_

struct VideoFileSettings
{
	BITMAPINFOHEADER m_bihVideoFormat;
	DWORD			 m_VideoFourCC;
	double			 m_fFPS;
	size_t			 m_MaxVideoFrameSize;
	size_t			 m_MaxUserDataSize;
};

class IVideoFileReader
{
public:

	virtual ~IVideoFileReader() {}

	virtual unsigned int FindNearestKeyFrame( unsigned int nStartFrameNumber, bool bForwardSearch ) = 0;

	virtual const VideoFileSettings& GetFileInfo( ) = 0;

	virtual bool	ReadFrame(
						unsigned int nSourceFrameNumber,
						void* pVideoData, size_t& nVideoDataSize, 
						void* pUserData, size_t& nUserDataSize
		) = 0;
};

class IVideoFileWriter
{
public:
	//	IVideoFileReader( const std::tstring& sFileName );
	virtual ~IVideoFileWriter() {}

	virtual bool	WriteFrame(
						const void* pVideoData, size_t nVideoDataSize, 
						const void* pUserData, size_t nUserDataSize,
						bool bKeyFrame,
						unsigned int nFrameNumber
						) = 0;
};

class IVideoFileFactory
{
public:
	virtual std::auto_ptr<IVideoFileWriter>	
				CreateVideoFileWriter( LPCTSTR szFileName, const VideoFileSettings& FileSettings  ) = 0;
	virtual std::auto_ptr<IVideoFileReader>	
				CreateVideoFileReader() = 0;
};

IVideoFileFactory*	GetVideoFileFactory();

#endif // _VIDEO_FILE_INTERFACE_5908590709651876_