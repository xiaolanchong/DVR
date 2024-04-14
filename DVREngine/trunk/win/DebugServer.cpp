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
#include <iostream>

#include "../interface/IServer.h"
#include "../server/MessageStdOut.h"

// See the Microsoft Knowledge Base for more details.
// http://support.microsoft.com/default.aspx?scid=KB;EN-US;q164787&
//
//	hwnd - window handle that should be used as the owner window for
//	any windows your DLL creates
//	hinst - your DLL's instance handle
//	lpszCmdLine - ASCIIZ command line your DLL should parse
//	nCmdShow - describes how your DLL's windows should be displayed

void __stdcall DebugServer(HWND hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(hinst);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	// Command line init.


	//////////////////////////////////////////////////////////////////////////
	::AllocConsole();
	FILE	*NewFile, *OldFile;
	NewFile = 0;
	OldFile = stdout;
	::freopen_s( &NewFile, "conout$", "w", OldFile );
	NewFile = 0;
	OldFile = stderr;
	::freopen_s( &NewFile, "conout$", "w", OldFile );
	::fprintf(stdout, "stdout: ok\n");
	::fprintf(stderr, "stderr: ok\n");

	//////////////////////////////////////////////////////////////////////////

	using namespace DVREngine;

	try
	{
		boost::shared_ptr<Elvees::IMessage> msg( new MessageStdOut);
	//	boost::shared_ptr<IServer> pServer = boost::shared_ptr< IServer >( CreateServerInterface(false, msg, msg, msg) ); 
		_getch();
	}
	catch ( std::exception& ex ) 
	{
		std::cout << "[exception]=" << ex.what() << std::endl;
		_getch();
	}
	catch(...)
	{
		std::cout << "[Unknown exception]" << std::endl;
		_getch();
	}
}

