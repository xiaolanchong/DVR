/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/palette.h
// Purpose:     wxPalette class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: palette.h,v 1.14 2006/02/09 00:49:00 VZ Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_
#define _WX_PALETTE_H_

#include "wx/gdiobj.h"

class WXDLLEXPORT wxPalette;

class WXDLLEXPORT wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPalette;
public:
    wxPaletteRefData(void);
    ~wxPaletteRefData(void);
protected:
 WXHPALETTE m_hPalette;
};

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLEXPORT wxPalette: public wxPaletteBase
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

public:
  wxPalette(void);

  wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  ~wxPalette(void);
  bool Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  int GetPixel(unsigned char red, unsigned char green, unsigned char blue) const;
  bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;

  virtual bool Ok(void) const { return (m_refData != NULL) ; }

  inline bool operator == (const wxPalette& palette) const { return m_refData == palette.m_refData; }
  inline bool operator != (const wxPalette& palette) const { return m_refData != palette.m_refData; }

  virtual bool FreeResource(bool force = false);

  inline WXHPALETTE GetHPALETTE(void) const { return (M_PALETTEDATA ? M_PALETTEDATA->m_hPalette : 0); }
  void SetHPALETTE(WXHPALETTE pal);
};

#endif
    // _WX_PALETTE_H_
