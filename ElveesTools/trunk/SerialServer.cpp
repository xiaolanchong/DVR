// SerialServer.cpp: implementation of the CSerialServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialServer.h"
#include "Exceptions.h"
#include "SystemInfo.h"

#define DUMPPACKETS

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CSerialServer
//////////////////////////////////////////////////////////////////////

CSerialServer::CSerialServer(
	unsigned maxFreeBuffers,
	unsigned bufferSize,
	unsigned numThreads)
	: CIOBuffer::Allocator(bufferSize, maxFreeBuffers)
	, m_numThreads(numThreads)
	, m_iocp(0)
{
	if(numThreads == 0)
	{
		throw CException(TEXT("CSerialServer::CSerialServer()"), TEXT("numThreads = 0"));
	}
}

CSerialServer::~CSerialServer()
{
}

CSerialServer::Serial* CSerialServer::OpenSerial(LPCTSTR stPort, DWORD dwBaudrate)
{
	Serial* pSerial = CreateSerial();

	if(!pSerial)
	{
		Output(TError, TEXT("Memory operation failed"));
		return NULL;
	}

	do
	{
		if(!pSerial->Open(stPort))
			break;

		if(!pSerial->SetMask())
		{
			OutputF(TError, TEXT("SetMask failed with error %ld"), ::GetLastError());
			break;
		}

		if(!pSerial->Setup(dwBaudrate, 8, NOPARITY, ONESTOPBIT))
			break;

		if(!pSerial->SetupReadTimeouts())
			break;

		if(!pSerial->SetupHandshaking())
			break;

		{
			CCriticalSection::Owner lock(m_listSection);
			m_portList.PushNode(pSerial);
		}

		m_iocp.AssociateDevice(pSerial->m_port, (DWORD)(DWORD_PTR)pSerial);

		OnCommunicationCreated(pSerial);

		pSerial->WaitEvent();
		pSerial->AddRef();
		return pSerial;

	}
	while(false);

	pSerial->Release();
	
	return NULL;
}

CSerialServer::WorkerThread *CSerialServer::CreateWorkerThread(CIOCP &iocp)
{
	return new WorkerThread(*this, iocp);
}

CSerialServer::Serial* CSerialServer::CreateSerial()
{
	return new Serial(*this);
}

void CSerialServer::DestroySerial(CSerialServer::Serial *pSerial)
{
	delete pSerial;
}

void CSerialServer::ReleaseSerials()
{
	CCriticalSection::Owner lock(m_listSection);

	while(m_portList.Head())
		ReleaseSerial(m_portList.Head());
}

void CSerialServer::ReleaseSerial(Serial *pSerial)
{
	if(!pSerial)
	{
		Output(TWarning, TEXT("CSerialServer::ReleaseSerial - pSerial is null"));
		return;
	}

	pSerial->Close();

	{
		CCriticalSection::Owner lock(m_listSection);
		pSerial->RemoveFromList();
	}

	DestroySerial(pSerial);
}

void CSerialServer::InitiateShutdown()
{
	// signal that the dispatch thread should
	// shut down all worker threads and then exit
	
	m_shutdownEvent.Set();

	CCriticalSection::Owner lock(m_listSection);
	Serial *pSerial = m_portList.Head();

	while(pSerial)
	{
		Serial *pNext = m_portList.Next(pSerial);

		pSerial->CancelCommIo();
		pSerial = pNext;
	}
}

void CSerialServer::WaitForShutdownToComplete()
{
	// if we haven't already started a shut down, do so...

	if(!m_shutdownEvent.Wait(0))
		InitiateShutdown();

	Wait();

	ReleaseSerials();
	Flush();
}

