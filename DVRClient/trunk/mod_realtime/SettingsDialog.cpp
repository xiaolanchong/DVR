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

#include "stdafx.h"
#include "SettingsDialog.h"

//======================================================================================//
//                                 class SettingsDialog                                 //
//======================================================================================//

SettingsDialog::SettingsDialog(
							   DVREngine::IEngineSettingsReader* pParamReader,
							   DVREngine::IEngineSettingsWriter* pParamWriter
							   ):
	m_pParamReader( pParamReader ),
	m_pParamWriter( pParamWriter ) 
{

}

SettingsDialog::~SettingsDialog()
{
}

BEGIN_EVENT_TABLE(SettingsDialog, wxDialog) 
	EVT_BUTTON( wxID_OK,  SettingsDialog::OnOK )
END_EVENT_TABLE() 

void	SettingsDialog::Init()
{
	m_pAlgoSensitivity	= XRCCTRL( *this, wxT("Slider_AlgorithmSensitivity"), wxSlider);
	m_pFreeDiskSpace	= XRCCTRL( *this, wxT("Spin_FreeDiskSpace"), wxSpinCtrl);
	m_pStoreDays		= XRCCTRL( *this, wxT("Spin_StoreDays"), wxSpinCtrl);

	m_pWriteArchiveNever = XRCCTRL( *this, wxT( "RadioButton_Never"), wxRadioButton ) ;
	m_pWriteArchiveMotion = XRCCTRL( *this, wxT( "RadioButton_Motion"), wxRadioButton ) ;
	m_pWriteArchiveAlways = XRCCTRL( *this, wxT( "RadioButton_Always"), wxRadioButton ) ;

	//pParamReader->GetAlgorithmSensitivity();
	try
	{
		m_pAlgoSensitivity->SetValue( m_pParamReader->GetAlgorithmSensitivity() );
	}
	catch ( std::exception& ex) 
	{
		TraceDebug( ex.what() );
	}

	try
	{
		DVREngine::ArchiveStorageParam param;
		m_pParamReader->GetArchiveStorageParameters( param );
		m_pFreeDiskSpace->SetValue( param.FreePercentageDiskSpace );
		m_pStoreDays->SetValue( param.StorageDayPeriod );
	}
	catch ( std::exception& ex) 
	{
		TraceDebug( ex.what() );
	}

	try
	{
		switch( m_pParamReader->GetArchiveWriteCondition() )  
		{
		case  DVREngine::Never :
				m_pWriteArchiveNever->SetValue(true);
				break;
		case  DVREngine::OnlyWhenMotion:
				m_pWriteArchiveMotion->SetValue(true);
				break;
		case  DVREngine::Always	:
				m_pWriteArchiveAlways->SetValue( true );
				break;
		default:
			_ASSERTE(false);
		}
		
	}
	catch (std::exception& ex) 
	{
		m_pWriteArchiveAlways->SetValue(true);
		TraceDebug( ex.what() );
	}
}

void	SettingsDialog::OnOK(wxCommandEvent& event)
{
	try
	{
		m_pParamWriter->SetAlgorithmSensitivity( m_pAlgoSensitivity->GetValue() );
	}
	catch ( std::exception& ex ) 
	{
		TraceDebug( ex.what() );
	}

	DVREngine::ArchiveStorageParam param;

	param.FreePercentageDiskSpace	= m_pFreeDiskSpace->GetValue( );
	param.StorageDayPeriod			= m_pStoreDays->GetValue( );

	try
	{
		m_pParamWriter->SetArchiveStorageParameters( param );
	}
	catch ( std::exception& ex ) 
	{
		TraceDebug( ex.what() );
	}

	DVREngine::ArchiveWriteCondition cnd = DVREngine::Always;
	if( m_pWriteArchiveNever->GetValue() )
	{
		cnd = DVREngine::Never;
	}
	else if( m_pWriteArchiveMotion->GetValue() )
	{
		cnd = DVREngine::OnlyWhenMotion;
	}
	else if( m_pWriteArchiveAlways->GetValue() )
	{
		cnd = DVREngine::Always;
	}
	else
	{
		_ASSERTE(false);
	}

	try
	{
		m_pParamWriter->SetArchiveWriteCondition( cnd );
	}
	catch (std::exception& ex) 
	{
		TraceDebug( ex.what() );
	}

	wxDialog::OnOK(event);
}