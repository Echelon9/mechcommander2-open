#define MECHLISTBOX_CPP
/*************************************************************************************************\
MechListBox.cpp			: Implementation of the MechListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "MechListBox.h"
#include "LogisticsMech.h"
#include "paths.h"
#include "iniFile.h"
#include "err.h"
#include "userInput.h"
#include "MechBayScreen.h"
#include "LogisticsData.h"
#include "MechPurchaseScreen.h"
#include "gameSound.h"

MechListBoxItem* MechListBoxItem::s_templateItem = NULL;

bool MechListBox::s_DrawCBills = true;
bool MechListBoxItem::bAddCalledThisFrame = 0;



MechListBox::MechListBox( bool bDel, bool bInclude  )
{
	bDeleteIfNoInventory = bDel;
	bIncludeForceGroup = bInclude;
	bOrange = 0;
	skipAmount = 5;
}

//-------------------------------------------------------------------------------------------------

MechListBox::~MechListBox()
{
	removeAllItems( true );

	delete MechListBoxItem::s_templateItem;
	MechListBoxItem::s_templateItem = NULL;
}

void	MechListBox::setScrollBarOrange()
{
	scrollBar->setOrange();
}
void	MechListBox::setScrollBarGreen()
{
	scrollBar->setGreen();
}

void	MechListBox::drawCBills( bool bDraw )
{
	s_DrawCBills = bDraw;
}

void MechListBox::update()
{
	aListBox::update();
	MechListBoxItem::bAddCalledThisFrame = false;
	

	if ( bDeleteIfNoInventory )
	{
		for ( int i = 0; i < itemCount; i++ )
		{
			if ( ((MechListBoxItem*)items[i])->mechCount <= 0 )
			{
				RemoveItem( items[i], true );
				//i--;
				//disableItemsWith0Q();
				disableItemsThatCanNotGoInFG();


				// find better thing to select if necessary
				if ( itemSelected >= itemCount || itemSelected == -1 
					|| items[itemSelected]->getState() == aListItem::DISABLED
				//	|| !LogisticsData::instance->canAddMechToForceGroup( ((MechListBoxItem*)items[itemSelected])->getMech()  )
				)
				{
					if ( itemCount )
					{
						for ( int j = 0; j < itemCount; j++ )
							if ( items[j]->getState() != aListItem::DISABLED )
							{
								SelectItem( j );
								break;
							}
						
					}
					else
						itemSelected = -1;					
				}
				
			}
			//magic 20082012 begin
			else
			{
			char newDamage[8] = {0,0,0,0,0,0,0,0};

			float * baseArmArray = ((MechListBoxItem*)items[i])->pMech->getBaseArmorArray();
			float * curArmArray = ((MechListBoxItem*)items[i])->pMech->getCurArmorArray();
			unsigned char * maxIntArray = ((MechListBoxItem*)items[i])->pMech->getMaxInternalArray();
			float * curIntArray = ((MechListBoxItem*)items[i])->pMech->getCurInternalArray();
			float curArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
			float curInt[8] = {0,0,0,0,0,0,0,0};
			float baseArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
			float baseInt[8] = {0,0,0,0,0,0,0,0};

			for (int i1=0; i1<8; i1++)
			{
				baseArm[i1] += (baseArmArray[i1] + (float)maxIntArray[i1]);
				curArm[i1] += (curArmArray[i1] + curIntArray[i1]);
			}
	
			baseArm[1] += (baseArmArray[1] + baseArmArray[8]); //MECH_ARMOR_LOCATION_CTORSO + MECH_ARMOR_LOCATION_RCTORSO
			curArm[1] += (curArmArray[1] + curArmArray[8]);
			baseArm[2] += (baseArmArray[2] + baseArmArray[9]); //MECH_ARMOR_LOCATION_LTORSO + MECH_ARMOR_LOCATION_RLTORSO
			curArm[2] += (curArmArray[2] + curArmArray[9]);
			baseArm[3] += (baseArmArray[3] + baseArmArray[10]); //MECH_ARMOR_LOCATION_RTORSO + MECH_ARMOR_LOCATION_RRTORSO
			curArm[3] += (curArmArray[3] + curArmArray[10]);

			//head
			float tmp = curArm[0]/baseArm[0];

			if ( tmp <= .0 )
				newDamage[7] = 4;
			else if ( tmp < .35 )
				newDamage[7] = 3;
			else if ( tmp < .5 )
				newDamage[7] = 2;
			else if ( tmp < .75 )
				newDamage[7] = 1;

			//Ctorso
			tmp = curArm[1]/baseArm[1];

			if ( tmp <= .0 )
				newDamage[6] = 4;
			else if ( tmp < .35 )
				newDamage[6] = 3;
			else if ( tmp < .5 )
				newDamage[6] = 2;
			else if ( tmp < .75 )
				newDamage[6] = 1;

			//Ltorso
			tmp = curArm[2]/baseArm[2];

			if ( tmp <= .0 )
				newDamage[5] = 4;
			else if ( tmp < .35 )
				newDamage[5] = 3;
			else if ( tmp < .5 )
				newDamage[5] = 2;
			else if ( tmp < .75 )
				newDamage[5] = 1;

			//Rtorso
			tmp = curArm[3]/baseArm[3];

			if ( tmp <= .0 )
				newDamage[4] = 4;
			else if ( tmp < .35 )
				newDamage[4] = 3;
			else if ( tmp < .5 )
				newDamage[4] = 2;
			else if ( tmp < .75 )
				newDamage[4] = 1;

			//Lleg
			tmp = curArm[6]/baseArm[6];

			if ( tmp <= .0 )
				newDamage[0] = 4;
			else if ( tmp < .35 )
				newDamage[0] = 3;
			else if ( tmp < .5 )
				newDamage[0] = 2;
			else if ( tmp < .75 )
				newDamage[0] = 1;

			//Rleg
			tmp = curArm[7]/baseArm[7];

			if ( tmp <= .0 )
				newDamage[1] = 4;
			else if ( tmp < .35 )
				newDamage[1] = 3;
			else if ( tmp < .5 )
				newDamage[1] = 2;
			else if ( tmp < .75 )
				newDamage[1] = 1;

			//Larm
			tmp = curArm[4]/baseArm[4];

			if ( tmp <= .0 )
				newDamage[2] = 4;
			else if ( tmp < .35 )
				newDamage[2] = 3;
			else if ( tmp < .5 )
				newDamage[2] = 2;
			else if ( tmp < .75 )
				newDamage[2] = 1;

			//Rarm
			tmp = curArm[5]/baseArm[5];

			if ( tmp <= .0 )
				newDamage[3] = 4;
			else if ( tmp < .35 )
				newDamage[3] = 3;
			else if ( tmp < .5 )
				newDamage[3] = 2;
			else if ( tmp < .75 )
				newDamage[3] = 1;


			//char * oldDamage = ((MechListBoxItem*)items[i])->damage;
			//((MechListBoxItem*)items[i])->mechIcon.doDraw( newDamage, ((MechListBoxItem*)items[i])->damage, 0, 0, 0 );
			((MechListBoxItem*)items[i])->mechIcon.doDraw2( newDamage );

		} //magic 20082012 end
		}
	}
}

LogisticsMech* MechListBox::getCurrentMech()
{
	if ( itemSelected != -1 )
	{
		return ((MechListBoxItem*)items[itemSelected])->pMech;
	}

	return 0;
}

LogisticsMech* MechListBox::getListMech(int itemNum)
{
	if ( itemNum != -1 )
	{
		return ((MechListBoxItem*)items[itemNum])->pMech;
	}

	return 0;
}

int MechListBox::init()
{
	if ( MechListBoxItem::s_templateItem )
		return 0;


	char path[256];
	strcpy( path, artPath );
	strcat( path, "mcl_gn_availablemechentry.fit" );
	/*//magic 16092011 begin
	if ( Environment.screenWidth == 800 ) 
		strcat( path, "mcl_gn_availablemechentry.fit" );
	if ( Environment.screenWidth == 1280 )
		strcat( path, "mcl_gn_availablemechentry_1280x1024.fit" );
	if ( Environment.screenWidth == 1440 )
		strcat( path, "mcl_gn_availablemechentry.fit" );
	//magic 16092011 end*/
	FitIniFile file;
	if ( NO_ERR != file.open( path ) )
	{
		char errorStr[256];
		sprintf( errorStr, "couldn't open file %s", path );
		Assert( 0, 0, errorStr );
		return -1;
	}

	MechListBoxItem::init( file );
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
bool	MechListBoxItem::pointInside(long xPos, long yPos) const
{

	int minX = location[0].x + outline.globalX();
	int minY = location[0].y + outline.globalY();
	int maxX = location[0].x + outline.globalX() + outline.width();
	int maxY = location[0].y + outline.globalY() + outline.height();

	if ( minX < xPos && xPos < maxX
		&& minY < yPos && yPos < maxY )
		return true;

	return 0;
}
MechListBoxItem::MechListBoxItem( LogisticsMech* pRefMech, long count )
{
	bIncludeForceGroup = 0;
	bOrange = 0;
	mechCount = count; //1;
	if ( s_templateItem )
	{
		*this = *s_templateItem;
	}

	animTime = 0.f;
	/*
	//magic 11052012 begin
	memset( damage, 0, 8 );
	//magic 11052012 end
	*/ //magic 21082012 disabled
	pMech = pRefMech;
	if ( !pMech )
		return;

	aObject::init( 0, outline.top(), outline.width(), outline.height() );
	setColor( 0, 0 );

	//chassisName.setText( pMech->getChassisName() ); //magic 31102011 disabled
	//magic 31102011 begin
	//EString vNameTxt = pMech->getName();
	//int blankPos = vNameTxt.Find(" ");
	//chassisName.setText(vNameTxt.SubString(0,blankPos));
	chassisName.setText(pMech->getRealChassisName());
	//magic 31102011 end
	
	char text[32];
	sprintf( text, "%ld", pMech->getCost() );

	costText.setText( text );

	//mechCount = LogisticsData::instance->getVariantsInInventory( pRefMech->getVariant(), bIncludeForceGroup );//M12
	sprintf( text, "%ld", mechCount );
	countText.setText( text );


	//MechListBox::initIcon( pRefMech, mechIcon ); //magic 11052012 here is icon initialized
	/*
	//magic 12052012 begin
	char newDamage[8] = {0,0,0,0,0,0,0,0};
	//memset( newDamage, 0, 8 );

	float * baseArmArray = pMech->getBaseArmorArray();
	float * curArmArray = pMech->getCurArmorArray();
	unsigned char * maxIntArray = pMech->getMaxInternalArray();
	float * curIntArray = pMech->getCurInternalArray();
	float curArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float curInt[8] = {0,0,0,0,0,0,0,0};
	float baseArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float baseInt[8] = {0,0,0,0,0,0,0,0};

	for (int i=0; i<8; i++)
	{
		baseArm[i] += (baseArmArray[i] + (float)maxIntArray[i]);
		curArm[i] += (curArmArray[i] + curIntArray[i]);
	}
	
	baseArm[1] += (baseArmArray[1] + baseArmArray[8]); //MECH_ARMOR_LOCATION_CTORSO + MECH_ARMOR_LOCATION_RCTORSO
	curArm[1] += (curArmArray[1] + curArmArray[8]);
	baseArm[2] += (baseArmArray[2] + baseArmArray[9]); //MECH_ARMOR_LOCATION_LTORSO + MECH_ARMOR_LOCATION_RLTORSO
	curArm[2] += (curArmArray[2] + curArmArray[9]);
	baseArm[3] += (baseArmArray[3] + baseArmArray[10]); //MECH_ARMOR_LOCATION_RTORSO + MECH_ARMOR_LOCATION_RRTORSO
	curArm[3] += (curArmArray[3] + curArmArray[10]);


	//head
		float tmp = curArm[0]/baseArm[0];

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[7] = 4;
		else if ( tmp < .35 )
			newDamage[7] = 3;
		else if ( tmp < .5 )
			newDamage[7] = 2;
		else if ( tmp < .75 )
			newDamage[7] = 1;
		//magic 13052012 end

	//Ctorso
		tmp = curArm[1]/baseArm[1];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[6] = 4;
		else if ( tmp < .35 )
			newDamage[6] = 3;
		else if ( tmp < .5 )
			newDamage[6] = 2;
		else if ( tmp < .75 )
			newDamage[6] = 1;
		//magic 13052012 end

	//Ltorso
		tmp = curArm[2]/baseArm[2];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[5] = 4;
		else if ( tmp < .35 )
			newDamage[5] = 3;
		else if ( tmp < .5 )
			newDamage[5] = 2;
		else if ( tmp < .75 )
			newDamage[5] = 1;
		//magic 13052012 end

	//Rtorso
		tmp = curArm[3]/baseArm[3];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[4] = 4;
		else if ( tmp < .35 )
			newDamage[4] = 3;
		else if ( tmp < .5 )
			newDamage[4] = 2;
		else if ( tmp < .75 )
			newDamage[4] = 1;
		//magic 13052012 end

	//Lleg
		tmp = curArm[6]/baseArm[6];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[0] = 4;
		else if ( tmp < .35 )
			newDamage[0] = 3;
		else if ( tmp < .5 )
			newDamage[0] = 2;
		else if ( tmp < .75 )
			newDamage[0] = 1;
		//magic 13052012 end

	//Rleg
		tmp = curArm[7]/baseArm[7];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[1] = 4;
		else if ( tmp < .35 )
			newDamage[1] = 3;
		else if ( tmp < .5 )
			newDamage[1] = 2;
		else if ( tmp < .75 )
			newDamage[1] = 1;
		//magic 13052012 end

	//Larm
		tmp = curArm[4]/baseArm[4];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[2] = 4;
		else if ( tmp < .35 )
			newDamage[2] = 3;
		else if ( tmp < .5 )
			newDamage[2] = 2;
		else if ( tmp < .75 )
			newDamage[2] = 1;
		//magic 13052012 end

	//Rarm
		tmp = curArm[5]/baseArm[5];


		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[3] = 4;
		else if ( tmp < .35 )
			newDamage[3] = 3;
		else if ( tmp < .5 )
			newDamage[3] = 2;
		else if ( tmp < .75 )
			newDamage[3] = 1;
		//magic 13052012 end
					


	char * oldDamage = damage;
	MechListBox::initIcon2( pMech->getIconIndex(), mechIcon, newDamage, oldDamage ); //magic 11052012
	//magic 12052012 end */ //magic 21082012 disabled

	//magic 21082012 begin
	char path[256];
	strcpy( path, artPath );
	strcat( path, "mechicons\\" );
	strcat( path, pMech->getRealChassisName() );
	strcat( path, ".tga" );
	_strlwr( path );

	//aObject* newMechIcon = new aObject((MechListBoxItem::s_templateItem->mechIcon));
	aObject* newMechIcon = new aObject();
	mechIcon = *newMechIcon;
	mechIcon.initUniq(4,5,25,30, path, pMech->getID()); //magic 21082012
	//magic 21082012 end

	variantName.setText( pMech->getName() );
	
	sprintf( text, "%.0lf", pMech->getMaxWeight() );
	weightText.setText( text );

	addChild( &weightIcon );
	addChild( &mechIcon );
	addChild( &costIcon );

	addChild( &chassisName );
	addChild( &weightText );
	addChild( &countText );
	addChild( &variantName );
	addChild( &costText );

	//	addChild( &line );
	//	addChild( &outline );

	bDim = 0;
	
}

