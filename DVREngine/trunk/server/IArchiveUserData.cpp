//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Сериализация рамок для архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "../interface/IArchiveUserData.h"

class RawBufferWriter
{
	std::vector<unsigned char>& m_RawData;
public:
	void Write( const void* pData, size_t DataSize )
	{
		m_RawData.insert( m_RawData.end(), 
						static_cast<const unsigned char*>(pData),  
						static_cast<const unsigned char*>(pData) + DataSize
						);
	}

	template<typename T> void	Write( T Value )
	{
		Write( &Value, sizeof( T ) );
	}

	RawBufferWriter( std::vector<unsigned char>& RawData ) : 
		m_RawData(RawData)
	{
	}
};

class RawBufferReaderError : public std::exception
{
public:
	const char* what() const 
	{
		return "RawBufferReader error";
	}
};

class RawBufferReader
{
	const std::vector<unsigned char>& m_RawData;
	size_t m_CurrentPos;
public:
	void Read( void* pData, size_t DataSize )
	{
		if( m_CurrentPos + DataSize > m_RawData.size() )
		{
			throw RawBufferReaderError();
		}
		memcpy( pData, &m_RawData[m_CurrentPos], DataSize );
		m_CurrentPos += DataSize;
		_ASSERTE( m_CurrentPos <= m_RawData.size() );
	}

	template<typename T> void	Read( T& Value )
	{
		Read( &Value, sizeof( T ) );
	}

	size_t	GetUnreadLength() const
	{
		return m_RawData.size() - m_CurrentPos;
	}

	void	Seek( size_t NewBufferPos )
	{
		_ASSERTE(NewBufferPos <= m_RawData.size() );
		if( NewBufferPos > m_RawData.size() )
		{
			throw RawBufferReaderError();
		}
		m_CurrentPos = NewBufferPos;
	}

	RawBufferReader( const std::vector<unsigned char>& RawData ) : 
		m_RawData(RawData), m_CurrentPos(0)
	{
	}
};

static const char c_Format[4] = { 'F', 'R', 'M', '1' };

//======================================================================================//
//                                class IArchiveUserData                                //
//======================================================================================//

class ArchiveUserDataImpl : public IArchiveUserData
{
	
	static const size_t		  c_nRectBoundNumber = 4;
	virtual bool	Serialize( 
		const std::vector<DBBridge::Frame::Rect>& AlarmFrames, 
		boost::int64_t FrameTimeStamp ,
		std::vector<unsigned char>& RawData
		)
	{
		
//		const size_t DataSize = sizeof( c_DataID ) + AlarmFrames.size() * sizeof(float) * c_nRectBoundNumber;
//		RawData.resize(DataSize);
//		std::raw_storage_iterator<float*, float> it( reinterpret_cast<float*>( &RawData[0] ) );
		RawBufferWriter OutBuffer( RawData );
		OutBuffer.Write( c_Format, sizeof(c_Format) );
		OutBuffer.Write( FrameTimeStamp );
		const size_t nMin =  std::min<size_t>( AlarmFrames.size(), 20 );
		for ( size_t i = 0; i < nMin; ++i )
		{
			OutBuffer.Write( AlarmFrames[i].x );
			OutBuffer.Write( AlarmFrames[i].y );
			OutBuffer.Write( AlarmFrames[i].w );
			OutBuffer.Write( AlarmFrames[i].h );
		}
		return true;
	}
	virtual bool	Unserialize( 
		const std::vector<unsigned char>& RawData,
		std::vector<DBBridge::Frame::Rect>& AlarmFrames, 
		boost::int64_t& FrameTimeStamp
		) 
	try
	{
#if 0
		const size_t nFrameNumber = RawData.size()/sizeof(float) * c_nRectBoundNumber;
		const float* it = reinterpret_cast<const float*>( &RawData[0] );

		for ( size_t i = 0; i < nFrameNumber;++i )
		{
			float x = *it; ++it;
			float y = *it; ++it;
			float w = *it; ++it;
			float h = *it; ++it;
			AlarmFrames.push_back( DBBridge::Frame::Rect( x, y, w, h ) );
		}
#else
		if( RawData.empty() )
		{
			return false;
		}

		AlarmFrames.clear();

		RawBufferReader InBuffer( RawData );
		char IdString[4];
		InBuffer.Read( IdString, 4 );
		if( memcmp( IdString, c_Format, 4 ) == 0 )
		{
			// new format
			InBuffer.Read( FrameTimeStamp );
		}
		else
		{
			// old format
			InBuffer.Seek(0);
		}

		const size_t nFrameNumberInBuffer = InBuffer.GetUnreadLength()/(sizeof(float) * c_nRectBoundNumber);

		for ( size_t i = 0; i < nFrameNumberInBuffer;++i )
		{
			float x, y, w, h;
			InBuffer.Read( x );
			InBuffer.Read( y );
			InBuffer.Read( w );
			InBuffer.Read( h );
			AlarmFrames.push_back( DBBridge::Frame::Rect( x, y, w, h ) );
		}
		_ASSERTE( InBuffer.GetUnreadLength() == 0 ); // whole information has been read?
#endif
		return true;
	}
	catch ( RawBufferReaderError& ) 
	{
		AlarmFrames.clear();
		return false;
	};
};

boost::shared_ptr<IArchiveUserData> CreateArchiveUserDataInterface()
{
	return boost::shared_ptr<IArchiveUserData>(new ArchiveUserDataImpl );
}