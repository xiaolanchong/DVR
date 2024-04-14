//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Поставка видеокадров + занесение информации в видео

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_VIDEO_PROVIDER_1148372305442738_
#define _I_VIDEO_PROVIDER_1148372305442738_

//======================================================================================//
//                                 class IVideoProvider                                 //
//======================================================================================//

//! \brief Поставка видеокадров + занесение информации в видео
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class IVideoProvider : public Elvees::IBaseVideoReader
{
public:
	virtual bool  SetUserDataToVideo( const void* pUserData, size_t UserDataSize ) = 0;
};

#endif // _I_VIDEO_PROVIDER_1148372305442738_