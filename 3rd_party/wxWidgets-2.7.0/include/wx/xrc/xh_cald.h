/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_cald.h
// Purpose:     XML resource handler for wxCalendarCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id: xh_cald.h,v 1.7 2006/07/04 18:08:06 ABX Exp $
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_CALD_H_
#define _WX_XH_CALD_H_

#include "wx/defs.h"

#if wxUSE_XRC && wxUSE_CALENDARCTRL

#include "wx/xrc/xmlres.h"

class WXDLLIMPEXP_XRC wxCalendarCtrlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxCalendarCtrlXmlHandler)
public:
    wxCalendarCtrlXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC && wxUSE_CALENDARCTRL

#endif // _WX_XH_CALD_H_
