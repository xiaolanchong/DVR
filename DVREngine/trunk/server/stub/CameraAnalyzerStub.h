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
#ifndef _CAMERA_ANALYZER_STUB_1211163448426010_
#define _CAMERA_ANALYZER_STUB_1211163448426010_

#include "../MessageImpl.h"

//======================================================================================//
//                               class CameraAnalyzerStub                               //
//======================================================================================//

//! \brief ICameraAnalyzer stub implemetation
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

class CameraAnalyzerStub : public Elvees::ICameraAnalyzer
{
	MessageImpl		m_Msg;
public:
	CameraAnalyzerStub(boost::shared_ptr<Elvees::IMessage> pMsg);
	virtual ~CameraAnalyzerStub();

	virtual bool Process(	const BITMAPINFOHEADER * pHeader,
							const ubyte            * pBytes,
							ulong					 timeStamp );

	virtual bool SetData( const Elvees::IDataType * pData ) ;
	virtual bool GetData( Elvees::IDataType * pData ) const ;

//	static boost::shared_ptr<Elvees::ICameraAnalyzer>	CreateAnalyzer( boost::shared_ptr<Elvees::IMessage>  pMsg  );
};

#endif // _CAMERA_ANALYZER_STUB_1211163448426010_