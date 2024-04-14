//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� �������-� ������� � ��

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_B_SERVER_1975522636479377_
#define _I_D_B_SERVER_1975522636479377_

#include "../../../Shared/Common/ExceptionTemplate.h"
//#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "Frame.h"

namespace DBBridge
{

//======================================================================================//
//                                   class IDBServer                                    //
//======================================================================================//

//! \brief ��������� �������-� ������� � ��
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class IDBServer
{
public:
	MACRO_EXCEPTION(DBServerException, std::runtime_error);

	virtual ~IDBServer() {};

	//! �������� ������ ��������������� ��������� �����
	//! \param CamArr ������ ���������������
	//! \exception DBServerException
	virtual void	GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr ) = 0;

	//! �������� ��������� ��������
	//! \param frames ���-� � �������
	//! \exception DBServerException
	virtual void	AddFrames( const std::vector<Frame>& frames  ) = 0;

	//! ������� ���-� �� ��, ����������� �� ������� ������ BoundTime
	//! \param BoundTime  ��������� �����
	//! \exception DBServerException
	virtual void	DeleteInfoBeforeTime( time_t BoundTime ) = 0;
};

//! ������� ���������� � ����������� IDBServer
//! \return smart pointer � ����������� ����������
//! \exception DBServerException
boost::shared_ptr<IDBServer>	CreateDBServer(const char* szConnectionString); 

}

#endif // _I_D_B_SERVER_1975522636479377_