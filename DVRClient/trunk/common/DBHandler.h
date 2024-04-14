//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����� �������������� � �� - ��������� ������ �����, ������ �� ������ �.�.

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   31.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_HANDLER_3252107636112560_
#define _D_B_HANDLER_3252107636112560_

//======================================================================================//
//                                   class DBHandler                                    //
//======================================================================================//

//! \brief ����� �������������� � �� - ��������� ������ �����, ������ �� ������ �.�.
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   31.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBHandler
{
public:
	DBHandler();
	virtual ~DBHandler();

	//! �������� ������ �����
	//! \param CameraArrID 
	void	GetCameraArr( std::vector<int>& CameraArrID );

	//! �������� ������ �� ������ �� ��
	//! \param StartTime ������ ����������, �� ���. �������������
	//! \param EndTime ����� ����������
	//! \param AlarmDataArr �������� ������
	void	GetAlarmData( time_t StartTime, time_t EndTime, Frames_t& AlarmDataArr );
};

#endif // _D_B_HANDLER_3252107636112560_