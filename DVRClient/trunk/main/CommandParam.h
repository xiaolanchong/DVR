//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������ ��������� ������ (������� ����������)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   25.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _COMMAND_PARAM_2726943634859345_
#define _COMMAND_PARAM_2726943634859345_

namespace po = boost::program_options;

//======================================================================================//
//                                  class CommandParam                                  //
//======================================================================================//

//! \brief ������ ��������� ������ (������� ����������)
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   25.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class CommandParam
{
	po::variables_map m_VariableMap;
public:
	CommandParam(  );

	//! parse command line
	//! \param argv pointer to the command paramter array
	//! \param argc number of parameters in argv
	//! \return proceed aplication execution?
	bool	ProcessCommandLine( char** argv, int argc );

	//! ��������� �� �������� ��������
	//! \return ��/���
	bool	LaunchAlgorithm() const;

	virtual ~CommandParam();
};

#endif // _COMMAND_PARAM_2726943634859345_