int CSerialServer::Run()
{
	try
	{
		WorkerThread *pThread;	
		TNodeList<WorkerThread> workerlist;

		for(unsigned i = 0; i < m_numThreads; ++i)
		{
			pThread = CreateWorkerThread(m_iocp);

			workerlist.PushNode(pThread);

			pThread->Start();
		}

		m_shutdownEvent.Wait();

		// Initialize shutdown
		for(unsigned i = 0; i < workerlist.Count(); ++i)
			m_iocp.PostStatus(0);

		while(!workerlist.Empty())
		{
			pThread = workerlist.PopLastNode();
			pThread->WaitForShutdownToComplete();

			delete pThread;
		}
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSerialServer::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}

	if(m_shutdownEvent.Wait(0))
		OnShutdownComplete();

	return 0;
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

void CSerialServer::PostIoOperation(Serial *pSerial, CIOBuffer *pBuffer, IO_Operation operation)
{
	pBuffer->SetOperation(operation);
	pBuffer->AddRef();

	pSerial->AddRef();

	m_iocp.PostStatus((DWORD)(DWORD_PTR)pSerial, 0, pBuffer);
}

//////////////////////////////////////////////////////////////////////
// CSerialServer default notifications handlers
//////////////////////////////////////////////////////////////////////

void CSerialServer::OnShutdownInitiated()
{
	DEBUG_ONLY(Output(TTrace, TEXT("Shutdown initiated")));
}

void CSerialServer::OnShutdownComplete()
{
	DEBUG_ONLY(Output(TTrace, TEXT("Shutdown complete")));
}

void CSerialServer::OnCommunicationCreated(Serial* pSerial)
{
	DEBUG_ONLY(OutputF(TTrace, TEXT("Communication created [%08lX]"), pSerial));
}

void CSerialServer::OnCommunicationClosed(Serial* pSerial)
{
	DEBUG_ONLY(OutputF(TTrace, TEXT("Communication closed [%08lX]"), pSerial));
}

void CSerialServer::OnCommunicationError(
	CommunicationErrorSource source,
	Serial *pSerial,
	CIOBuffer *pBuffer,
	DWORD lastError)
{
}

void  __cdecl CSerialServer::SetServerDataPtr(Serial *pSerial, void *pData)
{
	pSerial->SetServerDataPtr(pData);
}

void* __cdecl CSerialServer::GetServerDataPtr(const Serial *pSerial)
{
	return pSerial->GetServerDataPtr();
}

//////////////////////////////////////////////////////////////////////////////
// CSerialServer::Serial
//////////////////////////////////////////////////////////////////////

CSerialServer::Serial::Serial(CSerialServer &server)
	: m_outOfSequenceWrites(m_crit)
	, m_ref(1)
	, m_port(0)
	, m_server(server)
{
	m_ReadSequenceNo  = 0;
	m_WriteSequenceNo = 0;

	m_outstandingWrites = 0;
	m_outstandingBytes  = 0;

	m_closing = false;
	m_pServerData = NULL;
}

CSerialServer::Serial::~Serial()
{
}

void CSerialServer::Serial::Attach(HANDLE thePort)
{
	if(IsValid())
	{
		throw CException(TEXT("CSerialServer::Serial::Attach()"), TEXT("Serial already attached"));
	}

	m_port = thePort;

	m_outstandingWrites = 0;
	m_closing = false;
}

void CSerialServer::Serial::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CSerialServer::Serial::Release()
{
	if(0 == ::InterlockedDecrement(&m_ref))
	{
		if(IsValid())
		{
			AddRef();

			if(!m_closing)
			{
				m_closing = true;
				Close();
			}
			else
			{
				AbortiveClose();
			}

			Release();
			return;
		}

		m_server.ReleaseSerial(this);
	}
}

void CSerialServer::Serial::WritePending(long lBytes)
{
	InterlockedIncrement(&m_outstandingWrites);
	InterlockedExchangeAdd(&m_outstandingBytes, lBytes);
}

void CSerialServer::Serial::WriteCompleted(long lBytes)
{
	InterlockedDecrement(&m_outstandingWrites);
	InterlockedExchangeAdd(&m_outstandingBytes, -lBytes);
}

long CSerialServer::Serial::GetWriteCount() const
{
	return ::InterlockedExchange(
		const_cast<long volatile*>(&m_outstandingWrites), m_outstandingWrites);
}

long CSerialServer::Serial::GetWriteBytesCount() const
{
	return ::InterlockedExchange(
		const_cast<long volatile*>(&m_outstandingBytes), m_outstandingBytes);
}

bool CSerialServer::Serial::Read(CIOBuffer *pBuffer, bool throwOnFailure)
{
	if(!IsValid())
	{
		if(throwOnFailure)
			throw CException(TEXT("CSerialServer::Serial::Read()"), TEXT("Serial is closed"));
		else
			return false;
	}

	// Post a read request to the iocp so that the actual socket read gets performed by
	// one of the server's IO threads...

	if(!pBuffer)
	{
		pBuffer = m_server.Allocate();
	}
	else
	{
		pBuffer->AddRef();
	}

	m_server.PostIoOperation(this, pBuffer, IO_Read_Request);

	pBuffer->Release();

	return true;
}

bool CSerialServer::Serial::Write(const BYTE *pData, unsigned dataLength, bool throwOnFailure)
{
	return Write(reinterpret_cast<const char*>(pData), dataLength, throwOnFailure);
}

bool CSerialServer::Serial::Write(const char *pData, unsigned dataLength, bool throwOnFailure)
{
	if(!IsValid())
	{
		if(throwOnFailure)
			throw CException(TEXT("CSerialServer::Serial::Write()"), TEXT("Serial is closed"));
		else
			return false;
	}

	WritePending(dataLength);

	CIOBuffer *pBuffer = m_server.Allocate();

	pBuffer->AddData(pData, dataLength);
	pBuffer->SetSequenceNumber(GetWriteSequenceNumber());

	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

	pBuffer->Release();

	return true;
}

bool CSerialServer::Serial::Write(CIOBuffer *pBuffer,  bool throwOnFailure)
{
	if(!IsValid())
	{
		if(throwOnFailure)
			throw CException(TEXT("CSerialServer::Serial::Write()"), TEXT("Serial is closed"));
		else
			return false;
	}

	WritePending(pBuffer->GetUsed());

	pBuffer->SetSequenceNumber(GetWriteSequenceNumber());

	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

	return true;
}

bool CSerialServer::Serial::WaitEvent(CIOBuffer *pBuffer)
{
	if(!pBuffer)
		pBuffer = m_server.Allocate();
	else
		pBuffer->AddRef();

	m_server.PostIoOperation(this, pBuffer, IO_Event_Wait);

	pBuffer->Release();
	return true;
}

bool CSerialServer::Serial::Open(LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue)
{
	m_port = ::CreateFile(lpszDevice,
		GENERIC_READ|GENERIC_WRITE, // access ( read and write)
		0,							// (share) 0:cannot share the COM port						
		0,							// security  (None)				
		OPEN_EXISTING,				// creation : open_existing
		FILE_FLAG_OVERLAPPED,		// we want overlapped operation
		0);							// no templates file for COM port...

	if(m_port == INVALID_HANDLE_VALUE)
	{
		// Reset file handle
		m_port = 0;

		DWORD dwErr = ::GetLastError();
		
		switch(dwErr)
		{
		case ERROR_FILE_NOT_FOUND:
			OutputF(TError, TEXT("The specified COM-port \"%s\" does not exist"), lpszDevice);
			break;

		case ERROR_ACCESS_DENIED:
			OutputF(TError, TEXT("The specified COM-port is in use"), lpszDevice);
			break;

		default:
			OutputF(TError, TEXT("Fail to open specified COM-port \"%s\" err=0x%lx"), lpszDevice, dwErr);
			break;
		}

		return false;
	}

	if(dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least
		// 16 bytes large.

		dwInQueue = max(16, dwInQueue);
		dwOutQueue = max(16, dwOutQueue);

		if(!::SetupComm(m_port, dwInQueue, dwOutQueue))
		{
			OutputF(TWarning, TEXT("Serial::Open - Unable to setup the COM-port (in=%ld, out=%ld) err=0x%lx"),
				dwInQueue, dwOutQueue, ::GetLastError());

			CloseHandle(m_port);
			return false;	
		}
	}

#if 0
	COMMCONFIG commConfig;
	DWORD dwSize = sizeof(commConfig);

	ZeroMemory(&commConfig, dwSize);
	commConfig.dwSize = dwSize;

	if(::GetDefaultCommConfig(lpszDevice, &commConfig, &dwSize))
	{
		OutputF(TInfo, TEXT("Comm: \"%s\", Default: BR=%ld,BS=%ld,P=%ld,SB=%ld"), 
			lpszDevice,
			commConfig.dcb.BaudRate,
			commConfig.dcb.ByteSize,
			commConfig.dcb.Parity,
			commConfig.dcb.StopBits);
	}
#endif

	return true;
}

bool CSerialServer::Serial::Setup(DWORD dwBaudrate, BYTE bDataBits, BYTE bParity, BYTE bStopBits)
{
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);

	if(!::GetCommState(m_port, &dcb))
	{
		Output(TWarning, TEXT("Serial::Setup - Unable to obtain DCB information"));
		return false;
	}

	// Set the new data
	dcb.BaudRate = dwBaudrate;
	dcb.ByteSize = bDataBits;
	dcb.Parity   = bParity;
	dcb.StopBits = bStopBits;

	// Determine if parity is used
	dcb.fParity  = (bParity != NOPARITY);

	// Set the new DCB structure
	if(!::SetCommState(m_port, &dcb))
	{
		OutputF(TWarning, TEXT("Serial::Setup - Unable to set DCB information err=0x%lx"), GetLastError());
		return false;
	}

	return true;
}