MechListBoxItem::~MechListBoxItem()
{
	removeAllChildren( false );
}

void MechListBoxItem::init( FitIniFile& file )
{
	if ( !s_templateItem )
	{
		s_templateItem = new MechListBoxItem( NULL, 0 );
		
		file.seekBlock( "MainBox" );

		long width, height;

		file.readIdLong( "Width", width );
		file.readIdLong( "Height", height );

		((aObject*)s_templateItem)->init( 0, 0, width, height );

		memset( s_templateItem->animationIDs, 0, sizeof(long) * 9  );

		// rects
		s_templateItem->line.init( &file, "Rect1" );
		s_templateItem->outline.init( &file, "Rect0" );

		long curAnim = 0;
		// statics
		s_templateItem->weightIcon.init( &file, "Static0" );
		assignAnimation( file, curAnim );
	
		s_templateItem->mechIcon.init( &file, "Static1" );
		assignAnimation( file, curAnim );
		s_templateItem->costIcon.init( &file, "Static2" );
		assignAnimation( file, curAnim );

		// texts
		s_templateItem->chassisName.init( &file, "Text0" );
		assignAnimation( file, curAnim );
		s_templateItem->weightText.init( &file, "Text1" );
		assignAnimation( file, curAnim );
		s_templateItem->countText.init( &file, "Text2" );
		assignAnimation( file, curAnim );
		s_templateItem->variantName.init( &file, "Text3" );
		assignAnimation( file, curAnim );
		s_templateItem->costText.init( &file, "Text4" );
		assignAnimation( file, curAnim );

		char blockName[64];
		for ( int i = 0; i < 4; i++ )
		{
			sprintf( blockName, "OrangeAnimation%ld", i );
			s_templateItem->animations[1][i].init( &file, blockName );
			sprintf( blockName, "Animation%ld", i );
			s_templateItem->animations[0][i].init( &file, blockName );
		}
		
	}

}

