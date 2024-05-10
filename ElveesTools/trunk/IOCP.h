// IOCP.h: interface for the CIOCP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_IOCP_INCLUDED_
#define ELVEESTOOLS_IOCP_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CIOCP
//////////////////////////////////////////////////////////////////////

class CIOCP
{
public:
	explicit CIOCP(unsigned maxConcurrency);
	~CIOCP();

	void AssociateDevice(
		HANDLE hDevice,
		DWORD completionKey);

	void PostStatus(
		DWORD completionKey,
		DWORD dwNumBytes = 0,
		OVERLAPPED *pOverlapped = 0);

	bool GetStatus(
		DWORD *pCompletionKey,
		PDWORD pdwNumBytes,
		OVERLAPPED **ppOverlapped,
		DWORD dwMilliseconds = INFINITE);

private:
	HANDLE m_iocp;

	// No copies do not implement
	CIOCP(const CIOCP &rhs);
	CIOCP &operator=(const CIOCP &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees 

#endif // ELVEESTOOLS_IOCP_INCLUDED_
