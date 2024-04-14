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
#ifndef _SCALE_STRETCH_6994877494672124_
#define _SCALE_STRETCH_6994877494672124_

#include "IAreaScale.h"
//======================================================================================//
//                                  class ScaleStretch                                  //
//======================================================================================//

//! \brief Stretch strategy for areas
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   25.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class ScaleStretch : public IAreaScale
{
public:
	virtual void	Modify( int nWindowWidth, int nWindowHeight,
							int nUnitWidth, int nUnitHeight,
							std::vector<IAreaLayout::Rect>& Areas );
};

#endif // _SCALE_STRETCH_6994877494672124_