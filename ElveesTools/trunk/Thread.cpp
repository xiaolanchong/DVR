// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"
#include "Exceptions.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// Static function
//////////////////////////////////////////////////////////////////////

// Default locale
DWORD CThread::m_dwLCID = LOCALE_USER_DEFAULT;

// Disable exceptions re-throw
bool CThread::m_bDebugMode = false;

// Call SetLocale when application starting
void __cdecl CThread::SetLocale(DWORD dwLCID)
{
	m_dwLCID = dwLCID;
	SetThreadLocale(m_dwLCID);
}

void __cdecl CThread::SetDebugMode()
{
	m_bDebugMode = true;
}

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

CThread::CThread() : m_hThread(INVALID_HANDLE_VALUE), m_nThreadID(0)
{
}
      
CThread::~CThread()
{
	CloseThreadHandle();
}

inline void CThread::CloseThreadHandle()
{
	if(m_hThread != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hThread);

	m_hThread   = INVALID_HANDLE_VALUE;
	m_nThreadID = 0;
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

bool CThread::IsStarted() const
{
	// Note: Windows NT/2000: The handle must have SYNCHRONIZE access

	return (m_hThread == INVALID_HANDLE_VALUE) ? false :
		!(WaitForSingleObject(m_hThread, 0) == WAIT_OBJECT_0);
}

bool CThread::Start()
{
	if(!IsStarted())
	{
		CloseThreadHandle();

		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction,
			(void*)this, 0, &m_nThreadID);

		if(m_hThread != INVALID_HANDLE_VALUE)
			return true;
	}

	return false;
}

void CThread::Terminate(DWORD exitCode /* = 0 */)
{
	if(IsStarted())
	{
		if(!::TerminateThread(m_hThread, exitCode))
		{
			throw CWin32Exception(TEXT("CThread::Terminate() - TerminateThread"), ::GetLastError());
		}
		else
		{
			CloseThreadHandle();

			DEBUG_ONLY(Output(TTrace, TEXT("Thread terminated, cleanup...")));

			try
			{
				OnUninitialize();
			}
			catch(const CException& e)
			{
				OutputF(TError,	TEXT("CThread::OnUninitialize(): %s"), e.GetMessage());
			}

			DEBUG_ONLY(Output(TTrace, TEXT("Thread terminated")));
		}
	}
}

void CThread::Wait() const
{
   if(!Wait(INFINITE))
   {
      throw CException(TEXT("CThread::Wait()"), TEXT("Unexpected timeout on infinite wait"));
   }
}

bool CThread::Wait(DWORD timeoutMillis) const
{
	#pragma TODO("base class? Waitable?")

	bool ok;

	DWORD result = ::WaitForSingleObject(m_hThread, timeoutMillis);

	if (result == WAIT_TIMEOUT)
	{
		ok = false;
	}
	else if (result == WAIT_OBJECT_0)
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception(TEXT("CThread::Wait() - WaitForSingleObject"), ::GetLastError());
	}
    
	return ok;
}

bool CThread::PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return IsStarted() ? (TRUE == ::PostThreadMessage(m_nThreadID, nMsg, wParam, lParam)) : false;
}

bool CThread::GetExitCode(DWORD* pdwExitCode)
{
	if(!pdwExitCode)
		return false;

	*pdwExitCode = 0;

	if(m_hThread == INVALID_HANDLE_VALUE || IsStarted())
		return false;
	
	return (::GetExitCodeThread(m_hThread, pdwExitCode) == 0) ? false : true;
}

unsigned int __stdcall CThread::ThreadFunction(void *pV)
{
	CThread* pThis = (CThread*)pV;

	// Acquire and dispatch messages until a WM_QUIT message is received.
	::PostThreadMessage(pThis->m_nThreadID, WM_NULL, 0, 0);

	DEBUG_ONLY(OutputF(TTrace, TEXT("The thread started")));

	//	In a multithreaded environment, translator functions are maintained separately
	//	for each thread. Each new thread needs to install its own translator function.
	//	Thus, each thread is in charge of its own translation handling.
	//	_set_se_translator is specific to one thread; another DLL can install a different
	//	translation function.

	if(!m_bDebugMode)
		CStructuredException::MapSEtoCE();

	SetThreadLocale(m_dwLCID);
   
	int nResult = 0;
	bool bInited = false;
	
	try
	{
		bInited = pThis->OnInitialize();
	}
	catch(const CException& e)
	{
		OutputF(TError,	TEXT("CThread::OnInitialize(): %s"), e.GetMessage());
	}

	if(bInited)
	{
		try
		{
			nResult = pThis->Run();
		}
		catch(const CException& e)
		{
			OutputF(TError,	TEXT("CThread::Run(): %s"), e.GetMessage());
		}
	}

	try
	{
		pThis->OnUninitialize();
	}
	catch(const CException& e)
	{
		OutputF(TError,	TEXT("CThread::OnUninitialize(): %s"), e.GetMessage());
	}

	DEBUG_ONLY(OutputF(TTrace, TEXT("The thread has exited with code=0x%X"), nResult));

	return nResult;
}

bool CThread::OnInitialize()
{
	return true;
}

void CThread::OnUninitialize()
{
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
