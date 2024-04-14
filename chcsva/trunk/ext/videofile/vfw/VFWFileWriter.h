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
#ifndef _V_F_W_FILE_WRITER_2212842075833788_
#define _V_F_W_FILE_WRITER_2212842075833788_

#include "../VideoFileInterface.h"

//======================================================================================//
//                                 class VFWFileWriter                                  //
//======================================================================================//

//! \brief Интерфейсы чтения видеофайлов AVI с помощью VFW
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class VFWFileWriter : public IVideoFileWriter
{
	CComPtr<IAVIStream>	m_pVideoStream;
	CComPtr<IAVIStream>	m_pDataStream;

	bool	WriteVideoFrame(const void* pFrameData, size_t FrameDataSize, bool bKeyFrame, long lFrameNumber);
	bool	WriteDataFrame(  const void* pFrameData, size_t FrameDataSize, long nFrameNumber );
public:
	VFWFileWriter( LPCTSTR szFileName,  const VideoFileSettings& FileWriterSettings );
	virtual ~VFWFileWriter();

	virtual bool	WriteFrame(
						const void* pVideoData, size_t nVideoDataSize, 
						const void* pUserData, size_t nUserDataSize,
						bool bKeyFrame,
						unsigned int nFrameNumber
						) ;
};

#endif // _V_F_W_FILE_WRITER_2212842075833788_