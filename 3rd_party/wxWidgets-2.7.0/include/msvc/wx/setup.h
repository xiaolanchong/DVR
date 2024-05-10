/////////////////////////////////////////////////////////////////////////////
// Name:        msvc/wx/msw/setup.h
// Purpose:     wrapper around the real wx/setup.h for Visual C++
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-12-12
// RCS-ID:      $Id: setup.h,v 1.15 2006/06/29 07:11:51 JS Exp $
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// VC++ IDE predefines _DEBUG and _UNICODE for the new projects itself, but
// the other symbols (WXUSINGDLL, __WXUNIVERSAL__, ...) should be defined
// explicitly!

#ifdef _MSC_VER
    #ifdef _UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../../lib/vc_dll/mswud/wx/setup.h"
            #else
                #include "../../../lib/vc_dll/mswu/wx/setup.h"
            #endif
        #else
            #ifdef _DEBUG
                #include "../../../lib/vc_lib/mswud/wx/setup.h"
            #else
                #include "../../../lib/vc_lib/mswu/wx/setup.h"
            #endif
        #endif

        #ifdef _DEBUG
            #if wxUSE_XML
                #pragma comment(lib,"wxexpatd")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpegd")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpngd")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexud")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiffd")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlibd")
            #endif
            #pragma comment(lib,"wxbase27ud")
            #pragma comment(lib,"wxbase27ud_net")
            #pragma comment(lib,"wxbase27ud_xml")
            #pragma comment(lib,"wxmsw27ud_adv")
            #pragma comment(lib,"wxmsw27ud_core")
            #pragma comment(lib,"wxmsw27ud_html")
            #if wxUSE_DEBUGREPORT
            #pragma comment(lib,"wxmsw27ud_qa")
            #endif
            #pragma comment(lib,"wxmsw27ud_xrc")
            #pragma comment(lib,"wxmsw27ud_aui")
            #pragma comment(lib,"wxmsw27ud_richtext")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw27ud_media")
            #endif
            #if wxUSE_ODBC
                #pragma comment(lib,"wxbase27ud_odbc")
            #endif
        #else // release
            #if wxUSE_XML
                #pragma comment(lib,"wxexpat")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpeg")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpng")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexu")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiff")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlib")
            #endif
            #pragma comment(lib,"wxbase27u")
            #pragma comment(lib,"wxbase27u_net")
            #pragma comment(lib,"wxbase27u_xml")
            #pragma comment(lib,"wxmsw27u_adv")
            #pragma comment(lib,"wxmsw27u_core")
            #pragma comment(lib,"wxmsw27u_html")
            #if wxUSE_DEBUGREPORT
            #pragma comment(lib,"wxmsw27u_qa")
            #endif
            #pragma comment(lib,"wxmsw27u_xrc")
            #pragma comment(lib,"wxmsw27u_aui")
            #pragma comment(lib,"wxmsw27u_richtext")
            #if wxUSE_MEDIACTRL
                #pragma comment(lib,"wxmsw27u_media")
            #endif
            #if wxUSE_ODBC
                #pragma comment(lib,"wxbase27u_odbc")
            #endif
        #endif // debug/release
    #else // !_UNICODE
        #ifdef WXUSINGDLL
            #ifdef _DEBUG
                #include "../../../lib/vc_dll/mswd/wx/setup.h"
            #else
                #include "../../../lib/vc_dll/msw/wx/setup.h"
            #endif
        #else // static lib
            #ifdef _DEBUG
                #include "../../../lib/vc_lib/mswd/wx/setup.h"
            #else
                #include "../../../lib/vc_lib/msw/wx/setup.h"
            #endif
        #endif // shared/static

        #ifdef _DEBUG
            #if wxUSE_XML
                #pragma comment(lib,"wxexpatd")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpegd")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpngd")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregexd")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiffd")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlibd")
            #endif
            
            #pragma comment(lib,"wxmsw27d")
            #if wxUSE_GLCANVAS
            	#pragma comment(lib,"wxmsw27d_gl")
            #endif
        #else // release
            #if wxUSE_XML
                #pragma comment(lib,"wxexpat")
            #endif
            #if wxUSE_LIBJPEG
                #pragma comment(lib,"wxjpeg")
            #endif
            #if wxUSE_LIBPNG
                #pragma comment(lib,"wxpng")
            #endif
            #if wxUSE_REGEX
                #pragma comment(lib,"wxregex")
            #endif
            #if wxUSE_LIBTIFF
                #pragma comment(lib,"wxtiff")
            #endif
            #if wxUSE_ZLIB
                #pragma comment(lib,"wxzlib")
            #endif
            
            #pragma comment(lib,"wxmsw27")
            #if wxUSE_GLCANVAS
            	#pragma comment(lib,"wxmsw27_gl")
            #endif
        #endif // debug/release
    #endif // _UNICODE/!_UNICODE
#else
    #error "This file should only be included when using Microsoft Visual C++"
#endif

