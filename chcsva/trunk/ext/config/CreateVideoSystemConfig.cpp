//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: CreateVideoSystemConfig implementation

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   18.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "xml/XmlVideoSystemConfig.h"

using namespace CHCS;

namespace CHCS
{

CHCSVA_API IVideoSystemConfigEx* __stdcall	CreateVideoSystemConfig(LPCWSTR szConfigFileName)
{
	std::tstring sFileName(_T("chcsva_config.xml"));
	if( szConfigFileName && lstrlenW( szConfigFileName ) )
	{
		USES_CONVERSION;
		sFileName = CW2CT( szConfigFileName ); 
	}
	return new XmlVideoSystemConfig( sFileName.c_str() );
}

}