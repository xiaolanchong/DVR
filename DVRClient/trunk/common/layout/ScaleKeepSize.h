//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/*! Keep sizes strategy for areas

    author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    date   26.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _SCALE_KEEP_SIZE_2040441621245830_
#define _SCALE_KEEP_SIZE_2040441621245830_

#include "IAreaScale.h"
//======================================================================================//
//                                 class ScaleKeepSize                                  //
//======================================================================================//

class ScaleKeepSize : public IAreaScale
{
public:
	ScaleKeepSize();
	virtual ~ScaleKeepSize();

	virtual void	Modify( int nWindowWidth, int nWindowHeight,
							int nUnitWidth, int nUnitHeight,
							std::vector<IAreaLayout::Rect>& Areas );
};

#endif // _SCALE_KEEP_SIZE_2040441621245830_