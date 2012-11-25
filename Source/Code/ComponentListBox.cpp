#define COMPONENTLISTBOX_CPP
/*************************************************************************************************\
ComponentListBox.cpp			: Implementation of the ComponentListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "ComponentListBox.h"
#include "LogisticsVariant.h"
#include "LogisticsComponent.h"
#include "..\resource.h"
#include "malloc.h"
#include "mcLib.h"
#include "LogisticsData.h"
#include "MechLabScreen.h"
#include "gamesound.h"
#include "logisticserrors.h"

ComponentListItem* ComponentListItem::s_templateItem = NULL;

ComponentIconListBox* ComponentIconListBox::s_instance = NULL;



///////////////////////////////////////////////////////


ComponentListItem::ComponentListItem( LogisticsComponent* pComp )
{

	if ( s_templateItem )
		*this = *s_templateItem;

	pComponent = pComp;

	
	if ( pComp )
	{
		name.setText( pComponent->getName() );
		char numericText[32];
		sprintf( numericText, "%ld", pComponent->getCost() );
		costText.setText(  numericText  );
		sprintf( numericText, "%.0lf", pComponent->getHeat() );
		heatText.setText( numericText );


		int sizeX = pComponent->getComponentWidth();
		int sizeY = pComponent->getComponentHeight();
		
		const char* pFile = pComponent->getIconFileName();

		FullPathFileName path;
		path.init( artPath, pFile, "tga" );

		icon.setTexture( path );
		icon.resize( sizeX * LogisticsComponent::XICON_FACTOR, sizeY * LogisticsComponent::YICON_FACTOR);
		icon.setUVs( 0.f, 0.f, sizeX * 48.f, sizeY * 32.f );

		// figure out the difference between this thing's size and
		// the template objects
		int deltaY = icon.height() - s_templateItem->icon.height();
		int deltaX = icon.width() - s_templateItem->icon.width();

		// resize outlines and icon outlines
		outline.resize(outline.width(), outline.height()+deltaY);
		iconOutline.resize(iconOutline.width()+deltaX, iconOutline.height()+deltaY);

		disabledText.resize( outline.width(), outline.height() );
		disabledText.moveTo( 0, 0 );
		disabledText.alignment = 3;


		aObject::init( outline.left(), outline.top(), outline.width(), outline.height() );

		addChild( &icon );
		addChild( &name );
		addChild( &costText );
		addChild( &heatText );
		addChild( &costIcon );
		addChild( &heatIcon );
		addChild( &disabledText );
		// addChild( &quantText ); //MAGIC probati removechild

		if ( s_templateItem )
		{

			for ( int i = 0; i < 6; i++ )
			{
				for ( int j = 0; j < COMP_ANIMATION_COUNT; j++ )
				{
					if ( s_templateItem->pChildAnims[i] == &s_templateItem->animations[j] )
					{
						pChildAnims[i] = &animations[j];
					}
				}
			}
		}

	}
}

ComponentListItem::~ComponentListItem()
{
}

int ComponentListItem::init( FitIniFile& file )
{
	if ( !s_templateItem )
	{
		s_templateItem = new ComponentListItem( NULL );
		char animName[COMP_ANIMATION_COUNT][32];
		for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
		{
			sprintf( animName[i], "Animation%ld", i );
			s_templateItem->animations[i].init( &file, animName[i] );
		
		}
		
		s_templateItem->icon.init( &file, "ComponentIcon" );
		assignAnimation( file, 0, animName, &s_templateItem->icon );

		s_templateItem->iconOutline.init( &file, "ComponentIconOutlineRect" );

		s_templateItem->outline.init( &file, "ComponentEntryBox" );


		s_templateItem->name.init( &file, "ComponentNameText" );
		assignAnimation( file, 1, animName, &s_templateItem->name );

		s_templateItem->costText.init( &file, "ComponentCostText" );
		assignAnimation( file, 2, animName, &s_templateItem->costText );

		s_templateItem->heatText.init( &file, "ComponentHeatText" );
		assignAnimation( file, 3, animName, &s_templateItem->heatText );

		s_templateItem->costIcon.init( &file, "ComponentCostIcon" );
		assignAnimation( file, 4, animName, &s_templateItem->costIcon );

		s_templateItem->heatIcon.init( &file, "ComponentHeatIcon" );
		assignAnimation( file, 5, animName, &s_templateItem->heatIcon );

		s_templateItem->disabledText.init( &file, "ComponentDisabledText" );
		
	}

	return 0;
}

void ComponentListItem::assignAnimation( FitIniFile& file, int whichChild, char animNames[COMP_ANIMATION_COUNT][32], 
										aObject* object )
{
		s_templateItem->pChildAnims[whichChild] = 0;
		char tmpAnimName[32];
  		file.readIdString("Animation", tmpAnimName, 31);
		for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
		{
			if ( stricmp( animNames[i], tmpAnimName ) == 0 )
			{
				s_templateItem->pChildAnims[whichChild] = &s_templateItem->animations[i];
				break;
			}
		}

		s_templateItem->addChild( object );

}

void ComponentListItem::render( )
{
	aListItem::render();

	// draw rects, these aren't children
	outline.moveTo(globalX(), globalY());
	outline.setColor( animations[0].getCurrentColor( animations[0].getState() ) );
	outline.render();

	iconOutline.moveTo( globalX() + s_templateItem->iconOutline.left()
		, globalY() + s_templateItem->iconOutline.top());
	iconOutline.setColor( animations[0].getCurrentColor( animations[0].getState() ) );
	iconOutline.render();

	for ( int i = 0; i < 6; i++ )
	{
		if ( pChildAnims[i] )
		{
			long color = pChildAnims[i]->getCurrentColor( pChildAnims[i]->getState() );
			child(i)->setColor( color );
		}
	}


	if ( MechLabScreen::instance()->canAddComponent( pComponent ) )
	{
		icon.setColor( 0xff404040 );
	}
	else
		icon.setColor( 0xffffffff );

}

void	ComponentListItem::update()
{

	int bCanAdd = MechLabScreen::instance()->canAddComponent( pComponent );
	for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
	{
		animations[i].update();
	}
	bool isInside = pointInside( userInput->getMouseX(), userInput->getMouseY() );

	if ( state == aListItem::SELECTED ) 
	{
		if ( (userInput->isLeftClick() && isInside)
			|| ( ( animations[0].getState() != aAnimGroup::PRESSED )
			&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) ) )
		{
			setComponent();

			if ( bCanAdd )
			{
				soundSystem->playDigitalSample( LOG_WRONGBUTTON );
			}

			::helpTextID = IDS_HELP_COMP0 + pComponent->getID();

		}

		if ( !bCanAdd )
		{
			if ( animations[0].getState() != aAnimGroup::PRESSED )
			{
				for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
				{
					animations[i].setState( aAnimGroup::PRESSED );
					
				}
			}
			if ( isInside && ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) ) 
			{
				::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
			}
			disabledText.setText( "" );
		}
		else
		{
			for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
				animations[i].setState( aAnimGroup::DISABLED );

			if ( COMPONENT_TOO_HOT == bCanAdd )
			{
				disabledText.setText( IDS_MC_COMPONENT_TOO_HOT );
			}
			else if ( NO_MORE_ARMOR == bCanAdd )
			{
				disabledText.setText( IDS_MC_COMPONENT_TOO_MUCH_ARMOR );
			}
			else
				disabledText.setText( "" );


		}
		
		if ( !bCanAdd )
		{
			if ( userInput->isLeftDrag() && isInside &&
				pointInside( userInput->getMouseDragX(), userInput->getMouseDragY() )
				&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) )
				startDrag();
		}
	}
	else if ( isInside && !bCanAdd
		&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ))
	{
		if ( animations[0].getState() != aAnimGroup::HIGHLIGHT )
		{
			for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
			{
				animations[i].setState( aAnimGroup::HIGHLIGHT );
			}

			soundSystem->playDigitalSample( LOG_HIGHLIGHTBUTTONS );

		}
		state = aListItem::HIGHLITE;
		::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
	}
	else if ( !bCanAdd )
	{
		state = aListItem::ENABLED;
		for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
			animations[i].setState( aAnimGroup::NORMAL );

		disabledText.setText( "" );
	}
	else
	{
		state = DISABLED;

		if ( isInside
			&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ))
			::helpTextID = IDS_HELP_COMP0 + pComponent->getID();


		if ( (userInput->isLeftClick() && isInside
			&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) ) )
		{
			setComponent();
			soundSystem->playDigitalSample( LOG_WRONGBUTTON );
		}

		for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
			animations[i].setState( aAnimGroup::DISABLED );

		if ( COMPONENT_TOO_HOT == bCanAdd )
		{
			disabledText.setText( IDS_MC_COMPONENT_TOO_HOT );
		}
		else if ( NO_MORE_ARMOR == bCanAdd )
		{
			disabledText.setText( IDS_MC_COMPONENT_TOO_MUCH_ARMOR );
		}
		else
			disabledText.setText( "" );
	}

	if ( userInput->isLeftDoubleClick() && isInside 
		&& ComponentIconListBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) )
	{
		//setComponent();
		doAdd();
		//MechLabScreen::instance()->handleMessage( 111, 52);
	}

	aObject::update();
}


void ComponentListItem::doAdd()
{
	long x = -1;
	long y = -1;
	MechLabScreen::instance()->addComponent( pComponent, x, y);
	
}

void ComponentListItem::setComponent()
{
	MechLabScreen::instance()->setComponent( pComponent, 1 );
}

void ComponentListItem::startDrag()
{
	MechLabScreen::instance()->beginDrag( pComponent );
	MechLabScreen::instance()->setDragFromList( 1 ); //M3
}
//*************************************************************************************************

void ComponentIconListBox::setType( int newType, int otherNewType, int orThis )
{
	if ( newType == type && itemCount )
		return;

	type = newType;
	scrollBar->setGreen();

	
	removeAllItems( 0 );

	itemSelected = -1;

	
	//if ( !masterComponentList.Count() ) //M*2
	//{ //M*2
	/* M14
		if (firstpass)
		{
			MaxCinv = pIcount = 256;
			LogisticsData::instance->getAllInventoryComponents( Cinv, MaxCinv ); 
			LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );
			firstpass = false;
		}
	*/ //M14
		masterComponentList.Clear(); //M*2
		//int count = 256; //M9 global in header
		//int count = 256;
		//LogisticsComponent* pComp[256]; //M9 gobal in header
		//LogisticsData::instance->getAllComponents( pComp, count );
		//Mbegin
		//LogisticsData::instance->getAllInventoryComponents( pComp, count ); //M
		//getIComponent( pComp, count );
		//MechLabScreen::instance()->getCinventory(pIComp, pIcount);
		//Mend
		//for ( int j = 0; j < 2048; j++ )
		//{
			for ( int i = 0; i < pIcount; i++ )
			{
				ComponentListItem* pItem = new ComponentListItem( pIComp[i] );
				masterComponentList.Append( pItem );	
			}			
	

	//	for ( EList< ComponentListItem*, ComponentListItem* >::EIterator iter = masterComponentList.Begin();
	//	!iter.IsDone(); iter++ )
	//	{
	///		delete *iter;
	//		turn++;
	//	}

	//	masterComponentList.Clear();
	//	}
	//} //M*2

	for ( EList< ComponentListItem*, ComponentListItem* >::EIterator iter = masterComponentList.Begin();
		!iter.IsDone(); iter++ )
		{
			//if ( (*iter)->getComponent()->isAvailable() )
			//{
				if ( (*iter)->getComponent()->getType() == type || 
					(*iter)->getComponent()->getType() == otherNewType ||
					 (*iter)->getComponent()->getType() == orThis )
				{
					if ( (*iter)->globalX() > globalX() )
					{
						(*iter)->move( -globalX(), 0 );
					}
					addSortedItem( (*iter) );
				}
			//}
		}

	selectFirstAvailableComponent();

}

