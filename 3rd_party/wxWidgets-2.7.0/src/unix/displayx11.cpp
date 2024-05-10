///////////////////////////////////////////////////////////////////////////
// Name:        src/unix/displayx11.cpp
// Purpose:     Unix/X11 implementation of wxDisplay class
// Author:      Brian Victor, Vadim Zeitlin
// Modified by:
// Created:     12/05/02
// RCS-ID:      $Id: displayx11.cpp,v 1.26 2006/04/26 08:21:22 ABX Exp $
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#include "wx/display.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/gdicmn.h"
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif /* WX_PRECOMP */

#include "wx/display_impl.h"

/* These must be included after the wx files.  Otherwise the Data macro in
 * Xlibint.h conflicts with a function declaration in wx/list.h.  */
extern "C"
{
    #include <X11/Xlib.h>
    #include <X11/Xlibint.h>

    #include <X11/extensions/Xinerama.h>
    #ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
        #include <X11/extensions/xf86vmode.h>
    #endif
}

// ----------------------------------------------------------------------------
// helper class to automatically free XineramaQueryScreens() return value
// ----------------------------------------------------------------------------

class ScreensInfo
{
public:
    ScreensInfo()
    {
        m_screens = XineramaQueryScreens((Display *)wxGetDisplay(), &m_num);
    }

    ~ScreensInfo()
    {
        XFree(m_screens);
    }

    operator const XineramaScreenInfo *() const { return m_screens; }

    size_t GetCount() const { return wx_static_cast(size_t, m_num); }

private:
    XineramaScreenInfo *m_screens;
    int m_num;
};

// ----------------------------------------------------------------------------
// display and display factory classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDisplayImplX11 : public wxDisplayImpl
{
public:
    wxDisplayImplX11(size_t n, const XineramaScreenInfo& info)
        : wxDisplayImpl(n),
          m_rect(info.x_org, info.y_org, info.width, info.height)
    {
    }

    virtual wxRect GetGeometry() const { return m_rect; }
    virtual wxString GetName() const { return wxString(); }

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode);

private:
    wxRect m_rect;
    int m_depth;

    DECLARE_NO_COPY_CLASS(wxDisplayImplX11)
};

class wxDisplayFactoryX11 : public wxDisplayFactory
{
public:
    wxDisplayFactoryX11() { }

    virtual wxDisplayImpl *CreateDisplay(size_t n);
    virtual size_t GetCount();
    virtual int GetFromPoint(const wxPoint& pt);

protected:
    DECLARE_NO_COPY_CLASS(wxDisplayFactoryX11)
};

// ============================================================================
// wxDisplayFactoryX11 implementation
// ============================================================================

size_t wxDisplayFactoryX11::GetCount()
{
    return ScreensInfo().GetCount();
}

int wxDisplayFactoryX11::GetFromPoint(const wxPoint& p)
{
    ScreensInfo screens;

    const size_t numscreens(screens.GetCount());
    for ( size_t i = 0; i < numscreens; ++i )
    {
        const XineramaScreenInfo& s = screens[i];
        if ( p.x >= s.x_org && p.x < s.x_org + s.width &&
                p.y >= s.y_org && p.y < s.y_org + s.height )
        {
            return i;
        }
    }

    return wxNOT_FOUND;
}

wxDisplayImpl *wxDisplayFactoryX11::CreateDisplay(size_t n)
{
    ScreensInfo screens;

    return n < screens.GetCount() ? new wxDisplayImplX11(n, screens[n]) : NULL;
}

// ============================================================================
// wxDisplayImplX11 implementation
// ============================================================================

#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

//
//  See (http://www.xfree86.org/4.2.0/XF86VidModeDeleteModeLine.3.html) for more
//  info about xf86 video mode extensions
//

//free private data common to x (usually s3) servers
#define wxClearXVM(vm)  if(vm.privsize) XFree(vm.c_private)

