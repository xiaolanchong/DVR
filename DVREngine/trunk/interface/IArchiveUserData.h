//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Сериализация рамок для архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_ACHIVE_USER_DATA_1208085524760441_
#define _I_ACHIVE_USER_DATA_1208085524760441_

//======================================================================================//
//                                class IArchiveUserData                                 //
//======================================================================================//

//! \brief Сериализация рамок для архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class IArchiveUserData
{
public:
	virtual ~IArchiveUserData() {};

	virtual bool	Serialize( 
						const std::vector<DBBridge::Frame::Rect>& AlarmFrames, 
						boost::int64_t FrameTimeStamp ,
						std::vector<unsigned char>& RawData
						) = 0;
	virtual bool	Unserialize( 
						const std::vector<unsigned char>& RawData,
						std::vector<DBBridge::Frame::Rect>& AlarmFrames, 
						boost::int64_t& FrameTimeStamp 
						) = 0;
};

boost::shared_ptr<IArchiveUserData> CreateArchiveUserDataInterface();

#endif // _I_ACHIVE_USER_DATA_1208085524760441_