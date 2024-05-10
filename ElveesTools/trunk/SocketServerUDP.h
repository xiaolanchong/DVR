// SocketServerUDP.h: interface for the CSocketServerUDP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_SOCKETSERVERUDP_INCLUDED_
#define ELVEESTOOLS_SOCKETSERVERUDP_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include "Thread.h"
#include "IOBuffer.h"
#include "IOCP.h"
#include "Events.h"
#include "CriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP
//////////////////////////////////////////////////////////////////////

class CSocketServerUDP : protected CIOBuffer::Allocator
{
public:
	explicit CSocketServerUDP(unsigned maxFreeBuffers, unsigned numThreads);
	virtual ~CSocketServerUDP();

	void Start();
	void Stop();

protected:
	class Socket;
	friend class Socket;

	class WorkerThread;
	friend class WorkerThread;

	// Add datagramm socket to CSocketServerUDP framework
	// For read only socket stDstAddr can be NULL

	Socket* InitSocket(SOCKET hSocket, SOCKADDR_IN* pAddr);

	// Notifications
	virtual void OnStart() {}
	virtual void OnStop()  {}

	// CIOBuffer::Allocator
	virtual void OnBufferCreated()   {}
	virtual void OnBufferAllocated() {}
	virtual void OnBufferReleased()  {}
	virtual void OnBufferDestroyed() {}

	// CSocketServerUDP::WorkerThread
	virtual void OnThreadBeginProcessing() {}
	virtual void OnThreadEndProcessing()   {}

	// Server notifications
	virtual void ReadCompleted(Socket *pSocket, CIOBuffer *pBuffer) = 0;
	virtual void WriteCompleted(Socket *pSocket, CIOBuffer *pBuffer);

private:
//	virtual int Run();

	// Override this to create your worker thread
	virtual WorkerThread *CreateWorkerThread(CIOCP &iocp);

	// Override this to create your socket
	virtual Socket* CreateSocket();

	// Create new framework socket
	Socket* AllocateSocket();
	void ReleaseSocket(Socket *pSocket);
	void ReleaseSockets();

	enum IO_Operation 
	{ 
		IO_Read_Request,
		IO_Read_Completed,
		IO_Write_Request,
		IO_Write_Completed
	};

	void PostIoOperation(
		Socket *pSocket,
		CIOBuffer *pBuffer,
		IO_Operation operation);

private:
	const unsigned m_numThreads;

	TNodeList<Socket> m_socketList;
	TNodeList<WorkerThread> m_workerList;
	
	CIOCP m_iocp;
	CCriticalSection m_listSection;

	// No copies do not implement
	CSocketServerUDP(const CSocketServerUDP &rhs);
	CSocketServerUDP &operator=(const CSocketServerUDP &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP::Socket
//////////////////////////////////////////////////////////////////////

class CSocketServerUDP::Socket : public CNodeList::Node
{
private:
	explicit Socket(CSocketServerUDP &server);
	virtual ~Socket();

	void Attach(SOCKET socket);

	friend class CSocketServerUDP;
	friend class CSocketServerUDP::WorkerThread;

public:
	void AddRef();
	void Release();

	void Read(CIOBuffer *pBuffer = 0);
	void Write(CIOBuffer *pBuffer);
	void Write(const char *pData, unsigned dataLength);
	
private:
	CSocketServerUDP &m_server;
	
	SOCKET m_socket;
	SOCKADDR_IN m_fromAddr;

	long m_ref;

	// No copies do not implement
	Socket(const Socket &rhs);
	Socket &operator=(const Socket &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP::WorkerThread
//////////////////////////////////////////////////////////////////////

class CSocketServerUDP::WorkerThread : public CNodeList::Node, protected CThread
{
public:
	virtual ~WorkerThread();

protected:
	friend class CSocketServerUDP;
	explicit WorkerThread(CSocketServerUDP &server, CIOCP &iocp);

	void WaitForShutdown();

private:
	virtual int Run();

	void Read(
		CSocketServerUDP::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	void Write(
		CSocketServerUDP::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	CIOCP &m_iocp;
	CSocketServerUDP &m_server;

	// No copies do not implement
	WorkerThread(const WorkerThread &rhs);
	WorkerThread &operator=(const WorkerThread &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_SOCKETSERVERUDP_INCLUDED_
