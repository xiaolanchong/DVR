//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Диалог настройки параметров системы

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SETTINGS_DIALOG_4888067459899870_
#define _SETTINGS_DIALOG_4888067459899870_

//======================================================================================//
//                                 class SettingsDialog                                 //
//======================================================================================//

//! \brief Диалог настройки параметров системы
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   01.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class SettingsDialog : public wxDialog
{
	DVREngine::IEngineSettingsReader*	m_pParamReader;
	DVREngine::IEngineSettingsWriter*	m_pParamWriter;
public:
	SettingsDialog( 
			DVREngine::IEngineSettingsReader* pParamReader,
			DVREngine::IEngineSettingsWriter* pParamWriter
			);
	virtual ~SettingsDialog();

	void Init();

private:

	wxSlider*	m_pAlgoSensitivity;
	wxSpinCtrl* m_pFreeDiskSpace;
	wxSpinCtrl* m_pStoreDays;

	wxRadioButton* m_pWriteArchiveNever;
	wxRadioButton* m_pWriteArchiveMotion;
	wxRadioButton* m_pWriteArchiveAlways;

	DECLARE_EVENT_TABLE();

	void	OnOK(wxCommandEvent& event);
};

#endif // _SETTINGS_DIALOG_4888067459899870_