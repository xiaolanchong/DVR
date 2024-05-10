// Utils.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Utils.h"
#include "Exceptions.h"
#include "CriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

bool __cdecl IsFileExists(LPCTSTR stFile)
{
	HANDLE hFindFile;
	WIN32_FIND_DATA wfd;

	if((hFindFile = FindFirstFile(stFile, &wfd)) == INVALID_HANDLE_VALUE)
		return false;

	FindClose(hFindFile);
	return true;
}

bool __cdecl HexToString(const BYTE* pBuffer, int nBytes, LPTSTR lpDest, int cchDest)
{
	return false;
}

bool __cdecl StringToHex(LPCTSTR lpSource, BYTE *pBuffer, int nBytes)
{
	return false;
}

LPCTSTR __cdecl GetLastErrorMessage(DWORD dwError)
{
	static TCHAR srErrMsg[512];

	if(!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
		0,
		dwError,
		0,
		srErrMsg,
		511,
		NULL))
	{
		// if we fail, call ourself to find out why and return that error
		return (GetLastErrorMessage(GetLastError()));  
	}

	return srErrMsg;
}

LPCTSTR __cdecl GetCurrentDirectory()
{
	static TCHAR srCurDir[MAX_PATH];

	DWORD size = ::GetCurrentDirectory(0, 0);

	if(size > MAX_PATH || 0 == ::GetCurrentDirectory(size, srCurDir))
	{
		throw CException(TEXT("GetCurrentDirectory()"), TEXT("Failed to get current directory"));
	}

	return srCurDir;
}

LPCTSTR __cdecl GetDateTimeStamp()
{
	static TCHAR srDateTime[32];

	time_t ltime;
	struct tm *tmTime;
			
	time(&ltime);
	tmTime = localtime(&ltime);

	wsprintf(srDateTime, TEXT("%02hd/%02hd/%02hd %02hd:%02hd:%02hd"),
			tmTime->tm_mday, (1 + tmTime->tm_mon), (1900 + tmTime->tm_year),
			tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec);

	return srDateTime;
}

