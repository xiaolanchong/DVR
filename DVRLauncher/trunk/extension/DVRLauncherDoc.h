//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс документа для запуска DVREngine

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _C_D_V_R_LAUNCHER_DOC_5647639935505489_
#define _C_D_V_R_LAUNCHER_DOC_5647639935505489_

#include "../LauncherDoc.h"
#include "CommandOption.h"

//======================================================================================//
//                                class CDVRLauncherDoc                                 //
//======================================================================================//

//! \brief Класс документа для запуска DVREngine
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class CDVRLauncherDoc : private CLauncherDoc
{
	boost::shared_ptr<DVREngine::IServer>	m_pServer;
	std::auto_ptr<CommandOption>			m_Option;

	void	InitCommandOption();
	bool	IsCommandParameterExists( const char* Name) const;
public:
	CDVRLauncherDoc();
	DECLARE_DYNCREATE(CDVRLauncherDoc)

	virtual bool CreateEnvironment(CCreateContext& cc);
	virtual void DestroyEnvironment();

	virtual ~CDVRLauncherDoc();
};

#endif // _C_D_V_R_LAUNCHER_DOC_5647639935505489_