bool CSerialServer::Serial::SetEventChar(BYTE bEventChar, bool fAdjustMask)
{
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);

	if(!::GetCommState(m_port, &dcb))
	{
		Output(TWarning, TEXT("Serial::SetEventChar - Unable to obtain DCB information"));
		return false;
	}

	// Set the new event character
	dcb.EvtChar = bEventChar;

	// Adjust the event mask, to make sure the event will be received
	if(fAdjustMask)
	{
		// Enable 'receive event character' event.  Note that this
		// will generate an EEventNone if there is an asynchronous
		// WaitCommEvent pending.

		DWORD dwMask;
		if(!GetCommMask(m_port, &dwMask))
		{
			Output(TWarning, TEXT("Serial::SetEventChar - Unable to obtain event mask"));
			return false;
		}

		if(!SetMask(dwMask | EV_RXFLAG))
		{
			OutputF(TWarning, TEXT("Serial::SetEventChar - Unable to set event mask err=0x%lx"), GetLastError());
			return false;
		}
	}

	// Set the new DCB structure
	if(!::SetCommState(m_port, &dcb))
	{
		OutputF(TWarning, TEXT("Serial::SetEventChar - Unable to set DCB information err=0x%lx"), GetLastError());
		return false;
	}

	return true;
}

bool CSerialServer::Serial::SetMask(DWORD dwMask)
{
	return ::SetCommMask(m_port, dwMask) ? true : false;
}

