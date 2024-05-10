// IOBuffer.h: interface for the CIOBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_IOBUFFER_INCLUDED_
#define ELVEESTOOLS_IOBUFFER_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#define BUFALIGN __declspec(align(8))
#else
#define BUFALIGN
#endif

#include "CriticalSection.h"

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information
#include <map>

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CIOBuffer
//////////////////////////////////////////////////////////////////////

class CIOBuffer : public OVERLAPPED, public CNodeList::Node
{
public:
	class Allocator;
	friend class Allocator;

	class InOrderBufferList;

	WSABUF *GetWSABUF() const;

	void Empty();

	unsigned GetUsed() const;
	unsigned GetSize() const;
	unsigned GetAvailable() const;

	const BYTE *GetBuffer() const;
      
	void AddData(const char * const pData, unsigned dataLength);
	void AddData(const void * const pData, unsigned dataLength);
	void AddData(const BYTE * const pData, unsigned dataLength);
	void AddData(BYTE data);

	void ConsumeAndRemove(unsigned bytesToRemove);

	void Use(unsigned dataUsed);

	CIOBuffer *SplitBuffer(unsigned bytesToRemove);
	CIOBuffer *AllocateNewBuffer() const;

	CIOBuffer *CreateCopy();

	void AddRef();
	void Release();

	// Set WSABUF data
	void SetupZeroByteRead();
	void SetupRead();
	void SetupWrite();

	// Set WSABUF data, hold memory for SOCKADDR_IN
	void SetupReadUDP();

	// UDP operations only
	LPINT UDPAddressLen();
	LPSOCKADDR_IN UDPAddress();

	bool IsOperationCompleted();

	unsigned GetOperation() const;
	void SetOperation(unsigned operation);

	unsigned GetSequenceNumber() const;
	void SetSequenceNumber(unsigned sequenceNumber);

	void *GetUserPtr() const;
	void SetUserPtr(void *pData);

	unsigned GetUserData() const;
	void SetUserData(unsigned data);
	
private:
	unsigned m_operation;
	unsigned m_sequenceNumber;

	void *m_pUserData;

	WSABUF m_wsabuf;
	Allocator &m_allocator;

	long m_ref;
	const unsigned m_size;
	unsigned m_used;

	BUFALIGN BYTE m_buffer[0];	// start of the actual buffer, must remain the last
								// data member in the class.
private:
	static void *operator new(unsigned objSize/*, unsigned bufferSize*/);
	static void operator delete(void *pObj, unsigned bufferSize);
      
	explicit CIOBuffer(Allocator &m_allocator, unsigned size);

	// No copies do not implement
	CIOBuffer(const CIOBuffer &rhs);
	CIOBuffer &operator=(const CIOBuffer &rhs);
};

//////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
//////////////////////////////////////////////////////////////////////

class CIOBuffer::Allocator
{
public:
	friend class CIOBuffer;

	explicit Allocator(
		unsigned bufferSize,
		unsigned maxFreeBuffers);

	virtual ~Allocator();

	CIOBuffer *Allocate();

	unsigned GetBufferSize() const;

protected:
	void Flush();

private:
	void Release(CIOBuffer *pBuffer);
	void Destroy(CIOBuffer *pBuffer);

	virtual void OnBufferCreated()   {}
	virtual void OnBufferAllocated() {}
	virtual void OnBufferReleased()  {}
	virtual void OnBufferDestroyed() {}
	
	const unsigned m_bufferSize;

	typedef TNodeList<CIOBuffer> BufferList;
      
	BufferList m_freeList;
	BufferList m_activeList;
      
	const unsigned m_maxFreeBuffers;

	CCriticalSection m_criticalSection;

	// No copies do not implement
	Allocator(const Allocator &rhs);
	Allocator &operator=(const Allocator &rhs);
};


//////////////////////////////////////////////////////////////////////
// CIOBuffer::InOrderBufferList
//////////////////////////////////////////////////////////////////////

class CIOBuffer::InOrderBufferList
{
   public:

      explicit InOrderBufferList(
         CCriticalSection &lock);

      void AddBuffer(
         CIOBuffer *pBuffer);

      void ProcessBuffer();

      CIOBuffer *ProcessAndGetNext();

      CIOBuffer *GetNext();

      CIOBuffer *GetNext(
         CIOBuffer *pBuffer);

      void Reset();

      bool Empty() const;

   private :

      unsigned m_next;
   
      typedef std::map<unsigned, CIOBuffer *> BufferSequence;

      BufferSequence m_list;

      CCriticalSection &m_criticalSection;

	InOrderBufferList(const InOrderBufferList &rhs);
	InOrderBufferList &operator=(const InOrderBufferList &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_IOBUFFER_INCLUDED_
