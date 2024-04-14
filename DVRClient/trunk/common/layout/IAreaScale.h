//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Interface for layout scaling

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _I_AREA_SCALE_7487554638181210_
#define _I_AREA_SCALE_7487554638181210_

#include "IAreaLayout.h"
//======================================================================================//
//                                  struct IAreaScale                                   //
//======================================================================================//

//! \brief Interface for layout scaling
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class IAreaScale
{
	//! lexicografical compare
	struct FindDistance
	{
		bool operator() ( const IAreaLayout::Rect& l, const IAreaLayout::Rect& r ) const 
		{
			return ( l.x < r.x ) ? true : (l.y < r.y);
		}
	};
protected:
	//! переход в оконную систему координат
	struct TransformRect
	{
		float m_OffsetX, m_OffsetY, m_ScaleX, m_ScaleY;
		IAreaLayout::Rect operator () ( const IAreaLayout::Rect r ) const
		{
			float x = m_OffsetX + r.x * m_ScaleX;
			float y = m_OffsetY + r.y * m_ScaleY;
			float w = r.w * m_ScaleX;
			float h = r.h * m_ScaleY;
			return IAreaLayout::Rect( x, y, w, h);
		}
		TransformRect(float x, float y, float w, float h):
		m_OffsetX(x), m_OffsetY(y),
			m_ScaleX(w), m_ScaleY(h)
		{}
	};

	//! прямоугольник - x, y верхнего угла, ширина, высота
	typedef std::vector<IAreaLayout::Rect> Arr_t;

	std::pair<Arr_t::iterator, Arr_t::iterator> GetCornetPoints( Arr_t& Areas)
	{  
		return	boost::minmax_element( Areas.begin(), Areas.end(), FindDistance() );
	}
	std::pair<float, float> GetSize( Arr_t& Areas )
	{
		std::pair<Arr_t::iterator, Arr_t::iterator> mm =  GetCornetPoints(Areas);
		float TotalWidth	= mm.second->x + mm.second->w - mm.first->x;
		float TotalHeight	= mm.second->y + mm.second->h - mm.first->y;
		return std::make_pair( TotalWidth, TotalHeight );
	}
public:
	virtual ~IAreaScale() {};

	virtual void	Modify( int nWindowWidth, int nWindowHeight,
							int nUnitWidth, int nUnitHeight,
							std::vector<IAreaLayout::Rect>& Areas ) = 0;
};

#endif // _I_AREA_SCALE_7487554638181210_