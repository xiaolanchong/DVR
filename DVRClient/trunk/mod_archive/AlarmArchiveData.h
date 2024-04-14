//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������ �� ������� ������������ ������� ��� ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ALARM_RECT_DATA_1438854645977401_
#define _ALARM_RECT_DATA_1438854645977401_

//======================================================================================//
//                                 class AlarmRectData                                  //
//======================================================================================//

//! \brief ������ �� ������� ������������ ������� ��� ������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class AlarmRectData
{
	Frames_t	m_AllAlarms;
public:
	AlarmRectData( const Frames_t& AllAlarmData );

	AlarmRectData( );

	virtual ~AlarmRectData();

	//! ������ ������ ����� ��� ������ � �������� �����
	//! \param CameraID		������������� ������
	//! \param ArchiveTime	��������� ����� 
	//! \param AlarmRectForTimeAndCamera ������ ��������� ����� �� ��� �������
	void GetData( 
		int CameraID, 
		time_t  ArchiveTime, 
		Frames_t& AlarmRectForTimeAndCamera 
		) const;
};

#endif // _ALARM_RECT_DATA_1438854645977401_