void ComponentIconListBox::setType2( int newType, int otherNewType, int orThis, int plusThis, int Type5, int Type6 )
//new function magic 08022012
{
	if ( newType == type && itemCount )
		return;

	type = newType;
	scrollBar->setGreen();

	removeAllItems( 0 );
	itemSelected = -1;

	masterComponentList.Clear(); //M*2

	for ( int i = 0; i < pIcount; i++ )
	{
		ComponentListItem* pItem = new ComponentListItem( pIComp[i] );
		masterComponentList.Append( pItem );	
	}			

	for ( EList< ComponentListItem*, ComponentListItem* >::EIterator iter = masterComponentList.Begin();
		!iter.IsDone(); iter++ )
		{

				if ( (*iter)->getComponent()->getType() == type || 
					(*iter)->getComponent()->getType() == otherNewType ||
					 (*iter)->getComponent()->getType() == orThis ||
					 (*iter)->getComponent()->getType() == plusThis ||
					 (*iter)->getComponent()->getType() == Type5 ||
					 (*iter)->getComponent()->getType() == Type6
					 )
				{
					if ( (*iter)->globalX() > globalX() )
					{
						(*iter)->move( -globalX(), 0 );
					}
					addSortedItem( (*iter) );
				}

		}

	selectFirstAvailableComponent();

}

