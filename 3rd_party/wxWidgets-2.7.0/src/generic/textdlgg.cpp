/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: textdlgg.cpp,v 1.42 2006/05/26 19:02:53 RD Exp $
// Copyright:   (c) Julian Smart
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

#if wxUSE_TEXTDLG

#include "wx/generic/textdlgg.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

const wxChar wxGetTextFromUserPromptStr[] = wxT("Input Text");
const wxChar wxGetPasswordFromUserPromptStr[] = wxT("Enter Password");

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int wxID_TEXT = 3000;

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

/* Macro for avoiding #ifdefs when value have to be different depending on size of
   device we display on - take it from something like wxDesktopPolicy in the future
 */

#if defined(__SMARTPHONE__)
    #define wxLARGESMALL(large,small) small
#else
    #define wxLARGESMALL(large,small) large
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTextEntryDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent,
                                     const wxString& message,
                                     const wxString& caption,
                                     const wxString& value,
                                     long style,
                                     const wxPoint& pos)
                 : wxDialog(parent, wxID_ANY, caption, pos, wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE),
                   m_value(value)
{
    m_dialogStyle = style;
    m_value = value;

    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

#if wxUSE_STATTEXT
    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, wxLARGESMALL(10,0) );
#endif

    // 2) text ctrl
    m_textctrl = new wxTextCtrl(this, wxID_TEXT, value,
                                wxDefaultPosition, wxSize(300, wxDefaultCoord),
                                style & ~wxTextEntryDialogStyle);
    topsizer->Add( m_textctrl, style & wxTE_MULTILINE ? 1 : 0, wxEXPAND | wxLEFT|wxRIGHT, wxLARGESMALL(15,0) );

#if wxUSE_VALIDATORS
    wxTextValidator validator( wxFILTER_NONE, &m_value );
    m_textctrl->SetValidator( validator );
#endif // wxUSE_VALIDATORS

    // 3) buttons if any
    wxSizer *buttonSizer = CreateButtonSizer( style & ButtonSizerFlags , true, wxLARGESMALL(10,0) );
    if(buttonSizer->GetChildren().GetCount() > 0 )
    {
        topsizer->Add( buttonSizer, 0, wxEXPAND | wxALL, wxLARGESMALL(10,0) );
    }
    else
    {
        topsizer->AddSpacer( wxLARGESMALL(15,0) );
        delete buttonSizer;
    }

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    if ( style & wxCENTRE )
        Centre( wxBOTH );

    m_textctrl->SetSelection(-1, -1);
    m_textctrl->SetFocus();

    wxEndBusyCursor();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_VALIDATORS
    if( Validate() && TransferDataFromWindow() )
    {
        EndModal( wxID_OK );
    }
#else
    m_value = m_textctrl->GetValue();

    EndModal(wxID_OK);
#endif
  // wxUSE_VALIDATORS
}

void wxTextEntryDialog::SetValue(const wxString& val)
{
    m_value = val;

    m_textctrl->SetValue(val);
}

#if wxUSE_VALIDATORS
void wxTextEntryDialog::SetTextValidator( long style )
{
    wxTextValidator validator( style, &m_value );
    m_textctrl->SetValidator( validator );
}

void wxTextEntryDialog::SetTextValidator( const wxTextValidator& validator )
{
    m_textctrl->SetValidator( validator );
}

#endif
  // wxUSE_VALIDATORS

// ----------------------------------------------------------------------------
// wxPasswordEntryDialog
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxPasswordEntryDialog, wxTextEntryDialog)

wxPasswordEntryDialog::wxPasswordEntryDialog(wxWindow *parent,
                                     const wxString& message,
                                     const wxString& caption,
                                     const wxString& value,
                                     long style,
                                     const wxPoint& pos)
                 : wxTextEntryDialog(parent, message, caption, value,
                                     style | wxTE_PASSWORD, pos)
{
    // Only change from wxTextEntryDialog is the password style
}

#endif // wxUSE_TEXTDLG
