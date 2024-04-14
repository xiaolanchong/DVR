//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: x, y, w 7 h are wieghts of the window

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _WEIGHT_LAYOUT_7883014133381115_
#define _WEIGHT_LAYOUT_7883014133381115_

#include "IAreaLayout.h"
//======================================================================================//
//                                  class WeightLayout                                  //
//======================================================================================//

//! \brief x, y, w 7 h are wieghts of the window
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class WeightLayout : public IAreaLayout
{
	float	m_x, m_y, m_w, m_h;
public:
	WeightLayout(float x, float y, float w, float h);
	virtual ~WeightLayout();

	virtual void	Arrange( std::vector<IAreaLayout::Rect>& ArrangeArr ) ;
	virtual size_t	GetCellNumber() const { return 1;}
};

#endif // _WEIGHT_LAYOUT_7883014133381115_