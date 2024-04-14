//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     CameraStateControl.hpp		
//!	\date     2006-03-09		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    Camera State widget

#include "StdAfx.h"
#include "CameraStateControl.hpp"

//////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CameraStateControl, wxPanel )
	EVT_PAINT( CameraStateControl::OnPaint )
	EVT_ERASE_BACKGROUND( CameraStateControl::OnEraseBackground )
	EVT_SIZE( CameraStateControl::OnSize )
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////////
CameraStateControl::CameraStateControl(  wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size ):
	wxPanel( parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE  ),
	mBgColor( 128,128,128 ),
	mFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) 

{
	//Default values 
	mParamMap[PM_CamButtonWidth] = 50;


	//Left blank	
	DebugInit();
}

CameraStateControl::~CameraStateControl()
{
	//Left blank
}
//////////////////////////////////////////////////////////////////////////
void CameraStateControl::DebugInit()
{
	SetGrid( 10,4 );	
	SetColumnColor( 0, wxColor(255,0,0) );
	SetColumnColor( 1, wxColor(0,255,0) );

	SetCheck(0,0);
	SetCheck(1,0);
	SetCheck(1,1);
}

void CameraStateControl::SetGrid( int rows, int cols )
{
	assert( (rows >= 0) && (cols >= 0 ) && "Invalid value. Must be between 1 and 100" );
	assert( (rows < 100) && (cols < 100 ) && "Invalid value. Must be between 1 and 100 " );

	//Resize grid
	mRows.resize( rows );
	mCols.resize( cols );

	mGrid.resize( rows );
	for( size_t i = 0; i < mGrid.size(); ++i )
		mGrid[i].resize( cols );

	//Invoke resizing event
	Refresh( false );
}

void CameraStateControl::GetGrid( int& rows, int& cols ) const
{
	rows = static_cast<int>( mRows.size() );
	cols = static_cast<int>( mCols.size() );
}

//////////////////////////////////////////////////////////////////////////
void CameraStateControl::SetColumnColor( int col, const wxColor& color )
{
	if( col < static_cast<int>(mCols.size()) )
		mCols[col] = color;	
	Refresh( false );
}

void CameraStateControl::GetColumnColor( int col, wxColor& color ) const
{
	if( col < static_cast<int>(mCols.size()) )
		color = mCols[col];
}

void CameraStateControl::SetBgColor( const wxColor& color )
{
	mBgColor = color;
	Refresh( false );
}

void CameraStateControl::GetBgColor( wxColor& color )
{
	color = mBgColor;
}
//////////////////////////////////////////////////////////////////////////
void CameraStateControl::SetCheck( int row, int col, bool enable  )
{
	assert( ( row >= 0 ) && ( row < (int)mRows.size() ) );
	assert( ( col >= 0 ) && ( col < (int)mCols.size() ) );
	mGrid[row][col] = enable;	
	Refresh( false );
}

bool CameraStateControl::GetCheck( int row, int col ) const
{
	assert( ( row >= 0 ) && ( row < (int)mRows.size() ) );
	assert( ( col >= 0 ) && ( col < (int)mCols.size() ) );
	return mGrid[row][col];
}
//////////////////////////////////////////////////////////////////////////
void CameraStateControl::OnPaint( wxPaintEvent& event )
{
	int x0,x1,y0,y1;
	wxBufferedPaintDC dc(this);

	wxSize size = GetClientSize();

	dc.BeginDrawing();

	wxBrush defBrush( mBgColor );
	dc.SetBackgroundMode( wxTRANSPARENT );
	dc.SetBackground( defBrush );

	//Clear context
	dc.Clear();

	//Draw camera buttons line
	dc.DrawLine( mParamMap[PM_CamButtonWidth], 0, mParamMap[PM_CamButtonWidth], size.GetHeight() );

	dc.SetFont( mFont );
	wxCoord heightCoord = GetCharHeight();
	int vOffsetText = ( mParamMap[PM_RowHeight] - heightCoord ) / 2;

	//Draw rows
	x0 = 0;
	x1 = size.GetWidth();
	y0 = y1 = mParamMap[PM_RowHeight];
	int rowHeight = mParamMap[PM_RowHeight];

	for( size_t row = 0; row < mRows.size(); ++row )
	{
		std::string text = boost::str( boost::format( "Cam%02d" ) % row );
		dc.DrawText( text.c_str(), 5, y0 + vOffsetText - rowHeight );
		dc.DrawLine( x0,y0,x1,y1 );
		y0 += rowHeight;
		y1 += rowHeight;
	}

	//Draw cols
	x0 = x1 = mParamMap[PM_CamButtonWidth];
	y0 = 0;
	y1 = size.GetHeight();
	int colWidth = mParamMap[PM_ColWidth];

	for( size_t col = 0; col < mCols.size(); ++col )
	{
		dc.DrawLine( x0,y0,x1,y1 );
		x0 += colWidth;
		x1 += colWidth;
	}

	//Fill grid cells
	y0 = 0;
	for( size_t row = 0; row < mRows.size(); ++row )
	{
		x0 = mParamMap[PM_CamButtonWidth]; 
		for( size_t col = 0; col < mCols.size(); ++col )
		{
			//If cell is checked 
			if( mGrid[row][col] )
			{
				dc.SetBrush( wxBrush( mCols[col] ) );
			}
			else
			{
				dc.SetBrush( defBrush );
			}

			dc.DrawRectangle( x0, y0, colWidth, rowHeight );
			x0 += colWidth;
		}
		y0 += rowHeight;
	}

	dc.EndDrawing();

}

void CameraStateControl::OnEraseBackground( wxEraseEvent& event )
{
	//Keep it blank to avoid flickering
}

void CameraStateControl::OnSize( wxSizeEvent& event )
{
	wxSize clientSize = event.GetSize();
	
	int colsNum = static_cast<int>( mCols.size() );
	int rowsNum = static_cast<int>( mRows.size() );

	//Columns
	if( colsNum > 0 )
		mParamMap[PM_ColWidth] = ( clientSize.GetWidth() - mParamMap[PM_CamButtonWidth] ) / colsNum;
	else
		mParamMap[PM_ColWidth] = 0;

	//Rows
	if( rowsNum > 0)
		mParamMap[PM_RowHeight] = clientSize.GetHeight() / rowsNum;
	else 
		mParamMap[PM_RowHeight] = 0;


}
//////////////////////////////////////////////////////////////////////////

