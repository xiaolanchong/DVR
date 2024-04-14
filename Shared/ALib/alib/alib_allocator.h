///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_allocator.h
// ---------------------
// begin     : 1998
// modified  : 27 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_ALLOCATOR_H__
#define __DEFINE_ALIB_ALLOCATOR_H__

namespace alib
{

#ifdef  USE_ALIB_GREEDY_ALLOCATOR

#ifdef _MSC_VER
#pragma message( "Alib's allocator" )
#endif

///================================================================================================
/// \brief Greedy Memory Manager.
///================================================================================================
class GreedyMemManager
{
//private:
public:
  static const size_t BLOCK_SIZE = (1<<20);

  ///==================================
  /// \brief  A large piece of memory.
  ///==================================
  struct Block
  {
    unsigned char * pMem;

    ///--------------------------------------------------------------------------------------------
    /// \brief Constructor.
    ///--------------------------------------------------------------------------------------------
    Block() : pMem(0)
    {
    }

    ///--------------------------------------------------------------------------------------------
    /// \brief Function pads this block by zeros.
    ///--------------------------------------------------------------------------------------------
    void ZeroInit()
    {
      if (pMem != 0) memset( pMem, 0, BLOCK_SIZE );
    }

    ///--------------------------------------------------------------------------------------------
    /// \brief Function allocates a piece of memory (must be called once).
    ///--------------------------------------------------------------------------------------------
    void Allocate()
    {
      ASSERT( pMem == 0 );
      pMem = reinterpret_cast<unsigned char*>( malloc( BLOCK_SIZE ) );
      ASSERT( pMem != 0 );
      if (pMem == 0) throw std::runtime_error( "Memory allocation failed" );
      ZeroInit();
    }

    ///--------------------------------------------------------------------------------------------
    /// \brief Destructor.
    ///--------------------------------------------------------------------------------------------
    ~Block()
    {
      free( pMem );
    }
  }; // struct Block

  typedef  std::list<Block>  BlockLst;

