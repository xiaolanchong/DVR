/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_radbt.h
// Purpose:     XML resource handler for radio buttons
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id: xh_radbt.h,v 1.7 2006/07/04 18:08:07 ABX Exp $
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_RADBT_H_
#define _WX_XH_RADBT_H_

#include "wx/xrc/xmlres.h"
#include "wx/defs.h"

#if wxUSE_RADIOBOX

class WXDLLIMPEXP_XRC wxRadioButtonXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxRadioButtonXmlHandler)
public:
    wxRadioButtonXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif

#endif // _WX_XH_RADIOBUTTON_H_
