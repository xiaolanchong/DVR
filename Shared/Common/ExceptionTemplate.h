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

template <class T> class BaseException : public T 
{
public:
	explicit BaseException( const char* p ) : T(p){}
	explicit BaseException( const std::string& p ) : T(p){}
};

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

#endif // _EXCEPTION_TEMPLATE_7087572588809126_