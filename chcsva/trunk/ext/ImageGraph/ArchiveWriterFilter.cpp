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

#include "chcs.h"
#include "FramePin.h"
#include "ArchiveWriterFilter.h"
#include "../archive/ArchiveWriter.h"

//======================================================================================//
//                              class ArchiveWriterFilter                               //
//======================================================================================//

ArchiveWriterFilter::ArchiveWriterFilter(long StreamID, const ArchiveWriteParam& param):
	m_ArchiveParam( param ) ,
	m_StreamID( StreamID ) ,
	m_uTimeStart(0)
{
}

ArchiveWriterFilter::~ArchiveWriterFilter()
{
}

void ArchiveWriterFilter::PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize )
{
	if( m_uTimeStart == 0 )
	{
		m_uTimeStart = pFrame->GetRelativeTime();
	}
	unsigned int uTime  = pFrame->GetRelativeTime() - m_uTimeStart;
	long		 lFrame = MulDiv(uTime, m_ArchiveParam.m_FramesPerMinutes, 60000);

#if 0	// test graph
	Elvees::OutputF( Elvees::TError, _T("[%ld]ArchiveWriterFilter temporally disabled"), m_StreamID );
	SendImage( pFrame,  bKeyFrame, MaxImageSize );
	return;
#endif

	if( !m_pArchiveWriter.get() && GetID() > 0 )
	{
		BITMAPINFO bi;
		pFrame->GetFormat( &bi );

		ArchiveWriter::Settings ArchiveSettings;
		ArchiveSettings.m_bihVideoFormat	= bi.bmiHeader;
		ArchiveSettings.m_EncoderFourCC		= m_ArchiveParam.m_CodecFCC;
		ArchiveSettings.m_FramesPerMinute	= m_ArchiveParam.m_FramesPerMinutes;
		ArchiveSettings.m_MaxPackedSize		= static_cast<long>(MaxImageSize);
		// FIXME
		ArchiveSettings.m_UserDataMaxSize	= 1024;
		ArchiveSettings.m_VideoMaxSize		= m_ArchiveParam.m_VideoMaxSizeInSeconds;

		m_pArchiveWriter.reset( new ArchiveWriter( 
			m_ArchiveParam.m_VideoBasePath.c_str(), GetID(), ArchiveSettings ) );

	}
	if( m_pArchiveWriter.get() )
	{
		std::vector<BYTE>	TempCustomFrameDataBuffer;
		size_t nSize = 0;
		bool res = pFrame->GetCustomData( 0, nSize );
		if( res && nSize )
		{
			TempCustomFrameDataBuffer.resize(nSize);
			nSize = TempCustomFrameDataBuffer.size();
			bool res = pFrame->GetCustomData( &TempCustomFrameDataBuffer[0], nSize );
			_ASSERT(res);
		}

		BITMAPINFO bi;
		pFrame->GetFormat( &bi );

		res = m_pArchiveWriter->WriteFrame( 
			pFrame->GetBytes(), 
			bi.bmiHeader.biSizeImage, 
			TempCustomFrameDataBuffer.empty() ? 0 : &TempCustomFrameDataBuffer[0],
			TempCustomFrameDataBuffer.size(),
			pFrame->GetTime(), bKeyFrame, 
			lFrame );
#if 0
		_ASSERT(res);
#endif
	}
	SendImage( pFrame,  bKeyFrame, MaxImageSize );
}