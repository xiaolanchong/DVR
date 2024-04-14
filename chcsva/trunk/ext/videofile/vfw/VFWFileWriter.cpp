//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Èםעונפויסû קעוםטÿ גטהומפאיכמג AVI ס ןמלמשü‏ VFW

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "VFWFileWriter.h"

//#include <tchar.h>
#include <initguid.h>

// {00020000-0000-0000-C000-000000000046}
DEFINE_GUID(defaultAviHandler,
			0x00020000, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46);

//FIXME not global
DWORD g_fccStreamDataType	 = mmioFOURCC( 'U', 'D', 'A', 'T');
DWORD g_fccStreamDataHandler = mmioFOURCC( 'U', 'D', 'A', 'T');

void	FillRateInfo( AVISTREAMINFOW& strHdr, double fFPS) 
{
	DWORD FramesPerMinute = static_cast<DWORD>( fFPS * 60 );
	strHdr.dwScale    = FramesPerMinute > 60 ? 1 : 60;				// should be one for video
	strHdr.dwRate     = FramesPerMinute > 60 ? FramesPerMinute/60 : FramesPerMinute;	// fps
}

CComPtr<IAVIStream>		CreateVideoStream( 
	IAVIFile* pAVIFile, 
	const BITMAPINFOHEADER& bihFormat, 
	DWORD EncoderFourCC,
	double fFPS,
	size_t MaxVideoFrameSize
	)
{
	CComPtr<IAVIStream> pVideoStream;
	AVISTREAMINFOW strHdr;
	HRESULT hr;

	ZeroMemory(&strHdr, sizeof(strHdr));
	strHdr.fccType    = streamtypeVIDEO;					// video stream type
	strHdr.fccHandler = EncoderFourCC;			// Codec FCC
	FillRateInfo(strHdr, fFPS);
	strHdr.dwSuggestedBufferSize = static_cast<DWORD>(MaxVideoFrameSize);	// Recommended buffer size, in bytes, for the stream.

	// rectangle for stream
	::SetRect(&strHdr.rcFrame, 
		0, 0,						
		bihFormat.biWidth, bihFormat.biHeight);

	hr = pAVIFile->CreateStream(&pVideoStream, &strHdr);
	if(hr != AVIERR_OK)
	{
		return NULL;
	}

	//bug? - it can be non-BITMAPINFOHEADER?
	hr = pVideoStream->SetFormat(0, (void*)(&bihFormat), sizeof(BITMAPINFOHEADER) );
	if(hr != S_OK)
	{
		return NULL;
	}
	return pVideoStream;
}



CComPtr<IAVIStream> CreateDataStream( 
					IAVIFile* pAVIFile,
					DWORD VideoFrameWidth, 
					DWORD VideoFrameHeight,
					double fFPS,
					size_t MaxDataSize
					)
{
	CComPtr<IAVIStream> pDataStream;
	AVISTREAMINFOW strHdr;
	HRESULT hr;

	ZeroMemory(&strHdr, sizeof(strHdr));
	strHdr.fccType    = g_fccStreamDataType;			// video stream type
	strHdr.fccHandler = g_fccStreamDataHandler;			// Codec FCC
	FillRateInfo(strHdr, fFPS);

	strHdr.dwSuggestedBufferSize = static_cast<DWORD>(MaxDataSize);	// Recommended buffer size, in bytes, for the stream.
//	strHdr.dwStart = 2;

	// just ignore, but it can be same as the video frame size
	::SetRect(&strHdr.rcFrame, 0, 0, VideoFrameWidth, VideoFrameHeight);

	hr = pAVIFile->CreateStream(&pDataStream, &strHdr);
	if(hr != AVIERR_OK)
	{
		return NULL;
	}

	struct FrameSize
	{
		unsigned short	width,
						height;
	};

	FrameSize FmtHdr = 
	{	
		static_cast<unsigned short>(VideoFrameWidth), static_cast<unsigned short>(VideoFrameHeight)
	};
	hr = pDataStream->SetFormat(0, &FmtHdr, sizeof(FrameSize));

	if(hr != S_OK)
	{
		return NULL;
	}

	return pDataStream;
}

//======================================================================================//
//                                 class VFWFileWriter                                  //
//======================================================================================//

