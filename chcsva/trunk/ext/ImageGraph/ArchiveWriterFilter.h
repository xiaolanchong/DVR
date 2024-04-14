//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс записи изображений в файловый архив

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_WRITER_FILTER_9725966636827068_
#define _ARCHIVE_WRITER_FILTER_9725966636827068_

struct ArchiveWriteParam
{
//	BITMAPINFOHEADER m_biVideoFormat;
	DWORD			 m_CodecFCC;
	unsigned int	 m_FramesPerMinutes;
	unsigned int	 m_VideoMaxSizeInSeconds;
#ifdef UNICODE
	std::wstring	
#else
	std::string		 
#endif
					m_VideoBasePath;
};

class ArchiveWriter;

//======================================================================================//
//                              class ArchiveWriterFilter                               //
//======================================================================================//

//! \brief Класс записи изображений в файловый архив
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveWriterFilter : public IntermediateFilter
{
	ArchiveWriteParam	m_ArchiveParam;
	long				m_StreamID;
	unsigned int		m_uTimeStart;

	long				GetID() const
	{
		return m_StreamID;
	}

	std::auto_ptr<ArchiveWriter> m_pArchiveWriter;
public:
	ArchiveWriterFilter( long StreamID, const ArchiveWriteParam& param);
	virtual ~ArchiveWriterFilter();

	virtual void PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize );
};

#endif // _ARCHIVE_WRITER_FILTER_9725966636827068_