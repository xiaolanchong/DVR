/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/colour.h
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: colour.h,v 1.25 2006/04/24 14:51:50 ABX Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/object.h"

// ----------------------------------------------------------------------------
// Colour
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxColour : public wxColourBase
{
public:
    // constructors
    // ------------

    // default
    wxColour() { Init(); }
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS


    // dtor
    ~wxColour();


    // accessors
    // ---------

    bool Ok() const { return m_isInit; }

    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }

    // comparison
    bool operator==(const wxColour& colour) const
    {
        return m_isInit == colour.m_isInit
            && m_red == colour.m_red
            && m_green == colour.m_green
            && m_blue == colour.m_blue;
    }

    bool operator != (const wxColour& colour) const { return !(*this == colour); }

    WXCOLORREF GetPixel() const { return m_pixel; };


public:
    WXCOLORREF m_pixel;

protected:
    // Helper function
    void Init();

    virtual void InitWith(unsigned char red, unsigned char green, unsigned char blue);

private:
    bool          m_isInit;
    unsigned char m_red;
    unsigned char m_blue;
    unsigned char m_green;

private:
    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif
        // _WX_COLOUR_H_
