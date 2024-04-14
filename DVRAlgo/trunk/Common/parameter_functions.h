///////////////////////////////////////////////////////////////////////////////////////////////////
// parameter_functions.h
// ---------------------
// begin     : Aug 2004
// modified  : 19 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_DVR_PARAMETER_FUNCTIONS_H__
#define __DEFINE_DVR_PARAMETER_FUNCTIONS_H__

namespace Elvees
{

///------------------------------------------------------------------------------------------------
/// \brief Function initializes parameter structure from the binary image of parameters.
///
/// \param  pBinParams   pointer to source binary storage of parameters.
/// \param  pParameters  pointer to destination parameter structure.
/// \param  pOutput      pointer to the message handler or 0.
/// \return              Ok = true.
///------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool BinaryToParameters( const BinaryData  * pBinParams,
                         PARAMETERS        * pParameters,
                         Elvees::IMessage  * pOutput )
{
  if ((pBinParams == 0) || (pParameters == 0))
    return false;

  if (pBinParams->empty())
  {
    pParameters->set_default();
    return true;
  }

  const TCHAR * text = reinterpret_cast<const TCHAR*>( &((*pBinParams)[0]) );
  int           length = (int)(pBinParams->size()/sizeof(TCHAR));

  if ((length > 0) && (text[length-1] == _T('\0')))
  {
    std::basic_stringstream<TCHAR> strm;
    strm << text;
    if (pParameters->load( strm ))
      return true;
  }

  pParameters->set_default();
  if (pOutput != 0)
  {
    pOutput->Print( Elvees::IMessage::mt_error,
      _T("Failed to load parameters from binary storage.\nDefault ones will be set on.") );
  }
  return false;
}


///------------------------------------------------------------------------------------------------
/// \brief Function copies parameter structure to a binary storage.
///
/// \param  pParameters  pointer to source parameter structure.
/// \param  pBinParams   pointer to destination binary storage of parameters.
/// \param  pOutput      pointer to the message handler or 0.
/// \return              Ok = true.
///------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool ParametersToBinary( const PARAMETERS * pParameters,
                         BinaryData       * pBinParams,
                         Elvees::IMessage * pOutput )
{
  if ((pBinParams == 0) || (pParameters == 0))
    return false;

  std::basic_stringstream<TCHAR> strm;
  std::basic_string<TCHAR>       text;
  BinaryData::const_pointer      F = 0;
  BinaryData::const_pointer      L = 0;

  pBinParams->clear();
  if (!(pParameters->save( strm )))
  {
    if (pOutput != 0)
      pOutput->Print( Elvees::IMessage::mt_error, _T("Failed to save parameters in binary storage") );
    return false;
  }

  text = strm.str();
  pBinParams->resize( (text.size()+1) * sizeof(TCHAR) );
  std::fill( pBinParams->begin(), pBinParams->end(), BinaryData::value_type(0) );
  F = reinterpret_cast<BinaryData::const_pointer>( text.c_str() );
  L = F + (int)(text.size() * sizeof(TCHAR));
  std::copy( F, L, pBinParams->begin() );
  return true;
}

} // namespace Elvees

#endif // __DEFINE_DVR_PARAMETER_FUNCTIONS_H__

