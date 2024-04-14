//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ��� ���������� ��������������� �����

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

//! \brief ���������� ��� ���������� ��������������� �����
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class LayoutCameraManager : protected LayoutManager
{
	//! ��������� ������������� �����
	std::vector<int>	m_AvailableIds;
	//! ������������� �������������� �����, ����� ��� ����� ���� � InvalidCameraIndex
	//! ������ ������� ������ ���� ����� ������ 
	std::vector<int>	m_CurrentIds;
public:
	LayoutCameraManager(const std::vector<int>& AvailableCameras);
	virtual ~LayoutCameraManager();

	//! ������������ ����-� ������
	static	const int	InvalidCameraIndex = -1;
	//! ������������ ����-� ���� ������
	static	const int	InvalidWindowIndex = -1;

	//! ���������� ������ ��������������� �����
	//! \param Ids ������ ��������������� �����
	void	SetCameras( const std::vector<int>& Ids );

	void	GetCameras( std::vector<int>& Ids ); 

	//! �������� ��������� ������ (����������� � ����)
	//! \param Ids �������������� �����
	void	GetSelectedCameras( std::vector<int>& Ids ) const;

	//! ������ ��������������� �����, ������� �������������, ����� ����
	typedef boost::tuple<std::vector<int>, int, size_t >	WindowClick_t;

	//! ���������� �������
	//! \param x �-���������� ����
	//! \param y �-���������� ����
	//! \param nWindowWidth  ������ ����
	//! \param nWindowHeight ������ ����
	//! \return ��������� ��� ������ ����
	WindowClick_t	ProcessRClick( int x, int y, int nWindowWidth, int nWindowHeight );

	//! ���������� ����� ����
	//! \param nWindowID	����� ����
	//! \param nCameraIndex ������������� ��������� ������ ��� InvalidCameraIndex
	void	ProcessCameraMenu( size_t nWindowID, int nCameraIndex );

	//! ���������� ������� ����
	//! \param w ������ 
	//! \param h ������
	void	SetWindowSize(int w, int h);

	//! ������������ ������? �� ���� ����� ��������� 4�4 ������, ���� ������ ����
	//! \param i ����� ������� � �������
	//! \return ��/���
	bool	IsLayoutEnabled(size_t i ) const;

	//! ���������� ������ ������ ���� 1, 2�2, 3�3, ...
	//! \param i ���������� ����� � �������
	void	SetLayoutPattern( int i );

	//! ���������� ������ ���������� ����
	//! \param i ������ � ������� ��������
	void	SetScale( int i );

	//! �������� ������������ ���� �����
	//! \param LayoutArr		������ ��� �������������  ������ - ���� ������
	//! \param EmptyRects		������ ��������� ��� ������ ���� ������
	//! \param nWindowWidth		������� ����  � ���
	//! \param nWindowHeight	������� ���� � ���
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