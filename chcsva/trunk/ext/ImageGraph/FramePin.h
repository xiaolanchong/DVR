//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейсы для передачи изображений по потоку обработки кадра

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_PIN_4087270674091026_
#define _FRAME_PIN_4087270674091026_


namespace CHCS
{
	struct IFrame;
}

class IFramePin
{
public:
	virtual ~IFramePin()
	{
	}
};
/*
class IInputRawFramePin : public IFramePin
{
public:
	virtual void PushImage( const void* ) = 0; 
};*/

//////////////////////////////////////////////////////////////////////////

class IInputFramePin : public IFramePin
{
public:
	virtual void PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize ) = 0; 
};

class IOutputFramePin : public IFramePin
{
	IInputFramePin* m_pNextInputPin;
public:
	IOutputFramePin( IInputFramePin * p = 0):
		m_pNextInputPin( p )
	{
	}

	virtual void ConnectPin( IInputFramePin* pNextInputPin )
	{
		m_pNextInputPin = pNextInputPin;
	}
protected:
	void SendImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize )
	{
		if( m_pNextInputPin )
		{
			m_pNextInputPin->PushImage( pFrame, bKeyFrame, MaxImageSize );
		}
	}
};

class IntermediateFilter :	public IInputFramePin,
							public IOutputFramePin
{
public:
	IntermediateFilter ( IInputFramePin * pDownStreamFilter = 0 ): 
		IOutputFramePin( pDownStreamFilter )
		{
		}

};

#endif // _FRAME_PIN_4087270674091026_