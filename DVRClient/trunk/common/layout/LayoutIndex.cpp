//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Indexed array of areas

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   25.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "LayoutIndex.h"
//======================================================================================//
//                                  class LayoutIndex                                   //
//======================================================================================//

LayoutIndex::LayoutIndex()
{
}

LayoutIndex::~LayoutIndex()
{
}

void LayoutIndex::Arrange( std::vector<Rect>& ArrangeArr )
{
	_ASSERTE( m_Areas.size() );
	for ( size_t i =0 ; i < m_Areas.size(); ++i )
		if( m_Areas[i].get() ) m_Areas[i]->Arrange( ArrangeArr );
}

bool	LayoutIndex::Insert( size_t nIndex, boost::shared_ptr< IAreaLayout > p )
{
	_ASSERTE( m_Areas.size() > nIndex );
	if( m_Areas.size() <= nIndex ) return false;
	m_Areas[nIndex] = p;
	return true;
}

struct Func
{
	
};

size_t LayoutIndex::GetCellNumber() const
{

	size_t s = 0;
	for ( size_t i =0; i < m_Areas.size(); ++i )
	{
		s += m_Areas[i]->GetCellNumber();
	}
	return s;
//	std::accumulate( m_Areas.begin(), m_Areas.end(), 0, 
	/*	boost::bind( std::plus<size_t>(), _1, 0, boost::mem_fn( IAreaLayout::GetCellNumber() ) */
}