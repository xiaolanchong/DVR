///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_vec2d.h
// ---------------------
// begin     : 1998
// modified  : 27 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_VECTOR_2D_H__
#define __DEFINE_ALIB_VECTOR_2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Vector2D.
/// \brief Two-dimensional vector.
///
/// 'TYPE' - coordinate type, 'PRECISE' - superior type used to enhance computational precision.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE, class PRECISE >
struct Vector2D : public Point2D<TYPE,PRECISE>
{

typedef  Vector2D<TYPE,PRECISE>  this_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fucntion checks compatibility of specified numerical types.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool compatible_types()
{
  return ( (std::numeric_limits<value_type>::is_specialized) &&
           (std::numeric_limits<precise_type>::is_specialized) &&
           (std::numeric_limits<value_type>::is_integer ==
            std::numeric_limits<precise_type>::is_integer) &&
           (std::numeric_limits<value_type>::is_signed ==
            std::numeric_limits<precise_type>::is_signed) &&
           (sizeof(value_type) <= sizeof(precise_type)) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Contructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2D()
{
  ASSERT( compatible_types() );
  x = 0;
  y = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Contructor.
///
/// \param  nx  the new abscissa.
/// \param  ny  the new ordinate.
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2D( value_type nx, value_type ny )
{
  ASSERT( compatible_types() );
  x = nx;
  y = ny;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Copy contructor that does NOT check the overflow of coordinate values.
///
/// \param  v  the input vector with coordinates of any type.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class SOMEPOINT >
Vector2D( const SOMEPOINT & v )
{
  ASSERT( compatible_types() );
  if (std::numeric_limits<value_type>::is_integer &&
      !(std::numeric_limits<typename SOMEPOINT::value_type>::is_integer))
  {
    x = (value_type)(floor( v.x + 0.5 ));
    y = (value_type)(floor( v.y + 0.5 ));
  }
  else
  {
    x = (value_type)(v.x);
    y = (value_type)(v.y);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Copy operator that does NOT check the overflow of coordinate values.
///
/// \param  v  the input vector with coordinates of any type.
/// \return    reference to this vector with newly set coordinates.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class SOMEPOINT >
this_type & operator=( const SOMEPOINT & v )
{ 
  if (std::numeric_limits<value_type>::is_integer &&
      !(std::numeric_limits<typename SOMEPOINT::value_type>::is_integer))
  {
    x = (value_type)(floor( v.x + 0.5 ));
    y = (value_type)(floor( v.y + 0.5 ));
  }
  else
  {
    x = (value_type)(v.x);
    y = (value_type)(v.y);
  }
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns dimension of this vector.
///
/// \return  dimension (=2).
///////////////////////////////////////////////////////////////////////////////////////////////////
int size() const
{ 
  return 2;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets coordinates of this vector.
///
/// \param  nx  the new abscissa.
/// \param  ny  the new ordinate.
/// \return     reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & set( value_type nx, value_type ny )
{ 
  x = nx;
  y = ny;
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes inner product of two vectors.
///
/// \param  v  the second vector.
/// \return    inner product.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
precise_type operator&( const Point2D<TYPE2,precise_type> & v ) const
{
  return ((x)*(precise_type)(v.x) +
          (y)*(precise_type)(v.y));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes outer product of two vectors.
///
/// \param  v  the second vector.
/// \return    outer product (z-coordinate in 3D space).
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
precise_type operator*( const Point2D<TYPE2,precise_type> & v ) const
{
  return ((x)*(precise_type)(v.y) -
          (y)*(precise_type)(v.x));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator multiplies this vector by any scalar.
///
/// \param  c  the scalar value.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & operator*=( value_type c )
{ 
  x *= c;
  y *= c;
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator returns the result of multiplication of this vector by any scalar.
///
/// \param  c  the scalar value.
/// \return    resultant vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type operator*( value_type c ) const
{ 
  return this_type( x*c, y*c );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator adds specified vector to this one.
///
/// \param  v  the vector to be added.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
this_type & operator+=( const Point2D<TYPE2,precise_type> & v )
{
  x = (value_type)(x + v.x);
  y = (value_type)(y + v.y);
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator returns the result of summation of this vector with specified one.
///
/// \param  v  the vector to be added.
/// \return    the sum of this vector and specified one.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
this_type operator+( const Point2D<TYPE2,precise_type> & v ) const
{ 
  return this_type( (value_type)(x + v.x),
                    (value_type)(y + v.y) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator subtracts specified vector from this one.
///
/// \param  v  the vector to be subtracted.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
this_type & operator-=( const Point2D<TYPE2,precise_type> & v )
{
  x = (value_type)(x - v.x);
  y = (value_type)(y - v.y);
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator returns the vector of difference between this vector and specified one.
///
/// \param  v  the vector to be subtracted.
/// \return    the difference of this vector and specified one.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
this_type operator-( const Point2D<TYPE2,precise_type> & v ) const
{
  return this_type( (value_type)(x - v.x),
                    (value_type)(y - v.y) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes squared length of this vector.
///
/// \return  squared length of this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
precise_type sq_length() const
{ 
  return ((x)*(precise_type)(x) + (y)*(precise_type)(y));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes the length of this vector.
///
/// \return  the length of this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
double length() const
{ 
  return sqrt( (double)sq_length() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes squared length of difference between this vector and specified one.
///
/// \param  v  the second vector.
/// \return    squared length of vector difference.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
precise_type sq_distance( const Point2D<TYPE2,precise_type> & v ) const
{
  precise_type dx = (precise_type)(x) - (precise_type)(v.x);
  precise_type dy = (precise_type)(y) - (precise_type)(v.y);
  return (dx*dx + dy*dy);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes the length of difference between this vector and specified one.
///
/// \param  v  the 2nd vector.
/// \return    the length of vector difference.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE2 >
double distance( const Point2D<TYPE2,precise_type> & v ) const
{
  return sqrt( (double)(sq_distance( v )) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes the angle between this vector and X-axis.
///
/// \param  b0_2PI  if true, then the range of the angle will be [0...2PI], otherwise [-PI...PI].
/// \return         the angle.
///////////////////////////////////////////////////////////////////////////////////////////////////
double angle( bool b0_2PI = false ) const
{ 
  double a = atan2( (double)y, (double)x );
  if (b0_2PI && (a < 0)) a = fabs( a + (2.0*3.14159265358979323846) );
  return a;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function negates coordinates of this vector.
///
/// \return  reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & negate()
{ 
  x = -x;
  y = -y;
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the vector with negated coordinates of this one.
///
/// \return  the newly constructed vector with negative coordinates.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type operator-() const
{ 
  return this_type( -x, -y );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator addresses <i>x</i> or <i>y</i> coordinate.
///
/// \return  the <i>x</i> (<i>n</i>=0) or the <i>y</i> (<i>n</i>=1) coordinate.
///////////////////////////////////////////////////////////////////////////////////////////////////
value_type & operator[]( int n )
{ 
  ASSERT( (n == 0) || (n == 1) );
  return (n == 0) ? x : y;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator addresses <i>x</i> or <i>y</i> coordinate.
///
/// \return  the <i>x</i> (<i>n</i>=0) or the <i>y</i> (<i>n</i>=1) coordinate.
///////////////////////////////////////////////////////////////////////////////////////////////////
value_type operator[]( int n ) const
{ 
  ASSERT( (n == 0) || (n == 1) );
  return (n == 0) ? x : y;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns a vector that is perpendicular to this one.
///
/// \return  the vector that is perpendicular to this one.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type get_perpendicular() const
{ 
  return this_type( -y, x );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets this vector perpendicular to its initial direction.
///
/// \return  reference to this vector that is made perpendicular to the initial direction.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & set_perpendicular()
{
  value_type t = x;  x = -y;  y = t;
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns nonzero, if the vector has integer coordinates
///        (needed by some template functions).
///
/// \return  nonzero if the vector has integer coordinates.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool integer() const
{ 
  return std::numeric_limits<value_type>::is_integer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator divides this vector by a scalar.
///
/// \param  c  the scalar value.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & operator/=( value_type c )
{ 
  if (std::numeric_limits<value_type>::is_integer)
    ASSERT( c != 0 );
  else
    ASSERT( fabs( (double)c ) > (std::numeric_limits<value_type>::min() /
                                 std::numeric_limits<value_type>::epsilon()) );
  x /= c;
  y /= c;
  return (*this);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator returns this vector divides by a scalar.
///
/// \param  c  the scalar value.
/// \return    resultant vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type operator/( value_type c ) const
{ 
  if (std::numeric_limits<value_type>::is_integer)
    ASSERT( c != 0 );
  else
    ASSERT( fabs( (double)c ) > (std::numeric_limits<value_type>::min() /
                                 std::numeric_limits<value_type>::epsilon()) );
  return this_type( x/c, y/c );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function compares two vectors to determine whether they have the same coordinates.
///
/// \param  v  the vector to compare with.
/// \return    nonzero if two vectors are similar.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool operator==( const this_type & v ) const
{
  return ((x == v.x) && (y == v.y));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function compares two vectors to determine whether they have different coordinates.
///
/// \param  v  the vector to compare with.
/// \return    nonzero if two vectors are different.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool operator!=( const this_type & v ) const
{ 
  return ((x != v.x) || (y != v.y));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function compares lengthes of this vector and specified one.
///
/// \param  v  the vector to compare with.
/// \return    nonzero if this vector is shorter than specified one.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool operator<( const this_type & v ) const
{
  return (sq_length() < v.sq_length());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function compares lengthes of this vector and specified one.
///
/// \param  v  the vector to compare with.
/// \return    nonzero if this vector is longer than specified one.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool operator>( const this_type & v ) const
{
  return (sq_length() > v.sq_length());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function shifts vector's coordinates to the left
///        (useful when dealing with fixed precision coordinates).
///
/// \param  s  the shift.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type operator<<( int s ) const
{
  ASSERT( std::numeric_limits<value_type>::is_integer );
  return this_type( x << s, y << s );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function shifts vector's coordinates to the right
///        (useful when dealing with fixed precision coordinates).
///
/// \param  s  the shift.
/// \return    reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type operator>>( int s ) const
{
  ASSERT( std::numeric_limits<value_type>::is_integer );
  return this_type( x >> s, y >> s );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks whether specified point belongs to 8-tuple neighbourhood of this one.
///
/// \param  v  the point to compare with.
/// \return    -1 - points coincide, +1 - points are neighbours, 0 - points are not neighbours.
///////////////////////////////////////////////////////////////////////////////////////////////////
int is_neighbor( const this_type & v ) const
{
  ASSERT( std::numeric_limits<value_type>::is_integer );

  if ((x == v.x) && (y == v.y))
    return -1;
  else if ((abs( x - v.x ) <= 1) && (abs( y - v.y ) <= 1))
    return +1;
  else
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function normalizes this vector.
///
/// \return  the length of initial vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
double normalize()
{
  ASSERT( !(std::numeric_limits<value_type>::is_integer) );
  const value_type min = std::numeric_limits<value_type>::min() /
                         std::numeric_limits<value_type>::epsilon();

  double len = sqrt( x*(double)x + y*(double)y );
  if (len > min)
  {
    x /= (value_type)len;
    y /= (value_type)len;
  }
  else
  {
    x = (y = 0);
    len = 0.0;
  }
  return len;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns rotated vector.

/// \param  angle  the angle of rotation.
/// \return        rotated vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type get_rotated( double angle ) const
{
  ASSERT( !(std::numeric_limits<value_type>::is_integer) );
  double cs = cos( angle );
  double sn = sin( angle );
  return this_type( (value_type)(x*cs - y*sn),
                    (value_type)(x*sn + y*cs) );
}

#if (defined(_WINDOWS) && (defined(_WIN32) || defined(_WIN64)))

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Contructor initializes this vector from Windows' POINT instance.
///
/// \param  p  the object of POINT type.
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2D( const POINT & p )
{
  ASSERT( compatible_types() );
  x = (value_type)(p.x);
  y = (value_type)(p.y);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator converts this vector to Windows's POINT.
///
/// \return  the object of POINT type.
///////////////////////////////////////////////////////////////////////////////////////////////////
operator POINT() const
{ 
  POINT p;

  if (std::numeric_limits<value_type>::is_integer)
  {
    p.x = (LONG)x;
    p.y = (LONG)y;
  }
  else
  {
    p.x = (LONG)(floor( x + 0.5 ));
    p.y = (LONG)(floor( y + 0.5 ));
  }
  return p;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Operator converts Windows' POINT to this vector.
///
/// \return  reference to this vector.
///////////////////////////////////////////////////////////////////////////////////////////////////
this_type & operator=( const POINT & p )
{ 
  x = (value_type)(p.x);
  y = (value_type)(p.y);
  return (*this);
}

#endif // _WINDOWS && (_WIN32 || _WIN64)

};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Relation of three points.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum RelationOfThreePoints
{
  ALIB_REL3PNT_LEFT    = (1<<0), 
  ALIB_REL3PNT_RIGHT   = (1<<1), 
  ALIB_REL3PNT_BEYOND  = (1<<2),
  ALIB_REL3PNT_BEHIND  = (1<<3),
  ALIB_REL3PNT_BETWEEN = (1<<4),
  ALIB_REL3PNT_HEAD    = (1<<5),
  ALIB_REL3PNT_TAIL    = (1<<6)
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Function classifies relative positions of three points.
///
///  <tt><pre>
/// p1 *----------* p2
///    |
///    |
///    * p3
/// </pre></tt>
///
/// \param  p1  the first point.
/// \param  p2  the second point.
/// \param  p3  the third point.
/// \return     three-point relation type.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE, class PRECISE >
RelationOfThreePoints ClassifyPosition( const Vector2D<TYPE,PRECISE> & p1,
                                        const Vector2D<TYPE,PRECISE> & p2,
                                        const Vector2D<TYPE,PRECISE> & p3 )
{
  Vector2D<TYPE,PRECISE> a = p2 - p1;
  Vector2D<TYPE,PRECISE> b = p3 - p1;
  PRECISE                ab = a*b;

  if (ab > 0)
    return ALIB_REL3PNT_LEFT;

  if (ab < 0)
    return ALIB_REL3PNT_RIGHT;

  if (((PRECISE)(a.x)*(PRECISE)(b.x) < 0) ||
      ((PRECISE)(a.y)*(PRECISE)(b.y) < 0))
    return ALIB_REL3PNT_BEHIND;

  if (a.sq_length() < b.sq_length())
    return ALIB_REL3PNT_BEYOND;

  if (p1 == p3)
    return ALIB_REL3PNT_HEAD;

  if (p2 == p3)
    return ALIB_REL3PNT_TAIL;

  return ALIB_REL3PNT_BETWEEN;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Derived types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  Vector2D<boost::int16_t,boost::int32_t>  Vec2s;
typedef  Vector2D<boost::int32_t,boost::int32_t>  Vec2i;
typedef  Vector2D<float,double>                   Vec2f;
typedef  Vector2D<double,double>                  Vec2d;

typedef  std::vector<Vec2s>                       Vec2sArr;
typedef  std::vector<Vec2i>                       Vec2iArr;
typedef  std::vector<Vec2f>                       Vec2fArr;
typedef  std::vector<Vec2d>                       Vec2dArr;
                                 
typedef  std::list<Vec2s>                         Vec2sLst;
typedef  std::list<Vec2i>                         Vec2iLst;
typedef  std::list<Vec2f>                         Vec2fLst;
typedef  std::list<Vec2d>                         Vec2dLst;

// Counter-clockwise neighbors of a pixel, 8-connectivity.
const Vec2i ALIB_NEIBR8[8] = { Vec2i( 1, 0),
                               Vec2i( 1, 1),
                               Vec2i( 0, 1),
                               Vec2i(-1, 1),
                               Vec2i(-1, 0),
                               Vec2i(-1,-1),
                               Vec2i( 0,-1),
                               Vec2i( 1,-1) };

// Counter-clockwise neighbors of a pixel, 4-connectivity.
const Vec2i ALIB_NEIBR4[4] = { Vec2i( 1, 0),
                               Vec2i( 0, 1),
                               Vec2i(-1, 0),
                               Vec2i( 0,-1) };

#endif // __DEFINE_ALIB_VECTOR_2D_H__

