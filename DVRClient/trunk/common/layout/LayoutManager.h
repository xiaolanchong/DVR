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
#ifndef _LAYOUT_MANAGER_5607763664513351_
#define _LAYOUT_MANAGER_5607763664513351_

#include "IAreaLayout.h"
#include "IAreaScale.h"
//======================================================================================//
//                                 class LayoutManager                                  //
//======================================================================================//

//! \brief Camera areas layout manager
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class LayoutManager
{

//	int					m_nWidth;
//	int					m_nHeight;

	std::vector< boost::shared_ptr<IAreaLayout> > m_LayoutArr;

	boost::shared_ptr<IAreaLayout>	m_CurrentLayout;

	std::auto_ptr<IAreaScale>		m_pScale;

//	void	GetLayout( std::vector<IAreaLayout::Rect_t>& Areas, int ww, int wh );
//	void	SetRenderLayout(int w, int h);

	void	CreateLayouts();

protected:
	const IAreaLayout*	GetCurrentLayout() const { return m_CurrentLayout.get(); }
public:

	LayoutManager( );
	virtual ~LayoutManager();

	//! установить расположение
	//! \param i порядковый номер
	void	SetLayoutPattern(long i);

	//! установить режим масштабирования
	//! \param i порядковый номер
	void	SetScale(long i);

	void	GetLayout( std::vector<IAreaLayout::Rect>& LauoutArr, int nWindowWidth, int nWindowHeight ) ;

	void	GetLayoutCellNumber(std::vector<size_t>& LayoutSizes);
};

#endif // _LAYOUT_MANAGER_5607763664513351_