/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_datectrl.h
// Purpose:     XML resource handler for wxDatePickerCtrl
// Author:      Vaclav Slavik
// Created:     2005-02-07
// RCS-ID:      $Id: xh_datectrl.h,v 1.4 2006/07/04 18:08:06 ABX Exp $
// Copyright:   (c) 2005 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_DATECTRL_H_
#define _WX_XH_DATECTRL_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_DATEPICKCTRL

class WXDLLIMPEXP_XRC wxDateCtrlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxDateCtrlXmlHandler)
public:
    wxDateCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_DATEPICKCTRL

#endif // _WX_XH_DATECTRL_H_
