//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Абстрактная фабрика создания интерфейсного класса потока

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_ARCHIVE_STREAM_FACTORY_8686913323641527_
#define _I_ARCHIVE_STREAM_FACTORY_8686913323641527_

#include "ArchiveDataExchange.h"
#include "../../ArchiveReader.h"
#include "../remote/archive/ArchiveClientConnection.h"

//======================================================================================//
//                             class IArchiveStreamFactory                              //
//======================================================================================//

//! \brief Абстрактная фабрика создания интерфейсного класса потока
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class IArchiveStreamFactory
{
public:
	IArchiveStreamFactory();
	virtual ~IArchiveStreamFactory();

	virtual CHCS::IStream*	CreateArchiveStream( 
		long lSourceID, 
		INT64 startPos, 
		INT64 endPos,
		unsigned long address,
		const std::tstring& sVideoBasePath
		) = 0;
};

IArchiveStreamFactory* GetArchiveStreamFactory();

#endif // _I_ARCHIVE_STREAM_FACTORY_8686913323641527_