void MechListBoxItem::assignAnimation( FitIniFile& file, long& curAnim )
{
	char tmpStr[64];

	s_templateItem->animationIDs[curAnim] = -1;
	if ( NO_ERR == file.readIdString( "Animation", tmpStr, 63 ) )
	{
		for ( int j = 0; j < strlen( tmpStr ); j++ )
		{
			if ( isdigit( tmpStr[j] ) )
			{
				tmpStr[j+1] = 0;
				s_templateItem->animationIDs[curAnim] = atoi( &tmpStr[j] );
			}
		}
	}
	curAnim++;
}
MechListBoxItem& MechListBoxItem::operator=( const MechListBoxItem& src )
{
	if ( &src != this )
	{
		chassisName = src.chassisName;
		costIcon = src.costIcon;
		costText = src.costText;
		line = src.line;
		mechIcon = src.mechIcon;
		outline = src.outline;
		variantName = src.variantName;
		weightIcon = src.weightIcon;
		weightText = src.weightText;
		countText = src.countText;
		for ( int i = 0; i < ANIMATION_COUNT; i++ )
		{
			animations[0][i] = src.animations[0][i];
			animations[1][i] = src.animations[1][i];
		}

		for ( i = 0; i < 9; i++ )
		{
			animationIDs[i] = src.animationIDs[i];
		}
	}

	return *this;
}