VFWFileWriter::VFWFileWriter(LPCTSTR szFileName, const VideoFileSettings& FileWriterSettings)
{
	HRESULT hr;
	CComPtr<IAVIFile> pAVIFile;
	hr = AVIFileOpen(
			&pAVIFile,		// Address to contain the new file interface pointer
			szFileName,			// Null-terminated string containing the name of the file to open
			OF_CREATE | OF_READWRITE,	// Access mode to use when opening the file
			const_cast<LPCLSID>(&defaultAviHandler));	// use handler determined from file extension
	if(hr != S_OK)
	{
		throw std::runtime_error("AVIFileOpen failed. Check filename");
	}
	m_pVideoStream = CreateVideoStream( pAVIFile, 
						FileWriterSettings.m_bihVideoFormat,
						FileWriterSettings.m_VideoFourCC,
						FileWriterSettings.m_fFPS,
						FileWriterSettings.m_MaxVideoFrameSize );

	if(!m_pVideoStream)
	{
		throw std::runtime_error("CreateVideoStream failed. Check format");
	}
	if( FileWriterSettings.m_MaxUserDataSize )
	{
		m_pDataStream  = CreateDataStream( 
							pAVIFile,
							FileWriterSettings.m_bihVideoFormat.biWidth,
							FileWriterSettings.m_bihVideoFormat.biHeight,
							FileWriterSettings.m_fFPS,
							FileWriterSettings.m_MaxUserDataSize);
		if( !m_pDataStream )
		{
			throw std::runtime_error("CreateDataStream failed. Check format");
		}
	}
}

VFWFileWriter::~VFWFileWriter()
{
}

bool VFWFileWriter::WriteVideoFrame(const void* pFrameData, size_t FrameDataSize, bool bKeyFrame, long lFrameNumber)
{
	long FrameDataSizeAsLong = static_cast<long>(FrameDataSize);
	HRESULT hr;
	// Segment archive check by time or by file size
	//

	_ASSERT( m_pVideoStream);

	long WrittenByteNumber = 0;
	hr = m_pVideoStream->Write(
		lFrameNumber,				// time of this frame
		1, 							// one frame
		const_cast<void*>(pFrameData), 				// frame data
		FrameDataSizeAsLong,						// frame size
		bKeyFrame ? AVIIF_KEYFRAME : 0,	// I-Frame or not?
		NULL,
		&WrittenByteNumber);
#if 1
//	_ASSERT(WrittenByteNumber == FrameDataSize);
#else
	Elvees::OutputF( Elvees::TError, _T("Failed to write frame %ld, size=%ld"), lFrameNumber, FrameDataSizeAsLong );
#endif
	return (hr) == S_OK;
}

bool	VFWFileWriter::WriteDataFrame(  const void* pFrameData, size_t FrameDataSize, long nFrameNumber )
{
	_ASSERT( m_pDataStream);

	long WrittenByteNumber = 0;
	HRESULT hr = m_pDataStream->Write(
			nFrameNumber,				// time of this frame
			1, 							// one frame
			const_cast<void*>(pFrameData), 				// frame data
			static_cast<long>(FrameDataSize),						// frame size
			AVIIF_KEYFRAME,	// I-Frame or not?
			NULL,
			&WrittenByteNumber
		);

	_ASSERT(WrittenByteNumber == FrameDataSize);

	return hr == S_OK;
}

bool	VFWFileWriter::WriteFrame(
						   const void* pVideoData, size_t nVideoDataSize, 
						   const void* pUserData, size_t nUserDataSize,
						   bool bKeyFrame,
						   unsigned int nFrameNumber
						   ) 
{
	_ASSERT(m_pVideoStream);
	if(m_pVideoStream)
	{
		bool res = WriteVideoFrame( pVideoData, nVideoDataSize, bKeyFrame, nFrameNumber );
		if(!res)
		{
			return false;
		}
	}
	if( m_pDataStream )
	{
#if 0
		TCHAR buf[255];
		wsprintf(buf, "\n[%u]User data size = %u\n", nFrameNumber, nUserDataSize);
		OutputDebugString( buf );
#endif
		bool res = WriteDataFrame( pUserData, nUserDataSize, nFrameNumber );
		return res;
	}
	else
	{
		return true;
	}
}