//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/*! \brief Keep sizes strategy for areas

    \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    \date   26.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ScaleKeepSize.h"
//======================================================================================//
//                                 class ScaleKeepSize                                  //
//======================================================================================//

ScaleKeepSize::ScaleKeepSize()
{
}

ScaleKeepSize::~ScaleKeepSize()
{
}

void	ScaleKeepSize::Modify(	int nWindowWidth, int nWindowHeight,
								int nUnitWidth, int nUnitHeight,
								std::vector<IAreaLayout::Rect>& Areas )
{
	std::pair<float, float> p = GetSize( Areas );

	float imgW = nUnitWidth /** p.first*/;
	float imgH = nUnitHeight /** p.second*/;

	float ScaleX = imgW / nWindowWidth;
	float ScaleY = imgH / nWindowHeight;

	// center of the window is (0.5, 0.5)
	std::transform( Areas.begin(), Areas.end(), Areas.begin(), 
					TransformRect(	0.5f - ScaleX * p.first/2, 0.5f - ScaleY * p.second/2 , 
									ScaleX, ScaleY  ) );
}