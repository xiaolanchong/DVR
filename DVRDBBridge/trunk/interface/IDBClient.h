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
#ifndef _I_D_B_CLIENT_7073047081017222_
#define _I_D_B_CLIENT_7073047081017222_

#include "../../../Shared/Common/ExceptionTemplate.h"
#include "Frame.h"
//#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace DBBridge
{

//======================================================================================//
//                                   class IDBClient                                    //
//======================================================================================//

//! \brief ��������� �������-� ������� � ��
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class IDBClient
{
public:
	MACRO_EXCEPTION( DBClientException, std::runtime_error );

	//! �������� ������ ���� ����� �������
	//! \param CameraIDArr ������ ��������������� �����
	//! \exception DBClientException
	virtual void GetCameraArr(std::vector< std::pair<int, std::string> >& CameraIDArr) = 0;

	virtual void SetCameraDescription( int CameraID, const std::string& sNewCameraDesc ) = 0;

	//! �������� ������ ���� ����� �� ������
	//! \param StartTime	������ �������
	//! \param EndTime		����� �������
	//! \param FrameArr		������ �����
	//! \exception			DBClientException
	virtual void GetFrames( time_t StartTime, time_t EndTime, std::vector<Frame>& FrameArr ) = 0;

	virtual ~IDBClient() {};
};

//! ������� ���������� � ����������� IDBClient
//! \return ��������� �� ����������
//! \exception IDBClient::DBClientException
boost::shared_ptr<IDBClient>	CreateDBClient(const char* szConnection);

}

#endif // _I_D_B_CLIENT_7073047081017222_