/////////////////////////////////////////////////////////////////////////////
// Name:        helpwce.h
// Purpose:     Help system: Windows CE help implementation
// Author:      Julian Smart
// Modified by:
// Created:     2003-07-12
// RCS-ID:      $Id: helpwce.cpp,v 1.9 2005/09/23 12:55:26 MR Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HELP

#include "wx/filefn.h"
#include "wx/msw/wince/helpwce.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/missing.h"

IMPLEMENT_DYNAMIC_CLASS(wxWinceHelpController, wxHelpControllerBase)

bool wxWinceHelpController::Initialize(const wxString& filename)
{
    m_helpFile = filename;
    return true;
}

bool wxWinceHelpController::LoadFile(const wxString& file)
{
    if (!file.IsEmpty())
        m_helpFile = file;
    return true;
}

bool wxWinceHelpController::DisplayContents()
{
    return ViewURL();
}

// Use topic
bool wxWinceHelpController::DisplaySection(const wxString& section)
{
    return ViewURL(section);
}

// Use context number
bool wxWinceHelpController::DisplaySection(int WXUNUSED(section))
{
    return true;
}

bool wxWinceHelpController::DisplayContextPopup(int WXUNUSED(contextId))
{
    return true;
}

bool wxWinceHelpController::DisplayTextPopup(const wxString& WXUNUSED(text), const wxPoint& WXUNUSED(pos))
{
    return true;
}

bool wxWinceHelpController::DisplayBlock(long WXUNUSED(block))
{
    return true;
}

bool wxWinceHelpController::KeywordSearch(const wxString& WXUNUSED(k),
                               wxHelpSearchMode WXUNUSED(mode))
{
    return true;
}

bool wxWinceHelpController::Quit()
{
    return true;
}

// Append extension if necessary.
wxString wxWinceHelpController::GetValidFilename(const wxString& file) const
{
    wxString path, name, ext;
    wxSplitPath(file, & path, & name, & ext);

    wxString fullName;
    if (path.IsEmpty())
        fullName = name + wxT(".htm");
    else if (path.Last() == wxT('\\'))
        fullName = path + name + wxT(".htm");
    else
        fullName = path + wxT("\\") + name + wxT(".htm");

    if (!wxFileExists(fullName))
        fullName = wxT("\\Windows\\") + name + wxT(".htm");

    return fullName;
}

// View URL
bool wxWinceHelpController::ViewURL(const wxString& topic)
{
    if (m_helpFile.IsEmpty()) return false;

    wxString url( wxT("file:") + GetValidFilename(m_helpFile) );
    if (!topic.IsEmpty())
        url = url + wxT("#") + topic;

    return CreateProcess(wxT("peghelp.exe"),
        url, NULL, NULL, FALSE, 0, NULL,
        NULL, NULL, NULL) != 0 ;
}

#endif // wxUSE_HELP