bool CSerialServer::Serial::SetupHandshaking(long lHandshake)
{
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);

	if(!::GetCommState(m_port, &dcb))
	{
		Output(TWarning, TEXT("Serial::SetupHandshaking - Unable to obtain DCB information"));
		return false;
	}

	if(lHandshake == 0) // Handshake Off
	{
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
	}
	else if(lHandshake > 0) // Handshake Hardware
	{
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
	}
	else // Handshake Software
	{
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
	}

	// Set the new DCB structure
	if(!::SetCommState(m_port, &dcb))
	{
		OutputF(TWarning, TEXT("Serial::SetupHandshaking - Unable to set DCB information err=0x%lx"), GetLastError());
		return false;
	}

	return true;
}

bool CSerialServer::Serial::SetupReadTimeouts(bool bBlocking)
{
	// Determine the time-outs
	COMMTIMEOUTS cto;

	if(!::GetCommTimeouts(m_port, &cto))
	{
		Output(TWarning, TEXT("Serial::SetupReadTimeouts - Unable to obtain timeout information"));
		return false;
	}

	// Set the new timeouts
	if(bBlocking)
	{
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
	}
	else
	{
		// Setting the members of the COMMTIMEOUTS structure
		// in the following manner causes read operations to complete
		// immediately without waiting for any new data to arrive

		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
	}
	
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;

	// Set the new DCB structure
	if (!::SetCommTimeouts(m_port, &cto))
	{
		OutputF(TWarning, TEXT("Serial::SetupReadTimeouts - Unable to set timeout information err=0x%lx"), GetLastError());
		return false;
	}

	return true;
}