void ComponentIconListBox::update()
{
	aListBox::update();

	if ( itemSelected != -1 )
	{
		if ( items[itemSelected]->getState() == aListItem::DISABLED
			|| MechLabScreen::instance()->canAddComponent( 
			((ComponentListItem*)items[itemSelected])->getComponent() ) )
		{
			selectFirstAvailableComponent();
		}
	}
} 

int ComponentIconListBox::selectFirstAvailableComponent()
{
	bool bFound = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( items[i]->getState() != aListItem::DISABLED
			&& !MechLabScreen::instance()->canAddComponent( 
			((ComponentListItem*)items[i])->getComponent() ))
		{
			SelectItem( i );
			((ComponentListItem*)items[i])->setComponent();
			bFound = true;
			return i;
		}
	}

	if ( !bFound )
		SelectItem( -1 );

	return -1;
}

ComponentIconListBox::ComponentIconListBox()
{
	skipAmount = 5;
	type = -1;
	s_instance = this;
	firstpass = true;
	//MaxCinv = pIcount = 256;
	//LogisticsData::instance->getAllInventoryComponents( Cinv, MaxCinv ); 
	//LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );

}

ComponentIconListBox::~ComponentIconListBox()
{
	masterComponentList.Clear();

	s_instance = NULL;

	delete ComponentListItem::s_templateItem;
	ComponentListItem::s_templateItem = NULL;
}

