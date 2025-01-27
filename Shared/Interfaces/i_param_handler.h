///////////////////////////////////////////////////////////////////////////////////////////////////
// i_param_handler.h
// ---------------------
// begin     : 2006
// modified  : 8 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_IPARAMETER_HANDLER_H__
#define __DEFINE_ELVEES_IPARAMETER_HANDLER_H__

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Interface of any class that loads, saves and shows application settings.
///////////////////////////////////////////////////////////////////////////////////////////////////
class IParameterHandler
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Virtual destructor ensures safe deallocation.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual IParameterHandler() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function shows dialog window used to edit pParameters->
///
/// \param  fileName     the name of a file used to save modified parameters or 0.
/// \param  pBinParams   in: pointer to a binary storage of parameters, out: modified parameters.
/// \param  pOutput      pointer to the message handler or 0.
/// \param  hParentWnd   the handle of a parent window or 0.
/// \param  dlgWidth     desired width of dialog window.
/// \param  dlgHeight    desired height of dialog window.
/// \return              Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool ShowParameterDialog( const char       * filename,
                                  BinaryData       * pBinParams,
                                  Elvees::IMessage * pOutput,
                                  ulong              hParentWnd,
                                  uint               dlgWidth,
                                  uint               dlgHeight ) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief .
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool SetDefaultParameters( const char       * filename,
                                   BinaryData       * pBinParams,
                                   Elvees::IMessage * pOutput ) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief .
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool GetParameters( BinaryData       * pBinParams,
                            Elvees::IMessage * pOutput ) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief .
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadParameters( const char       * filename,
                     BinaryData       * pBinParams,
                     Elvees::IMessage * pOutput ) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief .
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SaveParameters( const char       * filename,
                     const BinaryData * pBinParams,
                     Elvees::IMessage * pOutput ) = 0;

};

} // namespace Elvees

#endif // __DEFINE_ELVEES_IPARAMETER_HANDLER_H__

