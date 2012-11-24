#define MAPSIZEDLG_CPP
/*************************************************************************************************\
mapsizeDlg.cpp		: Implementation of the mapsizeDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "mclib.h"
#include "mapsizedlg.h"

#include "resource.h"
#include "mclibresource.h"
#include "utilities.h"
#include "terrtxm2.h"

extern DWORD gameResourceHandle;		//Default handle must be used for mc2res.dll due to shared game/editor code

//----------------------------------------------------------------------
void MapSizeDlg::Init()
{
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_MAPSIZE );

	int index = pListBox->AddString( "60x60" );
	pListBox->SetItemData( index, 0 );

	index = pListBox->AddString( "80x80" );
	pListBox->SetItemData( index, 1 );

	index = pListBox->AddString( "100x100" );
	pListBox->SetItemData( index, 2 );

	index = pListBox->AddString( "120x120" );
	pListBox->SetItemData( index, 3 );
	//magic 21022011 begin
	index = pListBox->AddString( "140x140" );
	pListBox->SetItemData( index, 4 );

	index = pListBox->AddString( "160x160" );
	pListBox->SetItemData( index, 5 );
	//magic 23022011 begin
	index = pListBox->AddString( "180x180" );
	pListBox->SetItemData( index, 6 );

	index = pListBox->AddString( "200x200" );
	pListBox->SetItemData( index, 7 );
	//magic 23022011 end

	//magic 24032012 begin
	index = pListBox->AddString( "220x220" );
	pListBox->SetItemData( index, 8 );

	index = pListBox->AddString( "240x240" );
	pListBox->SetItemData( index, 9 );

	index = pListBox->AddString( "260x260" );
	pListBox->SetItemData( index, 10 );

	index = pListBox->AddString( "280x280" );
	pListBox->SetItemData( index, 11 );
	//magic 24032012 end
	//magic 21022011 end
	pListBox->SetCurSel( mapSize );
}

//----------------------------------------------------------------------
void MapSizeDlg::OnOK()
{
	int index = ((CListBox*)GetDlgItem( IDC_MAPSIZE ))->GetCurSel( );
	mapSize = ((CListBox*)GetDlgItem( IDC_MAPSIZE ))->GetItemData( index );

	CDialog::OnOK();
}

//----------------------------------------------------------------------
