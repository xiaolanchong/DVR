/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_hyperlink.h
// Purpose:     Hyperlink control (wxAdv)
// Author:      David Norris <danorris@gmail.com>
// Modified by: Ryan Norton, Francesco Montorsi
// Created:     04/02/2005
// RCS-ID:      $Id: xh_hyperlink.h,v 1.3 2006/05/29 11:14:31 ABX Exp $
// Copyright:   (c) 2005 David Norris
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_HYPERLINKH__
#define _WX_XH_HYPERLINKH__

#include "wx/defs.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/hyperlink.h"
#include "wx/xrc/xmlres.h"

class WXDLLIMPEXP_XRC wxHyperlinkCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    // Constructor.
    wxHyperlinkCtrlXmlHandler();

    // Creates the control and returns a pointer to it.
    virtual wxObject *DoCreateResource();

    // Returns true if we know how to create a control for the given node.
    virtual bool CanHandle(wxXmlNode *node);

    // Register with wxWindows' dynamic class subsystem.
    DECLARE_DYNAMIC_CLASS(wxHyperlinkCtrlXmlHandler)
};

#endif // wxUSE_HYPERLINKCTRL
#endif // _WX_XH_HYPERLINKH__
