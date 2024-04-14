///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_base2d.h
// ---------------------
// begin     : 1998
// modified  : 29 Sep 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ALIB_BASE_2D_H__
#define __ALIB_BASE_2D_H__

///================================================================================================
/// \struct Point2D.
/// \brief  Two-dimensional point.
///
/// 'TYPE' - coordinate type, 'PRECISE' - superior type used to enhance numerical precision.
///================================================================================================
template< class TYPE, class PRECISE >
struct Point2D
{
  typedef  TYPE     value_type;
  typedef  PRECISE  precise_type;

  value_type x;    //!< abscissa
  value_type y;    //!< ordinate
};

typedef  Point2D< boost::int16_t, boost::int32_t >  Pnt2s;
typedef  Point2D< boost::int32_t, boost::int32_t >  Pnt2i;
typedef  Point2D< float, double >                   Pnt2f;
typedef  Point2D< double, double >                  Pnt2d;
                                  

///================================================================================================
/// \struct Rectangle.
/// \brief  Rectangle.
///
/// <p><pre><tt>
///         -----------* (x2,y2)
///         |          |
///         |          |
///         |          |
///         |          |
/// (x1,y1) *-----------
/// </tt></pre></p>
///================================================================================================
struct ARectBase
{
  typedef  boost::int32_t  coord_type;

  coord_type  x1, y1, x2, y2;   //!< coordinates of rectangle's points
};


///////////////////////////////////////////////////////////////////////////////////////////////////
#define __ARR2D_IS_INSIDE__(x,y) ((0<=(x))&&((x)<(diff_type)m_W)&&(0<=(y))&&((y)<(diff_type)m_H))
///////////////////////////////////////////////////////////////////////////////////////////////////

