// IOBuffer.cpp: implementation of the CIOBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOBuffer.h"
#include "Exceptions.h"

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CIOBuffer
//////////////////////////////////////////////////////////////////////

CIOBuffer::CIOBuffer(Allocator &allocator, unsigned size)
	: m_allocator(allocator)
	, m_size(size)
	, m_used(0)
	, m_ref(1)
	, m_operation(0)
	, m_sequenceNumber(0)
	, m_pUserData(0)
{
	ZeroMemory(this, sizeof(OVERLAPPED));
	
	Empty();
}

WSABUF *CIOBuffer::GetWSABUF() const
{
	return const_cast<WSABUF*>(&m_wsabuf);
}

void CIOBuffer::Empty()
{
	m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
	m_wsabuf.len = u_long(m_size);

	m_used = 0;
}

unsigned CIOBuffer::GetUsed() const
{
	return m_used;
}

unsigned CIOBuffer::GetSize() const
{
	return m_size;
}

unsigned CIOBuffer::GetAvailable() const
{
	return m_size - m_used;
}

const BYTE *CIOBuffer::GetBuffer() const
{
	return &m_buffer[0];
}

void CIOBuffer::AddData(const char * const pData, unsigned dataLength)
{
	if(dataLength > m_size - m_used)
	{
		throw CException(TEXT("IOBuffer::AddData"), TEXT("Not enough space in buffer"));
	}

	CopyMemory(m_buffer + m_used, pData, dataLength);

	m_used += dataLength;
}

void CIOBuffer::AddData(const BYTE * const pData, unsigned dataLength)
{
	AddData(reinterpret_cast<const char*>(pData), dataLength);
}

void CIOBuffer::AddData(const void * const pData, unsigned dataLength)
{
	AddData(reinterpret_cast<const char*>(pData), dataLength);
}

void CIOBuffer::AddData(BYTE data)
{
	AddData(&data, 1);
}

void CIOBuffer::ConsumeAndRemove(unsigned bytesToRemove)
{
	m_used -= bytesToRemove;

	memmove(m_buffer, m_buffer + bytesToRemove, m_used);
}

void CIOBuffer::Use(unsigned dataUsed)
{
#ifdef _DEBUG
	if(IsBadReadPtr(this, sizeof(CIOBuffer)))
	{
		Output(TTrace, TEXT("Using IOBuffer::Use() after destroy"));
		return;
	}
#endif

	m_used += dataUsed;
}
 
CIOBuffer *CIOBuffer::SplitBuffer(unsigned bytesToRemove)
{
	CIOBuffer *pNewBuffer = m_allocator.Allocate();

	pNewBuffer->AddData(m_buffer, bytesToRemove);

	m_used -= bytesToRemove;

	memmove(m_buffer, m_buffer + bytesToRemove, m_used);

	return pNewBuffer;
}

CIOBuffer *CIOBuffer::AllocateNewBuffer() const
{
	return m_allocator.Allocate();
}

CIOBuffer *CIOBuffer::CreateCopy()
{
	CIOBuffer* pCopy = m_allocator.Allocate();

	if(pCopy)
	{
		pCopy->m_used = m_used;
		CopyMemory(pCopy->m_buffer, m_buffer, m_used);
	}

	return pCopy;
}

void CIOBuffer::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CIOBuffer::Release()
{
#ifdef _DEBUG
	if(IsBadReadPtr(this, sizeof(CIOBuffer)))
	{
		Output(TTrace, TEXT("Using IOBuffer::Release() after destroy"));
		return;
	}
#endif

	if(m_ref == 0)
	{
		// Error! double release
		throw CException(TEXT("IOBuffer::Release()"), TEXT("m_ref is already 0"));
	}

	if(0 == ::InterlockedDecrement(&m_ref))
	{
		m_sequenceNumber = 0;
		m_operation = 0;
		m_used = 0;

		if(IsBadReadPtr(&m_allocator, sizeof(m_allocator)))
		{
			DEBUG_ONLY(Output(TTrace, TEXT("CIOBuffer::Release() after allocator destroy")));
			delete this;
		}
		else
			m_allocator.Release(this);
	}
}

void CIOBuffer::SetupZeroByteRead()
{
	m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
	m_wsabuf.len = 0;
}

void CIOBuffer::SetupRead()
{
	if(m_used == 0)
	{
		m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
		m_wsabuf.len = u_long(m_size);
	}
	else
	{
		m_wsabuf.buf = reinterpret_cast<char*>(m_buffer) + m_used;
		m_wsabuf.len = u_long(m_size - m_used);
	}
}

void CIOBuffer::SetupWrite()
{
	m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
	m_wsabuf.len = u_long(m_used);

	m_used = 0;
}

