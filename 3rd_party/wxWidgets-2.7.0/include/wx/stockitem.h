/////////////////////////////////////////////////////////////////////////////
// Name:        wx/stockitem.h
// Purpose:     stock items helpers (privateh header)
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-15
// RCS-ID:      $Id: stockitem.h,v 1.6 2005/09/24 21:42:09 VZ Exp $
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STOCKITEM_H_
#define _WX_STOCKITEM_H_

#include "wx/defs.h"
#include "wx/wxchar.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// Helper functions for stock items handling:
// ----------------------------------------------------------------------------

// Returns true if the ID is in the list of recognized stock actions
WXDLLEXPORT bool wxIsStockID(wxWindowID id);

// Returns true of the label is empty or label of a stock button with
// given ID
WXDLLEXPORT bool wxIsStockLabel(wxWindowID id, const wxString& label);

// Returns label that should be used for given stock UI element (e.g. "&OK"
// for wxSTOCK_OK):
WXDLLEXPORT wxString wxGetStockLabel(wxWindowID id,
                                     bool withCodes = true,
                                     const wxString& accelerator = wxEmptyString);

#ifdef __WXGTK20__
// Translates stock ID to GTK+'s stock item string indentifier:
WXDLLEXPORT const char *wxGetStockGtkID(wxWindowID id);
#endif

#endif // _WX_STOCKITEM_H_
