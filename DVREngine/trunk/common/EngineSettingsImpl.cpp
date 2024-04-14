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

#include "stdafx.h"
#include "EngineSettingsImpl.h"

boost::shared_ptr<DBBridge::IVideoConfig> GetConfigInt(bool bOnlyLocalConfig)
{
	std::vector<std::tstring > AllIPArr;
	DBBridge::GetAllIPAddresses( AllIPArr );
//	_ASSERTE( !AllIPArr.empty() ); 
	const TCHAR * szIPServerAddress = 0;
	if( !AllIPArr.empty() && !bOnlyLocalConfig )
	{
		szIPServerAddress = AllIPArr[0].c_str();
	}
/*	else
	{
		szIPServerAddress = "127.0.0.1";
	}*/
	return boost::shared_ptr<DBBridge::IVideoConfig> ( DBBridge::CreateVideoConfig( szIPServerAddress ) );
}

static const  std::pair<DVREngine::ArchiveWriteCondition, unsigned int> 
c_Values [] = 
{
	std::make_pair( DVREngine::Never,			0 ),
	std::make_pair( DVREngine::OnlyWhenMotion,	1 ),
	std::make_pair( DVREngine::Always,			2 )
};

unsigned		int Condition2Number( DVREngine::ArchiveWriteCondition Condition )
{
	for ( size_t i = 0; i < sizeof(c_Values)/sizeof(c_Values[0]); ++i )
	{
		if( c_Values[i].first == Condition )
		{
			return c_Values[i].second;
		}
	}
	throw DVREngine::SettingsException("Unknown 'ArchiveWriteCondition' value");
}

DVREngine::ArchiveWriteCondition Number2Condition( unsigned int Value )
{
	for ( size_t i = 0; i < sizeof(c_Values)/sizeof(c_Values[0]); ++i )
	{
		if( c_Values[i].second == Value )
		{
			return c_Values[i].first;
		}
	}
	throw DVREngine::SettingsException("Correspondent 'ArchiveWriteCondition' value not found");
}

//======================================================================================//
//                               class EngineSettingsImpl                               //
//======================================================================================//

EngineSettingsImpl::EngineSettingsImpl( bool bOnlyLocalConfig):
	m_bOnlyLocalConfig( bOnlyLocalConfig ) 
{
}

EngineSettingsImpl::~EngineSettingsImpl()
{

}


unsigned int			EngineSettingsImpl::GetValue( LPCTSTR szName )
{
	DWORD v;

	boost::shared_ptr<DBBridge::IVideoConfig> pConfig( GetConfigInt( m_bOnlyLocalConfig ) );
	pConfig->GetValue( "Settings", szName, v);
	return v;
}


void			EngineSettingsImpl::SetValue( LPCTSTR szName, unsigned int Value)
{
	boost::shared_ptr<DBBridge::IVideoConfig> pConfig( GetConfigInt( m_bOnlyLocalConfig ) );
	pConfig->SetValue( "Settings", szName, Value);
}

double	EngineSettingsImpl::GetDoubleValue( LPCTSTR szName )
try
{
	std::tstring sValue;

	boost::shared_ptr<DBBridge::IVideoConfig> pConfig( GetConfigInt( m_bOnlyLocalConfig ) );
	pConfig->GetValue( "Settings", szName, sValue );
	
	return boost::lexical_cast<double>( sValue.c_str() );
}
catch ( DBBridge::IVideoConfig::VideoConfigException & ) 
{
	throw DVREngine::SettingsException(std::string(szName) + " - no such value");
}
catch ( boost::bad_lexical_cast& ) 
{
	throw DVREngine::SettingsException(std::string(szName) + " - invalid parameter value");
};

void			EngineSettingsImpl::SetValue( LPCTSTR szName, double Value)
try
{
	std::tstring sValue = boost::lexical_cast<std::tstring>( Value );

	boost::shared_ptr<DBBridge::IVideoConfig> pConfig( GetConfigInt( m_bOnlyLocalConfig ) );
	pConfig->SetValue( "Settings", szName, sValue.c_str() );
}
catch ( DBBridge::IVideoConfig::VideoConfigException & ) 
{
	throw DVREngine::SettingsException(std::string(szName) + " - failed to set parameter value");
}
catch ( boost::bad_lexical_cast& ) 
{
	throw DVREngine::SettingsException(std::string(szName) + " - invalid parameter value");
};

//////////////////////////////////////////////////////////////////////////


void					EngineSettingsImpl::GetArchiveStorageParameters( ArchiveStorageParam& NewParam ) 
{
	NewParam.FreePercentageDiskSpace	= GetDoubleValue( "FreePercentageDiskSpace" );
	NewParam.StorageDayPeriod			= GetValue( "StorageDayPeriod" );
}

unsigned int			EngineSettingsImpl::GetAlgorithmSensitivity(  ) 
{
	return GetValue( "AlgorithmSensitivity" );
}

ArchiveWriteCondition	EngineSettingsImpl::GetArchiveWriteCondition( ) 
{
	return Number2Condition( GetValue( "ArchiveWriteCondition" ) );
}

void	EngineSettingsImpl::SetArchiveStorageParameters( const ArchiveStorageParam& NewParam )
{
	SetValue( "FreePercentageDiskSpace",	NewParam.FreePercentageDiskSpace );
	SetValue( "StorageDayPeriod",			NewParam.StorageDayPeriod );
}

void	EngineSettingsImpl::SetAlgorithmSensitivity( unsigned int Sensitivity ) 
{
	SetValue( "AlgorithmSensitivity", Sensitivity );
}

void	EngineSettingsImpl::SetArchiveWriteCondition( ArchiveWriteCondition NewCondition ) 
{
	SetValue( "ArchiveWriteCondition", Condition2Number( NewCondition ) );
}
