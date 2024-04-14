///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_macros.h
// ---------------------
// begin     : 1998
// modified  : 17 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DEFINE_ALIB_MACROS_H__
#define  __DEFINE_ALIB_MACROS_H__

#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Sometimes we need macros to make debug version a bit faster.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define  ALIB_MAX(a,b)               ( ((a) > (b)) ? (a) : (b) )

#define  ALIB_MIN(a,b)               ( ((a) < (b)) ? (a) : (b) )

#define  ALIB_LIMIT(v,min,max)       ( ((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)) )

#define  ALIB_IS_LIMITED(v,min,max)  ( ((min) <= (v)) && ((v) <= (max)) )

#define  ALIB_IS_RANGE(v,min,max)    ( ((min) <= (v)) && ((v) < (max)) )

#define  ALIB_LengOf(array)          ( sizeof(array)/sizeof(array[0]) )

#define  ALIB_ROUND(v)               ( (int)((x) + (((x) < 0) ? (-0.5) : (+0.5))) )

#define  ALIB_SQR(v)                 ( (v)*(v) )

#define  ALIB_CUB(v)                 ( (v)*(v)*(v) )

/*
#define  ALIB_SWAP(T,v1,v2)          { T t = (v1);  (v1) = (v2);  (v2) = t; }

#define  ALIB_SIGN(v)                ( ((v) > 0) ? +1 : (((v) < 0) ? -1 : 0) )

#define  ALIB_INTERPOLATE(v1,v2,t)   ( (v1) + ((v2)-(v1))*(t) )

#define  ALIB_DELETE_OBJECT(p)       { delete   (p);  (p) = 0; }

#define  ALIB_DELETE_ARRAY(p)        { delete[] (p);  (p) = 0; }

#define  ALIB_ABS(x)                 ( ((x) >= 0) ? (x) : -(x) )

#define  ALIB_IS_FLOAT_TYPE(T)       ( std::numeric_limits<T>::is_specialized == true && \
                                       std::numeric_limits<T>::is_integer == false )

#define  ALIB_IS_INT_TYPE(T)         ( std::numeric_limits<T>::is_specialized == true && \
                                       std::numeric_limits<T>::is_integer == true )

#define  ALIB_DISTANCE(a, b, T, r)   { double dx = (double)((a).x) - (double)((b).x); \
                                       double dy = (double)((a).y) - (double)((b).y); \
                                       r = (T)sqrt( dx*dx + dy*dy ); }
*/

// Use the macro with integer types ONLY!
#define  ALIB_ARE_NEIGHBORS(x1,y1,x2,y2) \
  (((x1)==(x2) && (y1)==(y2)) ? -1 : (((abs((x1)-(x2))<=1) && (abs((y1)-(y2))<=1)) ? +1 : 0))

#endif // __DEFINE_ALIB_MACROS_H__

