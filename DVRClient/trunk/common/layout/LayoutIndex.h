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
#ifndef _LAYOUT_INDEX_8281187730217934_
#define _LAYOUT_INDEX_8281187730217934_

#include "IAreaLayout.h"
//======================================================================================//
//                                  class LayoutIndex                                   //
//======================================================================================//

//! \brief Indexed array of areas
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   25.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class LayoutIndex : public IAreaLayout
{
protected:
	typedef std::vector< boost::shared_ptr< IAreaLayout > >				Array2D_t;
	Array2D_t															m_Areas;
public:
	LayoutIndex();
	virtual ~LayoutIndex();

	virtual void Arrange( std::vector<IAreaLayout::Rect>& ArrangeArr);
	bool		 Insert( size_t nIndex, boost::shared_ptr< IAreaLayout > p ); 
	virtual size_t GetCellNumber() const;
};

#endif // _LAYOUT_INDEX_8281187730217934_