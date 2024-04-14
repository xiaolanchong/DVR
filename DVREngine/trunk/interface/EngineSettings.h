//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Натстройки работы сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   22.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_ENGINE_SETTINGS_READER_7507497807821007_
#define _I_ENGINE_SETTINGS_READER_7507497807821007_

namespace DVREngine
{

struct ArchiveStorageParam
{
	double  FreePercentageDiskSpace;
//	double  EndPercentageDiskSpace;
	unsigned int StorageDayPeriod ;
};

enum ArchiveWriteCondition
{
	Never,
	OnlyWhenMotion,
	Always
};

//======================================================================================//
//                             class IEngineSettingsReader                              //
//======================================================================================//

//! \brief Натстройки работы сервера
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.06.2006
//! \version 1.0
//! \bug 
//! \todo 

MACRO_EXCEPTION( SettingsException, std::runtime_error); 

class IEngineSettingsReader
{
public:
	virtual void					GetArchiveStorageParameters( ArchiveStorageParam& NewParam ) = 0;
	virtual unsigned int			GetAlgorithmSensitivity(  ) = 0;
	virtual ArchiveWriteCondition	GetArchiveWriteCondition( ) = 0;
};

class IEngineSettingsWriter
{
public:
	virtual void	SetArchiveStorageParameters( const ArchiveStorageParam& NewParam ) = 0;
	virtual void	SetAlgorithmSensitivity( unsigned int Sensitivity ) = 0;
	virtual void	SetArchiveWriteCondition( ArchiveWriteCondition NewCondition ) = 0;
};	

}

#endif // _I_ENGINE_SETTINGS_READER_7507497807821007_