void CSerialServer::Serial::Break()
{
	// Set the RS-232 port in break mode for a little while
	::SetCommBreak(m_port);
	::Sleep(100);
	::ClearCommBreak(m_port);
}

bool CSerialServer::Serial::Purge()
{
	return ::PurgeComm(m_port, PURGE_TXCLEAR | PURGE_RXCLEAR) ? true : false;
}

bool CSerialServer::Serial::CancelCommIo()
{
	// Cancel the I/O request
	return ::CancelIo(m_port) ? true : false;
}

DWORD CSerialServer::Serial::GetCommStatus(bool throwOnFailure)
{
	DWORD dwStatus = 0;

	if(!::GetCommModemStatus(m_port, &dwStatus))
	{
		if(throwOnFailure)
			throw CWin32Exception(TEXT("Serial::GetCommStatus()"), ::GetLastError());
		else
			OutputF(TWarning, TEXT("Serial::GetCommStatus() - Unable to obtain the modem status"), ::GetLastError());
	}

	return dwStatus;
}

DWORD CSerialServer::Serial::GetCommError(bool throwOnFailure)
{
	DWORD dwErrors = 0;

	if(!::ClearCommError(m_port, &dwErrors, 0))
	{
		if(throwOnFailure)
			throw CWin32Exception(TEXT("Serial::GetCommError()"), ::GetLastError());
		else
			OutputF(TWarning, TEXT("Serial::GetCommError() - Unable to obtain COM status"), ::GetLastError());
	}

	return dwErrors;
}

void CSerialServer::Serial::Close()
{
	CCriticalSection::Owner lock(m_crit);

	if(IsValid())
	{
		if(!::CloseHandle(m_port))
			OutputF(TError,	TEXT("CSerialServer::Serial::Close() - CloseHandle err=0x%lx"), ::GetLastError());

		::InterlockedExchange(reinterpret_cast<long volatile*>(&m_port), 0);

		m_server.OnCommunicationClosed(this);
	}
}

void CSerialServer::Serial::AbortiveClose()
{
	if(IsValid())
	{
		if(!CancelCommIo())
			OutputF(TError,	TEXT("CSerialServer::Serial::AbortiveClose() - CancelCommIo() err=0x%lx"), ::GetLastError());

		Close();
	}
}

bool CSerialServer::Serial::IsValid()
{
	return (0 != ::InterlockedExchange(
		reinterpret_cast<long volatile*>(&m_port), (LONG)(LONG_PTR)m_port));
}

void* CSerialServer::Serial::GetServerDataPtr() const
{
	return InterlockedExchange_Pointer(&m_pServerData, m_pServerData);
}

void CSerialServer::Serial::SetServerDataPtr(void *pData)
{
	InterlockedExchange_Pointer(&m_pServerData, pData);
}

