/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_listc.h
// Purpose:     XML resource handler for wxCalendarCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id: xh_listc.h,v 1.7 2006/07/04 18:08:07 ABX Exp $
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_LISTC_H_
#define _WX_XH_LISTC_H_

#include "wx/xrc/xmlres.h"

class WXDLLIMPEXP_XRC wxListCtrlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxListCtrlXmlHandler)
public:
    wxListCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_LISTC_H_