void CIOBuffer::SetupReadUDP()
{
	m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
	m_wsabuf.len = u_long(m_size - sizeof(int) - sizeof(SOCKADDR_IN));

	LPINT lpFromLen = UDPAddressLen();
	*lpFromLen = sizeof(SOCKADDR_IN);
}

LPINT CIOBuffer::UDPAddressLen()
{
	return reinterpret_cast<LPINT>(m_buffer + m_size - sizeof(SOCKADDR_IN) - sizeof(int));
}

LPSOCKADDR_IN CIOBuffer::UDPAddress()
{
	return reinterpret_cast<LPSOCKADDR_IN>(m_buffer + m_size - sizeof(SOCKADDR_IN));
}

unsigned CIOBuffer::GetOperation() const
{
	return m_operation;
}
      
void CIOBuffer::SetOperation(unsigned operation)
{
	m_operation = operation;
}

unsigned CIOBuffer::GetSequenceNumber() const
{
	return m_sequenceNumber;
}
      
void CIOBuffer::SetSequenceNumber(unsigned sequenceNumber)
{
	m_sequenceNumber = sequenceNumber;
}

void* CIOBuffer::GetUserPtr() const
{
	return InterlockedExchange_Pointer(&m_pUserData, m_pUserData);
}

void CIOBuffer::SetUserPtr(void *pData)
{
	InterlockedExchange_Pointer(&m_pUserData, pData);
}

unsigned CIOBuffer::GetUserData() const
{
	return (unsigned)(LONG_PTR)GetUserPtr();
}

void CIOBuffer::SetUserData(unsigned data)
{
	SetUserPtr((void*)(LONG_PTR)data);
}

bool CIOBuffer::IsOperationCompleted()
{
#ifdef _DEBUG
	if(IsBadReadPtr(this, sizeof(CIOBuffer)))
	{
		Output(TTrace, TEXT("Using IOBuffer::IsOperationCompleted() after destroy"));
		return true;
	}
#endif

	return (m_used == m_wsabuf.len) ? true : false;
}

void *CIOBuffer::operator new(unsigned objectSize/*, unsigned bufferSize*/)
{
	void *pMem = _aligned_malloc(/*objectSize + bufferSize*/2*objectSize, 8);
//	void *pMem = new char[objectSize + bufferSize];

	return pMem;
}

void CIOBuffer::operator delete(void *pObject, unsigned /* bufferSize*/)
{
	_aligned_free(pObject);
//	delete [] pObject;
}

//////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
//////////////////////////////////////////////////////////////////////

CIOBuffer::Allocator::Allocator(unsigned bufferSize, unsigned maxFreeBuffers)
	: m_bufferSize( (bufferSize + 7) & ~7 ) // 8-byte alignment
	, m_maxFreeBuffers(maxFreeBuffers)
{
	// TODO share this code with the socket pool
}

CIOBuffer::Allocator::~Allocator()
{
	#pragma TODO("Wait untill all buffers released... (buffers used after destroy)")
	DEBUG_ONLY(OutputF(TTrace, TEXT("Allocator [%X] destroyed, flushing buffers"), this));

	try
	{
		Flush();
	}
	catch(...)
	{
	}
}

CIOBuffer *CIOBuffer::Allocator::Allocate()
{
	CCriticalSection::Owner lock(m_criticalSection);

	CIOBuffer *pBuffer = 0;

	if(!m_freeList.Empty())
	{
		pBuffer = m_freeList.PopNode();
		pBuffer->AddRef();
	}
	else
	{
		pBuffer = new/*(m_bufferSize)*/ CIOBuffer(*this, m_bufferSize);
		
		if(!pBuffer)
		{
			throw CException(TEXT("IOBuffer::Allocator::Allocate()"), TEXT("Out of memory"));
		}

		DEBUG_ONLY(OutputF(TTrace, TEXT("[%p] IOBuffer::Allocate (%ld)"),
			this, m_activeList.Count() + 1));

		OnBufferCreated();
	}

	m_activeList.PushNode(pBuffer);

	OnBufferAllocated();

	return pBuffer;
}

void CIOBuffer::Allocator::Release(CIOBuffer *pBuffer)
{
	if(!pBuffer)
	{
		throw CException(TEXT("IOBuffer::Allocator::Release()"), TEXT("pBuffer is null"));
	}

	CCriticalSection::Owner lock(m_criticalSection);

	// unlink from the in use list
	pBuffer->RemoveFromList();

	if(m_maxFreeBuffers == 0 || m_freeList.Count() < m_maxFreeBuffers)
	{
		pBuffer->Empty();
      
		// add to the free list
		m_freeList.PushNode(pBuffer);
	}
	else
	{
		Destroy(pBuffer);
	}

	OnBufferReleased();
}

