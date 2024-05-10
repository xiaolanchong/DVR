/////////////////////////////////////////////////////////////////////////////
// Name:        common/clipcmn.cpp
// Purpose:     common (to all ports) wxClipboard functions
// Author:      Robert Roebling
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id: clipcmn.cpp,v 1.12 2005/09/23 12:52:41 MR Exp $
// Copyright:   (c) Robert Roebling
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

#include "wx/clipbrd.h"
#include "wx/module.h"

#if wxUSE_CLIPBOARD

static wxClipboard *gs_clipboard = NULL;

/*static*/ wxClipboard *wxClipboardBase::Get()
{
    if ( !gs_clipboard )
    {
        gs_clipboard = new wxClipboard;
    }
    return gs_clipboard;
}

// ----------------------------------------------------------------------------
// wxClipboardModule: module responsible for destroying the global clipboard
// object
// ----------------------------------------------------------------------------

class wxClipboardModule : public wxModule
{
public:
    bool OnInit() { return true; }
    void OnExit() { wxDELETE(gs_clipboard); }

private:
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule, wxModule)

#endif // wxUSE_CLIPBOARD
