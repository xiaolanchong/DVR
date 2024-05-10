///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/nbkbase.cpp
// Purpose:     common wxNotebook methods
// Author:      Vadim Zeitlin
// Modified by:
// Created:     02.07.01
// RCS-ID:      $Id: nbkbase.cpp,v 1.22 2005/10/21 19:00:37 ABX Exp $
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_NOTEBOOK

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/notebook.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxNotebookBase::CalcSizeFromPage(const wxSize& sizePage) const
{
    // this is, of course, totally bogus -- but we must do something by
    // default because not all ports implement this
    wxSize sizeTotal = sizePage;

    if ( HasFlag(wxBK_LEFT) || HasFlag(wxBK_RIGHT) )
    {
        sizeTotal.x += 90;
        sizeTotal.y += 10;
    }
    else // tabs on top/bottom side
    {
        sizeTotal.x += 10;
        sizeTotal.y += 40;
    }

    return sizeTotal;
}

#endif // wxUSE_NOTEBOOK
