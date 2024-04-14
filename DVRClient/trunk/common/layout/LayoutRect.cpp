//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Rectangle 2D-array arrange

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "LayoutRect.h"
#include "WeightLayout.h"
//======================================================================================//
//                                   class LayoutRect                                   //
//======================================================================================//

LayoutRect::LayoutRect( float x, float y, float w, float h, size_t DimX, size_t DimY):
	m_nDimX(DimX),
	m_nDimY(DimY)
{
	_ASSERTE( m_nDimX && m_nDimY );
	float xs = x;
	float ys = y;
	float c_Width	= w/m_nDimX;
	float c_Height	= h/m_nDimY;
	for ( size_t i =0 ; i < m_nDimX; ++i, ys += c_Height, xs = x )
		for( size_t j =0; j < m_nDimY; ++j )
		{
			m_Areas.push_back( boost::shared_ptr<IAreaLayout>( 
				new WeightLayout(	xs , ys , c_Width, c_Height ) ));
			xs += c_Width;
		}
}

LayoutRect::~LayoutRect()
{
}