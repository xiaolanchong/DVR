//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Engine get/set parameters implementaion class

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ENGINE_SETTINGS_IMPL_8284839702624317_
#define _ENGINE_SETTINGS_IMPL_8284839702624317_

#include "../interface/EngineSettings.h"

using namespace DVREngine;
//======================================================================================//
//                               class EngineSettingsImpl                               //
//======================================================================================//

//! \brief Engine get/set parameters implementaion class
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class EngineSettingsImpl 
{
	void			SetValue( LPCTSTR szName, unsigned int Value);
	unsigned int	GetValue( LPCTSTR szName ); 

	void			SetValue( LPCTSTR szName, double Value);
	double			GetDoubleValue( LPCTSTR szName ); 

	bool m_bOnlyLocalConfig;
public:
	EngineSettingsImpl(bool bOnlyLocalConfig);
	virtual ~EngineSettingsImpl();

	void					GetArchiveStorageParameters( ArchiveStorageParam& NewParam ) ;
	unsigned int			GetAlgorithmSensitivity(  ) ;
	ArchiveWriteCondition	GetArchiveWriteCondition( ) ;

	void	SetArchiveStorageParameters( const ArchiveStorageParam& NewParam );
	void	SetAlgorithmSensitivity( unsigned int Sensitivity ) ;
	void	SetArchiveWriteCondition( ArchiveWriteCondition NewCondition ) ;
};

#endif // _ENGINE_SETTINGS_IMPL_8284839702624317_