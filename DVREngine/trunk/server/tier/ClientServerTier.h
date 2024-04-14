//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: �������� ���-� �� ������� � ������� (������ � simple-������)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   24.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CLIENT_SERVER_TIER_3001154973708852_
#define _CLIENT_SERVER_TIER_3001154973708852_

#include "../AlarmInfo.h"

//======================================================================================//
//                                class IClientServerTier                                //
//======================================================================================//

//! \brief �������� ���-� �� ������� � ������� (������ � simple-������), ��� ���������� ���������� �����
//!			���������� ����������� ������, �������� � ����� ��������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class IClientServerTier
{
public:
	virtual ~IClientServerTier() {};

	//! ���������� ��� ������� �����
	virtual void	SetCurrentFrames( const Frames_t& ca )	= 0;
	//! �������� ����� � �������
	virtual void	GetCurrentFrames( Frames_t& ca )		= 0;
};

//! �������� ��������� ������ ������-������
//! \return ��������� �� ����������
IClientServerTier*	GetClientServerTier();

#endif // _CLIENT_SERVER_TIER_3001154973708852_