LPCTSTR __cdecl DumpData(const BYTE* pData, int dataLength, LPTSTR stOut, long cbOut, int lineLength)
{
	static const TCHAR stNull[] = TEXT("(null)");

	if(stOut == NULL || cbOut < 4)
		return stNull;
    
	int i, line, bytesPerLine;

	long cbResult = 0;
	long cbDisplay = 0;
	long cbDisplayHex = 0;

	// max text mode 80x?? - 24 per line

	TCHAR stDisplay[32];
	TCHAR stDisplayHex[128];

	bytesPerLine = (lineLength == 0) ? 16 : (lineLength - 1)/4;

	bytesPerLine = max(1,  bytesPerLine);
	bytesPerLine = min(24, bytesPerLine);

	i = line = 0;
	
	while(i < dataLength)
	{
		const BYTE c = pData[i++];
		
	#if 0
		// Prevent failer on printf dump
		if( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
			stDisplay[cbDisplay] = (TCHAR)c;
		else
			stDisplay[cbDisplay] = TEXT('.');
	#else
		stDisplay[cbDisplay] = isprint(c) ? (TCHAR)c : TEXT('.');
	#endif

		cbDisplay++;
		cbDisplayHex += wsprintf(&stDisplayHex[cbDisplayHex], TEXT("%02X "), c);
		
		if((bytesPerLine && (i % bytesPerLine == 0 && i != 0)) || i == dataLength)
		{
			if(i == dataLength && (bytesPerLine && (i % bytesPerLine != 0)))
			{
				for(int pad = i % bytesPerLine; pad < bytesPerLine; pad++)
					cbDisplayHex += wsprintf(&stDisplayHex[cbDisplayHex], TEXT("   "));
			}

			stDisplay[cbDisplay] = 0;

			if(cbResult + cbDisplay + cbDisplayHex > cbOut)
			{
				if(cbOut - cbResult > 5)
					lstrcpy(&stOut[cbResult], TEXT("\n...\n"));
				else
					lstrcpy(&stOut[cbResult - 5], TEXT("\n...\n"));

				break;
			}

			cbResult += wsprintf(&stOut[cbResult],
				line > 0 ? TEXT("\n%s   %s") : TEXT("%s   %s"),	stDisplayHex, stDisplay);

			cbDisplay = cbDisplayHex = 0;
			line++;
		}
	}

	// just to be sure...
	stOut[cbOut-1] = 0;

	return stOut;
}

LPCTSTR __cdecl GetComputerName()
{
	static bool  gotName = false;
	static TCHAR stComputerName[MAX_COMPUTERNAME_LENGTH + 1];

	if(!gotName)
	{
		DWORD dwNameLen = MAX_COMPUTERNAME_LENGTH ;

		if(!::GetComputerName(stComputerName, &dwNameLen))
		{
			lstrcpy(stComputerName, TEXT("UNAVAILABLE"));
		}

		gotName = true;
	}

	return stComputerName;
}

LPCTSTR __cdecl GetUserName()
{
	static bool  gotName = false;
	static TCHAR stUserName[256];

	if(!gotName)
	{
		DWORD dwNameLen = 255;

		if(!::GetUserName(stUserName, &dwNameLen))
			lstrcpy(stUserName, TEXT("UNAVAILABLE"));

		gotName = true;
	}

	return stUserName;
}

//////////////////////////////////////////////////////////////////////
// Output functions
//////////////////////////////////////////////////////////////////////

void __cdecl OutputF(TOutput type, LPCTSTR stFormat, ...)
{
	int nRet;
	TCHAR szMsg[1024];

	// Format the input string
	va_list pArgs;
	va_start(pArgs, stFormat);
	nRet = _vsntprintf(szMsg, 1023, stFormat, pArgs);
	va_end(pArgs);

	if(nRet < 0)
	{
		Output(TWarning, TEXT("Message trancated"));
		szMsg[1023] = 0;
	}
	
	Output(type, szMsg);
}

//////////////////////////////////////////////////////////////////////////
// Process data
//////////////////////////////////////////////////////////////////////////

DWORD GetIndex(LPTSTR pszCounter, LPTSTR szIndex)
{
	TCHAR szObject[256] = TEXT("");
	TCHAR szMasterKey[128] = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\");

	HKEY hKeyIndex = NULL;
	DWORD dwBytes = 0;
	LPTSTR pszTemp;
	LPTSTR pszBuffer = NULL;

	int i = 0;
	int j = 0;
	DWORD dwRetVal = ERROR_SUCCESS;

	// Пример Microsoft в этом месте не правилен. 
	// Я отделил эту строку, так что она может быть получена 
	// в другом месте, при необходимости

	lstrcat(szMasterKey, TEXT("009"));
	lstrcpy(szIndex, TEXT(""));

	do
	{
		dwRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,	szMasterKey,
			0, KEY_READ, &hKeyIndex);

		if(dwRetVal != ERROR_SUCCESS)
		{
			DEBUG_ONLY(Output(TTrace, TEXT("Can't open Perflib registry key")));
			break;
		}
	
		dwRetVal = RegQueryValueEx(hKeyIndex,
			TEXT("Counter"), NULL, NULL, NULL, &dwBytes);

		if(dwRetVal != ERROR_SUCCESS)
		{
			DEBUG_ONLY(OutputF(TTrace, TEXT("Perflib::Counter unknown size (%ld)"), dwBytes));
			break;
		}

		if(dwBytes < (DWORD)lstrlen(pszCounter) + 4)
		{
			dwRetVal = E_UNEXPECTED;
			DEBUG_ONLY(OutputF(TTrace, TEXT("Perflib::Counter not initialized")));
			break;
		}

		pszBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);

		if(pszBuffer == NULL)
		{
			dwRetVal = E_OUTOFMEMORY;
			DEBUG_ONLY(OutputF(TTrace, TEXT("Perflib::Counter - Out of memory (%ld)"), dwBytes));
			break;
		}

		dwRetVal = RegQueryValueEx(hKeyIndex,
			TEXT("Counter"), NULL, NULL, (LPBYTE)pszBuffer,	&dwBytes);

		if(dwRetVal != ERROR_SUCCESS)
		{
			DEBUG_ONLY(OutputF(TTrace, TEXT("Can't read Perflib::Counter")));
			break;
		}

		// Ищем значение индекса для PROCESS.
		pszTemp = pszBuffer;

		while(i != (int)dwBytes)
		{
			while(*(pszTemp+i) != TEXT('\0'))
			{
				szIndex[j] = *(pszTemp+i);
				i++;
				j++;
			}
			szIndex[j] = TEXT('\0');
			i++;
			j = 0;
			while(*(pszTemp+i) != TEXT('\0'))
			{
				szObject[j] = *(pszTemp+i);
				i++;
				j++;
			}
			szObject[j] = TEXT('\0');
			i++;
			j = 0;

			if(*(pszTemp+i) == TEXT('\0'))
				i++;

			if(lstrcmp(szObject, pszCounter) == 0)
				break;
		}
	}
	while(false);

	if(hKeyIndex)
		RegCloseKey(hKeyIndex);

	if(pszBuffer)
		HeapFree(GetProcessHeap(), 0, (LPVOID)pszBuffer);

	return dwRetVal;
}

