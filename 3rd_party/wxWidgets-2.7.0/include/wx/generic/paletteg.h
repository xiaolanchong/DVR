/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/paletteg.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// RCS-ID:      $Id: paletteg.h,v 1.12 2006/02/09 00:48:40 VZ Exp $
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_PALETTEG_H__
#define __WX_PALETTEG_H__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPalette;

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPalette: public wxPaletteBase
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

  public:

    wxPalette();
    wxPalette( int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue );
    ~wxPalette();
    bool operator == ( const wxPalette& palette ) const;
    bool operator != ( const wxPalette& palette ) const;
    virtual bool Ok() const;

    bool Create( int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    int GetPixel( unsigned char red, unsigned char green, unsigned char blue ) const;
    bool GetRGB( int pixel, unsigned char *red, unsigned char *green, unsigned char *blue ) const;

    // no data
};

#endif // __WX_PALETTEG_H__