void MechListBoxItem::update()
{
	char text[32];
	//int oldMechCount = mechCount; //M14
	if ( !pMech )
	{
		mechCount = 0;
		return;
	}
	//mechCount = LogisticsData::instance->getVariantsInInventory( pMech->getVariant(), false );// M12 bIncludeForceGroup );
	//problem mechCount always looks for num variants in inventory - get variants in list box
	// reseno u funkcijama incMechCount i decMechCount
	
	//if ( oldMechCount != mechCount ) //M14
/* M14 removed
	if ( drawMechCount )
	{
		animTime = .0001f;
	}
*/ //M14
	sprintf( text, "%ld", mechCount );
	countText.setText( text );
	if ( animTime )
	{
		if ( animTime < .25f 
			|| ( animTime > .5f && animTime <= .75f ) )
		{
			countText.setColor( 0 );
		}
		else 
			countText.setColor( 0xffffffff );

		animTime += frameLength;
		
		if ( animTime > 1.0f )
			animTime = 0.f;
		
	}

	bool isInside = pointInside( userInput->getMouseX(), userInput->getMouseY() );


	for ( int i = 0; i < ANIMATION_COUNT; i++ )
		animations[bOrange][i].update();

	if ( state == aListItem::SELECTED ) 
	{
		for ( int i = 0; i < ANIMATION_COUNT; i++ )
			animations[bOrange][i].setState( aAnimGroup::PRESSED );

		if ( userInput->isLeftClick() && isInside )
	//		setMech();
		MechPurchaseScreen::instance()->setSelectedMech(pMech);//magic 13022011
		
		if ( userInput->isLeftDrag() &&
			pointInside( userInput->getMouseDragX(), userInput->getMouseDragY() ) )
			startDrag();

	}
	else if ( state == aListItem::HIGHLITE )
	{
		for ( int i = 0; i < ANIMATION_COUNT; i++ )
			animations[bOrange][i].setState( aAnimGroup::HIGHLIGHT );

	}
	else if ( state == aListItem::DISABLED &&  isShowing() )
	{
		if ( userInput->isLeftClick() && isInside )
		{
			soundSystem->playDigitalSample( LOG_WRONGBUTTON );	
			setMech(); // need to call explicitly
		}

		for ( int i = 0; i < ANIMATION_COUNT; i++ )
			animations[bOrange][i].setState( aAnimGroup::DISABLED );
	}
	else
	{
		for ( int i = 0; i < ANIMATION_COUNT; i++ )
			animations[bOrange][i].setState( aAnimGroup::NORMAL );
	}
//Magic 72 disabled
	if ( userInput->isLeftDoubleClick() && isInside && state != aListItem::DISABLED && isShowing() )
	//if ( userInput->isRightClick() && isInside && state != aListItem::DISABLED && isShowing())// && ( state == aListItem::SELECTED ) )
	{
		//setMech();
		//doAddMechBay();
		//doAdd();
		//MechPurchaseScreen::instance()->setSelectedMech(pMech);
		//MechPurchaseScreen::instance()->setSelectedMech(pMech);//magic 13022011
		//MechPurchaseScreen::instance()->handleMessage( ID, MB_MSG_ADD );
		//RemoveItem( pItem, true );
		//removeChild(this);
		MechPurchaseScreen::instance()->setActionClick();
		//resetMech();
		//mechCount--;
	}
//Magic 72 disabled

	//magic 21082012 begin
	char newDamage[8] = {0,0,0,0,0,0,0,0};

	float * baseArmArray = pMech->getBaseArmorArray();
	float * curArmArray = pMech->getCurArmorArray();
	unsigned char * maxIntArray = pMech->getMaxInternalArray();
	float * curIntArray = pMech->getCurInternalArray();
	float curArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float curInt[8] = {0,0,0,0,0,0,0,0};
	float baseArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float baseInt[8] = {0,0,0,0,0,0,0,0};

	for (int i=0; i<8; i++)
	{
		baseArm[i] += (baseArmArray[i] + (float)maxIntArray[i]);
		curArm[i] += (curArmArray[i] + curIntArray[i]);
	}
	
	baseArm[1] += (baseArmArray[1] + baseArmArray[8]); //MECH_ARMOR_LOCATION_CTORSO + MECH_ARMOR_LOCATION_RCTORSO
	curArm[1] += (curArmArray[1] + curArmArray[8]);
	baseArm[2] += (baseArmArray[2] + baseArmArray[9]); //MECH_ARMOR_LOCATION_LTORSO + MECH_ARMOR_LOCATION_RLTORSO
	curArm[2] += (curArmArray[2] + curArmArray[9]);
	baseArm[3] += (baseArmArray[3] + baseArmArray[10]); //MECH_ARMOR_LOCATION_RTORSO + MECH_ARMOR_LOCATION_RRTORSO
	curArm[3] += (curArmArray[3] + curArmArray[10]);


	//head
		float tmp = curArm[0]/baseArm[0];

		if ( tmp <= .0 )
			newDamage[7] = 4;
		else if ( tmp < .35 )
			newDamage[7] = 3;
		else if ( tmp < .5 )
			newDamage[7] = 2;
		else if ( tmp < .75 )
			newDamage[7] = 1;

	//Ctorso
		tmp = curArm[1]/baseArm[1];

		if ( tmp <= .0 )
			newDamage[6] = 4;
		else if ( tmp < .35 )
			newDamage[6] = 3;
		else if ( tmp < .5 )
			newDamage[6] = 2;
		else if ( tmp < .75 )
			newDamage[6] = 1;

	//Ltorso
		tmp = curArm[2]/baseArm[2];

		if ( tmp <= .0 )
			newDamage[5] = 4;
		else if ( tmp < .35 )
			newDamage[5] = 3;
		else if ( tmp < .5 )
			newDamage[5] = 2;
		else if ( tmp < .75 )
			newDamage[5] = 1;

	//Rtorso
		tmp = curArm[3]/baseArm[3];

		if ( tmp <= .0 )
			newDamage[4] = 4;
		else if ( tmp < .35 )
			newDamage[4] = 3;
		else if ( tmp < .5 )
			newDamage[4] = 2;
		else if ( tmp < .75 )
			newDamage[4] = 1;

	//Lleg
		tmp = curArm[6]/baseArm[6];

		if ( tmp <= .0 )
			newDamage[0] = 4;
		else if ( tmp < .35 )
			newDamage[0] = 3;
		else if ( tmp < .5 )
			newDamage[0] = 2;
		else if ( tmp < .75 )
			newDamage[0] = 1;

	//Rleg
		tmp = curArm[7]/baseArm[7];

		if ( tmp <= .0 )
			newDamage[1] = 4;
		else if ( tmp < .35 )
			newDamage[1] = 3;
		else if ( tmp < .5 )
			newDamage[1] = 2;
		else if ( tmp < .75 )
			newDamage[1] = 1;

	//Larm
		tmp = curArm[4]/baseArm[4];

		if ( tmp <= .0 )
			newDamage[2] = 4;
		else if ( tmp < .35 )
			newDamage[2] = 3;
		else if ( tmp < .5 )
			newDamage[2] = 2;
		else if ( tmp < .75 )
			newDamage[2] = 1;

	//Rarm
		tmp = curArm[5]/baseArm[5];

		if ( tmp <= .0 )
			newDamage[3] = 4;
		else if ( tmp < .35 )
			newDamage[3] = 3;
		else if ( tmp < .5 )
			newDamage[3] = 2;
		else if ( tmp < .75 )
			newDamage[3] = 1;

		//doDraw(newDamage, damage,0,0,0);
		doDraw2(newDamage); //magic 21082012
	//magic 21082012 end

	aObject::update();

}



