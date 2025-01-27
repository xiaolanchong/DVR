/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Robert Roebling
// RCS-ID:      $Id: accel.h,v 1.12 2006/02/09 00:48:40 VZ Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_ACCEL_H_
#define _WX_GENERIC_ACCEL_H_

class WXDLLEXPORT wxKeyEvent;

// ----------------------------------------------------------------------------
// wxAcceleratorTable
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorTable : public wxObject
{
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);
    virtual ~wxAcceleratorTable();

#if WXWIN_COMPATIBILITY_2_4
    bool operator==(const wxAcceleratorTable& accel) const
        { return m_refData == accel.m_refData; }
    bool operator!=(const wxAcceleratorTable& accel) const
        { return !(*this == accel); }
#endif

    bool Ok() const;

    void Add(const wxAcceleratorEntry& entry);
    void Remove(const wxAcceleratorEntry& entry);

    // implementation
    // --------------

    wxMenuItem *GetMenuItem(const wxKeyEvent& event) const;
    int GetCommand(const wxKeyEvent& event) const;

    const wxAcceleratorEntry *GetEntry(const wxKeyEvent& event) const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif // _WX_GENERIC_ACCEL_H_

