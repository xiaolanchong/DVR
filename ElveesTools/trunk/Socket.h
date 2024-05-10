// Socket.h: interface for the CSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_SOCKET_INCLUDED_
#define ELVEESTOOLS_SOCKET_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CSocket
//////////////////////////////////////////////////////////////////////

class CSocket
{
public:
	class InternetAddress;

	// Construction

	explicit CSocket(SOCKET theSocket);
	CSocket();

	virtual ~CSocket();

	bool Create(
		int nAF = AF_INET,						// Address family specification
		int nType = SOCK_STREAM,				// Type specification
		int nProtocol = IPPROTO_IP,				// Protocol to be used with the socket
		LPWSAPROTOCOL_INFO lpInfo = NULL,		//
		DWORD dwFlags = WSA_FLAG_OVERLAPPED);	// Flag that specifies the socket attribute

	// Attributes

	SOCKET m_socket;
	operator SOCKET() const { return m_socket; }

	bool IsValid();

	void Attach(SOCKET theSocket);
	SOCKET Detatch();

	bool ReuseAddress(bool reuse = true);

	bool SetSockOpt(int nOptionName, const void* lpOptionValue,
		int nOptionLen, int nLevel = SOL_SOCKET);

	// Operations
	
	bool Listen(int backlog);

	bool Bind(const SOCKADDR_IN &address);
	bool Bind(const struct sockaddr &address, unsigned addressLength);

	bool Connect(const SOCKADDR_IN &address);
	bool Connect(const struct sockaddr &address, unsigned addressLength);

	bool Shutdown(int how);

	void Close();
	void AbortiveClose();

private:

	// No copies do not implement
	CSocket(const CSocket &rhs);
	CSocket &operator=(const CSocket &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSocket::InternetAddress
//////////////////////////////////////////////////////////////////////

class CSocket::InternetAddress : public SOCKADDR_IN
{
public:
	InternetAddress(
		const unsigned long address,
		const unsigned short port);

	InternetAddress(
		LPCTSTR lpAddress,
		const unsigned short port);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_SOCKET_INCLUDED_