unsigned CSerialServer::Serial::GetReadSequenceNumber()
{
	return InterlockedExchangeAdd(
		reinterpret_cast<volatile long*>(&m_ReadSequenceNo), 1);
}

unsigned CSerialServer::Serial::GetWriteSequenceNumber()
{
	return InterlockedExchangeAdd(
		reinterpret_cast<volatile long*>(&m_WriteSequenceNo), 1);
}

void CSerialServer::Serial::OnCommunicationError(
	CommunicationErrorSource source,
	CIOBuffer *pBuffer,
	DWORD lastError)
{
	// Communication terminated by user
	// cable, device or other reason...
	if(ERROR_GEN_FAILURE == lastError || ERROR_DEVICE_NOT_CONNECTED == lastError)
	{
		// Shutdown(SD_RECEIVE);
		AbortiveClose();
	}
	else if(ERROR_OPERATION_ABORTED == lastError)
	{
		Close();
	}
	// if(!IsValid())// && WSAENOTSOCK == lastError)
	// Swallow this error as we expect it...
	else
	{
		DEBUG_ONLY(OutputF(TTrace,	TEXT("%s error (%ld): Buffer=0x%p"),
			source == EventError ? TEXT("Event") :
			source == ReadError  ? TEXT("Read") :
			source == WriteError ? TEXT("Write") : TEXT("Unknown"),
			lastError, pBuffer));

		m_server.OnCommunicationError(source, this, pBuffer, lastError);
	}
}

//////////////////////////////////////////////////////////////////////
// CSerialServer::WorkerThread
//////////////////////////////////////////////////////////////////////

CSerialServer::WorkerThread::WorkerThread(CSerialServer &server, CIOCP &iocp)
	: m_server(server)
	, m_iocp(iocp)
{
}

void CSerialServer::WorkerThread::WaitForShutdownToComplete()
{
	Wait();
}

int CSerialServer::WorkerThread::Run()
{
	try
	{
		while(true) // continually loop to service io-completion packets
		{
			DWORD dwIoSize = 0;
			Serial *pSerial = 0;
			CIOBuffer *pBuffer = 0;

			bool weClosedPort = false;

			try
			{
				m_iocp.GetStatus((DWORD*)&pSerial, &dwIoSize, (OVERLAPPED**)&pBuffer);
			}
			catch(const CWin32Exception &e)
			{
				if( ERROR_GEN_FAILURE == e.GetError() ||
					ERROR_DEVICE_NOT_CONNECTED == e.GetError())
				{
					DEBUG_ONLY(Output(TTrace, TEXT("A device attached to the system is not functioning or  not connected.")));
				}
				else if(ERROR_OPERATION_ABORTED == e.GetError())
				{
					DEBUG_ONLY(Output(TTrace, TEXT("IOCP error - Communication dropped")));
				}
				else if(ERROR_NOACCESS == e.GetError())
				{
					DEBUG_ONLY(Output(TTrace, TEXT("IOCP error - Invalid access to memory location.")));
					continue;
				}
				else
				{
					DEBUG_ONLY(OutputF(TTrace, TEXT("KILL YOURSELF err=%ld"), e.GetError()));
					throw;
				}
			}

			if(!pSerial)
			{
				// A completion key of 0 is posted to the iocp
				// to request us to shut down...
				break;
			}

			if(pSerial->IsValid())
			{
				HandleOperation(pSerial, pBuffer, dwIoSize, weClosedPort);
			}
			else // Skip operations if port invalid...
			{
				pSerial->Release();
				pBuffer->Release();
			}
		} 
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSerialServer::WorkerThread::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}

	return 0;
}

