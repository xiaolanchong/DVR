// mapmem.h
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAPIMG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAPIMG_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifndef MAPIMG_DLL_DIRECT_ACCESS 
#ifdef  MAPIMG_EXPORTS
#define MAPIMG_API __declspec(dllexport)
#else
#define MAPIMG_API __declspec(dllimport)
//#pragma comment( lib, "mapimg.lib" )
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees::Win32
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// Orwell Image Interface
///////////////////////////////////////////////////////////////////////////////

class CImage
{
public:
	virtual long AddRef() = 0;
	virtual long Release() = 0;

	virtual bool IsManaged() = 0;
	virtual long GetSourceId() = 0;

	virtual bool GetImageInfo(LPBITMAPINFO pBih) = 0;
	virtual bool GetImageBytes(LPBYTE *ppBytes) = 0;
	virtual bool GetImageTime(double *pTime) = 0;

	virtual bool Draw(HDC hDC, long x, long y, long cx, long cy) = 0;
	virtual bool SaveImage(unsigned short *stImageFile) = 0;

	virtual bool CreateCopy(CImage **ppImage) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Image Creation
///////////////////////////////////////////////////////////////////////////////

#ifndef MAPIMG_DLL_DIRECT_ACCESS

MAPIMG_API bool __stdcall CreateImage(
	CImage **ppImage,  // CImage interface
	long lSourceID);   // Maped memory file base name

MAPIMG_API bool __stdcall CreateImageIndirect(
	CImage **ppImage,  // CImage interface
	LPBITMAPINFO pBih, // Bitmap info
	LPBYTE pBytes);    // Image bytes

#endif

///////////////////////////////////////////////////////////////////////////////
// Image Creation Functions Prototypes
//    for direct DLL access
///////////////////////////////////////////////////////////////////////////////

typedef bool (__stdcall *f_CreateImage)(CImage**, long);
typedef bool (__stdcall *f_CreateImageIndirect)(CImage**, LPBITMAPINFO, LPBYTE);

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace Elvees


//
// Create example:
// 
// Elvees::Win32::f_CreateImage CreateImage = GetProcAddress([HMODULE], "CreateImage");
//

#ifdef MAPIMG_NO_NAMESPACE
	using Elvees::Win32::CImage;

#ifndef MAPIMG_DLL_DIRECT_ACCESS 
	using Elvees::Win32::CreateImage;
	using Elvees::Win32::CreateImageIndirect;
#endif

	using Elvees::Win32::f_CreateImage;
	using Elvees::Win32::f_CreateImageIndirect;
#endif






