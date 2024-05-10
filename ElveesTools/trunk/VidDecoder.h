// VidDecoder.h: interface for the CVidDecoder class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_VIDDECODER_INCLUDED_
#define ELVEESTOOLS_VIDDECODER_INCLUDED_
#pragma once

//////////////////////////////////////////////////////////////////////
// VFW Codec DriverProc prototype
//////////////////////////////////////////////////////////////////////

#ifndef _DRIVERPROC_PROTO_
#define _DRIVERPROC_PROTO_

typedef LRESULT (__stdcall *funcDriverProc)(
	DWORD dwDriverId, 
	HDRVR hDriver, 
	UINT  uMsg, 
	LPARAM lParam1, 
	LPARAM lParam2);

#endif

//////////////////////////////////////////////////////////////////////
// CVidCompressor
//////////////////////////////////////////////////////////////////////

class CVidDecoder
{
public:
	CVidDecoder();
	virtual ~CVidDecoder();

	bool InitDecompressor(
		const BITMAPINFO *pbiInput,
		const BITMAPINFO *pbiOutput,
		const funcDriverProc fDriverProc = NULL);

	bool StartDecompression();
	void StopDecompression();
	void FreeDecompressor();

	bool IsFormatChanged(const BITMAPINFO *pbiFormat, long cbFormat);
	bool IsDecompressionInited() const { return m_bInited; }
	bool IsDecompressionStarted() const { return m_bStarted; }

	void* DecompressData(const void *pData, long lSize, bool bKeyframe);
	void* DecompressData(const void *pData, long lSize, unsigned uFrameSeq);

	long GetInFormat(BITMAPINFO *pbi);
	long GetOutFormat(BITMAPINFO *pbi);

	long GetBufferSize() const;
	void* GetBuffer() { return m_lpOutBuffer; }

	long GetDecompressedPitch();

protected:
	// Wrapped driver function
	LRESULT DriverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);

	// Translate the message code into something human readable
	LPCTSTR GetMessageDescription(UINT uMsg);

private:
	// Non-direct mode
	HIC	m_hic;					// VfW decoder struct

	// Decoder data
	DWORD	m_dwDriver;			// Decoder
	ICOPEN  m_icOpen;			// Decoder open struct
	HMODULE m_hDriver;			// It's module

	// Codec proc function
	funcDriverProc m_fDriverProc;

	BITMAPINFO *m_pbiInput;		// Compressed data format
	BITMAPINFO *m_pbiOutput;	// Requested decoded format

	long  m_cbOutBuffer;		// Output buffer size
	char *m_lpOutBuffer;		// Decompress to my buffer

	bool m_bInited;				// Decoder state
	bool m_bStarted;

#ifdef _DEBUG
public: // Read decoder info for debug purposes
#endif

	bool m_bWaitIFrame;					// sequence decoding
	unsigned short m_uFrameSeq;			// Last frame sequence
	unsigned short m_uIFrameSeq;		// Last key frame sequence
	
private:
	// No copies do not implement
	CVidDecoder(const CVidDecoder &rhs);
	CVidDecoder &operator=(const CVidDecoder &rhs);
};

#endif // ELVEESTOOLS_VIDDECODER_INCLUDED_
