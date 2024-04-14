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
#include "stdafx.h"
#include "WeightLayout.h"
//======================================================================================//
//                                  class WeightLayout                                  //
//======================================================================================//

WeightLayout::WeightLayout(float x, float y, float w, float h):
	m_x(x), m_y(y), m_w(w), m_h(h)
{
}

WeightLayout::~WeightLayout()
{
}

void WeightLayout::Arrange( std::vector<IAreaLayout::Rect>& ArrangeArr )
{
	ArrangeArr.push_back( IAreaLayout::Rect( m_x, m_y, m_w, m_h ) );
}