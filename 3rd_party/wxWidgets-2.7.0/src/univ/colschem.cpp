///////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/colschem.cpp
// Purpose:     (trivial) wxColourScheme implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.08.00
// RCS-ID:      $Id: colschem.cpp,v 1.13 2006/05/24 07:16:25 ABX Exp $
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxColourScheme
// ----------------------------------------------------------------------------

wxColourScheme::~wxColourScheme()
{
}

// ----------------------------------------------------------------------------
// wxSystemSettings
// ----------------------------------------------------------------------------

wxColour wxSystemSettings::GetColour(wxSystemColour index)
{
    // the elements of this enum must be kept in sync with wxSystemColour
    static const wxColourScheme::StdColour s_mapSysToThemeCol[] =
    {
        wxColourScheme::SCROLLBAR /* wxSYS_COLOUR_SCROLLBAR */,
        wxColourScheme::WINDOW /* wxSYS_COLOUR_BACKGROUND */,
        wxColourScheme::TITLEBAR_ACTIVE_TEXT /* wxSYS_COLOUR_ACTIVECAPTION */,
        wxColourScheme::TITLEBAR_TEXT /* wxSYS_COLOUR_INACTIVECAPTION */,
        wxColourScheme::MAX /* wxSYS_COLOUR_MENU */,
        wxColourScheme::MAX /* wxSYS_COLOUR_WINDOW */,
        wxColourScheme::MAX /* wxSYS_COLOUR_WINDOWFRAME */,
        wxColourScheme::CONTROL_TEXT /* wxSYS_COLOUR_MENUTEXT */,
        wxColourScheme::CONTROL_TEXT /* wxSYS_COLOUR_WINDOWTEXT */,
        wxColourScheme::CONTROL_TEXT /* wxSYS_COLOUR_CAPTIONTEXT */,
        wxColourScheme::MAX /* wxSYS_COLOUR_ACTIVEBORDER */,
        wxColourScheme::MAX /* wxSYS_COLOUR_INACTIVEBORDER */,
        wxColourScheme::MAX /* wxSYS_COLOUR_APPWORKSPACE */,
        wxColourScheme::HIGHLIGHT /* wxSYS_COLOUR_HIGHLIGHT */,
        wxColourScheme::HIGHLIGHT_TEXT /* wxSYS_COLOUR_HIGHLIGHTTEXT */,
        wxColourScheme::CONTROL /* wxSYS_COLOUR_BTNFACE */,
        wxColourScheme::SHADOW_DARK /* wxSYS_COLOUR_BTNSHADOW */,
        wxColourScheme::CONTROL_TEXT_DISABLED /* wxSYS_COLOUR_GRAYTEXT */,
        wxColourScheme::CONTROL_TEXT /* wxSYS_COLOUR_BTNTEXT */,
        wxColourScheme::MAX /* wxSYS_COLOUR_INACTIVECAPTIONTEXT */,
        wxColourScheme::SHADOW_HIGHLIGHT /* wxSYS_COLOUR_BTNHIGHLIGHT */,
        wxColourScheme::SHADOW_DARK /* wxSYS_COLOUR_3DDKSHADOW */,
        wxColourScheme::SHADOW_OUT /* wxSYS_COLOUR_3DLIGHT */,
        wxColourScheme::MAX /* wxSYS_COLOUR_INFOTEXT */,
        wxColourScheme::MAX /* wxSYS_COLOUR_INFOBK */,
        wxColourScheme::WINDOW /* wxSYS_COLOUR_LISTBOX */,
    };

    wxCHECK_MSG( index < (int)WXSIZEOF(s_mapSysToThemeCol), wxNullColour,
                 _T("invalid wxSystemColour") );

    wxColourScheme::StdColour col = s_mapSysToThemeCol[index];
    if ( col == wxColourScheme::MAX )
    {
        // we don't have theme-equivalent for this colour
        return wxSystemSettingsNative::GetColour(index);
    }

    return wxTheme::Get()->GetColourScheme()->Get(col);
}
