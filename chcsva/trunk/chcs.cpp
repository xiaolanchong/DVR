// chcs.cpp
//

#include "chcs.h"

// One and only Module object
CComModule _Module;

#pragma comment(lib, "comctl32.lib")	// InitCommonControls
#pragma comment(lib, "Ws2_32.lib")		// Winsock2
#pragma comment(lib, "Winmm.lib")		// timeGetTime()
#pragma comment(lib, "vfw32.lib")		// Windows compression manager
#pragma comment(lib, "dbghelp.lib")		// MiniDumpWriteDump
#pragma comment(lib, "Rpcrt4.lib")		// UUID create convert
#pragma comment(lib, "strmiids.lib")	// DX uuids

// Don't forget to check dependances
#pragma comment(lib, "ElveesTools.lib")

bool StringToUuid(LPTSTR stUuid, UUID* Uuid)
{
	if(!stUuid || !Uuid)
		return false;

	// Copy string to our buffer
	// Fail on const char* strings

	TCHAR StringUuid[56];
	lstrcpy(StringUuid, stUuid);

	if(UuidFromString((UuidString)StringUuid, Uuid) != RPC_S_OK)
	{
		if(lstrlen(StringUuid) < 37)
			return false;

		StringUuid[37] = 0;

		if(UuidFromString((UuidString)&StringUuid[1], Uuid) != RPC_S_OK)
			return false;
	}

	return true;
}

void	GetFrameUserData( CHCS::IFrame* pFrame, std::vector<unsigned char>& UserFrameData )
{
	size_t DataSize = 0;
	bool res = pFrame->GetCustomData( 0, DataSize );
	if( res && DataSize)
	{
		UserFrameData.resize( DataSize );
		bool res = pFrame->GetCustomData( &UserFrameData[0], DataSize );
		if(!res)
		{
			UserFrameData.clear();
		}
	}
	else
	{
		UserFrameData.clear();
	}
}