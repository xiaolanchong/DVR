//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейсы чтения видеофайлов AVI с помощью VFW

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _V_F_W_FILE_READER_6984624028636304_
#define _V_F_W_FILE_READER_6984624028636304_

#include "../VideoFileInterface.h"

//======================================================================================//
//                                 class VFWFileReader                                  //
//======================================================================================//

//! \brief Интерфейсы чтения видеофайлов AVI с помощью VFW
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class VFWFileReader : public IVideoFileReader
{
	VideoFileSettings	m_Settings;

	CComPtr<IAVIStream>	m_pVideoStream;
	CComPtr<IAVIStream>	m_pDataStream;

	bool	GetDataStream( IAVIFile* pAVIFile );
public:
	VFWFileReader( LPCTSTR szFileName );
	virtual ~VFWFileReader();

	virtual unsigned int FindNearestKeyFrame( unsigned int nStartFrameNumber, bool bForwardSearch ) ;

	virtual const VideoFileSettings& GetFileInfo( ) ;

	virtual bool	ReadFrame(
		unsigned int nSourceFrameNumber,
		void* pVideoData, size_t& nVideoDataSize, 
		void* pUserData, size_t& nUserDataSize
		);
};

#endif // _V_F_W_FILE_READER_6984624028636304_