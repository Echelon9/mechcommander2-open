#define COMPONENTPURCHASESCREEN_CPP
/*************************************************************************************************\
ComponentPurchase.cpp			: Implementation of the ComponentPurchase component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "ComponentPurchaseScreen.h"
#include "IniFile.h"
#include "LogisticsData.h"
#include "MechBayScreen.h"
#include "cmponent.H"
#include "windows.h"
#include "..\resource.h"
#include "LogisticsDialog.h"
#include <malloc.h>
#include "gamesound.h"
#include "ChatWindow.h"
#include "Multplyr.h"
extern bool useUnlimitedAmmo;
#include "Prefs.h"

ComponentPurchaseScreen* ComponentPurchaseScreen::s_instance = 0;


ComponentPurchaseScreen::ComponentPurchaseScreen(  ) 
{
	status = RUNNING;

	helpTextArrayID = 14;
	//pCurComponent = 0;
	gosASSERT( !s_instance );
	s_instance = this;
	//pDragComponent = NULL;
	pSelectedComponent = NULL;
	selI = -1;
	selJ = -1;
	dragX = -1;
	dragY = -1;

	countDownTime = .5;
	curCount = 0.0;
	previousAmount = 0;

	oldCBillsAmount  =0;

	oldHeat = 0;
	heatTime = 0;

	oldArmor = 0;
	newArmor = 0;
	armorTime = 0;
	bDragLeft = 0;
	
	selPRect = NULL;


}
ComponentPurchaseScreen::~ComponentPurchaseScreen()
{
	s_instance = NULL;
	purcomponentListBox.destroy();
	invcomponentListBox.destroy();
}
int ComponentPurchaseScreen::init( FitIniFile& file )
{
	ComponentPurItem::init( file );
	LogisticsScreen::init( file, "Static", "Text", "Rect", "Button" );
	purcomponentListBox.init( rects[2].left(), rects[2].top(),
							rects[2].width(), rects[2].height() );

	purcomponentListBox.setHighlightFX( -1 ); // want to do this by hand.

	invcomponentListBox.init( rects[0].left(), rects[0].top(),
							rects[0].width(), rects[0].height() );

	invcomponentListBox.setHighlightFX( -1 ); // want to do this by hand.

	char blockName[64];
	for ( int i = 0; i < COMPUR_ATTRIBUTE_METER_COUNT; i++ )
	{
		sprintf( blockName, "AttributeMeter%ld", i );
		attributeMeters[i].init( &file, blockName );
	}

	for ( i= 0; i < buttonCount; i++ )
	{
		switch( buttons[i].getID() )
		{
			case	COMPONENT_FORM_WEAPON_ENERGY:
			case	COMPONENT_FORM_WEAPON_BALLISTIC:
			case	COMPONENT_FORM_WEAPON_MISSILE:
			case	COMPONENT_FORM_JUMPJET:
				break;

			default:
				{
				buttons[i].setMessageOnRelease();
				buttons[i].setPressFX(LOG_CLICKONBUTTON );
				}

				
		}	
	}
	camera.init( rects[4].left(), rects[4].top(), rects[4].right(), rects[4].bottom() );

	getButton( COMPONENT_FORM_WEAPON_ENERGY )->setPressFX( LOG_VIDEOBUTTONS );
	getButton( COMPONENT_FORM_WEAPON_BALLISTIC )->setPressFX( LOG_VIDEOBUTTONS );
	getButton( COMPONENT_FORM_WEAPON_MISSILE )->setPressFX( LOG_VIDEOBUTTONS );
	getButton( COMPONENT_FORM_JUMPJET )->setPressFX( LOG_VIDEOBUTTONS );

	getButton( COMPONENT_FORM_WEAPON_ENERGY )->setHighlightFX( LOG_DIGITALHIGHLIGHT );
	getButton( COMPONENT_FORM_WEAPON_BALLISTIC )->setHighlightFX( LOG_DIGITALHIGHLIGHT );
	getButton( COMPONENT_FORM_WEAPON_MISSILE )->setHighlightFX( LOG_DIGITALHIGHLIGHT );
	getButton( COMPONENT_FORM_JUMPJET )->setHighlightFX( LOG_DIGITALHIGHLIGHT );
/*//M20
	// initialize the selection rectangles
	char fileName[256];
	FullPathFileName path;
	for ( i = 0; i < 5; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			sprintf( fileName, "mcl_mc_%ldby%ld_selection.tga", j+1, i+1 );
			path.init( artPath, fileName, ".tga" );

			if ( fileExists( path ) )
			{
				selPRects[j][i].setTexture( path );
				selPRects[j][i].resize( (j+1) * LogisticsComponent::XICON_FACTOR, (i+1) * LogisticsComponent::YICON_FACTOR );
				// now need to set the UV's
				selPRects[j][i].setUVs( 0, 0, selPRects[j][i].width(), selPRects[j][i].height()  );
			}

		}
	}
	
	path.init( artPath, "mcl_mc_jumpjets_selection", ".tga" );
	selPJumpJetRect.setTexture( path );
	selPJumpJetRect.resize( 2 * LogisticsComponent::XICON_FACTOR, 2 * LogisticsComponent::YICON_FACTOR );
	// now need to set the UV's
	selPJumpJetRect.setUVs( 0, 0, selPJumpJetRect.width(), selPJumpJetRect.height()  );
