// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_THREAD_INCLUDED_
#define ELVEESTOOLS_THREAD_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

class CThread
{
public:
	CThread();
	virtual ~CThread();

	HANDLE GetHandle() const;

	bool Start();
	void Terminate(DWORD exitCode = 0);

	bool IsStarted() const;

	void Wait() const;
	bool Wait(DWORD timeoutMillis) const;

	bool PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	bool GetExitCode(DWORD* pdwExitCode);

	static void __cdecl SetLocale(DWORD dwLCID);
	static void __cdecl SetDebugMode();

protected:
	virtual int Run() = 0;
	virtual bool OnInitialize();
	virtual void OnUninitialize();

private:
	void CloseThreadHandle();
	static unsigned int __stdcall ThreadFunction(void *pV);

	HANDLE m_hThread;
	unsigned int m_nThreadID;

	static DWORD m_dwLCID;
	static bool m_bDebugMode;

	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_THREAD_INCLUDED_
