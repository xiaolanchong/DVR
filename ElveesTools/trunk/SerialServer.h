// SerialServer.h: interface for the CSerialServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_SERIALSERVER_INCLUDED_
#define ELVEESTOOLS_SERIALSERVER_INCLUDED_

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
// CSerialServer
//////////////////////////////////////////////////////////////////////

class CSerialServer :
	protected CThread,
	protected CIOBuffer::Allocator
{
public:
	class Serial;
	friend class Serial;

	virtual ~CSerialServer();

	using CThread::Start;

	Serial* OpenSerial(LPCTSTR stPort, DWORD dwBaudrate = CBR_19200);

	void InitiateShutdown();
	void WaitForShutdownToComplete();

protected:
	class WorkerThread;
	friend class WorkerThread;

	explicit CSerialServer(
		unsigned maxFreeBuffers,
		unsigned bufferSize = 16,
		unsigned numThreads = 2);

	static void  __cdecl SetServerDataPtr(Serial *pSocket, void *pData);
	static void* __cdecl GetServerDataPtr(const Serial *pSocket);

	enum CommunicationErrorSource
	{
		EventError,
		ReadError,
		WriteError
	};

private:
	virtual int Run();

	// Override this to create your worker thread
	virtual WorkerThread *CreateWorkerThread(CIOCP &iocp);

	// Interface for derived classes to receive state change notifications...
	virtual void OnShutdownInitiated();
	virtual void OnShutdownComplete();

	virtual void OnCommunicationCreated(Serial*);
	virtual void OnCommunicationClosed(Serial*);

	virtual void ReadCompleted(Serial *pSerial, CIOBuffer *pBuffer) = 0;
	virtual void WriteCompleted(Serial *pSerial, CIOBuffer *pBuffer, DWORD dwWritten) = 0;

	virtual void OnCommunicationError(
		CommunicationErrorSource source,
		Serial *pSerial,
		CIOBuffer *pBuffer,
		DWORD lastError);

	enum IO_Operation 
	{ 
		IO_Event_Wait,
		IO_Event_Occured,
		IO_Read_Request,
		IO_Read_Completed,
		IO_Write_Request,
		IO_Write_Completed
	};

	void PostIoOperation(
		Serial *pSerial,
		CIOBuffer *pBuffer,
		IO_Operation operation);

	virtual Serial* CreateSerial();
	virtual void DestroySerial(Serial *pSerial);

	void ReleaseSerial(Serial *pSerial);
	void ReleaseSerials();

private:
	const unsigned m_numThreads;

	CCriticalSection m_serverSection;

	CIOCP m_iocp;
	CCriticalSection  m_listSection;
	CManualResetEvent m_shutdownEvent;

	TNodeList<Serial> m_portList;

	// No copies do not implement
	CSerialServer(const CSerialServer &rhs);
	CSerialServer &operator=(const CSerialServer &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSerialServer::Serial
//////////////////////////////////////////////////////////////////////

class CSerialServer::Serial : public CNodeList::Node
{
public:
	void AddRef();
	void Release();

	// called whenever event occured
	bool Read(CIOBuffer *pBuffer = 0, bool throwOnFailure = false);

	// start event monitoring
	bool WaitEvent(CIOBuffer *pBuffer = 0);

	bool Write(CIOBuffer *pBuffer, bool throwOnFailure = false);
	bool Write(const char *pData, unsigned dataLength, bool throwOnFailure = false);
	bool Write(const BYTE *pData, unsigned dataLength, bool throwOnFailure = false);

	long GetWriteCount() const;
	long GetWriteBytesCount() const;

	void Close();			// Close communication port
	void AbortiveClose();	// -/- and cancel IO operations

protected:
	friend class CSerialServer;
	friend class CSerialServer::WorkerThread;

	explicit Serial(CSerialServer &server);
	virtual ~Serial();

	void *GetServerDataPtr() const;
	void  SetServerDataPtr(void *pData);

	void Attach(HANDLE port);

	bool Open(
		LPCTSTR lpszDevice,
		DWORD dwInQueue = 0,
		DWORD dwOutQueue = 0);

	bool Setup(
		DWORD dwBaudrate = CBR_19200,
		BYTE bDataBits = 8,
		BYTE bParity   = NOPARITY,
		BYTE bStopBits = ONESTOPBIT);

	bool SetEventChar(BYTE bEventChar, bool fAdjustMask = true);

	bool SetMask(DWORD dwMask = EV_BREAK | EV_ERR | EV_RXCHAR);

	// <0 - Software handshaking (XON/XOFF)
	// =0 - No handshaking
	// >0 - Hardware handshaking (RTS/CTS)
	bool SetupHandshaking(long lHandshake = 0);

	bool SetupReadTimeouts(bool bBlocking = false);

	void Break();			// Send a break
	bool Purge();			// Purge all buffers
	bool CancelCommIo(); 	// CancelIo operations

	DWORD GetCommError(bool throwOnFailure = false);	// Obtain the error
	DWORD GetCommStatus(bool throwOnFailure = false);	// Obtain modem status

	void InternalClose();
	bool IsValid();

private:
	void OnCommunicationError(
		CommunicationErrorSource source,
		CIOBuffer *pBuffer,
		DWORD lastError);

	void WritePending(long lBytes);
	void WriteCompleted(long lBytes);

	unsigned GetReadSequenceNumber();
	unsigned GetWriteSequenceNumber();

private:
	void  *m_pServerData;

	long m_ref;
	long m_outstandingWrites;
	long m_outstandingBytes;

	HANDLE m_port;
	bool m_closing;
		
	CSerialServer &m_server;
	CCriticalSection m_crit;

	unsigned m_ReadSequenceNo;
	unsigned m_WriteSequenceNo;

	CIOBuffer::InOrderBufferList m_outOfSequenceWrites;

	// No copies do not implement
	Serial(const Serial &rhs);
	Serial &operator=(const Serial &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSerialServer::WorkerThread
//////////////////////////////////////////////////////////////////////

class CSerialServer::WorkerThread : public CNodeList::Node, protected CThread
{
protected:
	friend class CSerialServer;
	explicit WorkerThread(CSerialServer &server, CIOCP &iocp);

	void WaitForShutdownToComplete();

private:
	virtual int Run();

	void HandleOperation(
		CSerialServer::Serial *pSerial,
		CIOBuffer *pBuffer,
		DWORD dwIoSize,
		bool weClosedPort);

	void WaitEvent(
		Serial *pSerial,
		CIOBuffer *pBuffer) const;

	void Read(
		CSerialServer::Serial *pSerial,
		CIOBuffer *pBuffer) const;

	void Write(
		CSerialServer::Serial *pSerial,
		CIOBuffer *pBuffer) const;

private:
	CIOCP &m_iocp;
	CSerialServer &m_server;

	// No copies do not implement
	WorkerThread(const WorkerThread &rhs);
	WorkerThread &operator=(const WorkerThread &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_SERIALSERVER_INCLUDED_
