//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Загрузка/сохранение расположения камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   12.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _LAYOUT_KEEPER_1365495630254679_
#define _LAYOUT_KEEPER_1365495630254679_

//======================================================================================//
//                                  class LayoutKeeper                                  //
//======================================================================================//

//! \brief Загрузка/сохранение расположения камер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   12.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class LayoutKeeper
{
	std::string m_sSystemName;

	std::map<int, std::vector<int> >	m_LayoutMap;
public:
	LayoutKeeper( const char* szSystemName );
	virtual ~LayoutKeeper();

	void	SaveLayout( int Index, const std::vector<int>&	CameraIDArr );
	void	LoadLayout( int Index, std::vector<int>&		CameraIDArr );
};

#endif // _LAYOUT_KEEPER_1365495630254679_