//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Шаблон для генерации иерархии классов исключений

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _EXCEPTION_TEMPLATE_7087572588809126_
#define _EXCEPTION_TEMPLATE_7087572588809126_
//======================================================================================//
//                               class ExceptionTemplate                                //
//======================================================================================//

//! \brief Шаблон для генерации иерархии классов исключений
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.01.2006
//! \version 1.0
//! \bug 
//! \todo 

#define MACRO_EXCEPTION(Child, Parent) \
class Child : public Parent \
{ \
public: \
	Child(const std::string& Cause): \
		Parent(Cause){}  \
	Child(const char* Cause): \
		Parent(Cause){} \
	Child(const Child& ex): Parent(ex){} \
};

class CommonException : public std::exception
{
	std::string m_Cause;
public:
	CommonException( const std::string& cause):
	  m_Cause( cause )
	  {
//#define EXCEPTION_INT3 
#ifdef EXCEPTION_INT3 
		  _asm int 3
#endif
	  }

	  const char* what() const { return m_Cause.c_str();}
};
//typedef BaseException<std::runtime_error>  DVRBaseException;

#endif // _EXCEPTION_TEMPLATE_7087572588809126_