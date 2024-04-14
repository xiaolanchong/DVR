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
#include "VFWFileReader.h"

#include <initguid.h>

// {00020000-0000-0000-C000-000000000046}
DEFINE_GUID(defaultAviHandler,
			0x00020000, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46);

//======================================================================================//
//                                 class VFWFileReader                                  //
//======================================================================================//

VFWFileReader::VFWFileReader(LPCTSTR szFileName)
{
	CComPtr<IAVIFile> pAVIFile;
	HRESULT hr;
	// Use default handler, prevent change
	hr = AVIFileOpen(&pAVIFile,
		szFileName,		// file name
		OF_READ,		// mode to open file with
		const_cast<LPCLSID>(&defaultAviHandler));

	if(hr != AVIERR_OK || !pAVIFile)
	{
		throw std::runtime_error("Fail to open file");
	}

	hr = pAVIFile->GetStream(&m_pVideoStream, streamtypeVIDEO, 0L);
	if(hr != AVIERR_OK)
	{
		throw std::runtime_error("Fail get video stream");
	}

	LONG cbFormat;
	hr = m_pVideoStream->ReadFormat(0L, NULL, &cbFormat);
	if(FAILED(hr) && cbFormat <= 0)
	{
		throw std::runtime_error("Fail to read format");
	}
	std::vector<BYTE> FormatBuffer( cbFormat );
	BITMAPINFO* pbiFormat = reinterpret_cast<BITMAPINFO*>(&FormatBuffer[0]);

	hr = m_pVideoStream->ReadFormat(0L, &FormatBuffer[0], &cbFormat);
	if(hr != AVIERR_OK)
	{
		throw std::runtime_error("Fail to read format");
	}

	m_Settings.m_bihVideoFormat = pbiFormat->bmiHeader;

	AVISTREAMINFOW avisInfo;
	hr = m_pVideoStream->Info(&avisInfo, sizeof(avisInfo));
	if(hr != AVIERR_OK)
	{
		throw std::runtime_error("Fail get video stream info");;
	}

	m_Settings.m_fFPS = avisInfo.dwRate/double(avisInfo.dwScale);

	if(!GetDataStream( pAVIFile ))
	{
		m_Settings.m_MaxUserDataSize = 0;
	}
	m_Settings.m_MaxVideoFrameSize = max(0x10000, avisInfo.dwSuggestedBufferSize); 
}

VFWFileReader::~VFWFileReader()
{
}

unsigned int VFWFileReader::FindNearestKeyFrame( unsigned int nStartFrameNumber, bool bForwardSearch ) 
{
	if( !m_pVideoStream )
	{
		return unsigned(-1);
	}
	return m_pVideoStream->FindSample(nStartFrameNumber, (bForwardSearch? FIND_NEXT : FIND_PREV) | FIND_KEY);
}

const VideoFileSettings& VFWFileReader::GetFileInfo( ) 
{
	return m_Settings;
}

bool	VFWFileReader::ReadFrame(
						  unsigned int nSourceFrameNumber,
						  void* pVideoData, size_t& nVideoDataSize, 
						  void* pUserData, size_t& nUserDataSize
						  )
{
	if( !m_pVideoStream )
	{
		nVideoDataSize	= 0;
		nUserDataSize	= 0;
		return false;
	}
	HRESULT hr;
	LONG cbRead;
	hr = m_pVideoStream->Read(
		nSourceFrameNumber,
		1,
		pVideoData,
		nVideoDataSize,
		&cbRead,
		NULL);

	if( hr == S_OK )
	{
		nVideoDataSize = cbRead;
	}
	else
	{
		nVideoDataSize	= 0;
		nUserDataSize	= 0;
		return false;
	}

	if( m_pDataStream )
	{
		LONG cbRead;
		hr = m_pDataStream->Read( nSourceFrameNumber, 1, pUserData, nUserDataSize, &cbRead, 0 );
		nUserDataSize = cbRead;
	}
	else
	{
		nUserDataSize = 0;
	}
	return true;
}

bool	VFWFileReader::GetDataStream( IAVIFile* pAVIFile )
{
	HRESULT hr;
	m_pDataStream.Release();
	hr = pAVIFile->GetStream(&m_pDataStream, streamtypeTEXT, 0L);
	if(hr != AVIERR_OK)
	{
		return false;
	}

	AVISTREAMINFOW info;
	hr = m_pDataStream->Info(&info, sizeof(info));
	if(hr != AVIERR_OK)
	{
		return false;
	}

	//m_DataFPS = info.dwRate/double( info.dwScale );
	m_Settings.m_MaxUserDataSize = info.dwSuggestedBufferSize;
	return true;
}