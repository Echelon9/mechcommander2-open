#define COMPONENTPURBOX_CPP
/*************************************************************************************************\
ComponentPurBox.cpp			: Implementation of the ComponentPurBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "ComponentPurBox.h"
#include "LogisticsVariant.h"
#include "LogisticsComponent.h"
#include "..\resource.h"
#include "malloc.h"
#include "mcLib.h"
#include "LogisticsData.h"
//#include "MechLabScreen.h"
#include "ComponentPurchaseScreen.h"
#include "gamesound.h"
#include "logisticserrors.h"

ComponentPurItem* ComponentPurItem::s_templateItem = NULL;

ComponentIconPurBox* ComponentIconPurBox::s_instance = NULL;



///////////////////////////////////////////////////////


ComponentPurItem::ComponentPurItem( LogisticsComponent* pComp )
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

ComponentPurItem::~ComponentPurItem()
{
}

int ComponentPurItem::init( FitIniFile& file )
{
	if ( !s_templateItem )
	{
		s_templateItem = new ComponentPurItem( NULL );
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

void ComponentPurItem::assignAnimation( FitIniFile& file, int whichChild, char animNames[COMP_ANIMATION_COUNT][32], 
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

void ComponentPurItem::render( )
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


/*	if ( ComponentPurchaseScreen::instance()->canAddComponent( pComponent ) )
	{
		icon.setColor( 0xff404040 );
	}
	else
		icon.setColor( 0xffffffff );
*/
}

void	ComponentPurItem::update()
{

	int bCanAdd = false;//MechLabScreen::instance()->canAddComponent( pComponent );
	for ( int i = 0; i < COMP_ANIMATION_COUNT; i++ )
	{
		animations[i].update();
	}
	bool isInside = pointInside( userInput->getMouseX(), userInput->getMouseY() );

	if ( state == aListItem::SELECTED ) 
	{
		if ( (userInput->isLeftClick() && isInside)
			|| ( animations[0].getState() != aAnimGroup::PRESSED )
			&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ))
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
			if ( isInside && ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) ) 
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
		
		/*if ( !bCanAdd )
		{
			if ( userInput->isLeftDrag() && isInside &&
				pointInside( userInput->getMouseDragX(), userInput->getMouseDragY() )
				&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) )
				startDrag();
		}*/
	}
	else if ( isInside && !bCanAdd
		&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ))
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
			&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ))
			::helpTextID = IDS_HELP_COMP0 + pComponent->getID();


		if ( (userInput->isLeftClick() && isInside
			&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) ) )
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

/*	if ( userInput->isLeftDoubleClick() && isInside 
		&& ComponentIconPurBox::s_instance->pointInside( userInput->getMouseX(), userInput->getMouseY() ) )
	{
		//setComponent();
		//doAdd();
		ComponentPurchaseScreen::instance()->handleMessage( 111, 52);
	}
*/
	aObject::update();
}


/*void ComponentPurItem::doAdd()
{
	long x = -1;
	long y = -1;
//	ComponentPurchaseScreen::instance()->addComponent( pComponent, x, y);
	
}*/

void ComponentPurItem::setComponent()
{
	ComponentPurchaseScreen::instance()->setComponent( pComponent, 1 );
}

/*void ComponentPurItem::startDrag()
{
	//ComponentPurchaseScreen::instance()->beginDrag( pComponent );
	//ComponentPurchaseScreen::instance()->setDragFromList( 1 ); //M3
}*/
//*************************************************************************************************

void ComponentIconPurBox::setType( int newType, int otherNewType, int orThis )
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
				ComponentPurItem* pItem = new ComponentPurItem( pIComp[i] );
				masterComponentList.Append( pItem );	
			}			
	

	for ( EList< ComponentPurItem*, ComponentPurItem* >::EIterator iter = masterComponentList.Begin();
		!iter.IsDone(); iter++ )
		{

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
	
		}

	selectFirstAvailableComponent();

}