//M20*/
	//M14
	//purcomponentListBox.initPurComponents(); //not here
	//invcomponentListBox.initComponents(); //not here
	//inPurchaseList = true;
	//M14


	return true;
}

void ComponentPurchaseScreen::begin()
{
	purcomponentCount = 0;
	invcomponentCount = 0;//M19
	bSaveDlg = 0;
	status = RUNNING;
	bDragLeft= 0;
	bErrorDlg = 0;
	dragX = -1;
	dragY = -1;
	oldCBillsAmount = LogisticsData::instance->getCBills();
	//Magic 72 begin
		purcomponentListBox.removeAllItems( 1 ); 
		invcomponentListBox.removeAllItems( 1 );
	//Magic 72 end
	
	//M14
	purcomponentListBox.initPurComponents();
	invcomponentListBox.initComponents();
	//M14

		for ( int i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++ )
		{
			aButton* pButton = getButton( i );
			if ( pButton )
			{
				pButton->press( 0 );
			}
		}
		purcomponentListBox.removeAllItems( 0 ); 
		invcomponentListBox.removeAllItems( 0 );  //M19

		//purcomponentListBox.setType( COMPONENT_FORM_WEAPON_BALLISTIC, -1, -1 );
		//invcomponentListBox.setType( COMPONENT_FORM_WEAPON_BALLISTIC, -1, -1 );//M19
		//magic 08022012 begin
		purcomponentListBox.setType2( COMPONENT_FORM_WEAPON_BALLISTIC, -1, -1, -1, -1, -1 );
		invcomponentListBox.setType2( COMPONENT_FORM_WEAPON_BALLISTIC, -1, -1, -1, -1, -1  );
		//magic 08022012
		getButton( COMPONENT_FORM_WEAPON_BALLISTIC )->press( true );

		//oldCBillsAmount = 0;
	

	// init CBills
	char text[32];
	sprintf( text, "%ld ", LogisticsData::instance->getCBills() );
	textObjects[1].setText( text );

	textObjects[0].setText( "BUY/SELL COMPONENTS" );
	textObjects[3].setText( "INVENTORY" );
	// no changes, set cost to 0
	textObjects[4].setText( " " );//Magic 65
	textObjects[5].setText( " " );
	textObjects[11].setText( " " );
	textObjects[12].setText( " " );
/*
	int newComp = purcomponentListBox.selectFirstPurComponent();
	pPurSelectedComponent = ((ComponentPurItem*)purcomponentListBox.GetItem( newComp ))->getComponent();
	setComponent(pPurSelectedComponent);
	pSelectedComponent = pPurSelectedComponent;
	inPurchaseList = true;
	pInvSelectedComponent = invcomponentListBox.getComponent();
	purFirst = false;
	invFirst = false;
*/	
	pSelectedComponent = purcomponentListBox.getComponent();
	attributeMeters[5].showGUIWindow( 0 );
	attributeMeters[6].showGUIWindow( 0 );
}
void ComponentPurchaseScreen::end()
{
	camera.setComponent( NULL );
	//pVariant = NULL;
}

