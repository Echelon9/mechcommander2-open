#define MECHPURCHASESCREEN_CPP
/*************************************************************************************************\
MechPurchaseScreen.cpp			: Implementation of the MechPurchaseScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "MechPurchaseScreen.h"
#include "mcLib.h"
#include "LogisticsData.h"
#include "MechBayScreen.h"
#include "gamesound.h"
#include "..\resource.h"
#include "SimpleCamera.h"
#include "AttributeMeter.h"
#include "Chatwindow.h"
#include "Multplyr.h"

MechPurchaseScreen* MechPurchaseScreen::s_instance = NULL;

#define INVENTORY_ID 88
#define VARIANT_ID	89


//-------------------------------------------------------------------------------------------------

MechPurchaseScreen::MechPurchaseScreen(  ) 
: inventoryListBox( 1, 0 ), variantListBox( 1, 0 ) //Magic 15 was variantListBox( 0, 0 )
{
	status = RUNNING;
	gosASSERT( !s_instance );
	s_instance = this;
	acceptPressed = 0;
	pDragMech = NULL;
	dragStartLeft = 0;
	helpTextArrayID = 4; 
	inventoryListBox.setID( INVENTORY_ID );
	variantListBox.setID( VARIANT_ID );

	countDownTime = .5;
	curCount = 0.0;
	previousAmount = 0;

	oldCBillsAmount  =0;

	flashTime = 0.f;
}

//-------------------------------------------------------------------------------------------------

MechPurchaseScreen::~MechPurchaseScreen()
{
	s_instance = NULL;

	variantListBox.destroy();
	inventoryListBox.destroy();
}


//-------------------------------------------------------------------------------------------------

int MechPurchaseScreen::init( FitIniFile& file )
{
	MechListBox::init();

	mechDisplay.init();

	LogisticsScreen::init( file, "Static", "Text", "Rect", "Button" );

	((aObject*)&inventoryListBox)->init( rects[0].left(), rects[0].top(),
		rects[0].width(), rects[0].height() );

	inventoryListBox.setScrollBarOrange();

	((aObject*)&variantListBox)->init( rects[1].left(), rects[1].top(),
		rects[1].width(), rects[1].height() );

	variantListBox.setScrollBarOrange();


	for ( int i= 0; i < buttonCount; i++ )
		buttons[i].setMessageOnRelease();


	return NO_ERR;
}
//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::update1()
{
	/*//Magic 72 begin
	long mouseX = userInput->getMouseX();
	long mouseY = userInput->getMouseY();
	//Magic 72 end*/
	
	if ( !MPlayer || !ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY()) )
		LogisticsScreen::update();
	
/*//Magic 72 begin
	if ( rects[0].pointInside( mouseX, mouseY ) )
	{
		invFirst = true;
	}
	else
	{
		invFirst = false;
	}

	if ( rects[1].pointInside( mouseX, mouseY ) )
	{
		purFirst = true;
	}
	else
	{
		purFirst = false;
	}
//Magic 72 end*/
	// update CBills

	int amount = LogisticsData::instance->getCBills();
	long color = 0xff005392;
	if ( amount != oldCBillsAmount )
	{
		previousAmount = oldCBillsAmount - amount;
		curCount = .00001f;
		oldCBillsAmount = amount;
		if ( previousAmount < 0 )
			soundSystem->playDigitalSample( WINDOW_OPEN );
		else
			soundSystem->playDigitalSample( WINDOW_CLOSE );
	}
	if ( curCount && curCount + frameLength < countDownTime  )
	{
		curCount += frameLength;
		float curAmount = previousAmount - (curCount/countDownTime * previousAmount); 
		amount += curAmount;

		color = 0xffc8e100;
		if ( curAmount > 0 )
			color = 0xffa21600;
	}
	else if ( flashTime )
	{
		flashTime += frameLength;
		if ( flashTime < .125
			|| ( flashTime > .25 && flashTime < .3875 )
			|| ( flashTime > .5 && flashTime < .6625 ) )
		{
			color = 0xff000000;
		}
		else if ( flashTime > .75 )
		{
			flashTime = 0;
		}
		else
		{
			color = 0xffff0000;			
		}
	}
	

	char tmp[64];
	sprintf( tmp, "%ld ", amount);
	textObjects[1].setText( tmp );
	textObjects[1].setColor( color );

	int oldSell = inventoryListBox.GetSelectedItem();
	inventoryListBox.update();
	int newSell = inventoryListBox.GetSelectedItem();
	if ( oldSell != newSell && newSell != -1 )
	{
		variantListBox.SelectItem( -1 );
	}

	oldSell = variantListBox.GetSelectedItem();
	variantListBox.update();
	newSell = variantListBox.GetSelectedItem();

	if ( oldSell != newSell  && newSell != -1 )
	{
		inventoryListBox.SelectItem( -1 );
	}

	variantListBox.disableItemsThatCostMoreThanRP();

	LogisticsMech* pCurSell = inventoryListBox.getCurrentMech();
	LogisticsMech* pCurBuy = variantListBox.getCurrentMech();
	//magic videti gde je kursor i uraditi getCurrentMech samo tamo

	// disable the sell button
	buttons[2].disable( pCurSell ? 0 : 1);

	if ( pCurBuy )
	{
		if ( pCurBuy->getCost() > LogisticsData::instance->getCBills() )
		{
			pCurBuy = NULL;
		}

		setMech( pCurBuy );
		
	}
	else if ( pCurSell )
		setMech( pCurSell );

	buttons[1].disable( pCurBuy ? 0 : 1 );
	if ( !pCurBuy &&
		userInput->isLeftClick() && buttons[1].pointInside( userInput->getMouseX(), userInput->getMouseY() ) )
	{
		if ( !flashTime )
			flashTime = .0001f;
	}

	// drag and drop
	// update drag and drop
	if ( pDragMech )
	{
		dragIcon.moveTo( userInput->getMouseX() - dragIcon.width() / 2, 
			userInput->getMouseY() - dragIcon.height() / 2 );

		if ( userInput->leftMouseReleased(  ) )
		{
			endDrag();	
		}

	}

	mechDisplay.update();

	//Magic 72 begin
	//if (( userInput->isLeftDoubleClick() && purFirst ) && (pCurBuy))
	if ( userInput->isLeftDoubleClick() && (pCurBuy))
	{
		handleMessage( 111, MB_MSG_ADD);
	}
	if ( userInput->isLeftDoubleClick() && (pCurSell))
	
	{
		handleMessage( 111, MB_MSG_REMOVE);
	}
	//Magic 72 end

	inventoryListBox.enableAllItems();

	if ( MPlayer && ChatWindow::instance() )
		ChatWindow::instance()->update();

}

