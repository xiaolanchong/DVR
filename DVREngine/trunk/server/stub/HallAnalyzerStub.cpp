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

#include "stdafx.h"
#include "HallAnalyzerStub.h"

//======================================================================================//
//                                class HallAnalyzerStub                                //
//======================================================================================//

HallAnalyzerStub::HallAnalyzerStub()
{
}

HallAnalyzerStub::~HallAnalyzerStub()
{
}

bool HallAnalyzerStub::Process( ulong timeStamp )
{
	UNREFERENCED_PARAMETER(timeStamp);
	return true;
}

bool HallAnalyzerStub::SetData( const Elvees::IDataType * pData )
{
	UNREFERENCED_PARAMETER(pData);
	return true;
}

bool HallAnalyzerStub::GetData( Elvees::IDataType * pData ) 
{
	UNREFERENCED_PARAMETER(pData);
	return true;
}