/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_stlin.h
// Purpose:     XML resource handler for wxStaticLine
// Author:      Vaclav Slavik
// Created:     2000/09/00
// RCS-ID:      $Id: xh_stlin.h,v 1.7 2006/07/04 18:08:08 ABX Exp $
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STLIN_H_
#define _WX_XH_STLIN_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_STATLINE

class WXDLLIMPEXP_XRC wxStaticLineXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxStaticLineXmlHandler)
public:
    wxStaticLineXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_STLIN_H_