void CSerialServer::WorkerThread::HandleOperation(
	Serial *pSerial,
	CIOBuffer *pBuffer,
	DWORD dwIoSize,
	bool weClosedPort)
{
#if defined(DUMPPACKETS) && defined(_DEBUG)
	TCHAR stDump[256];
#endif

	if(pBuffer)
	{
		const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

		switch(operation)
		{
		case IO_Event_Wait:
			{
				WaitEvent(pSerial, pBuffer);

				pSerial->Release();
				pBuffer->Release();
			}
			break;

		case IO_Event_Occured:
			{
				try
				{
					if(dwIoSize == 0)
						throw CWin32Exception(TEXT("An error occurs while WaitCommEvent"), ::GetLastError());

					if(dwIoSize & EV_BREAK)
						Output(TInfo, TEXT("Event: BREAK"));

					if(dwIoSize & EV_RING)
						Output(TInfo, TEXT("Event: RING"));

					if(dwIoSize & EV_RXCHAR)
						Output(TError, TEXT("Event: RXCHAR"));

					if(dwIoSize & EV_ERR)
					{
						switch(pSerial->GetCommError(true))
						{
						case CE_BREAK:
							Output(TError, TEXT("Event: Break condition"));
							break;
						case CE_FRAME:
							Output(TError, TEXT("Event: Framing error"));
							break;
						case CE_IOE:
							Output(TError, TEXT("Event: IO device error"));
							break;
						case CE_MODE:
							Output(TError, TEXT("Event: Unsupported mode"));
							break;
						case CE_OVERRUN:
							Output(TError, TEXT("Event: Buffer overrun"));
							break;
						case CE_RXOVER:
							Output(TError, TEXT("Event: Input buffer overflow"));
							break;
						case CE_RXPARITY:
							Output(TError, TEXT("Event: Input parity error"));
							break;
						case CE_TXFULL:
							Output(TError, TEXT("Event: Output buffer full"));
							break;
						default:
							Output(TError, TEXT("Event: Unknown"));
							break;
						}
					}

					if(dwIoSize & (EV_CTS|EV_DSR|EV_RLSD))
					{
						DWORD dwStatus = pSerial->GetCommStatus(true);

						if(dwIoSize & EV_CTS)
							OutputF(TInfo, TEXT("Event: Clear to send %s"), (dwStatus & MS_CTS_ON) != 0 ? TEXT("on") : TEXT("off"));

						if(dwIoSize & EV_DSR)
							OutputF(TInfo, TEXT("Event: Data set ready %s"), (dwStatus & MS_DSR_ON) != 0 ? TEXT("on") : TEXT("off"));

						if(dwIoSize & EV_RLSD)
							OutputF(TInfo, TEXT("Event: RLSD/CD %s"), (dwStatus & MS_DSR_ON) != 0 ? TEXT("on") : TEXT("off"));
					}

					// Post read operation...
					pSerial->Read();

					// Wait ne w event from system...
					WaitEvent(pSerial, pBuffer);
				}
				catch(const CWin32Exception& e)
				{
					pSerial->OnCommunicationError(EventError, pBuffer, e.GetError());
				}

				pBuffer->Release();
				pSerial->Release();
			}
			break;

		case IO_Read_Request:
			{
				Read(pSerial, pBuffer);

				pSerial->Release();
				pBuffer->Release();
			}
			break;

		case IO_Read_Completed:
			{
				// Communication port send event on every char
				// if dwIoSize = 0 it meens that we already read
				// all buffer in previous request...

				pBuffer->Use(dwIoSize);

			#if defined(DUMPPACKETS) && defined(_DEBUG)
				if(0 != dwIoSize && dwIoSize < pBuffer->GetSize())
				{
					DumpData(pBuffer->GetBuffer(), dwIoSize, stDump, countof(stDump));

					OutputF(TTrace, TEXT("Read: %d bytes\n%s"),	dwIoSize, stDump);
				}
			#endif

				// Even if no bytes readed we must to provide
				// this buffer to ReadCompleted... 
				// because of read sequence number

				if(IsBadReadPtr(pBuffer, sizeof(CIOBuffer)))
					DebugBreak();

				m_server.ReadCompleted(pSerial, pBuffer);

				pSerial->Release();
				pBuffer->Release();
			}
			break;

		case IO_Write_Request:
			{
				Write(pSerial, pBuffer);

				pSerial->Release();
				pBuffer->Release();
			}
			break;

		case IO_Write_Completed:
			{
				if(pBuffer->GetUsed() != dwIoSize)
					Output(TWarning, TEXT("CSerialServer::IO_Write_Completed - not all data was written"));

				pSerial->WriteCompleted(dwIoSize);
				m_server.WriteCompleted(pSerial, pBuffer, dwIoSize);

				pBuffer->Release();
				pSerial->Release();
			}
			break;

		default:
			Output(TWarning, TEXT("CSerialServer::WorkerThread::HandleOperation() - Unexpected operation"));
			break;
		}
	}
	else
	{
		Output(TWarning, TEXT("CSerialServer::WorkerThread::HandleOperation() - Unexpected - pBuffer is 0"));
	}
}

