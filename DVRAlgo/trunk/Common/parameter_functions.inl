///////////////////////////////////////////////////////////////////////////////////////////////////
// parameter_functions.inl
// ---------------------
// begin     : Aug 2004
// modified  : 19 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "parameter_functions.h"
#include "exportable_functions.h"

namespace dvralgo
{

///------------------------------------------------------------------------------------------------
/// \brief Function loads parameters from a file.
///
/// \param  fileName    the name of source file.
/// \param  pBinParams  pointer to destination binary storage of parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool LoadParameters( const char * fileName, Elvees::IMessage * pOutput )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  BinaryData temp;
  if ((fileName != 0) && alib::LoadArray( fileName, temp ))
  {
    if (Elvees::BinaryToParameters( &temp, &(theLib.m_parameters), pOutput ))
      return true;
  }
  else if (pOutput != 0)
  {
    pOutput->Print( Elvees::IMessage::mt_error,
      _T("Failed to load parameters from a file.\nDefault ones will be set on.") );
  }
  theLib.m_parameters.set_default();
  return false;
}


///------------------------------------------------------------------------------------------------
/// \brief Function saves parameters to a file.
///
/// \param  fileName    the name of destination file.
/// \param  pBinParams  pointer to source binary storage of parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool SaveParameters( const char * fileName, Elvees::IMessage * pOutput )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  BinaryData temp;
  if (Elvees::ParametersToBinary( &(theLib.m_parameters), &temp, pOutput ))
  {
    if (alib::SaveArray( fileName, temp ))
    {
      return true;
    }
    else if (pOutput != 0)
    {
      pOutput->Print( Elvees::IMessage::mt_error, _T("Failed to save parameters to a file") );
    }
  }
  return false;
}

/*
///------------------------------------------------------------------------------------------------
/// \brief Function shows dialog window used to edit parameters stored in DLL class.
///
/// \param  pBinParams  in: pointer to binary storage of parameters, out: modified parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \param  hParentWnd  the handle of a parent window or 0.
/// \param  dlgWidth    desired width of dialog window or 0 (default width).
/// \param  dlgHeight   desired height of dialog window ot 0 (default height).
/// \return             Ok = +1, Cancel = -1, error = 0.
///------------------------------------------------------------------------------------------------

sint ShowParameterDialog( BinaryData       * pBinParams,
                          Elvees::IMessage * pOutput,
                          ulong              hParentWnd,
                          uint               dlgWidth,
                          uint               dlgHeight )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  dlgWidth  = (dlgWidth  == 0) ? DEFAULT_PARAM_DIALOG_WIDTH  : dlgWidth;
  dlgHeight = (dlgHeight == 0) ? DEFAULT_PARAM_DIALOG_HEIGHT : dlgHeight;

  if (pBinParams != 0)
  {
#if (defined(_WIN32) || defined(_WIN64))
    CWnd * pParent = ((hParentWnd == 0) ? ((CWnd*)0) : (CWnd::FromHandle( (HWND)hParentWnd )));
    if ((CParameterDialog( theLib.m_parameters, pParent, dlgWidth, dlgHeight )).DoModal() == IDOK)
#else
    ELVEES_ASSERT(0);
#endif
    {
      if (Elvees::ParametersToBinary( &(theLib.m_parameters), pBinParams, pOutput ))
        return (+1);
    }
    else return (-1);
  }
  return 0;
}


///------------------------------------------------------------------------------------------------
/// \brief Function shows dialog window used to edit specified parameters.
///
/// \param  pParams     in: pointer to parameter structure, out: modified parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \param  hParentWnd  the handle of a parent window or 0.
/// \param  dlgWidth    desired width of dialog window or 0 (default width).
/// \param  dlgHeight   desired height of dialog window ot 0 (default height).
/// \return             Ok = +1, Cancel = -1, error = 0.
///------------------------------------------------------------------------------------------------
sint ShowParameterDialog2( alib::BaseParamSection * pParams,
                           Elvees::IMessage           * pOutput,
                           ulong                        hParentWnd,
                           uint                         dlgWidth,
                           uint                         dlgHeight )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  pOutput;
  dlgWidth  = (dlgWidth  == 0) ? DEFAULT_PARAM_DIALOG_WIDTH  : dlgWidth;
  dlgHeight = (dlgHeight == 0) ? DEFAULT_PARAM_DIALOG_HEIGHT : dlgHeight;

  if (pParams != 0)
  {
#if (defined(_WIN32) || defined(_WIN64))
    CWnd * pParent = ((hParentWnd == 0) ? ((CWnd*)0) : (CWnd::FromHandle( (HWND)hParentWnd )));
    if ((CParameterDialog( *pParams, pParent, dlgWidth, dlgHeight )).DoModal() == IDOK)
#else
    ELVEES_ASSERT(0);
#endif
    {
      return (+1);
    }
    else return (-1);
  }
  return 0;
}

*/
///------------------------------------------------------------------------------------------------
/// \brief Function sets parameters stored in DLL by default values.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool SetDefaultParameters( BinaryData * pBinParams, Elvees::IMessage * pOutput )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  theLib.m_parameters.set_default();
  if (pBinParams != 0)
  {
    if (Elvees::ParametersToBinary( &(theLib.m_parameters), pBinParams, pOutput ))
      return true;
  }
  return false;
}


///------------------------------------------------------------------------------------------------
/// \brief Function fills a binary storage by the current parameters settings stored in DLL.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool GetParameters( BinaryData * pBinParams, Elvees::IMessage * pOutput )
{
  boost::mutex::scoped_lock lock( theLib.m_dataLocker );

  if (pBinParams == 0)
    return false;

  return (Elvees::ParametersToBinary( &(theLib.m_parameters), pBinParams, pOutput ));
}

} // namespace dvralgo

