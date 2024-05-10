// MemStream.h: interface for the CMemStream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_MEMSTREAM_H__INCLUDED
#define ELVEESTOOLS_MEMSTREAM_H__INCLUDED

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CMemStream
//////////////////////////////////////////////////////////////////////

class CMemStream : public IStream
{
public:
	explicit CMemStream(BYTE* pBuffer = NULL, long lSize = 0L);
	explicit CMemStream(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);
	virtual ~CMemStream();

	void Truncate();

	long  Size();
	BYTE* GetBuffer();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	// ISequentialStream
	STDMETHOD(Read)(LPVOID pv, ULONG cb, ULONG *pcbRead);
	STDMETHOD(Write)(LPCVOID pv, ULONG cb, ULONG *pcbWritten);

	// IStream
	STDMETHOD(Seek)(
			LARGE_INTEGER dlibMove,
			DWORD dwOrigin,
			ULARGE_INTEGER *plibNewPosition);

	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);

	STDMETHOD(CopyTo)( 
			IStream *pstm,
			ULARGE_INTEGER cb,
			ULARGE_INTEGER *pcbRead,
			ULARGE_INTEGER *pcbWritten);

	STDMETHOD(Commit)(DWORD grfCommitFlags);

	STDMETHOD(Revert)(void);

	STDMETHOD(LockRegion)(
			ULARGE_INTEGER libOffset,
			ULARGE_INTEGER cb,
			DWORD dwLockType);

	STDMETHOD(UnlockRegion)(
			ULARGE_INTEGER libOffset,
			ULARGE_INTEGER cb,
			DWORD dwLockType);

	STDMETHOD(Stat)(
			STATSTG *pstatstg,
			DWORD grfStatFlag);

	STDMETHOD(Clone)(IStream **ppstm);

private:
	BYTE *m_pBuffer;		// stream buffer
	long  m_lBuffer;		// buffer size
	
	long m_lSize;			// Max index position
	long m_lCurPos;			// Current index position

	bool m_bBufferOwner;

	ULONG m_cRef;			// Reference count (not used)

	// No copies do not implement
	CMemStream(const CMemStream &rhs);
	CMemStream &operator=(const CMemStream &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_MEMSTREAM_H__INCLUDED
