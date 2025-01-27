/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_gdctl.h
// Purpose:     XML resource handler for wxGenericDirCtrl
// Author:      Markus Greither
// Created:     2002/01/20
// RCS-ID:      $Id: xh_gdctl.h,v 1.7 2006/07/04 18:08:07 ABX Exp $
// Copyright:   (c) 2002 Markus Greither
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_GDCTL_H_
#define _WX_XH_GDCTL_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_DIRDLG

class WXDLLIMPEXP_XRC wxGenericDirCtrlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxGenericDirCtrlXmlHandler)
public:
    wxGenericDirCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_GDCTL_H_
