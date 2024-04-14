//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Rectangle 2D-array arrange

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _LAYOUT_RECT_1459803152261875_
#define _LAYOUT_RECT_1459803152261875_

#include "LayoutIndex.h"
//======================================================================================//
//                                   class LayoutRect                                   //
//======================================================================================//

//! \brief Rectangle 2D-array arrange
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class LayoutRect : public LayoutIndex
{
protected:
	int		m_nNativeWidth;
	int		m_nNativeHeight;
	const size_t m_nDimX;
	const size_t m_nDimY;
//	IAreaLayout::Rect m_Rect;
public:

	typedef	IAreaLayout::Rect Rect;

	LayoutRect(float x, float y, float w, float h,  size_t DimX, size_t DimY);
	virtual ~LayoutRect();
};

#endif // _LAYOUT_RECT_1459803152261875_