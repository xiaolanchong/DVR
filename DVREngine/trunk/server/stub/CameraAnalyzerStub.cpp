//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ICameraAnalyzer stub implemetation

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   19.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraAnalyzerStub.h"
//======================================================================================//
//                               class CameraAnalyzerStub                               //
//======================================================================================//

CameraAnalyzerStub::CameraAnalyzerStub(boost::shared_ptr<Elvees::IMessage>  pMsg):
	m_Msg( pMsg )
{
}

CameraAnalyzerStub::~CameraAnalyzerStub()
{
}

bool CameraAnalyzerStub::Process(	const BITMAPINFOHEADER * pHeader,
										const ubyte            * pBytes,
										ulong              timeStamp )
{
	UNREFERENCED_PARAMETER(pHeader);
	UNREFERENCED_PARAMETER(pBytes);
	UNREFERENCED_PARAMETER(timeStamp);
	return true;
}

bool CameraAnalyzerStub::SetData( const Elvees::IDataType * pData )
{
	UNREFERENCED_PARAMETER(pData);
	return true;
}

bool CameraAnalyzerStub::GetData( Elvees::IDataType * pData ) const
{
	UNREFERENCED_PARAMETER(pData);
	return true;
}
/*
boost::shared_ptr<Elvees::ICameraAnalyzer>	CreateAnalyzer( Elvees::IMessage* pMsg  )
{
	return boost::shared_ptr<Elvees::ICameraAnalyzer>( new CameraAnalyzerStub(pMsg) );
}*/