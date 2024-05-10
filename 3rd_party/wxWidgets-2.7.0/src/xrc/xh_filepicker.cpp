/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_filepicker.cpp
// Purpose:     XML resource handler for wxFilePickerCtrl
// Author:      Francesco Montorsi
// Created:     2006-04-17
// RCS-ID:      $Id: xh_filepicker.cpp,v 1.4 2006/07/15 15:21:25 VS Exp $
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_FILEPICKERCTRL

#include "wx/xrc/xh_filepicker.h"
#include "wx/filepicker.h"

IMPLEMENT_DYNAMIC_CLASS(wxFilePickerCtrlXmlHandler, wxXmlResourceHandler)

wxFilePickerCtrlXmlHandler::wxFilePickerCtrlXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxFLP_OPEN);
    XRC_ADD_STYLE(wxFLP_SAVE);
    XRC_ADD_STYLE(wxFLP_OVERWRITE_PROMPT);
    XRC_ADD_STYLE(wxFLP_FILE_MUST_EXIST);
    XRC_ADD_STYLE(wxFLP_CHANGE_DIR);
    XRC_ADD_STYLE(wxFLP_DEFAULT_STYLE);
    AddWindowStyles();
}

wxObject *wxFilePickerCtrlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(picker, wxFilePickerCtrl)

    picker->Create(m_parentAsWindow,
                  GetID(),
                  GetParamValue(wxT("value")),
                  GetParamValue(wxT("message")),
                  GetParamValue(wxT("wildcard")),
                  GetPosition(), GetSize(),
                  GetStyle(_T("style"), wxFLP_DEFAULT_STYLE),
                  wxDefaultValidator,
                  GetName());

    SetupWindow(picker);
    return picker;
}

bool wxFilePickerCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxFilePickerCtrl"));
}

#endif // wxUSE_XRC && wxUSE_FILEPICKERCTRL
