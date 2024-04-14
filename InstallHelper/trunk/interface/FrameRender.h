//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс отображения сжатых кадров от IFrame в контекст

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   31.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_RENDER_8284416030787029_
#define _FRAME_RENDER_8284416030787029_

class CVidDecoder;

namespace CHCS
{
struct  IFrame;
}

//======================================================================================//
//                                  class FrameRender                                   //
//======================================================================================//

//! \brief Класс отображения сжатых кадров от IFrame в контекст
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   31.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class FrameRender
{
	//std::auto_ptr<CVidDecoder>	m_pDecoder;
	CVidDecoder	m_Decoder;
	BITMAPINFO	m_biInput;
	BITMAPINFO	m_biOutput;

	bool	InitDecoder(CHCS::IFrame* pFrame);
public:
	FrameRender();
	virtual ~FrameRender();

	bool	DrawFrame( HDC hDC, const RECT& rcDrawArea, CHCS::IFrame* pFrame ) ;
};

#endif // _FRAME_RENDER_8284416030787029_