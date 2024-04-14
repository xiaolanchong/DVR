// ArchiveReader.h: interface for the CArchiveReader class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// {00020000-0000-0000-C000-000000000046}
DEFINE_GUID(defaultAviHandler,
0x00020000, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46);

//class CArchiveServer;
class IArchiveDataResponse;
class IVideoFileReader;

///////////////////////////////////////////////////////////////////////////////
// CArchiveReader
///////////////////////////////////////////////////////////////////////////////

class CArchiveReader : public Elvees::CThread
{
public:
	explicit CArchiveReader(IArchiveDataResponse* pResponseInterface, const std::tstring& sVideoBasePath);
	virtual ~CArchiveReader();

	void ProcessCommand(LPCSTR stCommand, size_t nSize);
	void Shutdown();

protected:
	virtual int Run();

	long GetFileLength(LPCTSTR stFile);
	bool FindFileByTime(INT64* pTime, LPTSTR stFile, long cbFile, long* plgap);

	bool OpenFile(LPCTSTR stFilename);
	void CloseFile();

private:
	long m_lSourceID;							// Source ID
	IArchiveDataResponse *m_pSocket;					// Server

	INT64 m_timePos;

	// avi file data
	long m_cbCompressed;
	void *m_pCompressed;
#if 1
	PAVISTREAM		m_pVideoStream;
	AVISTREAMINFOW	m_avisInfo;

	CComPtr<IAVIStream>	m_pDataStream;
	double				m_DataFPS;

	bool GetDataStream( IAVIFile* pAVIFile );

	std::vector<BYTE>	m_FrameUserDataBuffer;
#else
	std::auto_ptr<IVideoFileReader>	m_pVideoFileReader;
	double GetFPS() const;
#endif
	//

	Elvees::CManualResetEvent      m_timeEvent;
	Elvees::CManualResetEvent      m_shutdownEvent;
	
	// No copies do not implement
	CArchiveReader(const CArchiveReader &rhs);
	CArchiveReader &operator=(const CArchiveReader &rhs);

	std::vector<BYTE>	m_FormatBuffer;
	BITMAPINFO*			m_pbiFormat;
	bool	SendFormat();

	std::tstring m_sVideoBasePath;
	LPCTSTR GetVideoBasePath()
	{
		return m_sVideoBasePath.c_str();
	}
};