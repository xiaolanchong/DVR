//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Interface for area layout

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _I_AREA_LAYOUT_7348570252431155_
#define _I_AREA_LAYOUT_7348570252431155_

//======================================================================================//
//                                  struct IAreaLayout                                  //
//======================================================================================//

//! \brief Interface for area layout
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.11.2005
//! \version 1.0
//! \bug 
//! \todo 

struct IAreaLayout
{
	struct Rect
	{
		float x, y, w, h;
		Rect(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h){}
	};

	virtual void	Arrange( std::vector<Rect>& ArrangeArr )	= 0; 
	virtual size_t	GetCellNumber() const						= 0;
	virtual ~IAreaLayout() {}
};

#endif // _I_AREA_LAYOUT_7348570252431155_