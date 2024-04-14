//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ������������� ��� ������� - ������� ��� CHCSVA

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_VIDEO_READER_1734811997251930_
#define _SERVER_VIDEO_READER_1734811997251930_

#include "IVideoProvider.h"

//! ������ ��� ��������������� ������ �������� ���������� � boost::shared_ptr
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

//! \brief ���������� ������������� ��� ������� - ������� ��� CHCSVA
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerVideoReader : public IVideoProvider
{
	//! ����������, �� ���. ����� �����
	boost::shared_ptr<CHCS::IStream>			m_pStream;
	//! ������� ����������� ����
	boost::shared_ptr<CHCS::IFrame>				m_pFrame;
	//! ��������� �����, ������ ������
	BITMAPINFOHEADER							m_BmpHdr;
public:
	MACRO_EXCEPTION( ServerVideoReaderException, Elvees::VideoReaderException )	;

	//! \param pMsg ���������� �����
	//! \param nCameraID �������-� ������, � ������� ������� �����
	ServerVideoReader(boost::shared_ptr<CHCS::IStreamManager> pManager, int nCameraID);
	virtual ~ServerVideoReader();

	virtual const BITMAPINFOHEADER &	GetHeader() const ;
	virtual bool						Read( const ubyte *& pBytes, ulong & time );

	virtual bool  SetUserDataToVideo( const void* pUserData, size_t UserDataSize );
};

#endif // _SERVER_VIDEO_READER_1734811997251930_