LogisticsComponent* ComponentIconListBox::getComponent()
{
	if ( itemSelected != -1 )
	{
		return ((ComponentListItem*)items[itemSelected])->pComponent;
	}

	return NULL;
}

long ComponentIconListBox::getCostOfSelected()
{
	if ( itemSelected != -1 )
	{
		return ((ComponentListItem*)items[itemSelected])->pComponent->getCost();
	}

	return NULL;
}

void ComponentIconListBox::addSortedItem( ComponentListItem* pItem )
{
	int size = pItem->getComponent()->getComponentHeight() * 
				pItem->getComponent()->getComponentWidth();

	for ( int i = 0; i < itemCount; i++ )
	{
		LogisticsComponent* pTmp = ((ComponentListItem*)items[i])->getComponent();
		long tmpSize = pTmp->getComponentHeight() * pTmp->getComponentWidth();
		if ( size > tmpSize )
		{
			InsertItem( pItem, i );
			return;
		}
		else if ( size == tmpSize && 
			stricmp( pItem->getComponent()->getName(), pTmp->getName() ) < 0 )
		{
			InsertItem( pItem, i );
			return;
		}
	}

	
	aListBox::AddItem( pItem );
}

void ComponentIconListBox::removeIComponent( LogisticsComponent* pComp )
{
	int comCount = pIcount;
	if (comCount > 0)
	{
	for (int i = 0; i < comCount; i++)
	{
		if ((pIComp[i]->getID() == pComp->getID()) && (i < comCount-1))
		{
			pIComp[i] = pIComp[comCount-1];
			pIComp[comCount-1] = NULL;
			pIcount--;
			break;
		}
		if ((pIComp[i]->getID() == pComp->getID()) && (i == comCount-1))
		{
			pIComp[comCount-1] = NULL;
			pIcount--;
		}
	}
	}
}
void ComponentIconListBox::addIComponent( LogisticsComponent* pComp )
{
	pIComp[pIcount] = pComp;
	pIcount++;

}
void ComponentIconListBox::getIComponent( LogisticsComponent** pComp, int& maxCount )
{
		if (pIcount > 0)
		{
			for ( int i = 0; i < pIcount; i++ )
			{
				pComp[i] = pIComp[i];
				maxCount = pIcount;
			}
		}
}
void ComponentIconListBox::getCinv( LogisticsComponent** pComp, int& maxCount )
{
		if (MaxCinv > 0)
		{
			for ( int i = 0; i < MaxCinv; i++ )
			{
				pComp[i] = Cinv[i];
				maxCount = MaxCinv;
			}
		}
}
void ComponentIconListBox::reType()
{

	int otherNewType;
	int orThis;

	//magic 08022012 begin
	int Type4;
	int Type5;
	int Type6;

	switch( type )
	{
			case	5:
				{
					otherNewType = 10;
					orThis = 11;
					Type4 = 16;
					Type5 = 17;
					Type6 = 19;
				}
				break;
			case	10:
				{
					otherNewType = 5;
					orThis = 11;
					Type4 = 16;
					Type5 = 17;
					Type6 = 19;
				}
				break;
			case	11:
				{
					otherNewType = 5;
					orThis = 10;
					Type4 = 16;
					Type5 = 17;
					Type6 = 19;
				}
				break;
			case	16:
				{
					otherNewType = 5;
					orThis = 10;
					Type4 = 11;
					Type5 = 17;
					Type6 = 19;
				}
				break;
			case	17:
				{
					otherNewType = 5;
					orThis = 10;
					Type4 = 11;
					Type5 = 16;
					Type6 = 19;
				}
				break;
			case	19:
				{
					otherNewType = 5;
					orThis = 10;
					Type4 = 11;
					Type5 = 16;
					Type6 = 17;
				}
				break;
			
	}
	//magic 08022012 end
	removeAllItems( 0 );

	itemSelected = -1;

		masterComponentList.Clear(); 

			for ( int i = 0; i < pIcount; i++ )
			{
				ComponentListItem* pItem = new ComponentListItem( pIComp[i] );
				masterComponentList.Append( pItem );	
			}			

	for ( EList< ComponentListItem*, ComponentListItem* >::EIterator iter = masterComponentList.Begin();
		!iter.IsDone(); iter++ )
		{

				if ( (*iter)->getComponent()->getType() == type  || 
					 (*iter)->getComponent()->getType() == otherNewType ||
					 (*iter)->getComponent()->getType() == orThis || 
					 //magic 08022012 begin
					 (*iter)->getComponent()->getType() == Type4 ||
					 (*iter)->getComponent()->getType() == Type5 ||
					 (*iter)->getComponent()->getType() == Type6 
					 //magic 08022012 end
					 ) 

				{
					if ( (*iter)->globalX() > globalX() )
					{
						(*iter)->move( -globalX(), 0 );
					}
					addSortedItem( (*iter) );
				}

		}

	selectFirstAvailableComponent();

}