void ComponentPurchaseScreen::update()
{
	//Magic 73
	if ( status != RUNNING ) // can change after button updates
		return;
	//Magic 73

	int oldPurSel, oldInvSel;
	int newPurSel, newInvSel;
	long mouseX = userInput->getMouseX();
	long mouseY = userInput->getMouseY();

	if ( rects[0].pointInside( mouseX, mouseY ) )
	{
		invFirst = true;
	}
	else
	{
		invFirst = false;
	}

	if ( rects[2].pointInside( mouseX, mouseY ) )
	{
		purFirst = true;
	}
	else
	{
		purFirst = false;
	}

	//M23
	//pInvSelectedComponent = invcomponentListBox.getComponent();
	//pPurSelectedComponent = purcomponentListBox.getComponent();


	//M23
/*	if ( status != RUNNING ) // can change after button updates
		return;*/ //Magic 73 prebaceno na pocetak

	oldPurSel = purcomponentListBox.GetSelectedItem(); // need to check this before we update the list box
	oldInvSel = invcomponentListBox.GetSelectedItem(); // M19

	for ( int i = 0; i < COMPUR_ATTRIBUTE_METER_COUNT; i++ )
		attributeMeters[i].update();


	camera.update();

	LogisticsScreen::update();
	//int oldSel = purcomponentListBox.GetSelectedItem(); // need to check this before we update the list box
	//oldPurSel = purcomponentListBox.GetSelectedItem(); // M19
	purcomponentListBox.update(); //M20

	if (purcomponentListBox.GetItemCount() == 0) 
		getButton( MB_MSG_ADD )->disable( 1 );
	else
	{ 

		newPurSel = purcomponentListBox.GetSelectedItem(); // M20
		if ( oldPurSel != newPurSel)
		{
			if ( newPurSel != -1 )
			{
				pSelectedComponent = ((ComponentPurItem*)purcomponentListBox.GetItem( newPurSel ))->getComponent();
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 0 );
			}
			else
			{
				pSelectedComponent = purcomponentListBox.getComponent();
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
		else if (purFirst && (newPurSel != -1))
		{
			pSelectedComponent = ((ComponentPurItem*)purcomponentListBox.GetItem( newPurSel ))->getComponent();
			getButton( MB_MSG_REMOVE )->disable( 1 );
			getButton( MB_MSG_ADD )->disable( 0 );
		}
	} 
	
	invcomponentListBox.update(); //M20

	if (invcomponentListBox.GetItemCount() == 0)
		getButton( MB_MSG_REMOVE )->disable( 1 );
	else
	{ 
		newInvSel = invcomponentListBox.GetSelectedItem(); // M20
		if ( oldInvSel != newInvSel)
		{
			if (newInvSel != -1 )
			{
				pSelectedComponent = ((ComponentPurItem*)invcomponentListBox.GetItem( newInvSel ))->getComponent();
				getButton( MB_MSG_REMOVE )->disable( 0 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
			else
			{
				pSelectedComponent = invcomponentListBox.getComponent();
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
		else if (invFirst && (newInvSel != -1))
		{
			pSelectedComponent = ((ComponentPurItem*)invcomponentListBox.GetItem( newInvSel ))->getComponent();
			getButton( MB_MSG_REMOVE )->disable( 0 );
			getButton( MB_MSG_ADD )->disable( 1 );
		}
	}

	if (pSelectedComponent)
	{//Magic 72
		setComponent(pSelectedComponent);
		textObjects[14].setText( pSelectedComponent->getHelpID() ); //Magic 72
	}//Magic 72

	//Magic 30120210 begin
	if (( userInput->isLeftDoubleClick() && purFirst && (purcomponentListBox.GetItemCount() != 0)) && (pSelectedComponent))
	{
		handleMessage( 111, MB_MSG_ADD);
	}
	if (( userInput->isLeftDoubleClick() && invFirst && (invcomponentListBox.GetItemCount() != 0)) && (pSelectedComponent))
	{
		handleMessage( 111, MB_MSG_REMOVE);
	}
	//Magic 30122010 end

	/*if ( ::helpTextID )
	{
		textObjects[helpTextArrayID].setText( helpTextID );

	}*/
	
	char text[32];
	sprintf( text, "%ld ", LogisticsData::instance->getCBills() );
	textObjects[1].setText( text );
	
	purcomponentListBox.disableCThatCostMoreThanRP();

}

void ComponentPurchaseScreen::render(int xOffset, int yOffset)
{
	purcomponentListBox.move( xOffset, yOffset );
	purcomponentListBox.render();
	purcomponentListBox.move( -xOffset, -yOffset );

	invcomponentListBox.move( xOffset, yOffset );
	invcomponentListBox.render();
	invcomponentListBox.move( -xOffset, -yOffset );

	for ( int i = 0; i < COMPUR_ATTRIBUTE_METER_COUNT; i++ )
		attributeMeters[i].render(xOffset, yOffset);

	if ( !xOffset && !yOffset )
		camera.render();

	LogisticsScreen::render(xOffset, yOffset );

	for ( i = 0; i < purcomponentCount; i++ )
		purcomponentIcons[i].render(xOffset, yOffset);

	for ( i = 0; i < invcomponentCount; i++ ) //M19
		invcomponentIcons[i].render(xOffset, yOffset);//M19

/*	if ( pDragComponent )
	{
		dragIcon.moveTo( userInput->getMouseX() - dragIcon.width() / 2, 
			userInput->getMouseY() - dragIcon.height() / 2 );
		dragIcon.render();

		selPRect->render();	
		
	}
*/
}

int	ComponentPurchaseScreen::handleMessage( unsigned long msg, unsigned long who)
{
	int i;

	if ( msg == aMSG_SELCHANGED )
	{
		return 1;
	}
	switch( who )
	{
		case MB_MSG_NEXT: // actuall accept
			//{
				invcomponentListBox.saveInv( true );//M9
				invcomponentListBox.reType();
				purcomponentListBox.savePurInv( true );//M19
				purcomponentListBox.reType();
				status = MECHBAY;
			//}
			break;
		case MB_MSG_PREV: // actually cancel
			invcomponentListBox.saveInv( false );//M9
			purcomponentListBox.savePurInv( false );//M19
			//Magic 56 begin
			previousAmount = LogisticsData::instance->getCBills();
			LogisticsData::instance->addCBills( (oldCBillsAmount - previousAmount) );
			//Magic 56 end
			status = MECHBAY;
			break;

		case	COMPONENT_FORM_WEAPON_ENERGY:
		case	COMPONENT_FORM_WEAPON_BALLISTIC:
		case	COMPONENT_FORM_WEAPON_MISSILE:
			for ( i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++ )
			{
				aButton* pButton = getButton( i );
				if ( pButton )
				{
					pButton->press( 0 );
				}
			}
			getButton( who )->press( 1 );
			//purcomponentListBox.setType( who, -1, -1); 
			//invcomponentListBox.setType( who, -1, -1); //M19
			//magic 08022012 begin
			purcomponentListBox.setType2( who, -1, -1, -1, -1, -1); 
			invcomponentListBox.setType2( who, -1, -1, -1, -1, -1); //M19
			//magic 08022012 end
			
			break;
		case	COMPONENT_FORM_JUMPJET:
			for ( i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++ )
			{
				aButton* pButton = getButton( i );
				if ( pButton )
				{
					pButton->press( 0 );
				}
			}
			getButton( who )->press( 1 );
			//purcomponentListBox.setType( COMPONENT_FORM_JUMPJET, COMPONENT_FORM_BULK, COMPONENT_FORM_HEATSINK );
			//invcomponentListBox.setType( COMPONENT_FORM_JUMPJET, COMPONENT_FORM_BULK, COMPONENT_FORM_HEATSINK );//M19
			//magic 08022012 begin
			purcomponentListBox.setType2( COMPONENT_FORM_JUMPJET, COMPONENT_FORM_BULK, COMPONENT_FORM_HEATSINK, COMPONENT_FORM_AMMO, COMPONENT_FORM_ECM, COMPONENT_FORM_PROBE );
			invcomponentListBox.setType2( COMPONENT_FORM_JUMPJET, COMPONENT_FORM_BULK, COMPONENT_FORM_HEATSINK, COMPONENT_FORM_AMMO, COMPONENT_FORM_ECM, COMPONENT_FORM_PROBE );//M19
			//magic 08022012 end
			break;

		case MB_MSG_ADD:
			
			if (LogisticsData::instance->getCBills() > pSelectedComponent->getCost())
			{
				//magic 19092011 begin
				long oldIndex = purcomponentListBox.GetSelectedItem();
				//magic 19092011 end
				invcomponentListBox.addIComponent( pSelectedComponent ); //M19
				invcomponentListBox.reType();
				purcomponentListBox.removeIComponent( pSelectedComponent ); //M19
				purcomponentListBox.reType();
				//magic 19092011 begin
				purcomponentListBox.selectComponent(oldIndex);
				//magic 19092011 end
				LogisticsData::instance->decrementCBills(pSelectedComponent->getCost());
				pSelectedComponent = purcomponentListBox.getComponent();
/*				if (!pSelectedComponent)
				{
					getButton( MB_MSG_REMOVE )->disable( 1 );
					getButton( MB_MSG_ADD )->disable( 1 );
				}*/
			}
			else
				{				
					LogisticsOneButtonDialog::instance()->setText( IDS_MC_INSUFFICIENT_CBILLS, IDS_DIALOG_OK,
						IDS_DIALOG_OK );
					LogisticsOneButtonDialog::instance()->begin();
					bErrorDlg = true;
				}
				//message not enough cbills
			
			break;

		case MB_MSG_REMOVE:
			{
			//magic 19092011 begin
			long oldIndex = invcomponentListBox.GetSelectedItem();
			//magic 19092011 end
			invcomponentListBox.removeIComponent( pSelectedComponent ); //M19
			invcomponentListBox.reType();
			purcomponentListBox.addIComponent( pSelectedComponent ); //M19
			purcomponentListBox.reType();
			//magic 19092011 begin
			invcomponentListBox.selectComponent(oldIndex);
			//magic 19092011 end
			LogisticsData::instance->addCBills(pSelectedComponent->getCost());
			pSelectedComponent = invcomponentListBox.getComponent();
			}
/*			if (!pSelectedComponent)
			{
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}*/
			break;

		case MB_MSG_MAINMENU:
			status = MAINMENU;
			break;
	
	}

	return 0;
}

void	ComponentPurchaseScreen::setComponent( LogisticsComponent* pComponent, bool bMessageFromLB )
{
	if ( pComponent && pComponent == pCurComponent )
		return;

	pCurComponent = pComponent;

	// ammo doesn't deserve a meter
	attributeMeters[3].showGUIWindow( 0 );

	char text[64];
	if ( pCurComponent )
	{
		//set the little spinning figure uncomment this when the art is in, if ever...
		camera.setComponent( pCurComponent->getPictureFileName() );
		textObjects[6].setText( pComponent->getName() );


		// first figure out if this is a jump jet or not
		if ( pCurComponent->getType() == COMPONENT_FORM_JUMPJET )
		{
			showJumpJetItems( 1 );

			statics[10].showGUIWindow( 0 ); //Magic 10022012 Ammo icon
			// set up jump range
			attributeMeters[4].setValue((pCurComponent->getDamage() * 25.0f) / MAX_JUMP_RANGE ); //MAGIC read damage value from CSV, its range for JJ
			
			//attributeMeters[4].setValue(((float)pVariant->getMaxJumpRange() * 25.0f) / MAX_JUMP_RANGE ); //MAGIC removed with line above
			
			
				
			// sprintf( text, "%ld", pVariant->getMaxJumpRange() * 25); //Magic disabled
			//sprintf( text, "%ld", pCurComponent->getDamage()); //Magic added
			sprintf( text, "%.1lf", pCurComponent->getDamage()); //magic 29112011
			textObjects[7].setText( text );
			textObjects[7].setColor( 0xff005392 );
			rects[9].setHelpID( IDS_HELP_PM_JUMP );

	
			// set up heat
			attributeMeters[7].setValue(pCurComponent->getHeat()/LogisticsComponent::MAX_HEAT);
			attributeMeters[7].showGUIWindow( true );
			
			sprintf( text, "%.1lf", pCurComponent->getHeat() );
			textObjects[15].setText( text );
			
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[13].setText( "" );
			rects[10].setHelpID( IDS_HELP_MC_HEAT_GENERATED );


			attributeMeters[0].showGUIWindow( false );

			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );

		}
		else if ( pCurComponent->getType() == COMPONENT_FORM_HEATSINK )
		{
			showJumpJetItems( 1 );

			// hide jump range stuff
			attributeMeters[4].showGUIWindow( 0 );
			textObjects[7].showGUIWindow( 0 );
			statics[5].showGUIWindow( 0 );

			textObjects[15].setText( "" );

			// jump range - JJ heat
			statics[6].showGUIWindow( 0 );
			// armor
			statics[7].showGUIWindow( 0 );
			// damage
			statics[1].showGUIWindow( 0 );
			//Magic 65 begin
			// recycle rate
			statics[2].showGUIWindow( 0 );
			//Magic 65 end
			// heat
			statics[8].showGUIWindow( 1 );	

			statics[10].showGUIWindow( 0 ); //Magic 10022012 Ammo icon

			attributeMeters[7].showGUIWindow( 0 );
	
			// set up heat, negative heat is stored in the damage area of heat sinks, god knows why
			attributeMeters[8].setValue(pCurComponent->getDamage()/LogisticsComponent::MAX_HEAT);
			attributeMeters[8].showGUIWindow( true );
			
			sprintf( text, "+%.1lf", pCurComponent->getDamage() );
			textObjects[16].setText( text );
			textObjects[16].showGUIWindow( true );
			
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[13].setText( "" );

			attributeMeters[0].showGUIWindow( false );

			rects[9].setHelpID( IDS_HELP_MC_HEAT_GENERATED );
			rects[10].setHelpID( 0 );
			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );
		}
		else if ( pCurComponent->getType() == COMPONENT_FORM_BULK )
		{	
			showJumpJetItems( 1 );
			
			// jump range
			statics[6].showGUIWindow( 0 );
			// armor
			statics[7].showGUIWindow( 1 );
			// damage
			statics[1].showGUIWindow( 0 );
			//Magic 65 begin
			// recycle rate
			statics[2].showGUIWindow( 0 );
			//Magic 65 end
			statics[10].showGUIWindow( 0 ); //Magic 10022012 Ammo icon
			
			sprintf( text, "%.1lf", pCurComponent->getDamage() );
			textObjects[7].setText( text );
			textObjects[7].setColor( 0xff005392 );
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[15].setText( "" );

			textObjects[13].setText( "" );
			attributeMeters[7].showGUIWindow( false );

			for( int i = 0; i < 4; i++ )
			{
				attributeMeters[i].showGUIWindow( 0 );
			}

			rects[9].setHelpID( IDS_HELP_MC_ARMOR );
			rects[10].setHelpID( 0 );
			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );

		}
		//magic 10022012 begin
		else if ( pCurComponent->getType() == COMPONENT_FORM_AMMO )
		{	
			showJumpJetItems( 1 );
			
			// jump range
			statics[6].showGUIWindow( 0 );
			// armor
			statics[7].showGUIWindow( 0 );
			// damage
			statics[1].showGUIWindow( 0 );
			//Magic 65 begin
			// recycle rate
			statics[2].showGUIWindow( 0 );
			//Magic 65 end
			statics[10].showGUIWindow( 1 ); //Magic 10022012
			
			sprintf( text, "%.1lf", pCurComponent->getRecycleTime() ); //recycletime is num ammo
			textObjects[7].setText( text );
			textObjects[7].setColor( 0xff005392 );
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[15].setText( "" );

			textObjects[13].setText( "" );
			attributeMeters[7].showGUIWindow( false );

			for( int i = 0; i < 4; i++ )
			{
				attributeMeters[i].showGUIWindow( 0 );
			}
			
			rects[9].setHelpID( IDS_HELP_MC_AMMO );
			rects[10].setHelpID( 0 );
			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );

		}
		//magic 10022012 end
		//magic 10022012 begin
		else if ( pCurComponent->getType() == COMPONENT_FORM_PROBE )
		{	
			showJumpJetItems( 1 );
			
			// jump range
			statics[6].showGUIWindow( 0 );
			// armor
			statics[7].showGUIWindow( 0 );
			// damage
			statics[1].showGUIWindow( 0 );
			//Magic 65 begin
			// recycle rate
			statics[2].showGUIWindow( 0 );
			//Magic 65 end
			statics[10].showGUIWindow( 0 ); //Magic 10022012 ammo
			statics[5].showGUIWindow( 0 ); //Magic 10022012 jump

			statics[0].showGUIWindow( 1 ); //Magic 10022012 range
			
			sprintf( text, "%.1lf", pCurComponent->getDamage() ); //damage is probe effect
			textObjects[7].setText( text );
			textObjects[7].setColor( 0xff005392 );
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[15].setText( "" );

			textObjects[13].setText( "" );
			attributeMeters[7].showGUIWindow( false );

			for( int i = 0; i < 4; i++ )
			{
				attributeMeters[i].showGUIWindow( 0 );
			}
			
			rects[9].setHelpID( 0 );
			rects[10].setHelpID( 0 );
			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );

		}
		//magic 10022012 end
		//magic 10022012 begin
		else if ( pCurComponent->getType() == COMPONENT_FORM_ECM )
		{	
			showJumpJetItems( 1 );
			
			// jump range
			statics[6].showGUIWindow( 0 );
			// armor
			statics[7].showGUIWindow( 0 );
			// damage
			statics[1].showGUIWindow( 0 );
			//Magic 65 begin
			// recycle rate
			statics[2].showGUIWindow( 0 );
			//Magic 65 end
			statics[10].showGUIWindow( 0 ); //Magic 10022012 ammo
			statics[5].showGUIWindow( 0 ); //Magic 10022012 jump

			statics[0].showGUIWindow( 1 ); //Magic 10022012 range
			
			sprintf( text, "%.1lf", pCurComponent->getDamage() ); //damage is probe effect
			textObjects[7].setText( text );
			textObjects[7].setColor( 0xff005392 );
			textObjects[8].setText( "" );
			textObjects[9].setText( "" );
			textObjects[10].setText( "" );
			textObjects[15].setText( "" );

			textObjects[13].setText( "" );
			attributeMeters[7].showGUIWindow( false );

			for( int i = 0; i < 4; i++ )
			{
				attributeMeters[i].showGUIWindow( 0 );
			}
			
			rects[9].setHelpID( 0 );
			rects[10].setHelpID( 0 );
			rects[11].setHelpID( 0 );
			rects[12].setHelpID( 0 );
			rects[13].setHelpID( 0 );

		}
		//magic 10022012 end
		else
		{
			long rangeColors[3] = {0xff6e7c00, 0xff005392,0xffa21600  };

			showJumpJetItems( 0 );
			//textObjects[7].setText( pCurComponent->getRangeType() + IDS_SHORT_RANGE );
			//magic 11022012 begin
			sprintf( text, "%ld - %ld", pCurComponent->getMinWRange(), pCurComponent->getMaxWRange() );
			textObjects[7].setText( text );
			//magic 11022012 end
			textObjects[7].setColor( rangeColors[pCurComponent->getRangeType()] );


			attributeMeters[0].showGUIWindow( true );
			attributeMeters[0].setValue(pCurComponent->getDamage()/LogisticsComponent::MAX_DAMAGE);
			sprintf( text, "%.1lf", pCurComponent->getDamage() );
			textObjects[8].setText( text );
			
			if ( pCurComponent->getRecycleTime() )
			{
				attributeMeters[1].setValue((10.f/pCurComponent->getRecycleTime())/LogisticsComponent::MAX_RECYCLE);
				sprintf( text, "%.1lf", 10.f/pCurComponent->getRecycleTime() );
				textObjects[9].setText( text );
			}
			else
			{
				attributeMeters[1].setValue( 0.f );
				sprintf( text, "%.1lf", 0.f );
				textObjects[9].setText( text );
			}
			

			attributeMeters[2].setValue(pCurComponent->getHeat()/LogisticsComponent::MAX_HEAT);
			sprintf( text, "%.1lf", pCurComponent->getHeat() );
			textObjects[10].setText( text );

			attributeMeters[7].showGUIWindow( false );

			if ( !useUnlimitedAmmo )
			{
				sprintf( text, "%ld", pCurComponent->getAmmo() );
				textObjects[13].setText( text );
				attributeMeters[3].setValue( ((float)pCurComponent->getAmmo())/164.f );
			}
			else
				textObjects[13].setText( "" );

			textObjects[15].setText( "" );

			rects[9].setHelpID( IDS_HELP_MC_RANGE );
			rects[10].setHelpID( IDS_HELP_MC_DAMAGE );
			rects[11].setHelpID( IDS_HELP_MC_FIRE_RATE );
			rects[12].setHelpID( IDS_HELP_MC_HEAT_GENERATED );
			if ( !prefs.useUnlimitedAmmo )
				rects[13].setHelpID( IDS_HELP_MC_AMMO );
			else
				rects[13].setHelpID( 0 );
			
		}
	}
	else
	{
		for ( int i = 7; i < 11; i++ )
		{
			textObjects[i].setText( "" );
		}

		textObjects[13].setText( "" );

		for ( i = 0; i < 5; i++ )
		{
			attributeMeters[i].setValue( 0 );
			attributeMeters[i].showGUIWindow( 0 );
		}
	}
}

