//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     CameraStateControl.hpp		
//!	\date     2006-03-09		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    This is default brief			
#ifndef __CAMERASTATECONTROL_HPP__
#define __CAMERASTATECONTROL_HPP__

class CameraStateControl : public wxPanel
{
	enum ParamMap
	{
		PM_RowHeight,
		PM_ColWidth,
		PM_HeaderWidth,
		PM_CamButtonWidth
	};

public:
	CameraStateControl( wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size );
	virtual ~CameraStateControl();
public:
	//! \function SetColumns
	//! \brief Sets columns number
	void SetGrid( int rows, int cols );

	//! \function GetColumns
	//! \brief Gets columns number
	void GetGrid( int& rows, int& cols ) const;

public:
	//! \function SetColumnColor
	//! \brief Sets column color for checked and unchecked state
	void SetColumnColor( int col, const wxColor& color );

	//! \function GetColumnColor
	//! \brief Get column color for checked and unchecked state
	void GetColumnColor( int col, wxColor& color ) const;

	//! \function SetBgColor
	//! \brief Sets background color
	void SetBgColor( const wxColor& color );

	//! \function GetBgColor
	//! \brief Gets background color
	void GetBgColor( wxColor& color );
public:
	//! \function SetCheck
	//! \brief Set check state for the specified cell
	void SetCheck( int row, int col, bool enable = true );

	//! \function GetCheck
	//! \brief Returns check state for the specified cell
	bool GetCheck( int row, int col ) const;
public:
	//! \function RemoveAll
	//!	\brief Removes all rows&cols
	void RemoveAll();

private:
	//! \function DebugInit
	//! \brief Initialize component for testing
	void DebugInit();

protected:
	//Event handlers...
	void OnPaint( wxPaintEvent& event );
	void OnEraseBackground( wxEraseEvent& event );
	void OnSize( wxSizeEvent& event );
	DECLARE_EVENT_TABLE()

private:
	std::vector<wxColor> mCols;
	std::vector<std::string> mRows;	
	std::vector< std::vector<bool> > mGrid;
	std::map<ParamMap, int > mParamMap;

	wxFont mFont;
	wxColor mBgColor;
};


#endif //__CAMERASTATECONTROL_HPP__
