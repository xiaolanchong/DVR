//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: �����-��������� ���� ������ �� ����� ��� ��������� MsgHallRequestAlarmData

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_DATA_8586489668211100_
#define _CAMERA_DATA_8586489668211100_

//======================================================================================//
//                                   class CameraData                                   //
//======================================================================================//

//! \brief �����-��������� ���� ������ ��/��� ����� ��� �������� � ����������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class CameraData
{
	//! ������� ������ - �������������, �������� ������
	std::map<int, ByteArray_t>	m_CameraData;
public:
	CameraData(  ) ;
	virtual ~CameraData();

	//! ���������� � ������ ������
	MACRO_EXCEPTION( CameraDataException, std::runtime_error); 
	//! �������� ������ ������
	//! \param nCameraID �����-� ������
	//! \param BinData ������ ������
	void					AddData( int nCameraID, const ByteArray_t& BinData );
	//! �������� ������ ������
	//! \param nCameraID ����-�
	//! \return ������ �� ������
	//! \exception CameraDataException
	const	ByteArray_t&	GetData( int nCameraID) const;
	//! �������� ������ ���� �����
	void					ClearData();

	//! �������� ������ �����-� �����, ��� ���. ���� ������
	//! \param CameraIDArr ������ �����-���
	void					GetCameraIDArr( std::vector<int>& CameraIDArr ) const;
};

#endif // _CAMERA_DATA_8586489668211100_