void CSerialServer::WorkerThread::WaitEvent(Serial *pSerial, CIOBuffer *pBuffer) const
{
	ZeroMemory(pBuffer, sizeof(OVERLAPPED));

	pSerial->AddRef();
	pBuffer->AddRef();

	pBuffer->SetOperation(IO_Event_Occured);
	pBuffer->SetSequenceNumber(0);

	DWORD dwEvtMask = 0;

	if(!WaitCommEvent(pSerial->m_port, &dwEvtMask, pBuffer))
	{
		DWORD lastError = ::GetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			pSerial->OnCommunicationError(EventError, pBuffer, lastError);

			pSerial->Release();
			pBuffer->Release();
		}
	}
}

void CSerialServer::WorkerThread::Read(Serial *pSerial, CIOBuffer *pBuffer) const
{
	ZeroMemory(pBuffer, sizeof(OVERLAPPED));

	pSerial->AddRef();
	pBuffer->AddRef();
	
	pBuffer->SetOperation(IO_Read_Completed);
	pBuffer->SetSequenceNumber(pSerial->GetReadSequenceNumber());

	if(!ReadFile(
		pSerial->m_port,
		const_cast<BYTE *>(pBuffer->GetBuffer()),
		pBuffer->GetSize(),
		NULL,
		pBuffer))
	{
		DWORD lastError = ::GetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			pSerial->OnCommunicationError(ReadError, pBuffer, lastError);

			pSerial->Release();
			pBuffer->Release();
		}
	}
}

void CSerialServer::WorkerThread::Write(Serial *pSerial, CIOBuffer *pBuffer) const
{
	pSerial->AddRef();
	pBuffer->AddRef();

	pBuffer->SetOperation(IO_Write_Completed);

	// Write in order
	pBuffer = pSerial->m_outOfSequenceWrites.GetNext(pBuffer);

	while(pBuffer)
	{
		ZeroMemory(pBuffer, sizeof(OVERLAPPED));

	#if defined(DUMPPACKETS) && defined(_DEBUG)
		TCHAR stDump[512];
		OutputF(TTrace, TEXT("Write: %u bytes Seq: %u\n%s"),
			pBuffer->GetUsed(), pBuffer->GetSequenceNumber(),
			DumpData(pBuffer->GetBuffer(), pBuffer->GetUsed(), stDump, 511));
	#endif

		if(!WriteFile(
			pSerial->m_port,
			const_cast<BYTE *>(pBuffer->GetBuffer()),
			pBuffer->GetUsed(),
			NULL,
			pBuffer))
		{
			DWORD lastError = ::GetLastError();

			if(ERROR_IO_PENDING != lastError)
			{
				pSerial->OnCommunicationError(WriteError, pBuffer, lastError);
				pSerial->WriteCompleted(0); // this pending write will never complete...

				pSerial->Release();
				pBuffer->Release();
			}
		}

		pBuffer = pSerial->m_outOfSequenceWrites.ProcessAndGetNext();
	}
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees