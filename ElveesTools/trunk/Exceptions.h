// Exception.h: interface for the CException class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_EXCEPTIONS_INCLUDED_
#define ELVEESTOOLS_EXCEPTIONS_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#pragma comment(lib, "dbghelp.lib")
#endif

#define MAX_ERRORMSG_LEN  256
#define MAX_ERRORWRH_LEN  256

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

// Translate the exception code into something human readable
LPCTSTR __cdecl GetExceptionDescription(DWORD dwExceptionCode);

//////////////////////////////////////////////////////////////////////
// CException
//////////////////////////////////////////////////////////////////////

class CException
{
public: 
	explicit CException(LPCTSTR lpWhere, LPCTSTR lpMessage);
	virtual ~CException();

	virtual LPCTSTR GetWhere() const;
	virtual LPCTSTR GetMessage() const;

	void Report(HWND hWnd = NULL) const;

protected:
	TCHAR m_stWhere[MAX_ERRORWRH_LEN];
	TCHAR m_stMessage[MAX_ERRORMSG_LEN];
};

//////////////////////////////////////////////////////////////////////
// CWin32Exception
//////////////////////////////////////////////////////////////////////

class CWin32Exception : public CException
{
public:
	explicit CWin32Exception(LPCTSTR lpWhere, DWORD dwError);

	DWORD GetError() const;

protected:
	DWORD m_dwError;
};

//////////////////////////////////////////////////////////////////////
// CStructuredException
//////////////////////////////////////////////////////////////////////

class CStructuredException : public CException
{
public:
	explicit CStructuredException(PEXCEPTION_POINTERS pep);

	static void __cdecl MapSEtoCE();
	static void __cdecl EnableMiniDump(bool bEnable);

	// Create minidump if it enabled, return EXCEPTION_EXECUTE_HANDLER
	static int __cdecl DumpException(PEXCEPTION_POINTERS excpInfo);

	operator DWORD() { return m_er.ExceptionCode; }

private:
	static void DumpMiniDump(HANDLE hFile,
		PEXCEPTION_POINTERS excpInfo);

	static void __cdecl TranslateSEtoCE(UINT dwEC,
		PEXCEPTION_POINTERS pep);

private:
	EXCEPTION_RECORD m_er;
	CONTEXT          m_context;

	static bool m_bMiniDump;
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_EXCEPTIONS_INCLUDED_
