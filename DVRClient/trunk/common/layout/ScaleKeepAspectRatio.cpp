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
#include "stdafx.h"
#include "ScaleKeepAspectRatio.h"

//======================================================================================//
//                              class ScaleKeepAspectRatio                              //
//======================================================================================//

ScaleKeepAspectRatio::ScaleKeepAspectRatio()
{
}

ScaleKeepAspectRatio::~ScaleKeepAspectRatio()
{
}

void	ScaleKeepAspectRatio::Modify(	int nWindowWidth, int nWindowHeight,
										int nUnitWidth, int nUnitHeight,
										std::vector<IAreaLayout::Rect>& Areas )
{
	std::pair<float, float> p = GetSize( Areas );

	float imgW = nUnitWidth * p.first;
	float imgH = nUnitHeight * p.second;

	float ScaleX = (float)nWindowWidth / imgW;
	float ScaleY = (float)nWindowHeight / imgH;

	ScaleX = ScaleY = /*std::*/min( ScaleX, ScaleY);

	float W = ScaleX * imgW;
	float H = ScaleY * imgH;

	float gapX = (nWindowWidth  - W) / 2;
	float gapY = (nWindowHeight - H) / 2;

	std::transform( Areas.begin(), Areas.end(), Areas.begin(), 
					TransformRect(	gapX/nWindowWidth , gapY/nWindowHeight,
									W/(nWindowWidth*p.first), H/(nWindowHeight * p.second)) );
}