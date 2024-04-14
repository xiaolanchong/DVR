// StreamArchive.h: interface for the CStreamArchive class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "FrmBuffer.h"
#include "ext/archive/ArchiveDataExchange.h"

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

class CStreamArchive
	: public CHCS::IStream
	, public CFrmBuffer::Allocator
{
protected:
	CStreamArchive(long lSourceID, IArchiveDataRequest* pRequestInterface);
	virtual ~CStreamArchive();

public:
	static CStreamArchive* CreatStream(long lSourceID, INT64 startPos, IArchiveDataRequest* pRequestInterface);
public:
	virtual long AddRef();
	virtual long Release();
	virtual bool Start();
	virtual void Stop();
	virtual bool IsStarted();
	virtual long SeekTo(INT64 timePos);
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	virtual bool SetCustomFrameData( const void* pData, size_t DataSize )
	{
		return false;
	}

	long GetID();
#if OLD
	void SetConnection(Elvees::CSocketServer::Socket* pSocket);
#endif
public:
	virtual void InitDecompressor(DWORD dwFCC, BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput);
	virtual void DecompressData(const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime);
	virtual bool SetCustomData( const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime );

private:
	// Allocator source ID request
	virtual long AllocatorSourceID();

	void SetLastFrame(CHCS::IFrame*  pFrame);
	bool GetLastFrame(CHCS::IFrame** ppFrame);

	CHCS::IFrame *m_pFrameLast;
	Elvees::CManualResetEvent m_newImage;
	Elvees::CCriticalSection m_lastFrameSect;

private:
	long m_refCount;

	const long m_lStreamID;
	CVidDecoder* m_pDecoder;

#if OLD
	Elvees::CSocketServer::Socket* m_pSocket;
#else
	IArchiveDataRequest* m_pRequestInterface;
#endif
	// No copies do not implement
	CStreamArchive(const CStreamArchive &rhs);
	CStreamArchive &operator=(const CStreamArchive &rhs);
};