  BlockLst           m_blocks;      //!< list of allocated blocks
  BlockLst::iterator m_currBlockIt; //!< iterator that points to the current memory block
  int                m_counter;     //!< counter of used bytes in the current block

public:
  ///----------------------------------------------------------------------------------------------
  /// \brief Function clears this object but does not release memory.
  ///----------------------------------------------------------------------------------------------
  void clear()
  {
    m_currBlockIt = (m_blocks.empty()) ? m_blocks.end() : m_blocks.begin();
    m_counter = 0;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function totally deallocates captured memory pieces.
  ///----------------------------------------------------------------------------------------------
  void release()
  {
    m_blocks.clear();
    m_currBlockIt = m_blocks.end();
    m_counter = 0;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function allocates required piece of memory from the memory pool.
  ///----------------------------------------------------------------------------------------------
  void * allocate( size_t n )
  {
    if (n <= 0)
    {
      return 0;
    }
    else if (n > BLOCK_SIZE)
    {
      ASSERT(0);
      throw std::runtime_error( "Too large memory piece requested" );
    }

    if (m_blocks.empty())
    {
      m_blocks.push_back( Block() );
      m_currBlockIt = m_blocks.end();
      --m_currBlockIt;
      m_currBlockIt->Allocate();
      m_counter = 0;
    }
    else if ((m_counter+n) > BLOCK_SIZE) // Is it necessary to allocate a new block?
    {
      ASSERT( m_currBlockIt != m_blocks.end() );
      ++m_currBlockIt;
      if (m_currBlockIt == m_blocks.end())
      {
        m_blocks.push_back( Block() );
        m_currBlockIt = m_blocks.end();
        --m_currBlockIt;
        m_currBlockIt->Allocate();
        m_counter = 0;
      }
      else m_currBlockIt->ZeroInit();
      ASSERT( m_currBlockIt->pMem != 0 );
    }

    void * p = reinterpret_cast<void*>( m_currBlockIt->pMem + m_counter );
    m_counter += (int)n;
    return p;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Constructor.
  ///----------------------------------------------------------------------------------------------
  GreedyMemManager()
  {
    ASSERT( sizeof(char) == 1 );
    release();
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Destructor.
  ///----------------------------------------------------------------------------------------------
  ~GreedyMemManager()
  {
    release();
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function creates object of any type and places it within memory pool.
  ///----------------------------------------------------------------------------------------------
  template< class T >
  T * create_object( T * )
  {
    T * p = reinterpret_cast<T*>( allocate( sizeof(T) ) );
    new(p) T(this);                                        // just call constructor
    return p;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function destroys an object previously placed within memory pool.
  ///----------------------------------------------------------------------------------------------
  template< class T >
  T * delete_object( T * p )
  {
    if (p != 0)
      p->~T();                                             // just call destructor
    return 0;
  }
}; // class GreedyMemManager


///================================================================================================
/// \brief Greedy allocator.
///================================================================================================
template< class T >
class Allocator
{
public:
  typedef Allocator<T>       this_type;
  typedef T                  value_type;
  typedef value_type       * pointer;
  typedef value_type       & reference;
  typedef const value_type * const_pointer;
  typedef const value_type & const_reference;
  typedef size_t             size_type;
  typedef ptrdiff_t          difference_type;

public:
//protected:
  GreedyMemManager * m_pMemManager;  //!< pointer the instance of memory manager

public:
  ///================================================================
  /// \brief Object converts an Allocator<T> to an Allocator<OTHER>.
  ///================================================================
  template< class OTHER >
  struct rebind { typedef Allocator<OTHER> other; };

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns stored pointer to the instance of memory manager.
  ///----------------------------------------------------------------------------------------------
  GreedyMemManager * get_memory_manager() const
  {
    return m_pMemManager;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns address of mutable x.
  ///----------------------------------------------------------------------------------------------
  pointer address( reference x ) const
  {
    return (&x);
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns address of nonmutable x.
  ///----------------------------------------------------------------------------------------------
  const_pointer address( const_reference x ) const
  {
    return (&x);
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Constructs default allocator (do nothing).
  ///----------------------------------------------------------------------------------------------
  Allocator() : m_pMemManager(0)
  {
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Constructor assigns a memory manager.
  ///----------------------------------------------------------------------------------------------
  explicit Allocator( GreedyMemManager * pManager )
  {
    m_pMemManager = pManager;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Copy constructor.
  ///----------------------------------------------------------------------------------------------
  Allocator( const this_type & a )
  {
    m_pMemManager = a.m_pMemManager;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Copy constructor that accepts allocator of any suitable type.
  ///----------------------------------------------------------------------------------------------
  template<class OTHER>
  Allocator( const Allocator<OTHER> & a )
  {
    m_pMemManager = a.get_memory_manager();
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Operator copy.
  ///----------------------------------------------------------------------------------------------
  template<class OTHER>
  this_type & operator=( const Allocator<OTHER> & a )
  {
    m_pMemManager = a.get_memory_manager();
    return (*this);
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function allocates specified amount of memory.
  ///----------------------------------------------------------------------------------------------
  pointer allocate( size_type n )
  {
    ASSERT( m_pMemManager != 0 );
    return reinterpret_cast<pointer>( m_pMemManager->allocate( n*sizeof(T) ) );
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function deallocates captured memory.
  ///----------------------------------------------------------------------------------------------
  void deallocate( void *, size_type )
  { 
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function constructs an object of given type.
  ///----------------------------------------------------------------------------------------------
  void construct( pointer p, const T & val )
  { 
    new(p) T(val);   // just call constructor
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function destructs an object of given type.
  ///----------------------------------------------------------------------------------------------
  void destroy( pointer p )
  {
    p->~T();  p;
  }

  ///----------------------------------------------------------------------------------------------
  /// \brief Function returns the maximum possible length of a sequence.
  ///----------------------------------------------------------------------------------------------
  size_t max_size() const
  { 
    return (size_t)(INT_MAX/sizeof(T));
  }
}; // class Allocator

#else // !USE_ALIB_GREEDY_ALLOCATOR

#ifdef  _MSC_VER
#pragma message( "Default STL allocator" )
#endif

///----------------------------------------------------------------------------------------------
/// \brief Default memory manager just calls new/delete.
///----------------------------------------------------------------------------------------------
class GreedyMemManager
{
public:
  void clear() {}
  void release() {}
  void * allocate( size_t ) { return 0; }
  GreedyMemManager() {}
  ~GreedyMemManager() {}
  template< class T > T * create_object( T * p ) { p; return (new T(this)); }
  template< class T > T * delete_object( T * p ) { delete p; return 0;      }
};


///----------------------------------------------------------------------------------------------
/// \brief Class just inherits standard STL allocator.
///----------------------------------------------------------------------------------------------
template< class T >
class Allocator : public std::allocator<T>
{
public:
  Allocator() {}
  Allocator( GreedyMemManager & ) {}
  Allocator( const Allocator<T> & ) {}
  Allocator<T> & operator=( const Allocator<T> & ) { return (*this); }
  GreedyMemManager * get_memory_manager() const { return 0; }
};

#endif // USE_ALIB_GREEDY_ALLOCATOR

} // namespace alib

#endif // __DEFINE_ALIB_ALLOCATOR_H__