void ComponentIconPurBox::setType2( int newType, int otherNewType, int orThis, int plusThis, int Type5, int Type6 )
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
		ComponentPurItem* pItem = new ComponentPurItem( pIComp[i] );
		masterComponentList.Append( pItem );	
	}			

	for ( EList< ComponentPurItem*, ComponentPurItem* >::EIterator iter = masterComponentList.Begin();
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

void ComponentIconPurBox::update()
{
	aListBox::update();

	if ( itemSelected != -1 )
	{
		if ( items[itemSelected]->getState() == aListItem::DISABLED)
		{
			selectFirstAvailableComponent();
		}
	}
} 

int ComponentIconPurBox::selectFirstAvailableComponent()
{
	bool bFound = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( items[i]->getState() != aListItem::DISABLED)
		{
			SelectItem( i );
			((ComponentPurItem*)items[i])->setComponent();
			bFound = true;
			return i;
		}
	}

	if ( !bFound )
		SelectItem( -1 );

	return -1;
}

ComponentIconPurBox::ComponentIconPurBox()
{
	skipAmount = 5;
	type = -1;
	s_instance = this;
	firstpass = true;
}

ComponentIconPurBox::~ComponentIconPurBox()
{
	masterComponentList.Clear();

	s_instance = NULL;

	delete ComponentPurItem::s_templateItem;
	ComponentPurItem::s_templateItem = NULL;
}

LogisticsComponent* ComponentIconPurBox::getComponent()
{
	if ( itemSelected != -1 )
	{
		return ((ComponentPurItem*)items[itemSelected])->pComponent;
	}

	return NULL;
}

void ComponentIconPurBox::addSortedItem( ComponentPurItem* pItem )
{
	int size = pItem->getComponent()->getComponentHeight() * 
				pItem->getComponent()->getComponentWidth();

	for ( int i = 0; i < itemCount; i++ )
	{
		LogisticsComponent* pTmp = ((ComponentPurItem*)items[i])->getComponent();
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

void ComponentIconPurBox::removeIComponent( LogisticsComponent* pComp )
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
void ComponentIconPurBox::addIComponent( LogisticsComponent* pComp )
{
	pIComp[pIcount] = pComp;
	pIcount++;

}
void ComponentIconPurBox::getIComponent( LogisticsComponent** pComp, int& maxCount )
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
void ComponentIconPurBox::getCinv( LogisticsComponent** pComp, int& maxCount )
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
void ComponentIconPurBox::reType()
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
				ComponentPurItem* pItem = new ComponentPurItem( pIComp[i] );
				masterComponentList.Append( pItem );	
			}			

	for ( EList< ComponentPurItem*, ComponentPurItem* >::EIterator iter = masterComponentList.Begin();
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

void ComponentIconPurBox::saveInv( bool change )
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

void ComponentIconPurBox::savePurInv( bool change )
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
void ComponentIconPurBox::initComponents( )
{
	MaxCinv = pIcount = 256;
	LogisticsData::instance->getAllInventoryComponents( Cinv, MaxCinv ); 
	LogisticsData::instance->getAllInventoryComponents( pIComp, pIcount );

}

void ComponentIconPurBox::initPurComponents( )
{
	MaxCinv = pIcount = 256;
	LogisticsData::instance->getAllPurchaseComponents( Cinv, MaxCinv ); //Magic 72 disabled
	LogisticsData::instance->getAllPurchaseComponents( pIComp, pIcount );

}
int ComponentIconPurBox::selectFirstPurComponent()
{
	bool bFound = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( items[i]->getState() != aListItem::DISABLED)
		{
			SelectItem( i );
			((ComponentPurItem*)items[i])->setComponent();
			bFound = true;
			return i;
		}
	}

	if ( !bFound )
		SelectItem( -1 );

	return -1;
}
void ComponentIconPurBox::disableCThatCostMoreThanRP()
{
	bool bDisabledSel = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( ((ComponentPurItem*)items[i])->pComponent->getCost() > LogisticsData::instance->getCBills() )
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

int ComponentIconPurBox::selectComponent(long componentNumber) //magic 19092011 new function
{
	
	if (componentNumber < GetItemCount())
	{
		if ( items[componentNumber]->getState() != aListItem::DISABLED)
		{
			SelectItem( componentNumber );
			((ComponentPurItem*)items[componentNumber])->setComponent();
			
			return componentNumber;
		}
		//magic 02102011 begin
		else
			return selectFirstAvailableComponent();
		//magic 02102011 end
	}
	else
		return selectFirstAvailableComponent();
}
//*************************************************************************************************
// end of file ( ComponentPurBox.cpp )
