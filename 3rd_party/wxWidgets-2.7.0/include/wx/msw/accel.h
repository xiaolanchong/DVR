/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Julian Smart
// Modified by:
// Created:     31/7/98
// RCS-ID:      $Id: accel.h,v 1.17 2006/05/29 06:53:30 PC Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

class WXDLLIMPEXP_CORE wxWindow;

// ----------------------------------------------------------------------------
// the accel table has all accelerators for a given window or menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorTable : public wxObject
{
public:
    // default ctor
    wxAcceleratorTable() { }

    // load from .rc resource (Windows specific)
    wxAcceleratorTable(const wxString& resource);

    // initialize from array
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    bool operator==(const wxAcceleratorTable& accel) const;
    bool operator!=(const wxAcceleratorTable& accel) const
        { return !(*this == accel); }

    bool Ok() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL() const;

    // translate the accelerator, return true if done
    bool Translate(wxWindow *window, WXMSG *msg) const;

private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif
    // _WX_ACCEL_H_
