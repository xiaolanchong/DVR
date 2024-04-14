//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Strategy for areas - layout will stretch & keep the aspect ration of whole areas

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   25.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _SCALE_KEEP_ASPECT_RATIO_2562184149778500_
#define _SCALE_KEEP_ASPECT_RATIO_2562184149778500_

#include "IAreaScale.h"
//======================================================================================//
//                              class ScaleKeepAspectRatio                              //
//======================================================================================//

//! \brief Strategy for areas - layout will stretch & keep the aspect ration of whole areas
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   25.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class ScaleKeepAspectRatio : public IAreaScale
{
public:
	ScaleKeepAspectRatio();
	virtual ~ScaleKeepAspectRatio();

	virtual void	Modify( int nWindowWidth, int nWindowHeight,
							int nUnitWidth, int nUnitHeight,
							std::vector<IAreaLayout::Rect>& Areas );
};

#endif // _SCALE_KEEP_ASPECT_RATIO_2562184149778500_