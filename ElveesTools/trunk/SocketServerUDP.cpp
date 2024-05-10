// SocketServerUDP.cpp: implementation of the CSocketServerUDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketServerUDP.h"
#include "Exceptions.h"
#include "SystemInfo.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP
//////////////////////////////////////////////////////////////////////

// http://www.ietf.org/rfc/rfc0791.txt
//
//	0                   1                   2                   3   
//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|Version|  IHL  |Type of Service|          Total Length         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|         Identification        |Flags|      Fragment Offset    |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|  Time to Live |    Protocol   |         Header Checksum       |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                       Source Address                          |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                    Destination Address                        |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                    Options                    |    Padding    |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// UDP packet uses 16 bit fields for packet size, so max packet size 0xFFFF,
// when i use CIOBuffer with size less then 1024 GetQueuedCompletionStatus failed
// with error 230 (more data available)
//
// Since now packet size will be max UPD size and not changed!
// Also we use pBuffer to store received packed address/length

CSocketServerUDP::CSocketServerUDP(
	unsigned maxFreeBuffers,
	unsigned numThreads)
	: CIOBuffer::Allocator(0xFFFF + sizeof(int) + sizeof(SOCKADDR_IN), maxFreeBuffers)
	, m_numThreads(max(2, numThreads))
	, m_iocp(0)
{
}

CSocketServerUDP::~CSocketServerUDP()
{
	// If we want to be informed of any buffers or sockets being destroyed at destruction 
	// time then we need to release these resources now, whilst we, the derived class,
	// still exists. Once our destructor exits and the base destructor takes over we wont 
	// get any more notifications...

	ReleaseSockets();
}

CSocketServerUDP::Socket* CSocketServerUDP::InitSocket(SOCKET hSocket, SOCKADDR_IN* pAddr)
{
	CSocketServerUDP::Socket* pSocket = AllocateSocket();

	if(pSocket)
	{
		pSocket->Attach(hSocket);

		if(pAddr)
			CopyMemory(&pSocket->m_fromAddr, pAddr, sizeof(SOCKADDR_IN));
		
		// Setting key equal to socket handle !
		// Socket* stored in User Pointer of CIOBuffer
		m_iocp.AssociateDevice(reinterpret_cast<HANDLE>(hSocket), (DWORD)(DWORD_PTR)hSocket);
	}
		
	return pSocket;
}

void CSocketServerUDP::Start()
{
	unsigned i;
	WorkerThread *pThread;

	OnStart();

	for(i = 0; i < m_numThreads; ++i)
	{
		pThread = CreateWorkerThread(m_iocp);

		m_workerList.PushNode(pThread);
		pThread->Start();
	}
}

