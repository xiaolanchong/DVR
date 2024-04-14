//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: command option parser

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.09.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "CommandOption.h"

//======================================================================================//
//                                 class CommandOption                                  //
//======================================================================================//

/*
bool bNoAlgo		= IsCommandParameterExists( L"--noalgo" );
bool bNoBackup		= IsCommandParameterExists( L"--nobackup" );
bool bNoArchive		= IsCommandParameterExists( L"--noarchive" );
bool bNoNetArchive	= IsCommandParameterExists( L"--nonetarchive" );
bool bNoNetServer	= IsCommandParameterExists( L"--nonetserver" );
bool bUseGUI		= IsCommandParameterExists( L"--gui" );
bool bDebug			= IsCommandParameterExists( L"--debug" );
*/
CommandOption::CommandOption(wchar_t** argv, int argc)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help",		"produce help message"				)
		("noalgo",		"don't launch detection algorithm"	)
		("nobackup",	"don't launch archive backup"		)
		("noarchive",	"don't write archive"				)
		("nonetarchive","don't send network archive"		)
		("nonetserver",	"don't send network video"			)
		("gui",			"show video window"					)
		("debug",		"output debug messages"				)
		("keyname", po::value<std::string>(), "set protection device name")
		;
	po::store(po::parse_command_line(argc, argv, desc), m_VariableMap);
	po::notify(m_VariableMap);  

	std::ostringstream os;
	os << desc;
	m_Desc = os.str();
}

CommandOption::~CommandOption()
{
}

bool		CommandOption::OptionExists(const char* Name) const
{
	return m_VariableMap.count(Name) != 0;
}

std::string CommandOption::OptionValue(const char* Name) const
{
	if( !m_VariableMap.count(Name) )
	{
		return std::string();
	}
	else
	{
		USES_CONVERSION;
//		std::wstring s = m_VariableMap[Name].as<std::wstring>() ;
//		return std::string( W2CA( s.c_str() ) );
		return m_VariableMap[Name].as<std::string>();
	}
}

void	CommandOption::OutputDescription() const
{
	MessageBoxA( 0, m_Desc.c_str(), "Program options", MB_OK|MB_ICONASTERISK );
}