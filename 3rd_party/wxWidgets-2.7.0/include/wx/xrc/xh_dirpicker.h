/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_clrpicker.cpp
// Purpose:     XML resource handler for wxColourPickerCtrl
// Author:      Francesco Montorsi
// Created:     2006-04-17
// RCS-ID:      $Id: xh_dirpicker.h,v 1.2 2006/07/04 18:08:06 ABX Exp $
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_DIRPICKERCTRL_H_
#define _WX_XH_DIRPICKERCTRL_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_DIRPICKERCTRL

class WXDLLIMPEXP_XRC wxDirPickerCtrlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxDirPickerCtrlXmlHandler)
public:
    wxDirPickerCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_DIRPICKERCTRL

#endif // _WX_XH_DIRPICKERCTRL_H_