void ComponentPurchaseScreen::showJumpJetItems( bool bShow )
{
		// show jump jet stuff
		statics[5].showGUIWindow( bShow );
		statics[6].showGUIWindow( bShow );
		statics[7].showGUIWindow( 0 );
		attributeMeters[4].showGUIWindow( bShow );
		attributeMeters[0].showGUIWindow( 1 );
		statics[1].showGUIWindow( !bShow ); // damage
		statics[2].showGUIWindow( !bShow ); // range //Magic 65
		statics[10].showGUIWindow( bShow ); //Magic 10022012 Ammo icon

		textObjects[7].showGUIWindow( 1 );

		// hide all heat sink stuff, regardless of who
		attributeMeters[8].showGUIWindow( 0 );
		textObjects[16].showGUIWindow( 0 );
		statics[8].showGUIWindow( 0 );
		
		// turn off weapon stuff
		statics[3].showGUIWindow( !bShow ); // heat //Magic 66
		statics[4].showGUIWindow( !bShow ); // ammo //Magic 66
		/*magic 66 for ( int i = 3; i < 7; i++ )
		{
			statics[i].showGUIWindow( !bShow );
		}*/ //Magic 66

		// but not the heat one!
		//statics[3].showGUIWindow( true ); //Magic 66 disabled was 5

		// turn off ammo thing if appropriate
		if (!useUnlimitedAmmo && !bShow)
		{
			statics[4].showGUIWindow( 1 ); //Magic 66 was 6
			attributeMeters[3].showGUIWindow( 1 );
		}
		else
		{
			statics[4].showGUIWindow( 0 );
			attributeMeters[3].showGUIWindow( 0 );
		}

		// weapon stuff
		for ( int i = 1; i < 3; i++ )
		{
			attributeMeters[i].showGUIWindow( !bShow );
		}
}