void CIOBuffer::Allocator::Flush()
{
	CCriticalSection::Owner lock(m_criticalSection);

	while(!m_activeList.Empty())
	{
		OnBufferReleased();
		Destroy(m_activeList.PopNode());
	}

	while(!m_freeList.Empty())
		Destroy(m_freeList.PopNode());
}

void CIOBuffer::Allocator::Destroy(CIOBuffer *pBuffer)
{
	try
	{
		if(pBuffer)
			delete pBuffer;
	}
	catch(...)
	{
		DEBUG_ONLY(Output(TTrace, TEXT("Unexpected exceptiont on IOBuffer::Allocator::DestroyBuffer()")));
	}

	OnBufferDestroyed();
}

unsigned CIOBuffer::Allocator::GetBufferSize() const
{
	return m_bufferSize;
}


//////////////////////////////////////////////////////////////////////
// CIOBuffer::InOrderBufferList
//////////////////////////////////////////////////////////////////////

CIOBuffer::InOrderBufferList::InOrderBufferList(
   CCriticalSection &criticalSection)
   :  m_next(0),
      m_criticalSection(criticalSection)
{
}

void CIOBuffer::InOrderBufferList::AddBuffer(
   CIOBuffer *pBuffer)
{
	CCriticalSection::Owner lock(m_criticalSection);

	std::pair<BufferSequence::iterator, bool> result = m_list.insert(BufferSequence::value_type(pBuffer->GetSequenceNumber(), pBuffer));

	if(!result.second)
	{
		Output(TWarning, TEXT("InOrderBufferList::AddBuffer() UNEXPECTED, element already in map!"));
	}
}

CIOBuffer *CIOBuffer::InOrderBufferList::ProcessAndGetNext()
{
	CCriticalSection::Owner lock(m_criticalSection);

	m_next++;

	CIOBuffer *pNext = 0;

	BufferSequence::iterator it;

	it = m_list.begin();

	if(it != m_list.end()) 
	{
		if(it->first == m_next)
		{
			pNext = it->second;

			m_list.erase(it);
		}
		else
		{
#ifdef _DEBUG
			if( it->first - m_next > 5)
				OutputF(TTrace, TEXT("%08lX PAGN Got: %u Want: %u"), this, it->first, m_next);
#endif
		}
	}

	return pNext;
}

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext()
{
	CCriticalSection::Owner lock(m_criticalSection);

	CIOBuffer *pNext = 0;

	BufferSequence::iterator it;

	it = m_list.begin();

	if(it != m_list.end()) 
	{
		if(it->first == m_next)
		{
			pNext = it->second;

			m_list.erase(it);
		}
		else
		{
#ifdef _DEBUG
			if( it->first - m_next > 5)
				OutputF(TTrace, TEXT("%08lX GN0 Got: %u Want: %u"), this, it->first, m_next);
#endif
		}
	}

	return pNext;
}

// TODO resolve reference counting issues, the list should take a reference...

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext(
	CIOBuffer *pBuffer)
{
	CCriticalSection::Owner lock(m_criticalSection);

	if(m_next == pBuffer->GetSequenceNumber())
		return pBuffer;

	std::pair<BufferSequence::iterator, bool> result = m_list.insert(BufferSequence::value_type(pBuffer->GetSequenceNumber(), pBuffer));

	if(!result.second)
	{
		Output(TWarning, TEXT("InOrderBufferList::GetNext() UNEXPECTED, element already in map!"));
	}

	CIOBuffer *pNext = 0;

	BufferSequence::iterator it;

	it = m_list.begin();

	if(it != m_list.end()) 
	{
		if(it->first == m_next)
		{
			pNext = it->second;

			m_list.erase(it);
		}
		else
		{
#ifdef _DEBUG
			if( it->first - m_next > 5)
				OutputF(TTrace, TEXT("%08lX GN1 Got: %u Want: %u"), this, it->first, m_next);
#endif
		}
	}

	return pNext;
}

void CIOBuffer::InOrderBufferList::ProcessBuffer()
{
	CCriticalSection::Owner lock(m_criticalSection);

	DEBUG_ONLY(OutputF(TTrace, TEXT("%08lX Processed : %u"), this, m_next));

	m_next++;
}

void CIOBuffer::InOrderBufferList::Reset()
{
	m_next = 0;

	if(!m_list.empty())
	{
		DEBUG_ONLY(Output(TTrace, TEXT("InOrderBufferList::Reset() List not empty when reset !")));
		// worry!

		// TODO throw?
	}
}

bool CIOBuffer::InOrderBufferList::Empty() const
{
	return m_list.empty();
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees 