// Correct res rate from GLFW
#define wxCRR2(v,dc) (int) (((1000.0f * (float) dc) /*PIXELS PER SECOND */) / ((float) v.htotal * v.vtotal /*PIXELS PER FRAME*/) + 0.5f)
#define wxCRR(v) wxCRR2(v,v.dotclock)
#define wxCVM2(v, dc) wxVideoMode(v.hdisplay, v.vdisplay, DefaultDepth((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay())), wxCRR2(v,dc))
#define wxCVM(v) wxCVM2(v, v.dotclock)

wxArrayVideoModes wxDisplayImplX11::GetModes(const wxVideoMode& mode) const
{
    //Convenience...
    Display* pDisplay = (Display*) wxGetDisplay(); //default display
    int nScreen = DefaultScreen(pDisplay); //default screen of (default) display...

    //Some variables..
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    wxArrayVideoModes Modes; //modes to return...

    if (XF86VidModeGetAllModeLines(pDisplay, nScreen, &nNumModes, &ppXModes) == TRUE)
    {
        for (int i = 0; i < nNumModes; ++i)
        {
            if (mode == wxDefaultVideoMode || //According to display.h All modes valid if dafault mode...
                mode.Matches(wxCVM((*ppXModes[i]))) ) //...?
            {
                Modes.Add(wxCVM((*ppXModes[i])));
            }
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
        XFree(ppXModes);
    }
    else //OOPS!
    {
        wxLogSysError(_("Failed to enumerate video modes"));
    }

    return Modes;
}

wxVideoMode wxDisplayImplX11::GetCurrentMode() const
{
  XF86VidModeModeLine VM;
  int nDotClock;
  XF86VidModeGetModeLine((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                         &nDotClock, &VM);
  wxClearXVM(VM);
  return wxCVM2(VM, nDotClock);
}

bool wxDisplayImplX11::ChangeMode(const wxVideoMode& mode)
{
    XF86VidModeModeInfo** ppXModes; //Enumerated Modes (Don't forget XFree() :))
    int nNumModes; //Number of modes enumerated....

    if( !XF86VidModeGetAllModeLines((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()), &nNumModes, &ppXModes) )
    {
        wxLogSysError(_("Failed to change video mode"));
        return false;
    }

    bool bRet = false;
    if (mode == wxDefaultVideoMode)
    {
        bRet = XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                     ppXModes[0]) == TRUE;

        for (int i = 0; i < nNumModes; ++i)
        {
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
    }
    else
    {
        for (int i = 0; i < nNumModes; ++i)
        {
            if (!bRet &&
                ppXModes[i]->hdisplay == mode.w &&
                ppXModes[i]->vdisplay == mode.h &&
                wxCRR((*ppXModes[i])) == mode.refresh)
            {
                //switch!
                bRet = XF86VidModeSwitchToMode((Display*)wxGetDisplay(), DefaultScreen((Display*)wxGetDisplay()),
                         ppXModes[i]) == TRUE;
            }
            wxClearXVM((*ppXModes[i]));
        //  XFree(ppXModes[i]); //supposed to free?
        }
    }

    XFree(ppXModes);

    return bRet;
}


#else // !HAVE_X11_EXTENSIONS_XF86VMODE_H

wxArrayVideoModes wxDisplayImplX11::GetModes(const wxVideoMode& mode) const
{
    int count_return;
    int* depths = XListDepths((Display*)wxGetDisplay(), 0, &count_return);
    wxArrayVideoModes modes;
    if ( depths )
    {
        for ( int x = 0; x < count_return; ++x )
        {
            modes.Add(wxVideoMode(m_rect.GetWidth(), m_rect.GetHeight(), depths[x]));
        }

        XFree(depths);
    }
    return modes;
}

wxVideoMode wxDisplayImplX11::GetCurrentMode() const
{
    // Not implemented
    return wxVideoMode();
}

bool wxDisplayImplX11::ChangeMode(const wxVideoMode& mode)
{
    // Not implemented
    return false;
}

#endif // !HAVE_X11_EXTENSIONS_XF86VMODE_H

// ============================================================================
// wxDisplay::CreateFactory()
// ============================================================================

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    if ( XineramaIsActive((Display*)wxGetDisplay()) )
    {
        return new wxDisplayFactoryX11;
    }

    return new wxDisplayFactorySingle;
}

#endif /* wxUSE_DISPLAY */
