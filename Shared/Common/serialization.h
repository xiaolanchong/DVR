///////////////////////////////////////////////////////////////////////////////////////////////////
// serialization.h
// ---------------------
// begin     : Aug 2004
// modified  : 15 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_SERIALIZATION_H__
#define __DEFINE_ELVEES_SERIALIZATION_H__

namespace Elvees
{

enum SerializeAction
{
  SERIALIZE_SIZE,    //!< do not actual serialization, just determine required size of storage
  SERIALIZE_LOAD,    //!< load data
  SERIALIZE_SAVE     //!< save data
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function serializes an simple object of any type into a memory buffer.
///
/// \param  p       pointer to the memory buffer.
/// \param  x       the datum to serialize.
/// \param  action  the type of operation to do (read, write or estimate size).
/// \return         modified position inside the buffer.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
inline ubyte * Serialize( ubyte * p, T & x, SerializeAction action )
{
  switch (action)
  {
    case SERIALIZE_SIZE: break;
    case SERIALIZE_LOAD: ASSERT( p != 0 );  memcpy( &x, p, sizeof(T) );  break;
    case SERIALIZE_SAVE: ASSERT( p != 0 );  memcpy( p, &x, sizeof(T) );  break;
    default            : return p;
  }
  p += sizeof(T);
  return p;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
///       
/// 
/// \param  p       pointer to the memory buffer.
/// \param  x       the container to serialize.
/// \param  action  the type of operation to do (read, write or estimate size).
/// \return         modified position inside the buffer.
/// ///////////////////////////////////////////////////////////////////////////////////////////////////
template< class value_type >
inline void SerializeVector( const std::vector< value_type > & v, BinaryData & data )
{
	if ( v.size() == 0 )
	{
		data.clear();
		return;
	}

	const size_t byteSize = sizeof( value_type ) * v.size();

	data.resize( byteSize );
	memcpy( (void *)&data[0], (void *)&v[0], byteSize );
}

//////////////////////////////////////////////////////////////////////////
// return true if ok
//
template< class value_type >
inline bool DeserializeVector( std::vector< value_type > & v, const BinaryData & data )
{
	if ( data.empty() )
	{
		v.clear();
		return true;
	}
	else
	{
		const size_t elementSize = sizeof( value_type );
		if (  data.size() % elementSize != 0 )
			return false;
		size_t nItems = data.size() / elementSize;
		v.resize( nItems );
		memcpy( (void *) &v[0], (void *)&data[0], data.size() );
	}
	return true;
}

/*
template< class value_type >
inline ubyte * SerializeVector( ubyte * p, std::vector< value_type > & x, SerializeAction action )
{
  uint size = (uint)(x.size());
  p = Serialize( p, size, action );
  ASSERT( (0 <= size) && (size < (1<<24)) );
  uint byteSize = (ubyte)(size * sizeof(value_type));

  switch (action)
  {
    case SERIALIZE_SIZE: break;
    case SERIALIZE_SAVE: ASSERT( p != 0 );
                         if (size > 0) memcpy( p, &(x[0]), byteSize );
                         break;
    default: return p;
  }
  p += byteSize;
  return p;
}
*//*
template< class value_type >
inline void DeserializeVector( const ubyte * p, std::vector< value_type > & x )
{
	uint size = 0;
	ubyte * pCopy = const_cast<ubyte *>(p);// !œŒ“ŒÃ œ≈–≈ƒ≈À¿“‹
	pCopy = Serialize( pCopy, size, SERIALIZE_LOAD );
	ASSERT( (0 <= size) && (size < (1<<24)) );
	uint byteSize = (ubyte)(size * sizeof(value_type));
	x.resize( size );
	if (size > 0) memcpy( &(x[0]), pCopy, byteSize );
}

*/

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function serializes a container into a memory buffer,
///        assuming the container stores objects that have their own serialization method.
/// 
/// \param  p       pointer to the memory buffer.
/// \param  x       the container to serialize.
/// \param  action  the type of operation to do (read, write or estimate size).
/// \return         modified position inside the buffer.
/// ///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CONTAINER >
inline ubyte * SerializeComplexContainer( ubyte * p, CONTAINER & x, SerializeAction action )
{
  uint size = (uint)(x.size());
  p = Serialize( p, size, action );
  ASSERT( (0 <= size) && (size < (1<<24)) );

  switch (action)
  {
    case SERIALIZE_SIZE: break;
    case SERIALIZE_LOAD: x.resize( size );  break;
    case SERIALIZE_SAVE: break;
    default            : return p;
  }

  for (typename CONTAINER::iterator it = x.begin(); it != x.end(); ++it)
  {
    p = (*it).serialize( p, action );
  }
  return p;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function serializes an image in a memory buffer.
/// 
/// \param  p       pointer to the memory buffer.
/// \param  x       the image to serialize.
/// \param  action  the type of operation to do (read, write or estimate size).
/// \return         modified position inside the buffer.
/// ///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CONTAINER >
inline ubyte * SerializeImage( ubyte * p, CONTAINER & x, SerializeAction action )
{
  typedef  typename CONTAINER::value_type  value_type;

  uint width = (uint)(x.width());
  uint height = (uint)(x.height());
  p = Serialize( p, width, action );
  p = Serialize( p, height, action );
  ASSERT( (0 <= width)  && (width  < (1<<15)) );
  ASSERT( (0 <= height) && (height < (1<<15)) );
  uint byteSize = width * height * sizeof(value_type);

  switch (action)
  {
    case SERIALIZE_SIZE: break;

    case SERIALIZE_LOAD: x.resize( width, height, value_type(), false );
                         ASSERT( p != 0 );
                         if (byteSize > 0) memcpy( x.begin(), p, byteSize );
                         break;

    case SERIALIZE_SAVE: ASSERT( p != 0 );
                         if (byteSize > 0) memcpy( p, x.begin(), byteSize );
                         break;
    default : return p;
  }
  p += byteSize;
  return p;
}

} // namespace Elvees

#endif // __DEFINE_ELVEES_SERIALIZATION_H__

