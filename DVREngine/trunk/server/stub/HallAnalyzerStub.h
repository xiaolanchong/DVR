//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка реализации IHallAnalyzer для отладки

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _HALL_ANALYZER_STUB_5596719391231646_
#define _HALL_ANALYZER_STUB_5596719391231646_

#include "../MessageImpl.h"

//======================================================================================//
//                                class HallAnalyzerStub                                //
//======================================================================================//

//! \brief Заглушка реализации IHallAnalyzer для отладки
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class HallAnalyzerStub : public Elvees::IHallAnalyzer
{
public:
	HallAnalyzerStub();
	virtual ~HallAnalyzerStub();

	virtual bool Process( ulong timeStamp ) ;
	virtual bool SetData( const Elvees::IDataType * pData ) ;
	virtual bool GetData( Elvees::IDataType * pData )  ;
};

#endif // _HALL_ANALYZER_STUB_5596719391231646_