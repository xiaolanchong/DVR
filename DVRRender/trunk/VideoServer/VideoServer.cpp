// VideoServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "chcsva.h"
#include <conio.h>

int _tmain(int argc, _TCHAR* argv[])
{

	//Load chcsva library
	HMODULE	csvaHandle = LoadLibrary(TEXT("chcsva.dll"));

	if( csvaHandle == NULL ){
		return -1;
	}

	CHCS::funcInitStreamManager InitManager = (CHCS::funcInitStreamManager)
		GetProcAddress( csvaHandle, "InitStreamManager");

	CHCS::INITCHCS initCHCS;
	initCHCS.dwSize = sizeof(initCHCS);
	initCHCS.dwMode = CHCS_MODE_SERVER;
	initCHCS.dwReserved = 0;
	initCHCS.dwUserData = 0;
	initCHCS.procOutput = NULL;

	CHCS::IStreamManager* manager;

	while( true )
	{

		if(!(manager = InitManager( &initCHCS )) )
			return -1;

		_getch();
		manager->Release();
	}

	FreeLibrary( csvaHandle );
	return 0;
}

