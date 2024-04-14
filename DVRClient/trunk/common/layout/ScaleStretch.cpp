//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Stretch strategy for areas

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   25.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ScaleStretch.h"

//======================================================================================//
//                                  class ScaleStretch                                  //
//======================================================================================//

//! scale rectangle
struct MulValue
{
	float m_h; 
	float m_w;
	MulValue(float w, float h) : m_w(w), m_h(h){}
	IAreaLayout::Rect operator() ( IAreaLayout::Rect& e ) const 
	{
		e.x /= m_w;
		e.w /= m_w;

		e.y /= m_h;
		e.h /= m_h;
		return e;
	}
};

void	ScaleStretch::Modify(	int nWindowWidth, int nWindowHeight,
								int nUnitWidth, int nUnitHeight,
								std::vector<IAreaLayout::Rect>& Areas )
{
	std::pair<float, float> p = GetSize( Areas ) ;
	std::transform( Areas.begin(), Areas.end(), Areas.begin(), MulValue(p.first, p.second) );
}