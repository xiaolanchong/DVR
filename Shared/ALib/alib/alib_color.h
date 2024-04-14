///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_color.h
// ---------------------
// begin     : 1998
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaahaaah@hotmail.com, aaah@mail.ru
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_COLOR_H__
#define __DEFINE_ALIB_COLOR_H__

namespace alib
{

enum ColorIndex
{
  BLACK = 0, // RGB(0x00,0x00,0x00), // 0  Black
  MAROON,    // RGB(0x80,0x00,0x00), // 1  Maroon
  GREEN,     // RGB(0x00,0x80,0x00), // 2  Green
  NAVY,      // RGB(0x00,0x00,0x80), // 3  Navy
  OLIVE,     // RGB(0x80,0x80,0x00), // 4  Olive
  PURPLE,    // RGB(0x80,0x00,0x80), // 5  Purple
  TEAL,      // RGB(0x00,0x80,0x80), // 6  Teal
  GRAY,      // RGB(0x80,0x80,0x80), // 7  Gray
  RED,       // RGB(0xFF,0x00,0x00), // 8  Red
  LIME,      // RGB(0x00,0xFF,0x00), // 9  Lime
  BLUE,      // RGB(0x00,0x00,0xFF), // 10 Blue
  YELLOW,    // RGB(0xFF,0xFF,0x00), // 11 Yellow
  FUCHSIA,   // RGB(0xFF,0x00,0xFF), // 12 Fuchsia
  AQUA,      // RGB(0x00,0xFF,0xFF), // 13 Aqua
  WHITE,     // RGB(0xFF,0xFF,0xFF), // 14 White
  SILVER     // RGB(0xC0,0xC0,0xC0)  // 15 Silver
};

const RGBQUAD Colors16[16] = { {0x00,0x00,0x00,0x00},    // Black
                               {0x00,0x00,0x80,0x00},    // Maroon
                               {0x00,0x80,0x00,0x00},    // Green
                               {0x80,0x00,0x00,0x00},    // Navy
                               {0x00,0x80,0x80,0x00},    // Olive
                               {0x80,0x00,0x80,0x00},    // Purple
                               {0x80,0x80,0x00,0x00},    // Teal
                               {0x80,0x80,0x80,0x00},    // Gray
                               {0x00,0x00,0xFF,0x00},    // Red
                               {0x00,0xFF,0x00,0x00},    // Lime
                               {0xFF,0x00,0x00,0x00},    // Blue
                               {0x00,0xFF,0xFF,0x00},    // Yellow
                               {0xFF,0x00,0xFF,0x00},    // Fuchsia
                               {0xFF,0xFF,0x00,0x00},    // Aqua
                               {0xFF,0xFF,0xFF,0x00},    // White
                               {0xC0,0xC0,0xC0,0x00} };  // Silver

} // namespace alib

#endif // __DEFINE_ALIB_COLOR_H__