void MechListBoxItem::render()
{
	if ( !MechListBox::s_DrawCBills )
	{
		costText.showGUIWindow( 0 );
		costIcon.showGUIWindow( 0 );
	}
	else
	{
		costText.showGUIWindow( 1 );
		costIcon.showGUIWindow( 1 );

	}
	//magic 08052012 begin
	bool armorDamaged = isArmorDamaged();
	bool internalDamaged = isInternalDamaged();
	//magic 08052012 end

	for ( int i = 0; i < this->pNumberOfChildren; i++ )
	{
		long index = animationIDs[i];
		if ( index != -1 )
		{
			if ( pChildren[i]->isShowing() )
			{
				if ( !animTime || pChildren[i] != &countText )
				{
					//long color = animations[bOrange][index].getCurrentColor( animations[bOrange][index].getState());
					//pChildren[i]->setColor( color ); //magic 11052012 removed
					//magic 22082012 begin
					if ( state == DISABLED )
						pChildren[i]->setColor( SD_GRAY );
					else
						pChildren[i]->setColor( 0xffffffff ); 
					//magic 22082012 end
				}

				
			}
		}
		pChildren[i]->render();

	}

	if ( bDim )
	{
		mechIcon.setColor( 0xa0000000 );
		mechIcon.render();
	}
	//magic 10052012 begin
	if (internalDamaged)
	{
		outline.setColor(SB_RED);
		line.setColor(SB_RED);
	}
	else
		if (armorDamaged)
		{
			outline.setColor(SB_BLUE );
			line.setColor(SB_BLUE);
		}
		else
		{
			outline.setColor(animations[bOrange][2].getCurrentColor(animations[bOrange][2].getState()));
			line.setColor(animations[bOrange][2].getCurrentColor(animations[bOrange][2].getState()));
		}
	//magic 10052012 end		
	//outline.setColor(animations[bOrange][2].getCurrentColor(animations[bOrange][2].getState()));
	outline.render( location[0].x, location[0].y );

	//line.setColor(animations[bOrange][2].getCurrentColor(animations[bOrange][2].getState()));
	line.render(location[0].x, location[0].y);
}

void MechListBoxItem::setMech()
{
	MechBayScreen::instance()->setMech( pMech );
	MechPurchaseScreen::instance()->setMech( pMech, true );
	
}

void MechListBoxItem::startDrag()
{
	/*
	if ( state != DISABLED )
	{
		MechBayScreen::instance()->beginDrag( pMech );
		MechPurchaseScreen::instance()->beginDrag( pMech );
	}
	*/ //magic 20082012 disabled drag
}

void MechListBoxItem::doAdd()
{
	if ( !bAddCalledThisFrame ) // only select one, sometimes we auto scroll, don't want to be selecting each time
	{
		MechBayScreen::instance()->handleMessage( ID, MB_MSG_ADD );
		MechPurchaseScreen::instance()->handleMessage( ID, MB_MSG_ADD );
		bAddCalledThisFrame = true;
	}
}

void MechListBoxItem::doAddMechBay()
{
	if ( !bAddCalledThisFrame ) // only select one, sometimes we auto scroll, don't want to be selecting each time
	{
		MechBayScreen::instance()->handleMessage( ID, MB_MSG_ADD );
		//MechPurchaseScreen::instance()->handleMessage( ID, MB_MSG_ADD );
		bAddCalledThisFrame = true;
	}
}
/*
void MechListBox::initIcon( LogisticsMech* pMech, aObject& mechIcon )
{
	mechIcon = (MechListBoxItem::s_templateItem->mechIcon);

	long index = pMech->getIconIndex();
	//long xIndex = index % 10;
	//long yIndex = index / 10;
	//long xIndex = index % 20; //magic 03032011 20 is number of mech icons in a row for 512
	//long yIndex = index / 20; //magic 03032011

	long xIndex = index % 25; //magic 08052012 25 is number of mech icons in a row for 1024 using mcui_high7
	long yIndex = index / 25; //magic 08052012

	float fX = xIndex;
	float fY = yIndex;

	//float width = mechIcon.width();
	//float height = mechIcon.height();

	//magic 08052012 begin
	float width = 40.0f;
	float height = 48.0f;
	//magic 08052012 end

	float u = (fX * width);
	float v = (fY * height);

	fX += 1.f;
	fY += 1.f;
	
	float u2 = (fX * width);
	float v2 = (fY * height);

	//mechIcon.setFileWidth(256.f);
	//mechIcon.setFileWidth(512.f); //magic 03032011
	mechIcon.setFileWidth(1024.f); //magic 08052012
	mechIcon.setUVs( u, v, u2, v2 );

}*/ //magic 12052012 original