int ComponentPurchaseScreen::selectFirstLBComponent()
{
	pSelectedComponent = NULL;
	int index =  purcomponentListBox.selectFirstAvailableComponent();
	if ( index != -1 )
	{
		LogisticsComponent* pComp = purcomponentListBox.getComponent( );
		camera.setComponent( pComp->getPictureFileName() );
	}
	return index;
}

void ComponentPurchaseScreen::removeComponentListToDrag(LogisticsComponent* pComponent )
{
	//LogisticsData::instance->removeComponentFromInventory( pComponent ); //M9 OK
	purcomponentListBox.removeIComponent( pComponent );
	purcomponentListBox.reType();
//	updateDiagram();
			
}
int ComponentPurchaseScreen::addComponentListFromDrag( LogisticsComponent* pComponent )
{
	if ( !pComponent )
		return -1;

	int retVal = -1;

	if ( pComponent )
		purcomponentListBox.addIComponent( pComponent );
		purcomponentListBox.reType();
//	updateDiagram();

	return retVal;

}


void ComponentPurchaseScreen::buttonAddPressed( bool p1 )
{
	getButton( MB_MSG_ADD )->press( p1 );

}

int ComponentPurchaseScreen::selectFirstINComponent()
{
	
	pSelectedComponent = NULL;
	int index =  invcomponentListBox.selectFirstAvailableComponent();
	if ( index != -1 )
	{
		LogisticsComponent* pComp = invcomponentListBox.getComponent( );
		camera.setComponent( pComp->getPictureFileName() );
	}
	return index;
}



//*************************************************************************************************
// end of file ( ComponentPurchaseScreen.cpp )
