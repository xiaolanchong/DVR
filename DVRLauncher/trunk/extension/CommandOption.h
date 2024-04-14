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
#ifndef _COMMAND_OPTION_2784285295610085_
#define _COMMAND_OPTION_2784285295610085_

#include <boost/program_options.hpp>

namespace po = boost::program_options;

//======================================================================================//
//                                 class CommandOption                                  //
//======================================================================================//

//! \brief command option parser
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   14.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class CommandOption
{
	po::variables_map m_VariableMap;
	std::string			m_Desc;
public:
	CommandOption(wchar_t** argv, int argc);

	bool		OptionExists(const char* Name) const;
	std::string OptionValue(const char* Name) const;
	bool		ShowHelpMessage() const
	{
		return OptionExists("help");
	}
	void		OutputDescription() const;
	virtual ~CommandOption();
};

#endif // _COMMAND_OPTION_2784285295610085_