void ComponentIconListBox::saveInv( bool change )
{
	if (change)
		{
		LogisticsData::instance->UpdateCinventory( pIComp, pIcount );
		LogisticsData::instance->getAllInventoryComponents( Cinv, MaxCinv ); 
		//LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );
		}
	else
		getCinv( pIComp, pIcount );


}

void ComponentIconListBox::savePurInv( bool change )
{
	if (change)
		{
		LogisticsData::instance->UpdatePinventory( pIComp, pIcount );
		LogisticsData::instance->getAllPurchaseComponents( Cinv, MaxCinv ); 
		//LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );
		}
	else
		getCinv( pIComp, pIcount );


}
void ComponentIconListBox::initComponents( )
{
	MaxCinv = pIcount = 256;
	LogisticsData::instance->getAllInventoryComponents( Cinv, MaxCinv ); 
	LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );

}

void ComponentIconListBox::initPurComponents( )
{
	MaxCinv = pIcount = 256;
	LogisticsData::instance->getAllPurchaseComponents( Cinv, MaxCinv ); 
	LogisticsData::instance->getAllPurchaseComponents( pIComp, pIcount );

}
int ComponentIconListBox::selectFirstPurComponent()
{
	bool bFound = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( items[i]->getState() != aListItem::DISABLED)
		{
			SelectItem( i );
			((ComponentListItem*)items[i])->setComponent();
			bFound = true;
			return i;
		}
	}

	if ( !bFound )
		SelectItem( -1 );

	return -1;
}
void ComponentIconListBox::disableCThatCostMoreThanRP()
{
	bool bDisabledSel = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( ((ComponentListItem*)items[i])->pComponent->getCost() > LogisticsData::instance->getCBills() )
		{
			items[i]->setState( aListItem::DISABLED );
			if ( itemSelected == i )
				bDisabledSel = true;
		}
		else
		{
			if ( items[i]->getState() == aListItem::DISABLED )
				items[i]->setState( aListItem::ENABLED );
		}
	}

	if ( bDisabledSel )
	{
		for ( i = 0; i < itemCount; i++ )
		{
			if ( items[i]->getState() != aListItem::DISABLED )
			{
				SelectItem( i );
				bDisabledSel = 0;
				break;
			}
		}

		if ( bDisabledSel )
			SelectItem( -1 );
	}
}
//*************************************************************************************************
// end of file ( ComponentListBox.cpp )