void MechListBox::initIcon( LogisticsMech* pMech, aObject& mechIcon )
{
	mechIcon = (MechListBoxItem::s_templateItem->mechIcon);

	long index = pMech->getIconIndex();
	//long xIndex = index % 10;
	//long yIndex = index / 10;
	//long xIndex = index % 20; //magic 03032011 20 is number of mech icons in a row for 512
	//long yIndex = index / 20; //magic 03032011

	long xIndex = index % 25; //magic 08052012 25 is number of mech icons in a row for 1024 using mcui_high7
	long yIndex = index / 25; //magic 08052012

	float fX = xIndex;
	float fY = yIndex;

	//float width = mechIcon.width();
	//float height = mechIcon.height();

	//magic 08052012 begin
	float width = 40.0f;
	float height = 48.0f;
	//magic 08052012 end

	float u = (fX * width);
	float v = (fY * height);

	fX += 1.f;
	fY += 1.f;
	
	float u2 = (fX * width);
	float v2 = (fY * height);

	//mechIcon.setFileWidth(256.f);
	//mechIcon.setFileWidth(512.f); //magic 03032011
	mechIcon.setFileWidth(1024.f); //magic 08052012
	mechIcon.setUVs( u, v, u2, v2 );

		//magic 12052012 begin
	char newDamage[8] = {0,0,0,0,0,0,0,0};
	//memset( newDamage, 0, 8 );

	float * baseArmArray = pMech->getBaseArmorArray();
	float * curArmArray = pMech->getCurArmorArray();
	unsigned char * maxIntArray = pMech->getMaxInternalArray();
	float * curIntArray = pMech->getCurInternalArray();
	float curArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float curInt[8] = {0,0,0,0,0,0,0,0};
	float baseArm[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float baseInt[8] = {0,0,0,0,0,0,0,0};

	for (int i=0; i<8; i++)
	{
		baseArm[i] += (baseArmArray[i] + (float)maxIntArray[i]);
		curArm[i] += (curArmArray[i] + curIntArray[i]);
	}
	
	baseArm[1] += (baseArmArray[1] + baseArmArray[8]); //MECH_ARMOR_LOCATION_CTORSO + MECH_ARMOR_LOCATION_RCTORSO
	curArm[1] += (curArmArray[1] + curArmArray[8]);
	baseArm[2] += (baseArmArray[2] + baseArmArray[9]); //MECH_ARMOR_LOCATION_LTORSO + MECH_ARMOR_LOCATION_RLTORSO
	curArm[2] += (curArmArray[2] + curArmArray[9]);
	baseArm[3] += (baseArmArray[3] + baseArmArray[10]); //MECH_ARMOR_LOCATION_RTORSO + MECH_ARMOR_LOCATION_RRTORSO
	curArm[3] += (curArmArray[3] + curArmArray[10]);


	//head
		float tmp = curArm[0]/baseArm[0];
		/*if ( tmp <= .0 )
			newDamage[7] = 3;
		else if ( tmp < .4 )
			newDamage[7] = 2;
		else if ( tmp < .8 )
			newDamage[7] = 1;
		*/
		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[7] = 4;
		else if ( tmp < .35 )
			newDamage[7] = 3;
		else if ( tmp < .5 )
			newDamage[7] = 2;
		else if ( tmp < .75 )
			newDamage[7] = 1;
		//magic 13052012 end

	//Ctorso
		tmp = curArm[1]/baseArm[1];
		/*if ( tmp <= .0 )
			newDamage[6] = 3;
		else if ( tmp < .4 )
			newDamage[6] = 2;
		else if ( tmp < .8 )
			newDamage[6] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[6] = 4;
		else if ( tmp < .35 )
			newDamage[6] = 3;
		else if ( tmp < .5 )
			newDamage[6] = 2;
		else if ( tmp < .75 )
			newDamage[6] = 1;
		//magic 13052012 end

	//Ltorso
		tmp = curArm[2]/baseArm[2];
		/*if ( tmp <= .0 )
			newDamage[5] = 3;
		else if ( tmp < .4 )
			newDamage[5] = 2;
		else if ( tmp < .8 )
			newDamage[5] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[5] = 4;
		else if ( tmp < .35 )
			newDamage[5] = 3;
		else if ( tmp < .5 )
			newDamage[5] = 2;
		else if ( tmp < .75 )
			newDamage[5] = 1;
		//magic 13052012 end

	//Rtorso
		tmp = curArm[3]/baseArm[3];
		/*if ( tmp <= .0 )
			newDamage[4] = 3;
		else if ( tmp < .4 )
			newDamage[4] = 2;
		else if ( tmp < .8 )
			newDamage[4] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[4] = 4;
		else if ( tmp < .35 )
			newDamage[4] = 3;
		else if ( tmp < .5 )
			newDamage[4] = 2;
		else if ( tmp < .75 )
			newDamage[4] = 1;
		//magic 13052012 end

	//Lleg
		tmp = curArm[6]/baseArm[6];
		/*if ( tmp <= .0 )
			newDamage[0] = 3;
		else if ( tmp < .4 )
			newDamage[0] = 2;
		else if ( tmp < .8 )
			newDamage[0] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[0] = 4;
		else if ( tmp < .35 )
			newDamage[0] = 3;
		else if ( tmp < .5 )
			newDamage[0] = 2;
		else if ( tmp < .75 )
			newDamage[0] = 1;
		//magic 13052012 end

	//Rleg
		tmp = curArm[7]/baseArm[7];
		/*if ( tmp <= .0 )
			newDamage[1] = 3;
		else if ( tmp < .4 )
			newDamage[1] = 2;
		else if ( tmp < .8 )
			newDamage[1] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[1] = 4;
		else if ( tmp < .35 )
			newDamage[1] = 3;
		else if ( tmp < .5 )
			newDamage[1] = 2;
		else if ( tmp < .75 )
			newDamage[1] = 1;
		//magic 13052012 end

	//Larm
		tmp = curArm[4]/baseArm[4];
		/*if ( tmp <= .0 )
			newDamage[2] = 3;
		else if ( tmp < .4 )
			newDamage[2] = 2;
		else if ( tmp < .8 )
			newDamage[2] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[2] = 4;
		else if ( tmp < .35 )
			newDamage[2] = 3;
		else if ( tmp < .5 )
			newDamage[2] = 2;
		else if ( tmp < .75 )
			newDamage[2] = 1;
		//magic 13052012 end

	//Rarm
		tmp = curArm[5]/baseArm[5];
		/*if ( tmp <= .0 )
			newDamage[3] = 3;
		else if ( tmp < .4 )
			newDamage[3] = 2;
		else if ( tmp < .8 )
			newDamage[3] = 1;*/

		//magic 13052012 begin
		if ( tmp <= .0 )
			newDamage[3] = 4;
		else if ( tmp < .35 )
			newDamage[3] = 3;
		else if ( tmp < .5 )
			newDamage[3] = 2;
		else if ( tmp < .75 )
			newDamage[3] = 1;
		//magic 13052012 end
							/*//Magic Debug file open begin
							char tmp2[256];
							FitIniFile MtestFile;
							MtestFile.open("data\\initicon.fit", CREATE);
							//Magic Debug file write begin
							sprintf( tmp2, "max0 - cur0: %f - %f", baseArmArray[0],curArmArray[0]);
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max1- cur1: %f - %f", baseArmArray[1], curArmArray[1] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max2- cur2: %f - %f", baseArmArray[2], curArmArray[2] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max3- cur3: %f - %f", baseArmArray[3], curArmArray[3] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max4- cur4: %f - %f", baseArmArray[4], curArmArray[4]);
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max5- cur5: %f - %f", baseArmArray[5], curArmArray[5] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max6- cur6: %f - %f", baseArmArray[6], curArmArray[6] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max7- cur7: %f - %f", baseArmArray[7], curArmArray[7] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max8- cur8: %f - %f", baseArmArray[8], curArmArray[8] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max9- cur9: %f - %f", baseArmArray[9], curArmArray[9]  );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max10- cur10: %f - %f", baseArmArray[10], curArmArray[10]  );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "base armor sum - cur armor sum: ", pMech->getName() );
							MtestFile.writeBlock( tmp2 );
							MtestFile.close();
							//magic debug file write end*/



	//char * oldDamage = MechListBoxItem::s_templateItem->damage;
	char oldDamage[8] = {0,0,0,0,0,0,0,0}; //magic 21082012 damage removed from object
	//MechListBoxItem::s_templateItem->doDraw( newDamage, oldDamage, xIndex, yIndex, mechIcon.getTextureHandle(), index );
	//mechIcon.doDraw( newDamage, oldDamage, xIndex, yIndex, index );
	mechIcon.doDraw( newDamage, oldDamage, 0, 0, 0 ); //magic 21082012
	//magic 11052012 end

}
//magic 11052012 begin
void MechListBox::initIcon2( long index, aObject& mechIcon, char* newDamage, char* oldDamage )
{
	/*
	mechIcon = (MechListBoxItem::s_templateItem->mechIcon);

	//long index = pMech->getIconIndex();
	long xIndex = index % 25; //magic 08052012 25 is number of mech icons in a row for 1024 using mcui_high7
	long yIndex = index / 25; //magic 08052012

	float fX = xIndex;
	float fY = yIndex;

	//float width = mechIcon.width();
	//float height = mechIcon.height();

	//magic 08052012 begin
	float width = 40.0f;
	float height = 48.0f;
	//magic 08052012 end

	float u = (fX * width);
	float v = (fY * height);

	fX += 1.f;
	fY += 1.f;
	
	float u2 = (fX * width);
	float v2 = (fY * height);

	mechIcon.setFileWidth(1024.f); //magic 08052012
	mechIcon.setUVs( u, v, u2, v2 );
	*/ //magic 21082012 disabled
	
	//char * oldDamage = damage;
	//mechIcon.doDraw( newDamage, oldDamage, xIndex, yIndex, index );
	mechIcon.doDraw( newDamage, oldDamage, 0, 0, index ); //magic 21082012
	//magic 11052012 end

}
//magic 11052012 end

long MechListBox::AddItem(aListItem* itemString)
{
	itemString->setID( ID );
	MechListBoxItem* pItem = dynamic_cast<MechListBoxItem*>(itemString);
	EString addedName;
	char tmp[256];
	/*//magic 31102011 begin
	EString vNameTxt = pItem->getMech()->getName();
	long mNameID = pItem->getMech()->getChassisName();
	if (mNameID > 0)
	{
	//magic 31102011 end */
	cLoadString( pItem->getMech()->getChassisName(), tmp, 255 );
	addedName = tmp;
	/* //magic 31102011 begin
	}
	else
	{
		int blankPos = vNameTxt.Find(" ");
		addedName = vNameTxt.SubString(0,blankPos);
	}
	//magic 31102011 end */
	
	
	if ( pItem )
	{
		pItem->bOrange = bOrange;
		pItem->bIncludeForceGroup = bIncludeForceGroup;

		if ( !bDeleteIfNoInventory )
		{
			pItem->countText.setColor( 0 );
			pItem->countText.showGUIWindow( 0 );
		}

		EString chassisName;
		for ( int i = 0; i < itemCount; i++ )
		{

			//long ID = ((MechListBoxItem*)items[i])->pMech->getChassisName(); //magic 19032011 disabled
			aText cName = ((MechListBoxItem*)items[i])->chassisName; //magic 19032011
			//char tmpChassisName[256]; //magic 19032011 disabled
			//cLoadString( ID, tmpChassisName, 255 ); //magic 19032011 disabled
			//chassisName = tmpChassisName; tmpChassisName //magic 19032011 disabled
			chassisName = cName.text; //magic 19032011
			
			if ( ((MechListBoxItem*)items[i])->pMech->getMaxWeight() < pItem->pMech->getMaxWeight() )
			{
				return InsertItem( itemString, i );
				break;
			}
			else if ( ((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight()
				&& chassisName.Compare( addedName ) > 0 )
			{
				return InsertItem( itemString, i );
			}
			else if ( ((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight()
				&& chassisName.Compare( addedName ) == 0 
				&& ((MechListBoxItem*)itemString)->pMech->getName().Find("Prime") != -1 )
			{
				return InsertItem( itemString, i );
			}
			else if ( ((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight()
				&& chassisName.Compare( addedName ) == 0 
				&& ( ((MechListBoxItem*)items[i])->pMech->getName().Find("Prime" ) == -1 ) 
				&& ((MechListBoxItem*)items[i])->pMech->getName().Compare( pItem->pMech->getName() ) > 0 )
			{
				return InsertItem( itemString, i );
			}
		}

	}

	
	return aListBox::AddItem( itemString );
}

long MechListBox::RemoveMBItem(aListItem* itemString)
{
	itemString->setID( ID );
	MechListBoxItem* pItem = dynamic_cast<MechListBoxItem*>(itemString);
	//EString addedName;
	//char tmp[256];
	//cLoadString( pItem->getMech()->getChassisName(), tmp, 255 );
	//addedName = tmp;
	bool bFound = false;
	long counter = itemCount;
	if ( pItem )
	{
		for ( int i = 0; i < counter; i++ )
		{
			if ( (MechListBoxItem*)items[i] == pItem )
			//if ( ((MechListBoxItem*)items[i])->pMech == pMech )
			{
				bFound = true;
				//aListBox::RemoveItem( itemString, true );
				for (int j = i; j < counter - 1; j++)
				{
					//items[j] = items[j+1];
					//items[j]->move( 0, -height - skipAmount );
					//aListBox::RemoveItem( itemString, true );
					((MechListBoxItem*)items[j])->pMech = ((MechListBoxItem*)items[j+1])->pMech;
					((MechListBoxItem*)items[j])->mechIcon = ((MechListBoxItem*)items[j+1])->mechIcon;
					((MechListBoxItem*)items[j])->chassisName = ((MechListBoxItem*)items[j+1])->chassisName;
					((MechListBoxItem*)items[j])->variantName = ((MechListBoxItem*)items[j+1])->variantName;
					((MechListBoxItem*)items[j])->countText = ((MechListBoxItem*)items[j+1])->countText;
					((MechListBoxItem*)items[j])->weightText = ((MechListBoxItem*)items[j+1])->weightText;
				}
				//*((MechListBoxItem*)items[itemCount]) = NULL;
				//((MechListBoxItem*)items[j])->pMech = NULL;
				//itemCount --;
				//aListBox::RemoveItem( itemString, true );
				//removeChild(((MechListBoxItem*)items[counter]));
				break;

			}
		}

	if (bFound)
		return aListBox::RemoveItem( itemString, true );
	}

	
	return -1;//aListBox::RemoveItem( itemString, true );
}

void	MechListBox::dimItem( LogisticsMech* pMech, bool bDim )
{
		for ( int i = 0; i < itemCount; i++ )
		{
			if ( ((MechListBoxItem*)items[i])->pMech == pMech )
			{
				
				((MechListBoxItem*)items[i])->bDim = bDim;	
			}
		}
		
}

void MechListBox::undimAll()
{
	for ( int i = 0; i < itemCount; i++ )
	{
			
			((MechListBoxItem*)items[i])->bDim = 0;	
		
	}
}

void MechListBox::disableItemsThatCostMoreThanRP()
{
	bool bDisabledSel = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( ((MechListBoxItem*)items[i])->pMech->getCost() > LogisticsData::instance->getCBills() )
		//if ( ((MechListBoxItem*)items[i])->mechCost > LogisticsData::instance->getCBills() ) magic xx052012 disabled
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

void MechListBox::disableItemsWith0Q()
{
	bool bDisabledSel = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( ((MechListBoxItem*)items[i])->mechCount <= 0 )
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

void MechListBox::disableItemsThatCanNotGoInFG()
{
	bool bDisabledSel = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		if ( !LogisticsData::instance->canAddMechToForceGroup( ((MechListBoxItem*)items[i])->pMech ) )
		{
			if ( itemSelected == i )
				bDisabledSel = true;
			items[i]->setState( aListItem::DISABLED );
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

void MechListBox::setOrange( bool bNewOrange )
{
	bOrange = bNewOrange ? 1 : 0;

	for ( int i= 0; i < itemCount; i++ )
	{
		((MechListBoxItem*)items[i])->bOrange = bOrange;
	}

	if ( bNewOrange )
		scrollBar->setOrange( );
	else
		scrollBar->setGreen();

}

long MechListBox::getNumVariants(LogisticsMech* vMech )//bespotrebna
{
	long retVal = 0;
	for ( int i = 0; i < itemCount; i++ )
	{
		LogisticsMech* iMech = ((MechListBoxItem*)items[i])->pMech;
		if ( iMech->getVariant() == vMech->getVariant())
		{
			retVal++;
		}
	}
	
	return retVal;


}

void	MechListBox::incMechCount( LogisticsMech* pMech )
{
		for ( int i = 0; i < itemCount; i++ )
		{
			//if ( ((MechListBoxItem*)items[i])->pMech == pMech ) M14
			LogisticsMech* iMech = ((MechListBoxItem*)items[i])->pMech;
			if ( iMech->getVariant() == pMech->getVariant())
			{
				
				((MechListBoxItem*)items[i])->mechCount++;	

			}
		}
		
}

void	MechListBox::decMechCount( LogisticsMech* pMech )
{
		for ( int i = 0; i < itemCount; i++ )
		{
			//if ( ((MechListBoxItem*)items[i])->pMech == pMech ) //M14
			LogisticsMech* iMech = ((MechListBoxItem*)items[i])->pMech;
			if ( iMech->getVariant() == pMech->getVariant())
			{
				
				((MechListBoxItem*)items[i])->mechCount--;

			}
		}
		
}

long	MechListBox::getMechCount( LogisticsMech* pMech )
{
		for ( int i = 0; i < itemCount; i++ )
		{
			//if ( ((MechListBoxItem*)items[i])->pMech == pMech ) //M14
			LogisticsMech* iMech = ((MechListBoxItem*)items[i])->pMech;
			if ( iMech->getVariant() == pMech->getVariant())
			{
				
				return ((MechListBoxItem*)items[i])->mechCount;
				
			}
		}
		return 0;
}

//magic 08052012 begin
bool	MechListBoxItem::isArmorDamaged()
{

	float* baseArmorLog = pMech->getBaseArmorArray();
	float* curArmorLog = pMech->getCurArmorArray();
	float sumBase = 0.f;
	float sumCur = 0.f;
	for (int i=0; i<11; i++)
	{
		sumBase += baseArmorLog[i];
		sumCur += curArmorLog[i];
	}

	if ((sumBase - sumCur) > 1.0)
		return true;
	else
		return false;
}

bool	MechListBoxItem::isInternalDamaged()
{

	unsigned char* maxIntLogC = pMech->getMaxInternalArray();
	float maxIntLog[8];
	for (int i=0; i<8; i++)
		maxIntLog[i] = (float)maxIntLogC[i];
	float* curIntLog = pMech->getCurInternalArray();
	float sumMax = 0.f;
	float sumCur = 0.f;
	for (int i=0; i<8; i++)
	{
		sumMax += maxIntLog[i];
		sumCur += curIntLog[i];
	}

	if ((sumMax - sumCur) > 1.0)
		return true;
	else
		return false;
}
	
//magic 08052012 end

//*************************************************************************************************
// end of file ( MechListBox.cpp )
