/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fontenumcmn.cpp
// Purpose:     wxFontEnumerator class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     7/5/2006
// RCS-ID:      $Id: fontenumcmn.cpp,v 1.2 2006/05/29 00:03:20 VZ Exp $
// Copyright:   (c) 1999-2003 Vadim Zeitlin <vadim@wxwindows.org>
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

#include "wx/fontenum.h"

// ============================================================================
// implementation
// ============================================================================

// A simple wxFontEnumerator which doesn't perform any filtering and
// just returns all facenames and encodings found in the system
class WXDLLEXPORT wxSimpleFontEnumerator : public wxFontEnumerator
{
public:
    wxSimpleFontEnumerator() { }

    // called by EnumerateFacenames
    virtual bool OnFacename(const wxString& facename)
    {
        m_arrFacenames.Add(facename);
        return true;
    }

    // called by EnumerateEncodings
    virtual bool OnFontEncoding(const wxString& WXUNUSED(facename),
                                const wxString& encoding)
    {
        m_arrEncodings.Add(encoding);
        return true;
    }

public:
    wxArrayString m_arrFacenames, m_arrEncodings;
};


/* static */
wxArrayString wxFontEnumerator::GetFacenames(wxFontEncoding encoding, bool fixedWidthOnly)
{
    wxSimpleFontEnumerator temp;
    temp.EnumerateFacenames(encoding, fixedWidthOnly);
    return temp.m_arrFacenames;
}

/* static */
wxArrayString wxFontEnumerator::GetEncodings(const wxString& facename)
{
    wxSimpleFontEnumerator temp;
    temp.EnumerateEncodings(facename);
    return temp.m_arrEncodings;
}

/* static */
bool wxFontEnumerator::IsValidFacename(const wxString &facename)
{
    // we cache the result of wxFontEnumerator::GetFacenames supposing that
    // the array of face names won't change in the session of this program
    static wxArrayString s_arr = wxFontEnumerator::GetFacenames();

#ifdef __WXMSW__
    // Quoting the MSDN:
    //     "MS Shell Dlg is a mapping mechanism that enables 
    //     U.S. English Microsoft Windows NT, and Microsoft Windows 2000 to 
    //     support locales that have characters that are not contained in code 
    //     page 1252. It is not a font but a face name for a nonexistent font."
    // Thus we need to consider "Ms Shell Dlg" and "Ms Shell Dlg 2" as valid
    // font face names even if they are enumerated by wxFontEnumerator
    if (facename.IsSameAs(wxT("Ms Shell Dlg"), false) ||
        facename.IsSameAs(wxT("Ms Shell Dlg 2"), false))
        return true;
#endif

    // is given font face name a valid one ?
    if (s_arr.Index(facename, false) == wxNOT_FOUND)
        return false;

    return true;
}

