// MemStream.cpp: implementation of the CMemStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemStream.h"

//#define MEMSTREAM_DEBUG

#ifdef MEMSTREAM_DEBUG
#define MEMSTREAM_TEST(x)   x
#else
#define MEMSTREAM_TEST(x)
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CMemStream
//////////////////////////////////////////////////////////////////////

CMemStream::CMemStream(BYTE* pBuffer, long lSize)
	: m_cRef(1)
{
	m_pBuffer = pBuffer;
	m_lSize = lSize;
	m_lBuffer = lSize;
	m_lCurPos = 0;

	if(!pBuffer)
	{
		m_lBuffer = (lSize + 0xFFF) & ~0xFFF; // 4K round
		
		m_pBuffer = (BYTE*)malloc(m_lBuffer);
		m_lSize = 0;

		ZeroMemory(m_pBuffer, m_lBuffer);
		m_bBufferOwner = true;
	}
	else
		m_bBufferOwner = false;
}

CMemStream::CMemStream(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
	: m_cRef(1)
{
	HRSRC hResInfo;
	HGLOBAL hResData;
	LPVOID pData;
	DWORD dwData;

	m_pBuffer = NULL;
	m_lSize = 0;
	m_lBuffer = 0;
	m_lCurPos = 0;
	m_bBufferOwner = false;

	hResInfo = FindResource(hModule, lpName, lpType);
	if(hResInfo)
	{
		hResData = LoadResource(hModule, hResInfo);
		if(hResData)
		{
			pData = LockResource(hResData);
			dwData = SizeofResource(hModule, hResInfo);

			m_lSize = (dwData + 0xFFF) & ~0xFFF; // 4K round
			m_pBuffer = (BYTE*)malloc(m_lSize);

			if(m_pBuffer)
			{
				CopyMemory(m_pBuffer, pData, dwData);
				m_lBuffer = dwData;
				m_bBufferOwner = true;
			}
		}
		else
		{
			Elvees::OutputF(Elvees::TTrace,
				TEXT("CMemStream::LoadResource failed err=%ld"), GetLastError());
		}
	}
	else
	{
		Elvees::OutputF(Elvees::TTrace,
			TEXT("CMemStream::FindResource failed err=%ld"), GetLastError());
	}
}

CMemStream::~CMemStream()
{
	if(m_pBuffer && m_bBufferOwner)
		free(m_pBuffer);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

long CMemStream::Size()
{
	return m_lSize;
}

BYTE* CMemStream::GetBuffer()
{
	return m_pBuffer;
}

void CMemStream::Truncate()
{
	m_lSize = 0;
	m_lCurPos = 0;
}

//////////////////////////////////////////////////////////////////////////
// IUnknown
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::QueryInterface(REFIID riid, LPVOID* ppvObject)
{
	*ppvObject = NULL;

	if(riid == IID_IUnknown) *ppvObject = this;
	if(riid == IID_IStream) *ppvObject = this;
	if(riid == IID_ISequentialStream) *ppvObject = this;
	
	if(*ppvObject)
	{
		( (IUnknown*) *ppvObject )->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CMemStream::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CMemStream::Release(void)
{
	return --m_cRef;
}

//////////////////////////////////////////////////////////////////////////
// ISequentialStream
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::Read(LPVOID pv, ULONG cb, ULONG *pcbRead)
{
	MEMSTREAM_TEST(Elvees::OutputF(Elvees::TTrace,
		TEXT("CMemStream::Read(0x%X, %ld, 0x%X)"), pv, cb, pcbRead));

	// Check parameters.
	if( pcbRead ) *pcbRead = 0;
	if( !pv ) return STG_E_INVALIDPOINTER;
	if( 0 == cb ) return S_OK; 

	// Calculate bytes left and bytes to read.
	ULONG cBytesLeft = m_lSize - m_lCurPos;
	ULONG cBytesRead = cb > cBytesLeft ? cBytesLeft : cb;

	// If no more bytes to retrieve return S_FALSE.
	if( 0 == cBytesLeft ) return S_FALSE;

	// Copy to users buffer the number of bytes requested or remaining
	memcpy( pv, m_pBuffer + m_lCurPos, cBytesRead );
	m_lCurPos += cBytesRead;

	// Return bytes read to caller.
	if( pcbRead ) *pcbRead = cBytesRead;
	if( cb != cBytesRead ) return S_FALSE; 

	return S_OK;
}

STDMETHODIMP CMemStream::Write(LPCVOID pv, ULONG cb, ULONG *pcbWritten)
{
	MEMSTREAM_TEST(Elvees::OutputF(Elvees::TTrace,
		TEXT("CMemStream::Write(0x%X, %ld, 0x%X)"), pv, cb, pcbWritten));

	// Check parameters.
	if( !pv ) return STG_E_INVALIDPOINTER;
	if( pcbWritten ) *pcbWritten = 0;
	if( 0 == cb ) return S_OK;

	long lNewPos = m_lCurPos + (long)cb;
	
	if(lNewPos > m_lSize)
	{
		if(lNewPos > m_lBuffer)
		{
			if(!m_bBufferOwner)
				return E_FAIL;

			// Grow internal buffer to new size
			m_lBuffer = (lNewPos + 0xFFF) & ~0xFFF;
			m_pBuffer  = (BYTE*)realloc(m_pBuffer, m_lBuffer);

			// Check for out of memory situation.
			if(m_pBuffer == NULL)
			{
				m_lBuffer = 0L;
				m_lSize = 0L;
				m_lCurPos = 0L;

				return STG_E_INSUFFICIENTMEMORY;
			}
		}

		m_lSize = lNewPos;
	}

	// Copy callers memory to internal buffer and update write position.
	memcpy(m_pBuffer + m_lCurPos, pv, cb);
	m_lCurPos = lNewPos;

	// Return bytes written to caller.
	if( pcbWritten ) *pcbWritten = cb;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IStream
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::Seek(
	LARGE_INTEGER dlibMove,
	DWORD dwOrigin,
	ULARGE_INTEGER *plibNewPosition)
{
	MEMSTREAM_TEST(Elvees::OutputF(Elvees::TTrace,
		TEXT("CMemStream::Seek(%ld:%ld, %ld, 0x%X)"),
		dlibMove.LowPart, dlibMove.HighPart, dwOrigin, plibNewPosition));

	long lOffset = (long)dlibMove.LowPart;

	if(dlibMove.HighPart != -1L && dlibMove.HighPart != 0L)
		return E_FAIL;	// to big offset

	//----------------------------------------------------------

	long lNewPos = m_lCurPos;

	if(dwOrigin == STREAM_SEEK_SET)
		lNewPos = lOffset;
	else if(dwOrigin == STREAM_SEEK_CUR)
		lNewPos += lOffset;
	else if(dwOrigin == STREAM_SEEK_END)
		lNewPos = m_lSize + lOffset;
	else
		return STG_E_INVALIDFUNCTION;

	if(lNewPos < 0)
		return E_FAIL;

	if(lNewPos > m_lSize)
	{
		if(lNewPos > m_lBuffer)
		{
			if(!m_bBufferOwner)
				return E_FAIL;

			m_lBuffer = (lNewPos + 0xFFF) & ~0xFFF;
			m_pBuffer  = (BYTE*)realloc(m_pBuffer, m_lBuffer);

			if(m_pBuffer == NULL)
			{
				m_lBuffer = 0L;
				m_lSize = 0L;
				m_lCurPos = 0L;

				return STG_E_INSUFFICIENTMEMORY;
			}
		}

		m_lSize = lNewPos;
	}

	m_lCurPos = lNewPos;

	if(plibNewPosition)
	{
		plibNewPosition->LowPart = m_lCurPos;
		plibNewPosition->HighPart = 0;
	}

	return S_OK;
}

STDMETHODIMP CMemStream::SetSize(ULARGE_INTEGER libNewSize)
{
	Elvees::OutputF(Elvees::TWarning,
		TEXT("CMemStream::SetSize(%ld:%ld) not implemented"),
		libNewSize.LowPart, libNewSize.HighPart);

	return E_FAIL;
}

STDMETHODIMP CMemStream::CopyTo( 
	IStream *pstm,
	ULARGE_INTEGER cb,
	ULARGE_INTEGER *pcbRead,
	ULARGE_INTEGER *pcbWritten)
{
	Elvees::Output(Elvees::TWarning,
		TEXT("CMemStream::CopyTo() not implemented"));

	return E_FAIL;
}

STDMETHODIMP CMemStream::Commit(DWORD grfCommitFlags)
{
	Elvees::OutputF(Elvees::TWarning,
		TEXT("CMemStream::Commit(%ld) not implemented"), grfCommitFlags);

	return E_FAIL;
}

STDMETHODIMP CMemStream::Revert(void)
{
	Elvees::Output(Elvees::TWarning,
		TEXT("CMemStream::Revert() not implemented"));

	return E_FAIL;
}

STDMETHODIMP CMemStream::LockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	Elvees::OutputF(Elvees::TWarning,
		TEXT("CMemStream::LockRegion(%ld:%ld, %ld:%ld, %ld) not implemented"),
		libOffset.LowPart, libOffset.HighPart,
		cb.LowPart, cb.HighPart, dwLockType);

	return E_FAIL;
}

STDMETHODIMP CMemStream::UnlockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	Elvees::OutputF(Elvees::TWarning,
		TEXT("CMemStream::UnlockRegion(%ld:%ld, %ld:%ld, %ld) not implemented"),
		libOffset.LowPart, libOffset.HighPart,
		cb.LowPart, cb.HighPart, dwLockType);

	return E_FAIL;
}

STDMETHODIMP CMemStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	MEMSTREAM_TEST(Elvees::OutputF(Elvees::TWarning,
		TEXT("CMemStream::Stat(0x%X, %s) not implemented"), pstatstg,
		grfStatFlag == STATFLAG_DEFAULT ? TEXT("DEFAULT") :
		grfStatFlag == STATFLAG_NONAME  ? TEXT("NONAME") :
		grfStatFlag == STATFLAG_NOOPEN  ? TEXT("NOOPEN") : TEXT("UNK")));

	return E_FAIL;
}

STDMETHODIMP CMemStream::Clone(IStream **)
{
	Elvees::Output(Elvees::TWarning,
		TEXT("CMemStream::Clone() not implemented"));

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