void CSocketServerUDP::Stop()
{
	unsigned i;
	WorkerThread *pThread;

	OnStop();

	for(i = 0; i < m_workerList.Count(); ++i)
		m_iocp.PostStatus(0);

	while(!m_workerList.Empty())
	{
		pThread = m_workerList.PopLastNode();
		pThread->WaitForShutdown();
		delete pThread;
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CSocketServerUDP::WorkerThread *CSocketServerUDP::CreateWorkerThread(CIOCP &iocp)
{
	return new WorkerThread(*this, iocp);
}

CSocketServerUDP::Socket* CSocketServerUDP::CreateSocket()
{
	Socket *pSocket = new Socket(*this);
	return pSocket;
}

CSocketServerUDP::Socket *CSocketServerUDP::AllocateSocket()
{
	CCriticalSection::Owner lock(m_listSection);

	Socket *pSocket = CreateSocket();

	if(pSocket)
		m_socketList.PushNode(pSocket);

	return pSocket;
}

void CSocketServerUDP::ReleaseSocket(Socket *pSocket)
{
	CCriticalSection::Owner lock(m_listSection);

	pSocket->RemoveFromList();

	try { delete pSocket; } catch(...) {}
}

void CSocketServerUDP::ReleaseSockets()
{
	CCriticalSection::Owner lock(m_listSection);

	// release any remaining active sockets     
	while(!m_socketList.Empty())
		ReleaseSocket(m_socketList.Head());
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

void CSocketServerUDP::PostIoOperation(CSocketServerUDP::Socket *pSocket,
					CIOBuffer *pBuffer, IO_Operation operation)
{
	pBuffer->SetOperation(operation);
	pBuffer->SetUserPtr(pSocket);

	pBuffer->AddRef();
	pSocket->AddRef();

	m_iocp.PostStatus((DWORD)(DWORD_PTR)pSocket->m_socket, 0, pBuffer);
}

void CSocketServerUDP::WriteCompleted(Socket *pSocket, CIOBuffer *pBuffer)
{
#ifdef _DEBUG
	if(!pBuffer->IsOperationCompleted())
		Output(TTrace, TEXT("CSocketServerUDP::WriteCompleted - not all data was written"));
#endif
}

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP::Socket
//////////////////////////////////////////////////////////////////////

CSocketServerUDP::Socket::Socket(CSocketServerUDP &server)
	: m_server(server)
	, m_socket(INVALID_SOCKET)
	, m_ref(1)
{

}

CSocketServerUDP::Socket::~Socket()
{
	if(m_socket != INVALID_SOCKET)
	{
		if(0 != ::closesocket(m_socket))
			OutputF(TWarning, TEXT("CSocketServerUDP::~Socket() - closesocket err=0x%lX"), ::WSAGetLastError());
	}
}

void CSocketServerUDP::Socket::Attach(SOCKET theSocket)
{
	if(INVALID_SOCKET != m_socket)
	{
		throw CException(TEXT("CSocketServerUDP::Socket::Attach()"),
			TEXT("Socket already attached"));
	}

	m_socket = theSocket;
}

void CSocketServerUDP::Socket::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CSocketServerUDP::Socket::Release()
{
	if(0 == ::InterlockedDecrement(&m_ref))
		m_server.ReleaseSocket(this);
}

void CSocketServerUDP::Socket::Write(const char *pData, unsigned dataLength)
{
	if(dataLength > m_server.GetBufferSize())
	{
		OutputF(TError, TEXT("CSocketServerUDP::Write() Too big packet (%u)."), dataLength);
		return;
	}

	CIOBuffer *pBuffer = m_server.Allocate();

	pBuffer->AddData(pData, dataLength);
	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);
	pBuffer->Release();
}

void CSocketServerUDP::Socket::Write(CIOBuffer *pBuffer)
{
	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);
}

void CSocketServerUDP::Socket::Read(CIOBuffer *pBuffer /*= NULL*/)
{
	if(!pBuffer)
		pBuffer = m_server.Allocate();
	else
		pBuffer->AddRef();
	
	m_server.PostIoOperation(this, pBuffer, IO_Read_Request);
	pBuffer->Release();
}

//////////////////////////////////////////////////////////////////////
// CSocketServerUDP::WorkerThread
//////////////////////////////////////////////////////////////////////

CSocketServerUDP::WorkerThread::WorkerThread(CSocketServerUDP &server, CIOCP &iocp)
	: m_iocp(iocp)
	, m_server(server)
{
	// All work done in initialiser list
}

CSocketServerUDP::WorkerThread::~WorkerThread()
{
}

void CSocketServerUDP::WorkerThread::WaitForShutdown()
{
	m_iocp.PostStatus(0);
	Wait();
}

int CSocketServerUDP::WorkerThread::Run()
{
	try
	{
		while(true)   
		{
			// continually loop to service IO completion packets

			bool closeSocket = false;

			DWORD  dwIoSize = 0;
			SOCKET socket = INVALID_SOCKET;

			CIOBuffer *pBuffer = 0;

			try
			{
				m_iocp.GetStatus((PDWORD_PTR)&socket, &dwIoSize, (OVERLAPPED**)&pBuffer);
			}
			catch(const CWin32Exception &e)
			{
				if( e.GetError() != ERROR_NETNAME_DELETED  &&
					e.GetError() != ERROR_PORT_UNREACHABLE &&
					e.GetError() != ERROR_OPERATION_ABORTED)
				{
					DEBUG_ONLY(OutputF(TTrace, TEXT("IOCP %s"), e.GetMessage()));

					if(pBuffer)
					{
						Socket *pSocket = static_cast<Socket*>(pBuffer->GetUserPtr());

						pBuffer->Release();
						pSocket->Release();
					}

					throw;
				}

				OutputF(TWarning, TEXT("Connection dropped %s"), e.GetMessage());
			}

			if(!socket)
			{
				// A completion key of 0 is posted to the iocp to request us to shut down...
				break;
			}

			m_server.OnThreadBeginProcessing();

			if(pBuffer)
			{
				Socket *pSocket = static_cast<Socket*>(pBuffer->GetUserPtr());

				const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

				switch(operation)
				{
				case IO_Read_Request:
					{
						Read(pSocket, pBuffer);
					}
					break;

				case IO_Read_Completed:
					{
						if(0 != dwIoSize)
						{
							pBuffer->Use(dwIoSize);

							//	DEBUG_ONLY(Output(TEXT("RX: ") + ToString(pBuffer) + TEXT("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetWSABUF()->buf), dwIoSize, 40)));
							m_server.ReadCompleted(pSocket, pBuffer);
						}
						else
						{
							// client connection dropped...
							DEBUG_ONLY(Output(TTrace, TEXT("ReadCompleted - client connection dropped")));
						}

						pSocket->Release();
						pBuffer->Release();
					}
					break;

				case IO_Write_Request:
					{
						Write(pSocket, pBuffer);
					}
					break;

				case IO_Write_Completed:
					{
						pBuffer->Use(dwIoSize);

						m_server.WriteCompleted(pSocket, pBuffer);

						pSocket->Release();
						pBuffer->Release();
					}
					break;

				default:
					OutputF(TWarning, TEXT("CSocketServerUDP::WorkerThread::Run() - Unexpected operation"));
					break;
				} 
			}
			else
			{
				OutputF(TWarning, TEXT("CSocketServerUDP::WorkerThread::Run() - Unexpected - pBuffer is 0"));
			}

			m_server.OnThreadEndProcessing();
		} 
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSocketServerUDP::WorkerThread::Run() - Exception: %s - %s"), 
			e.GetWhere(), e.GetMessage());
	}

	return 0;
}

