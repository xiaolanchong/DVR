#pragma once

// in convert_a.asm
extern "C" 
{
  extern void __cdecl mmx_YUY2toRGB24(const BYTE* src, BYTE* dst, const BYTE* src_end, int src_pitch, int row_size, int matrix);
  extern void __cdecl mmx_YUY2toRGB32(const BYTE* src, BYTE* dst, const BYTE* src_end, int src_pitch, int row_size, int matrix);
}
