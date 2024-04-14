//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка для видеоридера сервера

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   21.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_FAKE_READER_5811310306801475_
#define _SERVER_FAKE_READER_5811310306801475_

#include "../../../../shared/Common/ExceptionTemplate.h"
#include "../../../../shared/Common/integers.h"
#include "../../../../shared/Interfaces/i_video_reader.h"

//======================================================================================//
//                                class ServerFakeReader                                //
//======================================================================================//

//! \brief Заглушка для видеоридера сервера, поставляет одноцветные сгенерированные изображения
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerFakeReader : public Elvees::IBaseVideoReader
{
	//! заголовок 
	BITMAPINFOHEADER					m_BmpHdr;
	//! данные изображения
	std::vector<ubyte>					m_ByteBuffer;
	//! номер кадра
	ulong								m_nFrameNumber;
public:
	ServerFakeReader();
	virtual ~ServerFakeReader();

	virtual const BITMAPINFOHEADER &	GetHeader() const ;
	virtual bool						Read( const ubyte *& pBytes, ulong & time );

	virtual bool  SetUserDataToVideo( const void* pUserData, size_t UserDataSize )
	{
		UNREFERENCED_PARAMETER(pUserData);
		UNREFERENCED_PARAMETER(UserDataSize);
		return false;
	}
};

#endif // _SERVER_FAKE_READER_5811310306801475_