void CSocketServerUDP::WorkerThread::Read(
	Socket *pSocket, CIOBuffer *pBuffer) const
{
	DWORD dwFlags = 0;
	DWORD dwNumBytes = 0;

	pBuffer->SetOperation(IO_Read_Completed);
	pBuffer->SetupReadUDP();
	
	if(SOCKET_ERROR == ::WSARecvFrom(
		pSocket->m_socket,
		pBuffer->GetWSABUF(),
		1,
		&dwNumBytes,
		&dwFlags,
		(LPSOCKADDR)pBuffer->UDPAddress(),
		pBuffer->UDPAddressLen(),
		pBuffer, 
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			OutputF(TError, TEXT("WSARecvFrom: (Error: %ld) %s"),
				lastError, GetLastErrorMessage(lastError));

			if(lastError == WSAECONNABORTED || 
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)
			{
				/// 
			}

			pSocket->Release();
			pBuffer->Release();
		}
	}
}

void CSocketServerUDP::WorkerThread::Write(
	Socket *pSocket,
	CIOBuffer *pBuffer) const
{
	DWORD dwFlags = 0;
	DWORD dwSendNumBytes = 0;

	pBuffer->SetOperation(IO_Write_Completed);
	pBuffer->SetupWrite();

	if(SOCKET_ERROR == ::WSASendTo(
		pSocket->m_socket,
		pBuffer->GetWSABUF(),
		1,
		&dwSendNumBytes,
		dwFlags,
		(LPSOCKADDR)&pSocket->m_fromAddr,
		sizeof(SOCKADDR_IN),
		pBuffer,
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			OutputF(TError, TEXT("WSASendTo: (Error: %ld) %s"),
				lastError, GetLastErrorMessage(lastError));

			if( lastError == WSAECONNABORTED || 
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)
			{
				//
			}

			pSocket->Release();
			pBuffer->Release();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
