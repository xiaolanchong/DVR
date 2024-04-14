//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс-хранилище всех данных от камер для сообщения MsgHallRequestAlarmData

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_DATA_8586489668211100_
#define _CAMERA_DATA_8586489668211100_

//======================================================================================//
//                                   class CameraData                                   //
//======================================================================================//

//! \brief Класс-хранилище всех данных от/для камер при передачи в сообщениях
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class CameraData
{
	//! словарь данных - идентификатор, бинарные данные
	std::map<int, ByteArray_t>	m_CameraData;
public:
	CameraData(  ) ;
	virtual ~CameraData();

	//! исключение в случае ошибки
	MACRO_EXCEPTION( CameraDataException, std::runtime_error); 
	//! добавить данные камеры
	//! \param nCameraID идент-р камеры
	//! \param BinData данные камеры
	void					AddData( int nCameraID, const ByteArray_t& BinData );
	//! получить данные камеры
	//! \param nCameraID иден-р
	//! \return ссылку на данные
	//! \exception CameraDataException
	const	ByteArray_t&	GetData( int nCameraID) const;
	//! очистить данные всех камер
	void					ClearData();

	//! получить массив идент-р камер, для кот. есть данные
	//! \param CameraIDArr массив идент-ров
	void					GetCameraIDArr( std::vector<int>& CameraIDArr ) const;
};

#endif // _CAMERA_DATA_8586489668211100_