///================================================================================================
/// \class Arr2D.
/// \brief Two-dimensional array of elements of any type.
///================================================================================================
template< class TYPE, class PRECISE, bool bMALLOC >
class Arr2D
{
public:
  typedef  TYPE                         value_type;      // type of entry
  typedef  PRECISE                      precise_type;    // type of intermediate result
  typedef  Arr2D<TYPE,PRECISE,bMALLOC>  this_type;       // type of this class
  typedef  TYPE &                       reference;
  typedef  const TYPE &                 const_reference;
  typedef  TYPE *                       iterator;
  typedef  const TYPE *                 const_iterator;
  typedef  TYPE *                       pointer;
  typedef  const TYPE *                 const_pointer;
  typedef  boost::uint32_t              size_type;
  typedef  boost::int32_t               diff_type;

protected:
  value_type * m_head;      //!< pointer to the first entry of 2D-array
  value_type * m_tail;      //!< pointer that points beyond the last entry (STL convention)
  size_type    m_W;         //!< width
  size_type    m_H;         //!< height
  size_type    m_size;      //!< size = width*height
  size_type    m_capacity;  //!< the maximal size allowed without reallocation
  size_type    m_bExternal; //!< if nonzero, then uses external memory buffer, otherwise allocates from the heap
  size_type    m_reserved;  //!< added for 64-bit alignment

public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane 'begin' iterator, i.e. object is considered as 1D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator begin()
{
  return m_head;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane constant 'begin' iterator, i.e. object is considered as 1D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator begin() const
{
  return m_head;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane 'end' iterator, i.e. object is considered as 1D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator end()
{
  return m_tail;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane constant 'end' iterator, i.e. object is considered as 1D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator end() const
{
  return m_tail;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero for an empty object.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool empty() const
{
  return (m_size == 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero if horizontal and vertical dimensions are equal.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool square() const
{
  return (m_W == m_H);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns horizontal dimension.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type width() const
{
  return ((diff_type)m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns vertical dimension.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type height() const
{
  return ((diff_type)m_H);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the total number of entries of this 2D-array (width * height).
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type size() const
{ 
  return ((diff_type)m_size);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns increment of a pointer that scans this array in horizontal direction.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type row_increment() const
{
  return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns increment of a pointer that scans this array in vertical direction.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type column_increment() const
{
  return ((diff_type)m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns bounding rectangle of this 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
ARectBase rect() const
{
  ARectBase rect;
  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = m_W;
  rect.y2 = m_H;
  return rect;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks whether specified point lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool inside( diff_type x, diff_type y ) const
{
  return __ARR2D_IS_INSIDE__( x, y );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks whether specified point lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool inside( const Pnt2i & p ) const
{ 
  return __ARR2D_IS_INSIDE__( p.x, p.y );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function clears this object without (!) deallocation (STL convention).
///////////////////////////////////////////////////////////////////////////////////////////////////
void clear()
{ 
  m_tail = m_head;
  m_W = 0;
  m_H = 0;
  m_size = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function clears this object and releases allocated memory.
///////////////////////////////////////////////////////////////////////////////////////////////////
void release()
{ 
  if (!m_bExternal)
  {
    bool bUseMalloc = bMALLOC;
    if (bUseMalloc)
      free( m_head );
    else
      delete [] m_head;
  }

  m_head = 0;
  m_tail = 0;
  m_W = 0;
  m_H = 0;
  m_size = 0;
  m_capacity = 0;
  m_bExternal = 0;
  m_reserved = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function assigns the same new value to every entry of this array.
///
/// \param  vfill  fill value.
///////////////////////////////////////////////////////////////////////////////////////////////////
void fill( value_type vfill )
{
  if (!empty())
  {
    for (size_type i = 0; i < m_size; i++)
      m_head[i] = vfill;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function resizes this object.
///
/// \param  W      new width.
/// \param  H      new height.
/// \param  vfill  fill value.
/// \param  bInit  if nonzero, then the object will be filled up with specified value.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool resize( size_type W, size_type H, value_type vfill = value_type(), bool bInit = true )
{
  if ((m_W != W) || (m_H != H))
  {
    if ((W > 0) && (H > 0))
    {
      if (m_capacity < (W*H))
      {
        bool bUseMalloc = bMALLOC;
        release();
        m_W = W;
        m_H = H;
        m_size = W*H;
        m_capacity = m_size;
        m_head = (bUseMalloc ? reinterpret_cast<value_type*>( malloc( m_size*sizeof(value_type) ) )
                             : (new value_type [m_size]));
        m_tail = m_head + m_size;
        if (m_head == 0)
        {
          release();
          return false;
        }
      }
      else
      {
        clear();
        m_W = W;
        m_H = H;
        m_size = W*H;
        m_tail = m_head + m_size;
      }
    }
    else release();
  }

  if (bInit)
    fill( vfill );

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function resizes this object according to dimensions of specified one.
///
/// \param  q      any object that has methods 'width()' and 'height()'.
/// \param  bInit  if nonzero, then the object will be filled up by default value.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class Q >
bool resize2( Q & q, bool bInit = true )
{
  return (resize( q.width(), q.height(), value_type(), bInit ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function wraps two-dimensional array around an external memory buffer,
///        which will NOT be automatically deallocated by this object.
///
///  Note, that contructors/destructors will NOT be called for array's entries. Furthermore,
///  external memory buffer must exist until the method 'release()' of this object is called.
///
/// \param  width   width of 2D-array.
/// \param  height  height of 2D-array.
/// \param  first   pointer to the beginning of external buffer.
/// \param  last    pointer to the end of external buffer (points just beyond the last entry).
/// \param  vfill   fill value.
/// \param  bInit   if nonzero, then the object will be filled up by specified value.
/// \return         Ok = pointer to the rest free space of external buffer, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
void * wrap( size_type width, size_type height, void * first, void * last,
             value_type vfill = value_type(), bool bInit = true )
{
  ASSERT( (first != 0) && (last != 0) && (first <= last) && (width > 0) && (height > 0) );
  release();
  pointer F = reinterpret_cast<pointer>( first );
  pointer L = reinterpret_cast<pointer>( last );
  if ((int)(std::distance( F, L )) >= (int)(width*height))
  {
    m_size = (m_capacity = (m_W = width)*(m_H = height));
    m_head = F;
    m_tail = m_head + m_size;
    m_bExternal = 1;

    if (bInit)
      fill( vfill );

    return reinterpret_cast<void*>( m_tail );
  }
  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function copies the source object into this one.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & operator=( const this_type & x )
{
  if (&x != this)
  {
    resize( x.width(), x.height(), value_type(), false );
    std::copy( x.begin(), x.end(), begin() );
  }
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function swaps contents between this object and specified one.
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap( this_type & x )
{
  std::swap( m_head,      x.m_head      );
  std::swap( m_tail,      x.m_tail      );
  std::swap( m_W,         x.m_W         );
  std::swap( m_H,         x.m_H         );
  std::swap( m_size,      x.m_size      );
  std::swap( m_capacity,  x.m_capacity  );
  std::swap( m_bExternal, x.m_bExternal );
  std::swap( m_reserved,  x.m_reserved  );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns reference to an entry at a position (x,y).
///////////////////////////////////////////////////////////////////////////////////////////////////
reference operator()( diff_type x, diff_type y )
{
  ASSERT( __ARR2D_IS_INSIDE__( x, y ) );
  return (m_head[ x + y*m_W ]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant reference to an entry at a position (x,y).
///////////////////////////////////////////////////////////////////////////////////////////////////
const_reference operator()( diff_type x, diff_type y ) const
{
  ASSERT( __ARR2D_IS_INSIDE__( x, y ) );
  return (m_head[ x + y*m_W ]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns pointer to an entry at a position (x,y).
///////////////////////////////////////////////////////////////////////////////////////////////////
pointer ptr( diff_type x, diff_type y )
{
  ASSERT( __ARR2D_IS_INSIDE__( x, y ) );
  return (m_head + (x + y*m_W));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant pointer to an entry at a position (x,y).
///////////////////////////////////////////////////////////////////////////////////////////////////
const_pointer ptr( diff_type x, diff_type y ) const
{
  ASSERT( __ARR2D_IS_INSIDE__( x, y ) );
  return (m_head + (x + y*m_W));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns pointer to an entry at a position <b>p</b>.
///////////////////////////////////////////////////////////////////////////////////////////////////
pointer ptr( const Pnt2i & p )
{
  ASSERT( __ARR2D_IS_INSIDE__( p.x, p.y ) );
  return (m_head + (p.x + p.y*m_W));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant pointer to an entry at a position <b>p</b>.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_pointer ptr( const Pnt2i & p ) const
{
  ASSERT( __ARR2D_IS_INSIDE__( p.x, p.y ) );
  return (m_head + (p.x + p.y*m_W));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns reference to an entry of at a position <b>p</b>.
///////////////////////////////////////////////////////////////////////////////////////////////////
reference operator[]( const Pnt2i & p )
{
  ASSERT( __ARR2D_IS_INSIDE__( p.x, p.y ) );
  return (m_head[ p.x + p.y*m_W ]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant reference to an entry at a position <b>p</b>.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_reference operator[]( const Pnt2i & p ) const
{
  ASSERT( __ARR2D_IS_INSIDE__( p.x, p.y ) );
  return (m_head[ p.x + p.y*m_W ]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets new value at a position (x,y), if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
void set_if_in( diff_type x, diff_type y, const_reference value )
{
  if (__ARR2D_IS_INSIDE__( x, y ))
    m_head[ x + y*m_W ] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets new value at a position <b>p</b>, if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
void set_if_in( const Pnt2i & p, const_reference value )
{
  if (__ARR2D_IS_INSIDE__( p.x, p.y ))
    m_head[ p.x + p.y*m_W ] = value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero constant pointer to a point (x,y), if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_pointer get_if_in( diff_type x, diff_type y ) const
{
  return ((__ARR2D_IS_INSIDE__( x, y )) ? (m_head + (x + y*m_W)) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero pointer to a point (x,y), if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
pointer get_if_in( diff_type x, diff_type y )
{
  return ((__ARR2D_IS_INSIDE__( x, y )) ? (m_head + (x + y*m_W)) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero constant pointer to a point <b>p</b>, if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_pointer get_if_in( const Pnt2i & p ) const
{
  return ((__ARR2D_IS_INSIDE__( p.x, p.y )) ? (m_head + (p.x + p.y*m_W)) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero pointer to a point <b>p</b>, if it lies inside 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
pointer get_if_in( const Pnt2i & p )
{
  return ((__ARR2D_IS_INSIDE__( p.x, p.y )) ? (m_head + (p.x + p.y*m_W)) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane offset of a point (x,y) within 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type offset( diff_type x, diff_type y ) const 
{
  ASSERT( __ARR2D_IS_INSIDE__( x, y ) );
  return (x + y*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane offset of a point <b>p</b> within 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type offset( const Pnt2i & p ) const
{
  ASSERT( __ARR2D_IS_INSIDE__( p.x, p.y ) );
  return (p.x + p.y*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane offset if a point (x,y) is internal one, and -1 otherwise.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type offset_if_in( diff_type x, diff_type y ) const
{
  return ((__ARR2D_IS_INSIDE__( x, y )) ? (x + y*m_W) : (diff_type)(-1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns plane offset if a point <b>p</b> is internal one, and -1 otherwise.
///////////////////////////////////////////////////////////////////////////////////////////////////
diff_type offset_if_in( const Pnt2i & p ) const
{
  return ((__ARR2D_IS_INSIDE__( p.x, p.y )) ? (p.x + p.y*m_W) : (diff_type)(-1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns reference to an entry at a plane index position [n].
///////////////////////////////////////////////////////////////////////////////////////////////////
reference operator[]( size_type n )
{
  ASSERT( n < m_size );
  return m_head[n];
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant reference to an entry at a plane index position [n].
///////////////////////////////////////////////////////////////////////////////////////////////////
const_reference operator[]( size_type n ) const
{
  ASSERT( n < m_size );
  return m_head[n];
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function converts plane offset to (x,y) representation.
///////////////////////////////////////////////////////////////////////////////////////////////////
Pnt2i offset_to_point( size_type offset ) const
{
  ASSERT( offset < m_size );
  Pnt2i p;
  div_t d = ::div( (int)offset, (int)m_W );
  p.x = (Pnt2i::value_type)(d.rem);
  p.y = (Pnt2i::value_type)(d.quot);
  return p;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes coefficients of bilinear interpolation at a point (fx,fy).
///
/// Neighbour points, which participate in bilinear interpolation, are enumerated as follows:
/// <pre><tt>
/// P<sub>3</sub> P<sub>2</sub>
/// P<sub>0</sub> P<sub>1</sub>
/// </tt></pre>
/// These four points are the nearest ones to the point (fx,fy).
///
/// \param  fx       abscissa of a point of interest.
/// \param  fy       ordinate of a point of interest.
/// \param  coefs    resultant coefficient of bilinear interpolation.
/// \param  offsets  resultant plane offsets of neighbour points within 2D-array.
/// \return          nonzero if bilinear interpolation can be made at a point.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool bilinear_coefs( float fx, float fy, float coefs[4], diff_type offsets[4] ) const
{
  diff_type x = (diff_type)floor( fx );
  diff_type y = (diff_type)floor( fy );
  diff_type x1 = 1;
  diff_type y1 = (diff_type)m_W;
  diff_type W1 = (diff_type)m_W-1;
  diff_type H1 = (diff_type)m_H-1;

  // Border pixel requires additional processing.
  if ((x < 0) || (y < 0) || (x >= W1) || (y >= H1))
  {
    if ((m_size == 0) || (x < -1) || (y < -1) || (x >= (diff_type)m_W) || (y >= (diff_type)m_H))
      return false;

    if (x == -1) x1 = (x = 0); else if (x == W1) x1 = 0;
    if (y == -1) y1 = (y = 0); else if (y == H1) y1 = 0;
  }

  float     dx = (float)(fx - x);
  float     dy = (float)(fy - y);
  diff_type i = (diff_type)(x + y*m_W);

  coefs[2] = dx * dy;
  coefs[1] = dx - coefs[2];
  coefs[3] = dy - coefs[2];
  coefs[0] = 1.0f - dy - coefs[1];

  offsets[0] = i;
  offsets[1] = i+x1;
  offsets[2] = i+x1+y1;
  offsets[3] = i+y1;

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns bilinearly-interpolated value at a point (fx,fy).
///
/// Entries of type 'TYPE' must be supplied by following methods:
/// <ol><li> TYPE operator*(TYPE&, float); </li>
///     <li> TYPE operator+(TYPE&, TYPE&); </li></ol>
///
/// \param  fx  abscissa of a point of interest.
/// \param  fy  ordinate of a point of interest.
/// \return     interpolated value or TYPE(), if the point has occured outside of 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
value_type bilinear_at( float fx, float fy ) const
{
  float     coefs[4];
  diff_type offsets[4];

  if (bilinear_coefs( fx, fy, coefs, offsets ))
  {
    return (value_type)(m_head[ offsets[0] ] * coefs[0] +
                        m_head[ offsets[1] ] * coefs[1] +
                        m_head[ offsets[2] ] * coefs[2] +
                        m_head[ offsets[3] ] * coefs[3]);
  }
  return value_type();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns bilinearly-interpolated value at a point <b>p</b>.
///////////////////////////////////////////////////////////////////////////////////////////////////
value_type bilinear_at( const Pnt2f & p ) const
{
  return bilinear_at( p.x, p.y );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes bilinearly-interpolated value at a point (fx,fy) given a function of
///  summation that combines four neighhbour entries given coefficients of bilinear interpolation.
///
/// \param  fx      abscissa of a point of interest.
/// \param  fy      ordinate of a point of interest.
/// \param  func    the function that summarizes values of four neighbour points.
/// \param  result  result of bilinear interpolation.
/// \return         nonzero if bilinear interpolation can be made at a point.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class SUMMATOR, class RESULT >
bool bilinear_at( float fx, float fy, SUMMATOR func, RESULT & result ) const
{
  float     coefs[4];
  diff_type offsets[4];

  if (bilinear_coefs( fx, fy, coefs, offsets ))
  {
    result = (RESULT)(func( m_head[ offsets[0] ],
                            m_head[ offsets[1] ],
                            m_head[ offsets[2] ],
                            m_head[ offsets[3] ], coefs ));
    return true;
  }
  result = RESULT();
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant reference to a point (x,y) bounded by 2D-array's extent.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_reference bounded_at( diff_type x, diff_type y ) const
{
  ASSERT( m_size > 0 );
  if (x < 0) x = 0; else if (x >= (diff_type)m_W) x = (diff_type)m_W-1;
  if (y < 0) y = 0; else if (y >= (diff_type)m_H) y = (diff_type)m_H-1;
  return m_head[ x + y*m_W ];
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant reference to a point <b>p</b> bounded by 2D-array's extent.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_reference bounded_at( const Pnt2i & p ) const
{
  ASSERT( m_size > 0 );
  if (p.x < 0) p.x = 0; else if (p.x >= (diff_type)m_W) p.x = (diff_type)m_W-1;
  if (p.y < 0) p.y = 0; else if (p.y >= (diff_type)m_H) p.y = (diff_type)m_H-1;
  return m_head[ p.x + p.y*m_W ];
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns 'begin' iterator of specified row.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator row_begin( size_type r )
{
  ASSERT( r < m_H );
  return (m_head + r*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant 'begin' iterator of specified row.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator row_begin( size_type r ) const
{
  ASSERT( r < m_H );
  return (m_head + r*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns 'end' iterator of specified row.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator row_end( size_type r )
{
  ASSERT( r < m_H );
  return (m_head + (r+1)*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant 'end' iterator of specified row.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator row_end( size_type r ) const
{
  ASSERT( r < m_H );
  return (m_head + (r+1)*m_W);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns 'begin' iterator of specified column.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator column_begin( size_type c )
{
  ASSERT( c < m_W );
  return (m_head + c);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant 'begin' iterator of specified column.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator column_begin( size_type c ) const
{
  ASSERT( c < m_W );
  return (m_head + c);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns 'end' iterator of specified column.
///////////////////////////////////////////////////////////////////////////////////////////////////
iterator column_end( size_type c )
{
  ASSERT( c < m_W );
  return (m_tail + c);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns constant 'end' iterator of specified column.
///////////////////////////////////////////////////////////////////////////////////////////////////
const_iterator column_end( size_type c ) const
{
  ASSERT( c < m_W );
  return (m_tail + c);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
Arr2D()
: m_head(0), m_bExternal(0)
{
  release();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///
/// \param  W      width.
/// \param  H      height.
/// \param  vfill  fill value.
/// \param  bInit  if nonzero, then the object will be filled up with specified value.
///////////////////////////////////////////////////////////////////////////////////////////////////
Arr2D( size_type W, size_type H, value_type vfill = value_type(), bool bInit = true )
: m_head(0), m_bExternal(0)
{
  release();
  resize( W, H, vfill, true );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///
/// \param  x  an external array used to initialize this one.
///////////////////////////////////////////////////////////////////////////////////////////////////
Arr2D( const this_type & x )
: m_head(0), m_bExternal(0)
{
  release();
  (*this) = x;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
~Arr2D()
{
  release();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function quickly fills up this object by zeros (!!! D A N G E R O U S !!!).
///////////////////////////////////////////////////////////////////////////////////////////////////
void fast_zero()
{ 
  if (m_size > 0)
    memset( m_head, 0, sizeof(value_type)*m_size );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function quickly copies specified object to this one (!!! D A N G E R O U S !!!).
///////////////////////////////////////////////////////////////////////////////////////////////////
void fast_copy( const this_type & x )
{
  if (&x != this)
  {
    resize( x.width(), x.height(), value_type(), false );
    if (m_size > 0) 
      memcpy( m_head, x.m_head, sizeof(value_type)*m_size );
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks whether specified object has the same dimensions.
///
/// \param  x  the second object (must have the methods 'width()' and 'height()').
/// \return    nonzero, if objects have equal dimensions.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class OBJECT2D >
bool dimensions_equal( const OBJECT2D & x ) const
{
  return (((diff_type)(m_W) == (diff_type)(x.width())) &&
          ((diff_type)(m_H) == (diff_type)(x.height())));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function copies a portion of the source 2D-array into this one.
///
/// \param  dx   the destination start column.
/// \param  dy   the destination start row.
/// \param  src  the source 2D array (may coincide with this one).
/// \param  sx   the source start column.
/// \param  sy   the source start row.
/// \param  cx   the number of columns to copy, 0 results in using of all columns from sx up to end.
/// \param  cy   the number of rows to copy, 0 results in using of all rows from sy up to end.
/// \return      this 2D-array.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & copy( diff_type dx, diff_type dy,
                  const this_type & src, diff_type sx, diff_type sy, diff_type cx, diff_type cy )
{
  if (cx == 0) cx = src.m_W - sx;
  if (cy == 0) cy = src.m_H - sy;

  if ((cx <= 0) || (cy <= 0))
    return (*this);

  ASSERT( (dx >= 0) && (dy >= 0) && (sx >= 0) && (sy >= 0) && (cx > 0) && (cy > 0) );
  ASSERT( ((dx+cx) <= (diff_type)    (m_W)) && ((dy+cy) <= (diff_type)    (m_H)) );
  ASSERT( ((sx+cx) <= (diff_type)(src.m_W)) && ((sy+cy) <= (diff_type)(src.m_H)) );

  if (dy > sy)
  {
    for (diff_type j = cy-1; j >= 0; j--)
    {
      const_pointer s = src.ptr( sx, sy+j );
      pointer       d = ptr( dx, dy+j );

      for (diff_type i = 0; i < cx; i++)
        d[i] = s[i];
    }
  }
  else if (dy == sy)
  {
    for (diff_type j = 0; j < cy; j++)
    {
      const_pointer s = src.ptr( sx, sy+j );
      pointer       d = ptr( dx, dy+j );

      if (dx > sx)
      {
        for (diff_type i = cx-1; i >= 0; i--)
          d[i] = s[i];
      }
      else if ((dx < sx) || (this != &src))
      {
        for (diff_type i = 0; i < cx; i++)
          d[i] = s[i];
      }
    }
  }
  else // if (dy < sy)
  {
    for (diff_type j = 0; j < cy; j++)
    {
      const_pointer s = src.ptr( sx, sy+j );
      pointer       d = ptr( dx, dy+j );

      for (diff_type i = 0; i < cx; i++)
        d[i] = s[i];
    }
  }
  return (*this);
}

};

//////////////////////////////
#undef  __ARR2D_IS_INSIDE__ //
//////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Derived types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  Arr2D< boost::int8_t,  boost::int32_t,  true >   Arr2sb;
typedef  Arr2D< boost::uint8_t, boost::uint32_t, true >   Arr2ub;

typedef  Arr2D< boost::int16_t,  boost::int32_t,  true >  Arr2sw;
typedef  Arr2D< boost::uint16_t, boost::uint32_t, true >  Arr2uw;

typedef  Arr2D< boost::int32_t,  boost::int32_t,  true >  Arr2si;
typedef  Arr2D< boost::uint32_t, boost::uint32_t, true >  Arr2ui;

typedef  Arr2D< float, double, true >                     Arr2f;
typedef  Arr2D< double, double, true >                    Arr2d;
                                                   
typedef  Arr2D< Pnt2i, Pnt2i, true >                      Pnt2iImg;
typedef  Arr2D< Pnt2f, Pnt2f, true >                      Pnt2fImg;
typedef  Arr2D< Pnt2d, Pnt2d, true >                      Pnt2dImg;
                                                   
#endif // __ALIB_BASE_2D_H__