#define INVALID_INDEX  (DWORD)(-1)
#define INVALID_OFFSET (DWORD)(-1)

bool __cdecl GetUsedMemorySize(DWORD dwProcessId, DWORD* pdwWorkingSet, DWORD* pdwPrivateBytes)
{
	if(!pdwWorkingSet && !pdwPrivateBytes)
		return true;

	DWORD dwWorkingSet   = 0;
	DWORD dwPrivateBytes = 0;

	// Get counter indexes
	//

	static DWORD dwProcessIndex = INVALID_INDEX;
	static DWORD dwProcessIdIndex = INVALID_INDEX;
	static DWORD dwWorkingSetIndex = INVALID_INDEX;
	static DWORD dwPrivateBytesIndex = INVALID_INDEX;

	TCHAR stIndex[32];

	if(dwProcessIndex == INVALID_INDEX)
	{
		if(GetIndex(TEXT("Process"), stIndex) == ERROR_SUCCESS)
			dwProcessIndex = _ttoi(stIndex);
	}
	
	if(dwProcessIdIndex == INVALID_INDEX)
	{
		if(GetIndex(TEXT("ID Process"), stIndex) == ERROR_SUCCESS)
			dwProcessIdIndex = _ttoi(stIndex);
	}
	
	if(dwWorkingSetIndex == INVALID_INDEX)
	{
		if(GetIndex(TEXT("Working Set"), stIndex) == ERROR_SUCCESS)
			dwWorkingSetIndex = _ttoi(stIndex);
	}
	
	if(dwPrivateBytesIndex == INVALID_INDEX)
	{
		if(GetIndex(TEXT("Private Bytes"), stIndex) == ERROR_SUCCESS)
			dwPrivateBytesIndex = _ttoi(stIndex);
	}

	if( dwProcessIndex == INVALID_INDEX ||
		dwProcessIdIndex == INVALID_INDEX ||
		dwWorkingSetIndex == INVALID_INDEX ||
		dwPrivateBytesIndex == INVALID_INDEX)
	{
		return false;
	}

	// Allocating memory for PPERF_DATA_BLOCK
	//

	PPERF_DATA_BLOCK pdb;
	static DWORD dwPerfDataBytes = 4096;

	pdb = (PPERF_DATA_BLOCK)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY, dwPerfDataBytes);

	wsprintf(stIndex, TEXT("%ld"), dwProcessIndex);

	// Getting performance data.
	while(RegQueryValueEx(HKEY_PERFORMANCE_DATA, 
		(LPTSTR)stIndex, 
		NULL,
		NULL,
		(LPBYTE)pdb, 
		&dwPerfDataBytes) == ERROR_MORE_DATA)
	{
		// 
		dwPerfDataBytes += 4096;

		// reallocating memory
		pdb = (PPERF_DATA_BLOCK)HeapReAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			(LPVOID)pdb,
			dwPerfDataBytes);
	}

	// Getting Counter offsets
	//

	DWORD dwIndex;
	DWORD dwProcessIdOffset    = INVALID_OFFSET;
	DWORD dwWorkingSetOffset   = INVALID_OFFSET;
	DWORD dwPrivateBytesOffset = INVALID_OFFSET;

	PPERF_OBJECT_TYPE pot;
	PPERF_COUNTER_DEFINITION pcd;

	if(!pdb)
		return false;

	// PERF_OBJECT_TYPE.
	pot = (PPERF_OBJECT_TYPE)((PBYTE)pdb + pdb->HeaderLength);

	// First counter offset.
	pcd = (PPERF_COUNTER_DEFINITION)((PBYTE)pot + pot->HeaderLength);

	for(dwIndex = 0; dwIndex < pot->NumCounters; dwIndex++)
	{
		if(pcd->CounterNameTitleIndex == dwProcessIdIndex)
		{
			dwProcessIdOffset = pcd->CounterOffset;
		}
		else if(pcd->CounterNameTitleIndex == dwWorkingSetIndex)
		{
			dwWorkingSetOffset = pcd->CounterOffset;
		}
		else if(pcd->CounterNameTitleIndex == dwPrivateBytesIndex)
		{
			dwPrivateBytesOffset = pcd->CounterOffset;
		}

		pcd = ((PPERF_COUNTER_DEFINITION)((PBYTE)pcd + pcd->ByteLength));
	}

	bool bFound = false;

	if( dwProcessIdOffset    != INVALID_OFFSET &&
		dwPrivateBytesOffset != INVALID_OFFSET)
	{
		PPERF_INSTANCE_DEFINITION pid;
		PPERF_COUNTER_BLOCK pcb;

		DWORD dwCurrentProcessId;

		// Получаем первый образец объекта. 
		pid = (PPERF_INSTANCE_DEFINITION)((PBYTE)pot + pot->DefinitionLength);

		// Получаем имя первого процесса.
		pcb = (PPERF_COUNTER_BLOCK) ((PBYTE)pid + pid->ByteLength );
		dwCurrentProcessId = *((DWORD *) ((PBYTE)pcb + dwProcessIdOffset));

		// Ищем объект процесса для переданного PID, и сравниваем его с нашим.
		for(long i = 1; i < pot->NumInstances && !bFound; i++)
		{
			if(dwProcessId == dwCurrentProcessId)
			{
				bFound = true;
				dwWorkingSet   = *((DWORD *) ((PBYTE)pcb + dwWorkingSetOffset));
				dwPrivateBytes = *((DWORD *) ((PBYTE)pcb + dwPrivateBytesOffset));
				break;
			}
			else
			{
				pid = (PPERF_INSTANCE_DEFINITION) ((PBYTE)pcb + pcb->ByteLength);
				pcb = (PPERF_COUNTER_BLOCK) ((PBYTE)pid + pid->ByteLength);
				dwCurrentProcessId = *((DWORD *) ((PBYTE)pcb + dwProcessIdOffset));
			}
		}
	}

	// Освобождаем выделенную память.
	HeapFree(GetProcessHeap(), 0, (LPVOID)pdb);

	// Закрываем дескриптор открытого ключа. 
	RegCloseKey(HKEY_PERFORMANCE_DATA);

	if(pdwWorkingSet)
		*pdwWorkingSet = dwWorkingSet;

	if(pdwPrivateBytes)
		*pdwPrivateBytes = dwPrivateBytes;

	return bFound;
}

LPTSTR __cdecl lstrrchr(LPCTSTR string, int ch)
{
	LPTSTR start = (LPTSTR)string;

	while(*string++);                       // find end of string
	// search towards front
	while(--string != start && *string != (TCHAR)ch);

	if(*string == (TCHAR)ch)				// char found ?
		return(LPTSTR)string;

	return NULL;
}

LPTSTR __cdecl GetFilePart(LPCTSTR source)
{
	LPTSTR result = lstrrchr(source, TEXT('\\'));
	
	if(result)
		result++;
	else
		result = (LPTSTR)source;

	return result;
}

} // End of namespace Elvees
