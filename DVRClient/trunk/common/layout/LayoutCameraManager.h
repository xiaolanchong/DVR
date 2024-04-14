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
#ifndef _LAYOUT_CAMERA_MANAGER_6583441964490322_
#define _LAYOUT_CAMERA_MANAGER_6583441964490322_

#include "LayoutManager.h"

//======================================================================================//
//                              class LayoutCameraManager                               //
//======================================================================================//

//! \brief Надстройка для присвоения идентификаторов окнам
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class LayoutCameraManager : protected LayoutManager
{
	//! доступные иентификаторы камер
	std::vector<int>	m_AvailableIds;
	//! расставленные идентификаторы камер, среди них могут быть и InvalidCameraIndex
	//! размер массива должен быть равен размер 
	std::vector<int>	m_CurrentIds;
public:
	LayoutCameraManager(const std::vector<int>& AvailableCameras);
	virtual ~LayoutCameraManager();

	//! неправильный иден-р камеры
	static	const int	InvalidCameraIndex = -1;
	//! неправильный иден-р окна вывода
	static	const int	InvalidWindowIndex = -1;

	//! установить список идентификаторов камер
	//! \param Ids список идентификаторов камер
	void	SetCameras( const std::vector<int>& Ids );

	void	GetCameras( std::vector<int>& Ids ); 

	//! получить выбранные камеры (назначенные в окна)
	//! \param Ids идентификаторы камер
	void	GetSelectedCameras( std::vector<int>& Ids ) const;

	//! список идентификаторов камер, текущий идентификатор, номер окна
	typedef boost::tuple<std::vector<int>, int, size_t >	WindowClick_t;

	//! обработать нажатие
	//! \param x х-координата мыши
	//! \param y у-координата мыши
	//! \param nWindowWidth  ширина окна
	//! \param nWindowHeight высота окна
	//! \return результат для вывода меню
	WindowClick_t	ProcessRClick( int x, int y, int nWindowWidth, int nWindowHeight );

	//! обработать выбор меню
	//! \param nWindowID	номер окна
	//! \param nCameraIndex иеднтификатор выбранной камеры или InvalidCameraIndex
	void	ProcessCameraMenu( size_t nWindowID, int nCameraIndex );

	//! установить размеры окна
	//! \param w ширина 
	//! \param h высота
	void	SetWindowSize(int w, int h);

	//! задействован шаблон? не имет смыла разрешать 4х4 массив, если камера одна
	//! \param i индек шаблона в массиве
	//! \return да/нет
	bool	IsLayoutEnabled(size_t i ) const;

	//! установить шаблон вывода окон 1, 2х2, 3х3, ...
	//! \param i порядковый номер в массиве
	void	SetLayoutPattern( int i );

	//! установить шаблон растяжения окон
	//! \param i индекс в массиве шаблонов
	void	SetScale( int i );

	//! получить расположение окон камер
	//! \param LayoutArr		массив пар идентификатор  камеры - окно вывода
	//! \param EmptyRects		массив незанятых под камеры окон вывода
	//! \param nWindowWidth		размеры окна  в пкс
	//! \param nWindowHeight	размеры окна в пкс
	void	GetLayout(	std::vector<std::pair<int, IAreaLayout::Rect> >& LayoutArr, 
						std::vector<IAreaLayout::Rect>&	EmptyRects,
						int nWindowWidth, int nWindowHeight );

	void	GetLayoutCellNumber(std::vector<size_t>& LayoutSizes)
	{
		LayoutManager::GetLayoutCellNumber( LayoutSizes );
	}

	void	SelectCameraInLayout( const std::vector<int>& CamArrID );

	void	GetLayoutSnapshot( std::vector<int>& Ids ) const;
};

#endif // _LAYOUT_CAMERA_MANAGER_6583441964490322_