void MechPurchaseScreen::update2()
{

	if ( status != RUNNING ) // can change after button updates
		return;

	int oldPurSel, oldInvSel;
	int newPurSel, newInvSel;
	long mouseX = userInput->getMouseX();
	long mouseY = userInput->getMouseY();
	bool invItem = true;

	if ( rects[0].pointInside( mouseX, mouseY ) )
	{
		invFirst = true;
	}
	else
	{
		invFirst = false;
	}

	if ( rects[1].pointInside( mouseX, mouseY ) )
	{
		purFirst = true;
	}
	else
	{
		purFirst = false;
	}

		// update CBills

	int amount = LogisticsData::instance->getCBills();
	long color = 0xff005392;
	if ( amount != oldCBillsAmount )
	{
		previousAmount = oldCBillsAmount - amount;
		curCount = .00001f;
		oldCBillsAmount = amount;
		if ( previousAmount < 0 )
			soundSystem->playDigitalSample( WINDOW_OPEN );
		else
			soundSystem->playDigitalSample( WINDOW_CLOSE );
	}
	if ( curCount && curCount + frameLength < countDownTime  )
	{
		curCount += frameLength;
		float curAmount = previousAmount - (curCount/countDownTime * previousAmount); 
		amount += curAmount;

		color = 0xffc8e100;
		if ( curAmount > 0 )
			color = 0xffa21600;
	}
	else if ( flashTime )
	{
		flashTime += frameLength;
		if ( flashTime < .125
			|| ( flashTime > .25 && flashTime < .3875 )
			|| ( flashTime > .5 && flashTime < .6625 ) )
		{
			color = 0xff000000;
		}
		else if ( flashTime > .75 )
		{
			flashTime = 0;
		}
		else
		{
			color = 0xffff0000;			
		}
	}
	

	char tmp[64];
	sprintf( tmp, "%ld ", amount);
	textObjects[1].setText( tmp );
	textObjects[1].setColor( color );

	oldPurSel = variantListBox.GetSelectedItem(); // need to check this before we update the list box
	oldInvSel = inventoryListBox.GetSelectedItem(); // M19

	/*for ( int i = 0; i < COMPUR_ATTRIBUTE_METER_COUNT; i++ )
		attributeMeters[i].update();
	camera.update();*/

	/*if ( oldPurSel != -1 )
	{
		pSelectedMech = variantListBox.getListMech(oldPurSel);
		if ( pSelectedMech->getCost() > LogisticsData::instance->getCBills() )
		{
			pSelectedMech = NULL;
		}

		setMech( pSelectedMech );
		
	}
	else if ( oldInvSel != -1 )
	{
		pSelectedMech = inventoryListBox.getListMech(oldPurSel);
		setMech( pSelectedMech );
	}*/

	variantListBox.disableItemsThatCostMoreThanRP();

	LogisticsScreen::update();

	variantListBox.update(); //M20

/*	if (variantListBox.GetItemCount() == 0) 
		getButton( MB_MSG_ADD )->disable( 1 );
	else
	{ 

		newPurSel = variantListBox.GetSelectedItem(); // M20
		if ( oldPurSel != newPurSel)
		{
			if ( newPurSel != -1 )
			{
				pSelectedMech = variantListBox.getListMech(newPurSel); //GetItem(newPurSel)->getCurrentMech();
				invItem = false; //magic 12022011
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 0 );
			}
			else
			{
				pSelectedMech = variantListBox.getListMech(newPurSel);
				invItem = false; //magic 12022011
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
		else if (purFirst && (newPurSel != -1))
		{
			pSelectedMech = variantListBox.getListMech(newPurSel);//((ComponentPurItem*)purcomponentListBox.GetItem( newPurSel ))->getComponent();
			invItem = false; //magic 12022011
			getButton( MB_MSG_REMOVE )->disable( 1 );
			getButton( MB_MSG_ADD )->disable( 0 );
		}
	} */

	//magic 12022011 begin
	if (variantListBox.GetItemCount() == 0)
		getButton( MB_MSG_ADD )->disable( 1 );
	else
	{ 
		if (purFirst)
		{
			newPurSel = variantListBox.GetSelectedItem(); // M20
			if (newPurSel != -1 )
			{
				//if ( oldPurSel != newPurSel)
				//{
						//pSelectedMech = variantListBox.getListMech( newPurSel );
						pSelectedMech = variantListBox.getListMech( newPurSel );
						setMech(pSelectedMech);
						invItem = false; //magic 12022011
						getButton( MB_MSG_REMOVE )->disable( 1 );
						getButton( MB_MSG_ADD )->disable( 0 );
				//}
			}
			else if (oldPurSel == -1)
			{
						invItem = true; //magic 12022011
						//purMech = NULL;
						getButton( MB_MSG_REMOVE )->disable( 1 );
						getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
	}
	//magic 12022011 end

	inventoryListBox.update(); //M20

/*	if (inventoryListBox.GetItemCount() == 0)
		getButton( MB_MSG_REMOVE )->disable( 1 );
	else
	{ 
		newInvSel = inventoryListBox.GetSelectedItem(); // M20
		if ( oldInvSel != newInvSel)
		{
			if (newInvSel != -1 )
			{
				pSelectedMech = inventoryListBox.getListMech( newInvSel );
				invItem = true; //magic 12022011
				getButton( MB_MSG_REMOVE )->disable( 0 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
			else
			{
				pSelectedMech = inventoryListBox.getListMech( newInvSel );
				invItem = true; //magic 12022011
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
		else if (invFirst && (newInvSel != -1))
		{
			pSelectedMech = inventoryListBox.getListMech( newInvSel );
			invItem = true; //magic 12022011
			getButton( MB_MSG_REMOVE )->disable( 0 );
			getButton( MB_MSG_ADD )->disable( 1 );
		}
	}*/

	//magic 12022011 begin
	if (inventoryListBox.GetItemCount() == 0)
		getButton( MB_MSG_REMOVE )->disable( 1 );
	else
	{ 
		if (invFirst)
		{
			newInvSel = inventoryListBox.GetSelectedItem(); // M20
			if (newInvSel != -1 )
			{
				//if ( oldInvSel != newInvSel)
				//{
						//pSelectedMech = inventoryListBox.getListMech( newInvSel );
						pSelectedMech = inventoryListBox.getListMech( newInvSel );
						setMech(pSelectedMech);
						invItem = true; //magic 12022011
						getButton( MB_MSG_REMOVE )->disable( 0 );
						getButton( MB_MSG_ADD )->disable( 1 );
				//}
			}
			else if (oldInvSel == -1)
			{
						invItem = false; //magic 12022011
						//selMech = NULL;
						getButton( MB_MSG_REMOVE )->disable( 1 );
						getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
	}
	//magic 12022011 end

	/*if (pSelectedMech)
	{//Magic 72
		//if ( pSelectedMech->getCost() > LogisticsData::instance->getCBills() )
		//{
		//	pSelectedMech = NULL;
		//}
		setMech(pSelectedMech);
		//textObjects[14].setText( pSelectedComponent->getHelpID() ); //Magic 72
	}//Magic 72*/

	// drag and drop
	// update drag and drop
	if ( pDragMech )
	{
		dragIcon.moveTo( userInput->getMouseX() - dragIcon.width() / 2, 
			userInput->getMouseY() - dragIcon.height() / 2 );

		if ( userInput->leftMouseReleased(  ) )
		{
			endDrag();	
		}

	}

	mechDisplay.update();

	//Magic 30120210 begin
	//if (( userInput->isLeftDoubleClick() && purFirst && (variantListBox.GetItemCount() != 0)) && (pSelectedMech) && )
	if ( userInput->isLeftDoubleClick() && purFirst && (variantListBox.GetItemCount() != 0) && (pSelectedMech) && !invItem  )
		//( variantListBox.pointInside( userInput->getMouseX(), userInput->getMouseY() ) )) //magic 12022011 
		// ((MechListBoxItem*)inventoryListBox.GetItem(newPurSel))->pointInside( userInput->getMouseX(), userInput->getMouseY() ));
	{
		//purMech = pSelectedMech;
		handleMessage( 111, MB_MSG_ADD);
	}
	if ( userInput->isLeftDoubleClick() && invFirst && (inventoryListBox.GetItemCount() != 0) && (pSelectedMech) && invItem )
		//( inventoryListBox.pointInside( userInput->getMouseX(), userInput->getMouseY() ) )) //magic 12022011
			// ((MechListBoxItem*)inventoryListBox.GetItem(newInvSel))->pointInside( userInput->getMouseX(), userInput->getMouseY() ));
	{
		//selMech = pSelectedMech;
		handleMessage( 111, MB_MSG_REMOVE);
	}
	//Magic 30122010 end

	/*if ( ::helpTextID )
	{
		textObjects[helpTextArrayID].setText( helpTextID );

	}*/
	
	//char text[32];
	//sprintf( text, "%ld ", LogisticsData::instance->getCBills() );
	//textObjects[1].setText( text );
	
	inventoryListBox.enableAllItems();
	//variantListBox.disableItemsThatCostMoreThanRP();;

}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::update()
{

	if ( status != RUNNING ) // can change after button updates
		return;

//	int oldPurSel, oldInvSel;
//	int newPurSel, newInvSel;
	long mouseX = userInput->getMouseX();
	long mouseY = userInput->getMouseY();
	bool invItem = true;

	if ( rects[0].pointInside( mouseX, mouseY ) )
	{
		invFirst = true;
	}
	else
	{
		invFirst = false;
	}

	if ( rects[1].pointInside( mouseX, mouseY ) )
	{
		purFirst = true;
	}
	else
	{
		purFirst = false;
	}

		// update CBills

	int amount = LogisticsData::instance->getCBills();
	long color = 0xff005392;
	if ( amount != oldCBillsAmount )
	{
		previousAmount = oldCBillsAmount - amount;
		curCount = .00001f;
		oldCBillsAmount = amount;
		if ( previousAmount < 0 )
			soundSystem->playDigitalSample( WINDOW_OPEN );
		else
			soundSystem->playDigitalSample( WINDOW_CLOSE );
	}
	if ( curCount && curCount + frameLength < countDownTime  )
	{
		curCount += frameLength;
		float curAmount = previousAmount - (curCount/countDownTime * previousAmount); 
		amount += curAmount;

		color = 0xffc8e100;
		if ( curAmount > 0 )
			color = 0xffa21600;
	}
	else if ( flashTime )
	{
		flashTime += frameLength;
		if ( flashTime < .125
			|| ( flashTime > .25 && flashTime < .3875 )
			|| ( flashTime > .5 && flashTime < .6625 ) )
		{
			color = 0xff000000;
		}
		else if ( flashTime > .75 )
		{
			flashTime = 0;
		}
		else
		{
			color = 0xffff0000;			
		}
	}
	

	char tmp[64];
	sprintf( tmp, "%ld ", amount);
	textObjects[1].setText( tmp );
	textObjects[1].setColor( color );

	variantListBox.disableItemsThatCostMoreThanRP();

	LogisticsScreen::update();

	variantListBox.update(); 
	inventoryListBox.update();

	if (pSelectedMech)
	{
		setMech(pSelectedMech);
		if (invFirst)
		{
			selMech = pSelectedMech;
			if (inventoryListBox.GetItemCount() == 0)
					getButton( MB_MSG_REMOVE )->disable( 1 );
			else
			{
				getButton( MB_MSG_REMOVE )->disable( 0 );
				getButton( MB_MSG_ADD )->disable( 1 );
			}
		}
		if (purFirst)
		{
			if (variantListBox.GetItemCount() == 0)
				getButton( MB_MSG_ADD )->disable( 1 );
			else
			{
				getButton( MB_MSG_REMOVE )->disable( 1 );
				getButton( MB_MSG_ADD )->disable( 0 );
			}
		}
	}
	else
	{
			getButton( MB_MSG_REMOVE )->disable( 1 );
			getButton( MB_MSG_ADD )->disable( 1 );
	}

	/*
	if ( pDragMech )
	{
		dragIcon.moveTo( userInput->getMouseX() - dragIcon.width() / 2, 
			userInput->getMouseY() - dragIcon.height() / 2 );

		if ( userInput->leftMouseReleased(  ) )
		{
			endDrag();	
		}

	}
	*/ //magic 20082012 disabled drag
	mechDisplay.update();
	//Magic Debug file begin
	//char tmp2[256];
	//FitIniFile MtestFile;
	//MtestFile.open("data\\addmechtest.fit",MC2_APPEND);
	//sprintf( tmp2, "item index %ld", (long)index );
	//MtestFile.writeBlock( tmp2 );
	//MtestFile.close();
	//Magic debug file end

	if (actionClickFromLB && purFirst && (variantListBox.GetItemCount() != 0) && (pSelectedMech))
	{
		long itemIndex = variantListBox.GetSelectedItem();
		//sprintf( tmp2, "item index %ld", itemIndex );
		//MtestFile.writeBlock( tmp2 );
		//MessageBox(NULL,tmp2,"item index",0);

		handleMessage( 111, MB_MSG_ADD);
		//addMech(pSelectedMech);
		//pSelectedMech = NULL;
		actionClickFromLB = false;
		if (itemIndex < variantListBox.GetItemCount())
			variantListBox.SelectItem( itemIndex );
		else
			variantListBox.SelectItem( 0 );
	}
	if (actionClickFromLB && invFirst  && (inventoryListBox.GetItemCount() != 0) && (pSelectedMech))
	{
		long itemIndex = inventoryListBox.GetSelectedItem();
		handleMessage( 111, MB_MSG_REMOVE);
		//removeMech(pSelectedMech); //crush the game
		//pSelectedMech = NULL;
		actionClickFromLB = false;
		if (itemIndex < inventoryListBox.GetItemCount())
			inventoryListBox.SelectItem( itemIndex );
		else
			inventoryListBox.SelectItem( 0 );
	}
	//MtestFile.close();

	
	inventoryListBox.enableAllItems();
}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::render( int xOffset, int yOffset )
{

	/*if ( !xOffset && !yOffset )
	{
		if ( !MPlayer && !LogisticsData::instance->isSingleMission() && LogisticsData::instance->newMechsAvailable() )
		{
			soundSystem->playBettySample( BETTY_NEW_MECHS );
			LogisticsData::instance->setNewMechsAcknowledged();
		}
	}*/

	inventoryListBox.move( xOffset, yOffset);
	inventoryListBox.render();
	inventoryListBox.move( -xOffset, -yOffset );

	variantListBox.move(xOffset, yOffset);
	variantListBox.render();
	variantListBox.move(-xOffset, -yOffset);	

	mechDisplay.render( xOffset, yOffset );

	LogisticsScreen::render( xOffset, yOffset );

	if ( pDragMech )
		dragIcon.render();

	
	/*if ( MPlayer && ChatWindow::instance() )
		ChatWindow::instance()->render(xOffset, yOffset);
	*/


}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::begin()
{

	variantListBox.removeAllItems(true);
	inventoryListBox.removeAllItems(true);
	// initialize both the inventory and icon lists
	EList< LogisticsMech*, LogisticsMech* > mechList;
	LogisticsData::instance->getInventory( mechList );
	EList<LogisticsVariant*, LogisticsVariant*> fullVarList;
	LogisticsData::instance->getAllVariants(fullVarList);
	prevInventory.Clear();

	LogisticsMech* pSelMech = 0;

	oldCBillsAmount = LogisticsData::instance->getCBills();


	for ( EList< LogisticsMech*, LogisticsMech* >::EIterator iter = mechList.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( (*iter)->getForceGroup() )
				continue;
		
			prevInventory.Append( *(*iter) );
			
	
			bool bFound = 0;

			for ( int i = 0; i < inventoryListBox.GetItemCount(); i++ )
			{
				//if ( ((MechListBoxItem*)inventoryListBox.GetItem(i))->getMech()->getVariant() == (*iter)->getVariant() )
				if ( ((MechListBoxItem*)inventoryListBox.GetItem(i))->getMech()->getID() == (*iter)->getID() ) //magic 21082012
				{
					bFound = true;
				//M12 da li ovde dodati increment
					inventoryListBox.incMechCount(((MechListBoxItem*)inventoryListBox.GetItem(i))->getMech());
				}
			}

			if ( !bFound )
			{
				MechListBoxItem* item = new MechListBoxItem( (*iter), 1 );
				inventoryListBox.AddItem( item );
				//inventoryListBox.incMechCount((*iter));

			}

		}

	MechListBoxItem* item = (MechListBoxItem*)inventoryListBox.GetItem( 0 );
	if ( item )
	{
		inventoryListBox.SelectItem( 0 );
		pSelMech = item->getMech();
	}
	

	//LogisticsVariant* pVariants[256];
	//int count = 256;
//magic 18122010 begin

	char tmp[64];
	//char tmp3[64];
	//FitIniFile MtestFile;
	//MtestFile.open("data\\mech_purchase.fit", CREATE);
	//MtestFile.writeBlock( vehicleName );
	//MtestFile.close();
	FullPathFileName mFullFileName;
	mFullFileName.init(missionPath, "mechPurchase", ".fit");
	FitIniFile mechPurchaseFile;
	
	//MtestFile.writeBlock( mFullFileName );
	//if (fileExists(mFullFileName))

	//int numMechsForPurchase = RandomNumber(10);

		//MtestFile.writeBlock( mFullFileName );
		//MtestFile.close();

	long result1 = mechPurchaseFile.open(mFullFileName);
	if (result1 != NO_ERR)
	{
		//******************************************************
		char tmp[32];
		EString purchaseFileName = LogisticsData::instance->getPurchaseFileName();
		purchaseFileName.MakeLower();

		if ( purchaseFileName.Length() < 1 )
		{
			purchaseFileName = missionPath;
			purchaseFileName += "purchase.fit";
		}

		//MtestFile.writeBlock( purchaseFileName );
		// make sure its around and you can open it 
		FitIniFile pfile;
		if ( NO_ERR == pfile.open( (char*)(const char*)purchaseFileName ) )
		{

			// go through each variant, and see if it's available
			char chassisFileName[255];

			int varNum;

			varNum = RandomNumber(5);
	
			pfile.seekBlock( "Mechs" );
			//int skipMe = 0;
			int varCount = 0;
			for ( int i = 0; i < 255; i++ )
			{
				sprintf( tmp, "Mech%ld", i );
				if ( NO_ERR != pfile.readIdString( tmp, chassisFileName, 254 ) )
					break;

				//MtestFile.writeBlock( chassisFileName );

				FullPathFileName path;
				path.init( objectPath, chassisFileName, ".csv" );
	
				CSVFile mechCSVFile;
				if ( mechCSVFile.open(path) == NO_ERR )
				{
					LogisticsVariant* purVar = LogisticsData::instance->getPurVariant( chassisFileName, varNum );
					//magic 24122010 begin
					if (!purVar->isDesignerMech())
						purVar = LogisticsData::instance->getPurVariant( chassisFileName, 0 );
					//magic 24122010 end
						bool bFound = 0;
						for ( int j = 0; j < variantListBox.GetItemCount(); j++ )
						{
					
							if ( ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant() == purVar )
							{
								bFound = true;
								variantListBox.incMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech());
							}
						}

						if ( !bFound )
						{
							//LogisticsMech* gMech = new LogisticsMech( purVar, 1 );
							LogisticsMech* gMech = new LogisticsMech( purVar, LogisticsData::instance->createInstanceID( purVar ) ); //magic 12052012
							MechListBoxItem* item = new MechListBoxItem( (gMech), 1 );
							variantListBox.AddItem( item );
						}

				}
				mechCSVFile.close();
				//else skipMe++;
			}
		}

		pfile.close();
		//******************************************************

		/*//ako ga nema ucitati purchase iz missije prvo pa na accept napraviti mechpurchasefile
		//ovo radi pravi random purchase OK
		FitIniFile writePurchaseFile;
		writePurchaseFile.open(mFullFileName, CREATE);
		int numMechsForPurchase = RandomNumber(10);
		int fullNumVariants = fullVarList.Count();
		int purMechs[10] = {0,0,0,0,0,0,0,0,0,0};
		//genarate random purchase here
		sprintf( tmp3, "numMechsForPurchase%ld", numMechsForPurchase );
		MtestFile.writeBlock( tmp3 );
		sprintf( tmp3, "FullnumVariants%ld", fullNumVariants );
		MtestFile.writeBlock( tmp3 );
		for (int i = 0; i < numMechsForPurchase; i++)
		{
			purMechs[i] = RandomNumber(fullNumVariants);
			sprintf( tmp3, "iterator i%ld", i );
			MtestFile.writeBlock( tmp3 );
			sprintf( tmp3, "Random variant num%ld", purMechs[i] );
			MtestFile.writeBlock( tmp3 );
		}

		writePurchaseFile.writeBlock( "General" );
		writePurchaseFile.writeIdLong( "PurMechsCount", numMechsForPurchase );
		
		for (int i = 0; i < numMechsForPurchase; i++)
		{
			char tmp[256];
			sprintf( tmp, "MechPurchase%ld", i );
			writePurchaseFile.writeBlock( tmp );
			int varCounter = 0;

			for ( EList< LogisticsVariant*, LogisticsVariant* >::EIterator mIter = fullVarList.Begin(); !mIter.IsDone(); mIter++ )
			{
			
					if (purMechs[i] == varCounter)
					{
						LogisticsMech* gMech = new LogisticsMech( (*mIter), 1 );
						MechListBoxItem* item = new MechListBoxItem( (gMech), 1 );
						variantListBox.AddItem( item );
					
						writePurchaseFile.writeIdString( "Chassis", (*mIter)->getFileName() );
						writePurchaseFile.writeIdString( "Variant", (*mIter)->getName() );
						writePurchaseFile.writeIdLong( "Quantity", 1 );
					}
					//((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getFileName();
			
				varCounter++;
			}
		}
		writePurchaseFile.close();*/

	}

	else
	{
		// read purchase from file
		long purMechsCount;
		mechPurchaseFile.seekBlock( "General" );
		mechPurchaseFile.readIdLong( "PurMechsCount", purMechsCount );
		
		//sprintf( tmp3, "purMechsCount %ld", purMechsCount );
		//MtestFile.writeBlock( tmp3 );

		for ( long i = 0; i < purMechsCount; i++ )
		{
			sprintf( tmp, "MechPurchase%ld", i ) ;
			if ( NO_ERR != mechPurchaseFile.seekBlock( tmp ) )
			{
				gosASSERT( 0 );
			}
			char tmp2[256];
			long qForPurchase;
			
			//MtestFile.writeBlock( tmp );
			long result = mechPurchaseFile.readIdString( "Variant", tmp2, 255 );
			//MtestFile.writeBlock( tmp2 );

			if (result == NO_ERR)
			{
				long result2 = mechPurchaseFile.readIdLong( "Quantity", qForPurchase );
				if (result2 != NO_ERR)
					qForPurchase = 1;

				//sprintf( tmp, "qForPurchase %ld", qForPurchase );
				//MtestFile.writeBlock( tmp );

				if ( (qForPurchase <= 0))// && (qForPurchase > 5))
					qForPurchase = 1;

				for ( EList< LogisticsVariant*, LogisticsVariant* >::EIterator mIter = fullVarList.Begin(); !mIter.IsDone(); mIter++ )
				{
					if ( (*mIter)->getName().Compare( tmp2, 0 ) == 0 )
					{
						//LogisticsMech* gMech = new LogisticsMech( (*mIter), 1 );
						LogisticsMech* gMech = new LogisticsMech( (*mIter), LogisticsData::instance->createInstanceID( (*mIter) ) ); //magic 12052012
						MechListBoxItem* item = new MechListBoxItem( (gMech), qForPurchase );
						variantListBox.AddItem( item );
					
					}
				}
			}
		}
		
	}
	mechPurchaseFile.close();

//MtestFile.close();

//magic 18122010 end
	/*//M16 begin novi metod
	EList<LogisticsVariant*, LogisticsVariant*> purVarList;
	LogisticsData::instance->getPurVariants( purVarList );


	for ( EList< LogisticsVariant*, LogisticsVariant* >::EIterator iter = purVarList.Begin();
		!iter.IsDone(); iter++ )
		{
	
			bool bFound = 0;

			for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
			{
				if ( ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant() == (*iter) )
				{
					bFound = true;
				//M12 da li ovde dodati increment
					variantListBox.incMechCount(((MechListBoxItem*)variantListBox.GetItem(i))->getMech());
				}
			}

			if ( !bFound )
			{
				LogisticsMech* gMech = new LogisticsMech( (*iter), 1 );
				MechListBoxItem* item = new MechListBoxItem( (gMech), 1 );
				variantListBox.AddItem( item );
				//inventoryListBox.incMechCount((*iter));

			}

		}

	//M16 end novi metod*/

	if ( !pSelMech )
	{
		MechListBoxItem* item = (MechListBoxItem*)variantListBox.GetItem( 0 );
		if ( item )
		{
			variantListBox.SelectItem( 0 );
			pSelMech = item->getMech();
		}
	}

	status = RUNNING;
	acceptPressed = 0;

	inventoryListBox.drawCBills(1);
	variantListBox.drawCBills(1);
	variantListBox.setOrange( 0 );
	inventoryListBox.setOrange( 1 );
	
	pSelectedMech = pSelMech;
	//setMech( pSelMech );
	setMech( pSelectedMech );
	//purMech = NULL;
	//selMech = NULL;
	actionClickFromLB = false;

	pDragMech = NULL;

	//magic 26032011 begin
	FullPathFileName cFullFileName;
	cFullFileName.init(missionPath, "mechPurchase", ".can");
	FitIniFile writePurchaseFile;
	writePurchaseFile.open(cFullFileName, CREATE);
	writePurchaseFile.writeBlock( "General" );
	writePurchaseFile.writeIdLong( "PurMechsCount", variantListBox.GetItemCount() );
	
	for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
	{
		char tmp[256];
		sprintf( tmp, "MechPurchase%ld", i );
		writePurchaseFile.writeBlock( tmp );
		writePurchaseFile.writeIdString( "Chassis", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getFileName() );
		writePurchaseFile.writeIdString( "Variant", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getName() );
		writePurchaseFile.writeIdLong( "Quantity", variantListBox.getMechCount(( (MechListBoxItem*)variantListBox.GetItem(i))->getMech()) ); //magic
	}
	writePurchaseFile.close();
	//magic 26032011 end

}
//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::end()
{
	if ( !acceptPressed && status != MAINMENU )
	{ 
		// sell all current stuff
		EList< LogisticsMech*, LogisticsMech* > list;
		LogisticsData::instance->getInventory( list );
		for ( EList< LogisticsMech*, LogisticsMech* >::EIterator iter = list.End(); !iter.IsDone(); iter-- )
		{
			LogisticsData::instance->sellMech( (*iter) );
		}


		unsigned long base, color1, color2;
		for ( MECH_LIST::EIterator pIter = prevInventory.Begin(); !pIter.IsDone(); pIter++ )
		{
			(*pIter).getColors(base, color1, color2);
			LogisticsData::instance->addMechToInventory( (*pIter).getVariant(), (*pIter).getPilot(),
				(*pIter).getForceGroup(), true );
		}

	}

	variantListBox.removeAllItems(true);
	inventoryListBox.removeAllItems(true);

	mechDisplay.setMech( NULL );
}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::setMech( LogisticsMech* pMech, bool FromLB )
{
	if ( status == RUNNING )
	{
		if ( FromLB ) // clicked on a disabled item
		{
			if ( !flashTime )
				flashTime = .000001f;
		/*}
		
		if ( FromLB ) // clicked on a disabled item
		{ */ //magic 29112011
			inventoryListBox.SelectItem( -1 );
			variantListBox.SelectItem( -1 );

		}
		if (pMech) //magic 19032011
			mechDisplay.setMech( pMech );		

	}
}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::beginDrag( LogisticsMech* pMech )
{
	if ( status != RUNNING )
		return;
	
	if ( !pDragMech )
	{
		pDragMech = pMech;
	}

	MechListBox::initIcon( pMech, dragIcon );

	if ( inventoryListBox.pointInside( userInput->getMouseDragX(),
		userInput->getMouseDragY() ) )
	{
		dragStartLeft = true;
		inventoryListBox.dimItem( pMech, true );
	}
	else 
	{
		dragStartLeft = false;
		variantListBox.dimItem( pMech, true );
	}
}

void MechPurchaseScreen::endDrag( )
{
	if ( pDragMech )
	{
	
		if ( inventoryListBox.pointInside( userInput->getMouseX(),
			userInput->getMouseY() ) )
		{
			if ( !dragStartLeft )
			{
				addMech( pDragMech );
			}
		}
		else
		{
			if ( dragStartLeft )
				removeMech( pDragMech );
		}
	}

	inventoryListBox.undimAll();
	variantListBox.undimAll();

	pDragMech = NULL;
}


//-------------------------------------------------------------------------------------------------

int	MechPurchaseScreen::handleMessage( unsigned long what, unsigned long who )
{
		

	if ( status == RUNNING )
	{
		switch (who)
		{
			case MB_MSG_ADD:
				{
				//addSelectedMech();
				if (pSelectedMech)
					addMech(pSelectedMech); //magic  02012011
				soundSystem->playDigitalSample( LOG_SELECT );
				//pSelectedMech = NULL; //magic 12022011
				//inventoryListBox.SelectItem( 0 );//magic 13012011
				//magic 19092011 begin
				pSelectedMech = NULL; 
				inventoryListBox.SelectItem( -1 );
				//magic 19092011 end
				//magic 23122010 begin
				FullPathFileName mFullFileName;
				mFullFileName.init(missionPath, "mechPurchase", ".fit");
				FitIniFile writePurchaseFile;
				writePurchaseFile.open(mFullFileName, CREATE);
				writePurchaseFile.writeBlock( "General" );
				writePurchaseFile.writeIdLong( "PurMechsCount", variantListBox.GetItemCount() );
	
				for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
				{
					char tmp[256];
					sprintf( tmp, "MechPurchase%ld", i );
					writePurchaseFile.writeBlock( tmp );
					writePurchaseFile.writeIdString( "Chassis", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getFileName() );
					writePurchaseFile.writeIdString( "Variant", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getName() );
					writePurchaseFile.writeIdLong( "Quantity", variantListBox.getMechCount(( (MechListBoxItem*)variantListBox.GetItem(i))->getMech()) ); //magic
				}
				writePurchaseFile.close();
				//magic 23122010 end
				reLoadPurchase(); //magic 19032011
				}
				break;
			case MB_MSG_REMOVE:
				{
				//removeSelectedMech();
				if (pSelectedMech)
					removeMech(pSelectedMech); //magic 02012011
				soundSystem->playDigitalSample( LOG_SELECT );
				//pSelectedMech = NULL; //magic 12022011
				//variantListBox.SelectItem( 0 );//magic 13022011
				//magic 19092011 begin
				pSelectedMech = NULL; 
				variantListBox.SelectItem( -1 );
				//magic 19092011 end
				//magic 23122010 begin
				FullPathFileName mFullFileName;
				mFullFileName.init(missionPath, "mechPurchase", ".fit");
				FitIniFile writePurchaseFile;
				writePurchaseFile.open(mFullFileName, CREATE);
				writePurchaseFile.writeBlock( "General" );
				writePurchaseFile.writeIdLong( "PurMechsCount", variantListBox.GetItemCount() );
	
				for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
				{
					char tmp[256];
					sprintf( tmp, "MechPurchase%ld", i );
					writePurchaseFile.writeBlock( tmp );
					writePurchaseFile.writeIdString( "Chassis", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getFileName() );
					writePurchaseFile.writeIdString( "Variant", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getName() );
					writePurchaseFile.writeIdLong( "Quantity", variantListBox.getMechCount(( (MechListBoxItem*)variantListBox.GetItem(i))->getMech()) ); //magic
				}
				writePurchaseFile.close();
				//magic 23122010 end
				reLoadPurchase(); //magic 19032011
				}
				break;
			case MB_MSG_NEXT:
				{
				//savePurVarChanges(purchaseVariants, maxPurchaseVariants);
				//LogisticsData::instance->UpdatePurVariants(purchaseVariants, maxPurchaseVariants);
				FullPathFileName mFullFileName;
				mFullFileName.init(missionPath, "mechPurchase", ".fit");
				FitIniFile writePurchaseFile;
				writePurchaseFile.open(mFullFileName, CREATE);
				writePurchaseFile.writeBlock( "General" );
				writePurchaseFile.writeIdLong( "PurMechsCount", variantListBox.GetItemCount() );
	
				for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
				{
					char tmp[256];
					sprintf( tmp, "MechPurchase%ld", i );
					writePurchaseFile.writeBlock( tmp );
					writePurchaseFile.writeIdString( "Chassis", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getFileName() );
					writePurchaseFile.writeIdString( "Variant", ((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant()->getName() );
					writePurchaseFile.writeIdLong( "Quantity", variantListBox.getMechCount(( (MechListBoxItem*)variantListBox.GetItem(i))->getMech()) ); //magic
				}
				writePurchaseFile.close();

				//magic 26032011 begin
				FullPathFileName cFullFileName;
				cFullFileName.init(missionPath, "mechPurchase", ".can");
				remove(cFullFileName);
				//magic 26032011 end

				acceptPressed = true;
				status = DOWN;
				}
				break;
			
			case MB_MSG_PREV:
				{
				//magic 26032011 begin
				FullPathFileName mFullFileName;
				mFullFileName.init(missionPath, "mechPurchase", ".fit");
				FullPathFileName cFullFileName;
				cFullFileName.init(missionPath, "mechPurchase", ".can");
				remove(mFullFileName);
				rename(cFullFileName, mFullFileName);
				//magic 26032011 end

				status = DOWN;
				}
				break;
			case MB_MSG_MAINMENU:
				status = MAINMENU;
				break;
		}
	}

	return 0;

}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::addSelectedMech()
{
	LogisticsMech* pMech = variantListBox.getCurrentMech();
	if ( pMech )
	{
		addMech( pMech );
	}

	variantListBox.disableItemsThatCostMoreThanRP();
	int curSel = variantListBox.GetSelectedItem();
	if ( curSel == -1 )
	{
		variantListBox.SelectItem( -1 );
		inventoryListBox.SelectItem( 0 );
	}

}

void MechPurchaseScreen::addMech( LogisticsMech* pMech )
{
	//M16 begin
	int index = variantListBox.GetSelectedItem();
	aListItem* pItem = variantListBox.GetItem( index );
	/*//magic 14022011 begin
	long numItems = variantListBox.GetItemCount();
	LogisticsMech* iLBItems[128];
	for (int i = 0; i < numItems; i++)
	{
		//if (i != index)
			iLBItems[i] = variantListBox.getListMech( i );
		//else
		//	iLBItems[i] = NULL;
		
	}
	//magic 14022011 end*/
	/*//Magic Debug file begin
	char tmp2[256];
	FitIniFile MtestFile;
	MtestFile.open("data\\addmechtest.fit", CREATE);
	sprintf( tmp2, "item index %ld", (long)index );
	MtestFile.writeBlock( tmp2 );
	MtestFile.close();
	//Magic debug file end*/
	//M16 end
	//bool bf = false;
	LogisticsData::instance->purchaseMech( pMech->getVariant() );

		if (variantListBox.getMechCount(pMech) < 2)
		{
			variantListBox.RemoveItem( pItem, true );
			//MechListBoxItem* pItem1 = new MechListBoxItem( pMech, 1 );
			//variantListBox.RemoveItem2( pItem );
			//variantListBox.RemoveMBItem( pItem );
			//variantListBox.decMechCount(pMech);
			/*//magic 14022011 begin
			variantListBox.removeAllItems(true);
			for (int i = 0; i < numItems; i++)
			{
				if (i != index)// != NULL)
				{
					MechListBoxItem* pItem1 = new MechListBoxItem( iLBItems[i], 1 );
					variantListBox.AddItem(pItem1);
				}
			}
			//magic 14022011 end*/
		}
		else
			variantListBox.decMechCount(pMech);
		/*
		if (inventoryListBox.getMechCount(pMech) > 0)
		{
			inventoryListBox.incMechCount(pMech); 
		}
		else
		*/ //magic 21082012 disabled
		{
			MechListBoxItem* pItem1 = new MechListBoxItem( pMech, 1 );
			inventoryListBox.AddItem( pItem1 );
		}

	
		//magic 13022011 begin
	if ( !variantListBox.GetItemCount() )
	{
		inventoryListBox.SelectItem( 0 );
	}
	else if ( -1 == variantListBox.GetSelectedItem() )
		variantListBox.SelectItem( 0 );
		//magic 13022011 end

}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::removeSelectedMech()
{
	LogisticsMech* pMech = inventoryListBox.getCurrentMech();
	if ( pMech )
	{
		removeMech( pMech );
	}
}

void MechPurchaseScreen::removeMech( LogisticsMech* pMech )
{
	int index = inventoryListBox.GetSelectedItem();
	aListItem* pItem = inventoryListBox.GetItem( index );
	/*//magic 14022011 begin
	long numItems = inventoryListBox.GetItemCount();
	LogisticsMech* iLBItems[128];
	for (int i = 0; i < numItems; i++)
	{
		//if (i != index)
			iLBItems[i] = inventoryListBox.getListMech( i );
		//else
		//	iLBItems[i] = NULL;
		
	}
	//magic 14022011 end*/
	//MechListBoxItem* MLBItems[128];
	//int oldCount = LogisticsData::instance->getVariantsInInventory( pMech->getVariant(), true );//M12
	//int oldCount = variantListBox.getMechCount( pMech );//M12
	if ( NO_ERR == LogisticsData::instance->sellMech( pMech ))//M15 && ( oldCount < 2 ) )
	{

		if (inventoryListBox.getMechCount(pMech) < 2)
		{
			inventoryListBox.RemoveItem( pItem, true );
			//MechListBoxItem* pItem1 = new MechListBoxItem( pMech, 1 );
			//inventoryListBox.RemoveItem2( pItem );
			//inventoryListBox.removeAllChildren();
			//inventoryListBox.RemoveMBItem( pItem );
			//inventoryListBox.decMechCount(pMech);
			/*//magic 14022011 begin
			inventoryListBox.removeAllItems(true);
			for (int i = 0; i < numItems; i++)
			{
				if (i != index)// != NULL)
				{
					MechListBoxItem* pItem1 = new MechListBoxItem( iLBItems[i], 1 );
					inventoryListBox.AddItem(pItem1);
				}
			}
			//magic 14022011 end*/
		}
		else
			inventoryListBox.decMechCount(pMech);

		if (variantListBox.getMechCount(pMech) > 0)
		{
			variantListBox.incMechCount(pMech); 
		}
		else
		{
			MechListBoxItem* pItem1 = new MechListBoxItem( pMech, 1 );
			variantListBox.AddItem( pItem1 );
		}

	}


	if ( !inventoryListBox.GetItemCount() )
	{
		selectFirstBuyableMech();
	}
	else if ( -1 == inventoryListBox.GetSelectedItem() )
		inventoryListBox.SelectItem( 0 );

}

bool MechPurchaseScreen::selectFirstBuyableMech()
{
	for ( int i = 0; i < variantListBox.GetItemCount(); i++ )
	{
		if ( NO_ERR == LogisticsData::instance->canPurchaseMech( 
			((MechListBoxItem*)variantListBox.GetItem(i))->getMech()->getVariant() ) )
		{
			variantListBox.SelectItem( i );
			return true;
		}
	}

	return false;
}



void MechPurchaseScreen::removePurVar( LogisticsVariant* pVar )//M nije vise potrebna
{	
	int purCount = maxPurchaseVariants;

	if (purCount > 0)
	{
	for (int i = 0; i < purCount; i++)
	{
		if ((purchaseVariants[i]->getID() == pVar->getID()) && (i < purCount-1))
		{
			purchaseVariants[i] = purchaseVariants[purCount-1];
			purchaseVariants[purCount-1] = NULL;
			maxPurchaseVariants--;
			break;
		}
		if ((purchaseVariants[i]->getID() == pVar->getID()) && (i == purCount-1))
		{
			purchaseVariants[purCount-1] = NULL;
			maxPurchaseVariants--;
		}
	}
	//ubaciti update mechlistboxa
		variantListBox.removeAllItems(true);

		for ( int i =0; i < maxPurchaseVariants; i++ )
		{

			//M13
			bool bFound = false;
			for ( int j = 0; j < variantListBox.GetItemCount(); j++ )
			{
				if ( ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant() == purchaseVariants[i] )
				{
					bFound = true;
				//M12 da li ovde dodati increment
					variantListBox.incMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech());
				}
			}

			if ( !bFound )
			{
				//LogisticsMech* pMech = new LogisticsMech( purchaseVariants[i], -1 );
				LogisticsMech* pMech = new LogisticsMech( purchaseVariants[i], LogisticsData::instance->createInstanceID( purchaseVariants[i] ) ); //magic 12052012
				MechListBoxItem* item = new MechListBoxItem( pMech, 1 );
				variantListBox.AddItem( item );
				//variantListBox.incMechCount( pMech );
			}

			//M13

		}
	}
}
void MechPurchaseScreen::addPurVar( LogisticsVariant* pVar )//M nije vise potrebna
{
	purchaseVariants[maxPurchaseVariants] = pVar;
	maxPurchaseVariants++;
	//ubaciti update mechlist boxa
		variantListBox.removeAllItems(true);

		for ( int i = 0; i < maxPurchaseVariants; i++ )
		{

			//M13
			bool bFound = false;
			for ( int j = 0; j < variantListBox.GetItemCount(); j++ )
			{
				if ( ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant() == purchaseVariants[i] )
				{
					bFound = true;
				//M12 da li ovde dodati increment
					variantListBox.incMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech());
				}
			}

			if ( !bFound )
			{ 
				//LogisticsMech* pMech = new LogisticsMech( purchaseVariants[i], -1 );
				LogisticsMech* pMech = new LogisticsMech( purchaseVariants[i], LogisticsData::instance->createInstanceID( purchaseVariants[i]) ); //magic 12052012
				MechListBoxItem* item = new MechListBoxItem( pMech, 1 );
				variantListBox.AddItem( item );
				//variantListBox.incMechCount( pMech );
			} 

			//M13

		} 
}

/*
long MechPurchaseScreen::getPurVariants( LogisticsVariant* pVar )
{

		//return variantListBox.getNumVariants(pVar);

}

long MechPurchaseScreen::getInvVariants( LogisticsVariant* pVar )
{
	
		//return inventoryListBox.getNumVariants(pVar);


}
*/
void MechPurchaseScreen::savePurVarChanges(LogisticsVariant** pVar, int& maxCount)
{
	maxCount = 0;
	
	for ( int j = 0; j < variantListBox.GetItemCount(); j++ )
	{
		if (variantListBox.getMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech()) > 1)
		{
			for (int i = 0; i < variantListBox.getMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech()); i++)
			{
				pVar[maxCount] = ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant();
				maxCount++;
			}	
		}
		else
		{
				pVar[maxCount] = ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant();
				maxCount++;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void MechPurchaseScreen::reLoadPurchase()
{

	variantListBox.removeAllItems(true);
	EList<LogisticsVariant*, LogisticsVariant*> fullVarList;
	LogisticsData::instance->getAllVariants(fullVarList);

	char tmp[64];
	//char tmp3[64];
	//FitIniFile MtestFile;
	//MtestFile.open("data\\mech_purchase.fit", CREATE);
	//MtestFile.writeBlock( vehicleName );
	//MtestFile.close();
	FullPathFileName mFullFileName;
	mFullFileName.init(missionPath, "mechPurchase", ".fit");
	FitIniFile mechPurchaseFile;
	
	//MtestFile.writeBlock( mFullFileName );
	//if (fileExists(mFullFileName))

	//int numMechsForPurchase = RandomNumber(10);

		//MtestFile.writeBlock( mFullFileName );
		//MtestFile.close();

	long result1 = mechPurchaseFile.open(mFullFileName);
	if (result1 != NO_ERR)
	{
		//******************************************************
		char tmp[32];
		EString purchaseFileName = LogisticsData::instance->getPurchaseFileName();
		purchaseFileName.MakeLower();

		if ( purchaseFileName.Length() < 1 )
		{
			purchaseFileName = missionPath;
			purchaseFileName += "purchase.fit";
		}

		//MtestFile.writeBlock( purchaseFileName );
		// make sure its around and you can open it 
		FitIniFile pfile;
		if ( NO_ERR == pfile.open( (char*)(const char*)purchaseFileName ) )
		{

			// go through each variant, and see if it's available
			char chassisFileName[255];

			int varNum;

			varNum = RandomNumber(5);
	
			pfile.seekBlock( "Mechs" );
			//int skipMe = 0;
			int varCount = 0;
			for ( int i = 0; i < 255; i++ )
			{
				sprintf( tmp, "Mech%ld", i );
				if ( NO_ERR != pfile.readIdString( tmp, chassisFileName, 254 ) )
					break;

				//MtestFile.writeBlock( chassisFileName );

				FullPathFileName path;
				path.init( objectPath, chassisFileName, ".csv" );
	
				CSVFile mechCSVFile;
				if ( mechCSVFile.open(path) == NO_ERR )
				{
					LogisticsVariant* purVar = LogisticsData::instance->getPurVariant( chassisFileName, varNum );
					//magic 24122010 begin
					if (!purVar->isDesignerMech())
						purVar = LogisticsData::instance->getPurVariant( chassisFileName, 0 );
					//magic 24122010 end
						bool bFound = 0;
						for ( int j = 0; j < variantListBox.GetItemCount(); j++ )
						{
					
							if ( ((MechListBoxItem*)variantListBox.GetItem(j))->getMech()->getVariant() == purVar )
							{
								bFound = true;
								variantListBox.incMechCount(((MechListBoxItem*)variantListBox.GetItem(j))->getMech());
							}
						}

						if ( !bFound )
						{
							//LogisticsMech* gMech = new LogisticsMech( purVar, 1 );
							LogisticsMech* gMech = new LogisticsMech( purVar, LogisticsData::instance->createInstanceID( purVar ) ); //magic 12052012
							MechListBoxItem* item = new MechListBoxItem( (gMech), 1 );
							variantListBox.AddItem( item );
						}

				}
				mechCSVFile.close();
				//else skipMe++;
			}
		}

		pfile.close();

	}

	else
	{
		// read purchase from file
		long purMechsCount;
		mechPurchaseFile.seekBlock( "General" );
		mechPurchaseFile.readIdLong( "PurMechsCount", purMechsCount );
		
		//sprintf( tmp3, "purMechsCount %ld", purMechsCount );
		//MtestFile.writeBlock( tmp3 );

		for ( long i = 0; i < purMechsCount; i++ )
		{
			sprintf( tmp, "MechPurchase%ld", i ) ;
			if ( NO_ERR != mechPurchaseFile.seekBlock( tmp ) )
			{
				gosASSERT( 0 );
			}
			char tmp2[256];
			long qForPurchase;
			
			//MtestFile.writeBlock( tmp );
			long result = mechPurchaseFile.readIdString( "Variant", tmp2, 255 );
			//MtestFile.writeBlock( tmp2 );

			if (result == NO_ERR)
			{
				long result2 = mechPurchaseFile.readIdLong( "Quantity", qForPurchase );
				if (result2 != NO_ERR)
					qForPurchase = 1;

				//sprintf( tmp, "qForPurchase %ld", qForPurchase );
				//MtestFile.writeBlock( tmp );

				if ( (qForPurchase <= 0))// && (qForPurchase > 5))
					qForPurchase = 1;

				for ( EList< LogisticsVariant*, LogisticsVariant* >::EIterator mIter = fullVarList.Begin(); !mIter.IsDone(); mIter++ )
				{
					if ( (*mIter)->getName().Compare( tmp2, 0 ) == 0 )
					{
						//LogisticsMech* gMech = new LogisticsMech( (*mIter), 1 );
						LogisticsMech* gMech = new LogisticsMech( (*mIter), LogisticsData::instance->createInstanceID( (*mIter) )); //magic 12052012
						MechListBoxItem* item = new MechListBoxItem( (gMech), qForPurchase );
						variantListBox.AddItem( item );
					
					}
				}
			}
		}
		
	}
	mechPurchaseFile.close();

//MtestFile.close();


/*

		MechListBoxItem* item = (MechListBoxItem*)variantListBox.GetItem( 0 );
		if ( item )
		{
			variantListBox.SelectItem( 0 );
			pSelectedMech = item->getMech();
		}


	status = RUNNING;
	acceptPressed = 0;

	inventoryListBox.drawCBills(1);
	variantListBox.drawCBills(1);
	variantListBox.setOrange( 0 );
	inventoryListBox.setOrange( 1 );
	
	setMech( pSelectedMech );

	//actionClickFromLB = false;

	pDragMech = NULL;*/

}
//*************************************************************************************************
// end of file ( MechPurchaseScreen.cpp )
