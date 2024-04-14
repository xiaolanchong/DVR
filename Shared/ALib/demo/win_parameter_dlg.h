///////////////////////////////////////////////////////////////////////////////////////////////////
// win_parameter_dlg.h
// ---------------------
// begin     : 1998
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_PARAMETER_LIB_DIALOG_H__
#define __DEFINE_PARAMETER_LIB_DIALOG_H__

#if (defined(_WIN32) || defined(_WIN64))

#include "../alib/alib.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class CParameterPage.
/// \brief Dialog page that contains controls of any section of parameter structure.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CParameterPage : public CPropertyPage
{
private:
  friend class CParameterDialog;
  DECLARE_DYNCREATE(CParameterPage)

  std::auto_ptr<DLGTEMPLATE>   m_pDlgTemplate;  //!< pointer to the dialog template storage
  alib::BaseParamSection * m_pSection;      //!< parameter section that is supplied by this page
  CWnd                       * m_controls;      //!< array of dialog's controls
  int                          m_nControl;      //!< number of controls
  bool                         m_bCreated;      //!< non-zero if the dialog has been created

 public:
  CParameterPage();
  virtual ~CParameterPage();
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange( CDataExchange * pDX );

 protected:
  DECLARE_MESSAGE_MAP()
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class CParameterDialog.
/// \brief Dialog that contains all pages of all parameter sections.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CParameterDialog : public CPropertySheet
{
private:
  DECLARE_DYNAMIC(CParameterDialog)
  CParameterDialog();                  // no default constructor

public:
  CParameterDialog( alib::BaseParamSection & root, CWnd * pParent, int cx, int cy );
  virtual ~CParameterDialog();

protected:
  DECLARE_MESSAGE_MAP()
};

#endif // (_WIN32 || _WIN64)

#endif // __DEFINE_PARAMETER_LIB_DIALOG_H__

