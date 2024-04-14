//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация видеограббера для сервера - адаптер для CHCSVA

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_VIDEO_READER_1734811997251930_
#define _SERVER_VIDEO_READER_1734811997251930_

#include "IVideoProvider.h"

//! шаблон для автоматического вызова удаления интерфейса в boost::shared_ptr
template<class T> struct ReleaseInterface
{
	void operator () (  T* p) 
	{
		if(p) p->Release();
	}
};

namespace CHCS
{
struct	IStream;
struct	IFrame;
class	IStreamManager;
}

//======================================================================================//
//                               class ServerVideoReader                                //
//======================================================================================//

//! \brief Реализация видеограббера для сервера - адаптер для CHCSVA
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerVideoReader : public IVideoProvider
{
	//! Видеопоток, из кот. тянем кадры
	boost::shared_ptr<CHCS::IStream>			m_pStream;
	//! текущий захваченный кадр
	boost::shared_ptr<CHCS::IFrame>				m_pFrame;
	//! заголовок кадра, формат потока
	BITMAPINFOHEADER							m_BmpHdr;
public:
	MACRO_EXCEPTION( ServerVideoReaderException, Elvees::VideoReaderException )	;

	//! \param pMsg отладочный поток
	//! \param nCameraID идентиф-р камеры, с которой берется видео
	ServerVideoReader(boost::shared_ptr<CHCS::IStreamManager> pManager, int nCameraID);
	virtual ~ServerVideoReader();

	virtual const BITMAPINFOHEADER &	GetHeader() const ;
	virtual bool						Read( const ubyte *& pBytes, ulong & time );

	virtual bool  SetUserDataToVideo( const void* pUserData, size_t UserDataSize );
};

#endif // _SERVER_VIDEO_READER_1734811997251930_