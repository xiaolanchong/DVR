//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Надстройка для присвоения идентификаторов окнам

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.02.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "LayoutCameraManager.h"


//======================================================================================//
//                              class LayoutCameraManager                               //
//======================================================================================//

LayoutCameraManager::LayoutCameraManager( const std::vector<int>& AvailableCameras )
{
	//! здесь должен быть максмимум из кол-ва окно в шаблонах расстановки 
	size_t MaxCurrentSize = max( 16, AvailableCameras.size());
	m_CurrentIds.resize(MaxCurrentSize, InvalidCameraIndex);
	m_AvailableIds = AvailableCameras;
//	_ASSERTE( m_AvailableIds.size() <= m_CurrentIds.size() );

	std::copy( m_AvailableIds.begin(), m_AvailableIds.end(), m_CurrentIds.begin() ); 
}

LayoutCameraManager::~LayoutCameraManager()
{
}

void	LayoutCameraManager::SetLayoutPattern(int i)
{
	LayoutManager::SetLayoutPattern(i);

	size_t nCameraNum = GetCurrentLayout()->GetCellNumber();
	std::fill( m_CurrentIds.begin() + nCameraNum, m_CurrentIds.end(), InvalidCameraIndex );
}

void	LayoutCameraManager::SetScale(int i)
{
	LayoutManager::SetScale(i);
}

void	LayoutCameraManager::SetCameras( const std::vector<int>& Ids )
{
	m_AvailableIds = m_CurrentIds = Ids;
}

void	LayoutCameraManager::GetCameras( std::vector<int>& Ids )
{
	Ids = m_AvailableIds;
}

void	LayoutCameraManager::GetSelectedCameras( std::vector<int>& Ids ) const
{
	std::set<int> UniqueIds( m_CurrentIds.begin(), m_CurrentIds.end() );
	UniqueIds.erase( LayoutCameraManager::InvalidCameraIndex );
	Ids.assign( UniqueIds.begin(), UniqueIds.end() );
}

void	LayoutCameraManager::GetLayoutSnapshot( std::vector<int>& Ids ) const
{
	size_t n = GetCurrentLayout()->GetCellNumber();
	Ids.assign( m_CurrentIds.begin(), m_CurrentIds.begin() + n );
}

//! find the rect containing the point
struct FindInRect
{
	float m_x, m_y;
	bool operator ()(const IAreaLayout::Rect& r) const 
	{
		return	r.x < m_x && 
				r.y < m_y &&
				r.x + r.w > m_x &&
				r.y + r.h > m_y ;
	}

	FindInRect(float x, float y) : m_x(x), m_y(y)
	{
	}
};

LayoutCameraManager::WindowClick_t 
LayoutCameraManager::ProcessRClick( int x, int y, int nWindowWidth, int nWindowHeight )
{
	_ASSERTE(nWindowWidth);
	_ASSERTE(nWindowHeight);
#if 1
	std::vector< IAreaLayout:: Rect > ArrangeArr;
	LayoutManager :: GetLayout( ArrangeArr, nWindowWidth, nWindowHeight );
	float xf = float(x)/nWindowWidth;
	float yf = float(y)/nWindowHeight;

	//! находим прямоуголник, куда кликнули
	std::vector<IAreaLayout::Rect>::const_iterator it =
		std::find_if( ArrangeArr.begin(), ArrangeArr.end(), FindInRect(xf, yf) );
	size_t nDist = std::distance<std::vector<IAreaLayout::Rect>::const_iterator>( ArrangeArr.begin(), it );
	if( it == ArrangeArr.end() ||
		nDist > m_CurrentIds.size() )
	{
		_ASSERTE(false);
		return boost::make_tuple( std::vector<int>(), InvalidCameraIndex, InvalidWindowIndex);
	}
	else 
	{
		std::vector<int>::const_iterator itCur = 
			std::find(m_AvailableIds.begin(), m_AvailableIds.end(), m_CurrentIds[ nDist ]);
		if( itCur != m_AvailableIds.end() )
		{
			//			m_ChosenAreaIndex = nDist;
			return boost::make_tuple( m_AvailableIds, 
				std::distance<std::vector<int>::const_iterator>
				( m_AvailableIds.begin() , itCur ), nDist );
		}
		else
		{
			return boost::make_tuple( m_AvailableIds, InvalidCameraIndex, nDist );
		}
	}
#else
	return boost::make_tuple( m_AvailableIds, InvalidCameraIndex, 0 );
#endif
}

void	LayoutCameraManager::ProcessCameraMenu( size_t nWindowID, int nCameraIndex )
{
	_ASSERT( nWindowID < m_CurrentIds.size() );
	if ( nCameraIndex == InvalidCameraIndex )
	{
		m_CurrentIds[nWindowID] = InvalidCameraIndex;
	}
	else
		m_CurrentIds[nWindowID] = m_AvailableIds[nCameraIndex];
}

using std::min;

void	LayoutCameraManager::SelectCameraInLayout( const std::vector<int>& CamArrID )
{
	const size_t CopiedCamNumber = min( CamArrID.size(), m_CurrentIds.size() );

	std::copy( CamArrID.begin(), CamArrID.begin() + CopiedCamNumber, m_CurrentIds.begin() );
}

void LayoutCameraManager::GetLayout(std::vector<std::pair<int, IAreaLayout::Rect> >& LayoutArr, 
									std::vector<IAreaLayout::Rect>&	EmptyRects,
									int nWindowWidth, int nWindowHeight )
{
	LayoutArr.clear();
	EmptyRects.clear();
	_ASSERTE(nWindowWidth);
	_ASSERTE(nWindowHeight);
	std::vector< IAreaLayout::Rect > RcArr;
	LayoutManager::GetLayout( RcArr, nWindowWidth, nWindowHeight );
//	_ASSERTE( RcArr.size() <=  )
	size_t nMin = min (m_CurrentIds.size(), RcArr.size() );
	for ( size_t i =0; i < nMin; ++i )
	{
		int nID = m_CurrentIds.size() > i ? m_CurrentIds[i] : InvalidCameraIndex;
		if(nID != InvalidCameraIndex)
		{
			LayoutArr.push_back( std::make_pair( m_CurrentIds[i], RcArr[i] ) );
		}
		else
		{
			EmptyRects.push_back( RcArr[i] );
		}
	}
}