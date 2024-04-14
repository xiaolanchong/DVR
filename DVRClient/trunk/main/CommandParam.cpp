//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Парсер командной строки (задание параметров)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   25.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "CommandParam.h"
#include <fstream>

//======================================================================================//
//                                  class CommandParam                                  //
//======================================================================================//

CommandParam::CommandParam()
{
}

CommandParam::~CommandParam()
{
}

bool	CommandParam::ProcessCommandLine( char** argv, int argc )
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help",	"produce help message"				)
		("noalgo",	"don't launch detection algorithm"	)
		;
	po::store(po::parse_command_line(argc, argv, desc), m_VariableMap);
	po::notify(m_VariableMap);  

	if (m_VariableMap.count("help")) 
	{
		std::ostringstream os;
		os << desc;
		wxMessageBox( os.str().c_str(), "Program options" );
		return false;
	}
	else
	{
		return true;
	}
}

bool	CommandParam::LaunchAlgorithm() const
{
	return !m_VariableMap.count("noalgo");
}