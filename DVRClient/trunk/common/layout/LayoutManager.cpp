//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Camera areas layout manager

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   24.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "LayoutManager.h"

#include "LayoutRect.h"
#include "WeightLayout.h"

#include "ScaleStretch.h"
#include "ScaleKeepAspectRatio.h"
#include "ScaleKeepSize.h"


using namespace boost::assign;

//======================================================================================//
//                                 class LayoutManager                                  //
//======================================================================================//

LayoutManager::LayoutManager(  )
{
	CreateLayouts();
}

LayoutManager::~LayoutManager()
{
}

void	LayoutManager::SetLayoutPattern(long i)
{
	_ASSERTE( m_LayoutArr.size() > i && i >= 0 );

	m_CurrentLayout = m_LayoutArr[i];
}

void	LayoutManager::CreateLayouts()
{
	// 0:
	m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( new WeightLayout( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
	// 1:
	m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( new LayoutRect( 0.0f, 0.0f, 2.0f, 2.0f, 2, 2 ) ) );
	// 2:
	m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( new LayoutRect( 0.0f, 0.0f, 3.0f, 3.0f, 3, 3 ) ) );
	// 3:
	{
		LayoutIndex* lt =  new LayoutRect( 0.0f, 0.0f, 2.0f, 2.0f, 2, 2 ) ;
		
		boost::shared_ptr<IAreaLayout> SmallerRect(new LayoutRect( 1.0f, 1.0f, 1.0f, 1.0f, 2, 2 ));
		lt->Insert( 3, SmallerRect );

		m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( lt ) );
	}
	// 4:
	{
		LayoutIndex* lt =  new LayoutRect( 0.0f, 0.0f, 2.0f, 2.0f, 2, 2 ) ;
		
		lt->Insert( 1,  
			boost::shared_ptr<IAreaLayout> (new LayoutRect( 1.0f, 0.0f, 1.0f, 1.0f, 2, 2 ))
			);
		lt->Insert( 2,  
			boost::shared_ptr<IAreaLayout> (new LayoutRect( 0.0f, 1.0f, 1.0f, 1.0f, 2, 2 ))
			);
		lt->Insert( 3,  
			boost::shared_ptr<IAreaLayout> (new LayoutRect( 1.0f, 1.0f, 1.0f, 1.0f, 2, 2 ))
			);
		m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( lt ) );

	}
	// 5:
	m_LayoutArr.push_back( boost::shared_ptr<IAreaLayout>( new LayoutRect( 0.0f, 0.0f, 4.0f, 4.0f, 4, 4 ) ) );

	m_CurrentLayout = m_LayoutArr[0];
}

void	LayoutManager::SetScale(long i)
{
	switch( i )
	{
	case 0:
	{
		m_pScale = std::auto_ptr<IAreaScale>( new ScaleKeepAspectRatio );
		break;
	}
	case 1:
	{
		m_pScale = std::auto_ptr<IAreaScale>( new ScaleStretch );
		break;
	}
	case 2:
	{
		m_pScale = std::auto_ptr<IAreaScale>( new ScaleKeepSize );
		break;
	}
	default: _ASSERTE(0);
	}

//	SetRenderLayout( m_nWidth, m_nHeight );
}

void	LayoutManager::GetLayout( std::vector<IAreaLayout::Rect>& Areas, int ww, int wh ) 
{
	_ASSERTE( m_pScale.get() );
	_ASSERTE( m_CurrentLayout.get() );
	m_CurrentLayout->Arrange( Areas );
	_ASSERTE( !Areas.empty() );
	
	//FIXME : camera image size!
	m_pScale->Modify( ww, wh, 320, 240, Areas );
	_ASSERTE( !Areas.empty() );
}

void	LayoutManager::GetLayoutCellNumber(std::vector<size_t>& LayoutSizes)
{
	for ( size_t i =0; i < m_LayoutArr.size(); ++i  )
	{
		LayoutSizes.push_back( m_LayoutArr[i]->GetCellNumber() );
	}
}


//////////////////////////////////////////////////////////////////////////