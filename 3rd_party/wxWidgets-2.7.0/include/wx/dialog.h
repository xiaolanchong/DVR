/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dialog.h
// Purpose:     wxDialogBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.06.99
// RCS-ID:      $Id: dialog.h,v 1.47 2006/01/23 03:27:31 MR Exp $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_BASE_
#define _WX_DIALOG_H_BASE_

#include "wx/defs.h"
#include "wx/containr.h"
#include "wx/toplevel.h"

class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxStdDialogButtonSizer;

#define wxDIALOG_NO_PARENT      0x0001  // Don't make owned by apps top window

#ifdef __WXWINCE__
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxMAXIMIZE | wxCLOSE_BOX | wxNO_BORDER)
#else
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#endif

extern WXDLLEXPORT_DATA(const wxChar) wxDialogNameStr[];

class WXDLLEXPORT wxDialogBase : public wxTopLevelWindow
{
public:

    enum
    {
        // all flags allowed in wxDialogBase::CreateButtonSizer()
        ButtonSizerFlags = wxOK|wxCANCEL|wxYES|wxNO|wxHELP|wxNO_DEFAULT
    };

    wxDialogBase() { Init(); }
    virtual ~wxDialogBase() { }

    void Init();

    // Modal dialogs have a return code - usually the id of the last
    // pressed button
    void SetReturnCode(int returnCode) { m_returnCode = returnCode; }
    int GetReturnCode() const { return m_returnCode; }

    // The identifier for the affirmative button
    void SetAffirmativeId(int affirmativeId) { m_affirmativeId = affirmativeId; }
    int GetAffirmativeId() const { return m_affirmativeId; }

    // Identifier for Esc key translation
    void SetEscapeId(int escapeId) { m_escapeId = escapeId; }
    int GetEscapeId() const { return m_escapeId; }

#if wxUSE_STATTEXT // && wxUSE_TEXTCTRL
    // splits text up at newlines and places the
    // lines into a vertical wxBoxSizer
    wxSizer *CreateTextSizer( const wxString &message );
#endif // wxUSE_STATTEXT // && wxUSE_TEXTCTRL

    // places buttons into a horizontal wxBoxSizer
    wxSizer *CreateButtonSizer( long flags,
                                bool separated = false,
                                wxCoord distance = 0 );
#if wxUSE_BUTTON
    wxStdDialogButtonSizer *CreateStdDialogButtonSizer( long flags );
#endif // wxUSE_BUTTON

protected:
    // The return code from modal dialog
    int m_returnCode;

    // The identifier for the affirmative button (usually wxID_OK)
    int m_affirmativeId;

    // The identifier for cancel button (usually wxID_CANCEL)
    int m_escapeId;

    DECLARE_NO_COPY_CLASS(wxDialogBase)
    DECLARE_EVENT_TABLE()
    WX_DECLARE_CONTROL_CONTAINER();
};


#if defined(__WXUNIVERSAL__) && !defined(__WXMICROWIN__)
    #include "wx/univ/dialog.h"
#else
    #if defined(__WXPALMOS__)
        #include "wx/palmos/dialog.h"
    #elif defined(__WXMSW__)
        #include "wx/msw/dialog.h"
    #elif defined(__WXMOTIF__)
        #include "wx/motif/dialog.h"
    #elif defined(__WXGTK20__)
        #include "wx/gtk/dialog.h"
    #elif defined(__WXGTK__)
        #include "wx/gtk1/dialog.h"
    #elif defined(__WXMAC__)
        #include "wx/mac/dialog.h"
    #elif defined(__WXCOCOA__)
        #include "wx/cocoa/dialog.h"
    #elif defined(__WXPM__)
        #include "wx/os2/dialog.h"
    #endif
#endif

#endif
    // _WX_DIALOG_H_BASE_
