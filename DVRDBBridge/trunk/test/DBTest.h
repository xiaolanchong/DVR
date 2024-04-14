//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   08.12.2005
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_TEST_4143635070772551_
#define _D_B_TEST_4143635070772551_

#include "../interface/IVideoConfig.h"

//======================================================================================//
//                                     class DBTest                                     //
//======================================================================================//

//! \brief Not available
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   08.12.2005
//! \version 1.0
//! \bug 
//! \todo 

class DBTest
{
	std::auto_ptr<DBBridge::IVideoConfig>	m_pConfig;
	void						TestVideoCfg();
	void						TestStringParam();
	void						TestNumericParam();

	void						TestCamera();
public:
	DBTest();
	void			Test();
	virtual ~DBTest();
};

#endif // _D_B_TEST_4143635070772551_