//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   07.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include <conio.h>
#include "../interface/IDBServer.h"

// See the Microsoft Knowledge Base for more details.
// http://support.microsoft.com/default.aspx?scid=KB;EN-US;q164787&
//
//	hwnd - window handle that should be used as the owner window for
//	any windows your DLL creates
//	hinst - your DLL's instance handle
//	lpszCmdLine - ASCIIZ command line your DLL should parse
//	nCmdShow - describes how your DLL's windows should be displayed

void __stdcall Debug(HWND hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(hinst);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	// Command line init.
	const char * szStdOut =
#if 1
		"conout$"
#else
		"1.txt"
#endif
	;
	//////////////////////////////////////////////////////////////////////////
	FILE* DontUsed;
	::AllocConsole();
	::freopen_s( &DontUsed, szStdOut, "w", stdout);
	::freopen_s( &DontUsed, szStdOut, "w", stderr);
	::fprintf(stdout, "stdout: ok\n");
	::fprintf(stderr, "stderr: ok\n");

	//////////////////////////////////////////////////////////////////////////

	_getch();
}
