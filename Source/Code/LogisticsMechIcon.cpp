#define LOGISTICSMECHICON_CPP
//===========================================================================//
//LogisticsMechIcon.cpp	: Implementation of the LogisticsMechIcon component. //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "LogisticsMechIcon.h"
#include "MechIcon.h"
#include "LogisticsMech.h"
#include "LogisticsPilot.h"
#include "..\resource.h"
#include "aListBox.h"
#include "LogisticsPilotListBox.h"
#include "gameSound.h"

LogisticsMechIcon*	LogisticsMechIcon::s_pTemplateIcon = NULL;


LogisticsMechIcon::LogisticsMechIcon( )
{
	if( s_pTemplateIcon )
		*this = *s_pTemplateIcon;

	pMech = 0;
	state = 0;
	bJustSelected = 0;
	bDisabled = 0;
}

LogisticsMechIcon& LogisticsMechIcon::operator =( const LogisticsMechIcon& src )
{
	if ( &src != this )
	{
		pilotName = src.pilotName;
		chassisName = src.chassisName;
		iconConnector = src.iconConnector;
		icon = src.icon;
		pilotIcon = src.pilotIcon;
		outline = src.outline;
		for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			animations[i] = src.animations[i];
		}

		pilotID = src.pilotID;
		chassisNameID = src.chassisNameID;
		iconConnectorID = src.iconConnectorID;
		iconID = src.iconID;
		pilotIconID = src.pilotIconID;
		outlineID = src.outlineID;
		helpID = src.helpID;

	}

	return *this;
}

//-------------------------------------------------------------------------------------------------

LogisticsMechIcon::~LogisticsMechIcon()
{

}



int LogisticsMechIcon::init( FitIniFile& file )
{
	if ( !s_pTemplateIcon )
	{
		s_pTemplateIcon = new LogisticsMechIcon;
		s_pTemplateIcon->pilotName.init( &file, "PilotNameText" );
		assignAnimation( file, s_pTemplateIcon->pilotID );

		s_pTemplateIcon->chassisName.init( &file, "MechNameText" );
		assignAnimation( file, s_pTemplateIcon->chassisNameID );

		s_pTemplateIcon->iconConnector.init( &file, "IconConnector" );
		assignAnimation( file, s_pTemplateIcon->iconConnectorID );

		s_pTemplateIcon->outline.init( &file, "BoxOutline" );
		assignAnimation( file, s_pTemplateIcon->outlineID );

		s_pTemplateIcon->icon.init( &file, "MechEntryIcon" );
		assignAnimation( file, s_pTemplateIcon->iconID );

		s_pTemplateIcon->pilotIcon.init( &file, "PilotIcon" );
		assignAnimation( file, s_pTemplateIcon->pilotIconID );



		char blockName[64];
		for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			sprintf( blockName, "Animation%ld", i );
			s_pTemplateIcon->animations[i].init( &file, blockName );
		}
	}
	

	return true;
}

void LogisticsMechIcon::assignAnimation( FitIniFile& file, long& number )
{
	number = -1;
	char buffer[64];
	if ( NO_ERR == file.readIdString( "Animation", buffer, 63 ) )
	{
		for ( int i = 0; i < strlen( buffer ); i++ )
		{
			if ( isdigit( buffer[i] ) )
			{
				buffer[i+1] = 0;
				number = atoi( &buffer[i] );
			}
		}
	}
}

void LogisticsMechIcon::setMech( LogisticsMech* pNewMech )
{
	pMech = pNewMech;

	if ( pMech )
	{
		/*
		// need to set the uv's of the mech icon
		long index = pMech->getIconIndex();
		//long xIndex = index % 10;
		//long yIndex = index / 10;
		//long xIndex = index % 20; //magic 03032011
		//long yIndex = index / 20; //magic 03032011

		long xIndex = index % 25; //magic 08052012 25 is number of mech icons in a row for 1024 using mcui_high7
		long yIndex = index / 25; //magic 08052012

		float fX = xIndex;
		float fY = yIndex;

		//float width = icon.width();
		//float height = icon.height();

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

		//icon.setFileWidth(256.f);
		//icon.setFileWidth(512.f); //magic 03032011 - loading icon file with 512x512 size
		icon.setFileWidth(1024.f); //magic 08052012

		icon.setUVs( u, v, u2, v2 );
		*/ //magic 21082012 disabled

		//magic 21082012 begin
		char path[256];
		strcpy( path, artPath );
		strcat( path, "mechicons\\" );
		strcat( path, pMech->getRealChassisName() );
		strcat( path, ".tga" );
		_strlwr( path );
		//icon.setTexture(path); 
		icon.setTextureUniq(path, pMech->getID()); //magic 21082012
		/*//magic 21082012 begin
		aObject* newMechIcon = new aObject();
		icon = *newMechIcon;
		icon.init3(3,31,25,30, path);
		//magic 21082012 end*/

		icon.setUVs( 0, 0, 40, 48 );
		//magic 21082012 end

		//chassisName.setText( pMech->getChassisName() ); //magic 31102011 disabled
		//magic 31102011 begin
		chassisName.setText(pMech->getRealChassisName());
		//magic 31102011 end

		iconConnector.showGUIWindow( true );

		if ( pMech->getPilot() )
		{
			pilotName.setText( pMech->getPilot()->getName() );
			
		}
		else
		{
			pilotName.setText( IDS_NOPILOT );
			
		}
	}
	else
		iconConnector.showGUIWindow( 0 );

		

}

void LogisticsMechIcon::render(long xOffset, long yOffset )
{

	if ( bDisabled )
	{
		GUI_RECT tmprect = { outline.left() + xOffset, outline.top() + yOffset,
			outline.right() + xOffset, outline.bottom() + yOffset };

		drawRect( tmprect, 0xff000000 );

		return;

	}

	if ( !pMech )
		return;


	long color = animations[outlineID].getCurrentColor(animations[outlineID].getState());

	outline.setColor( color );
	outline.render(xOffset, yOffset);	

	xOffset += outline.globalX();
	yOffset += outline.globalY();

	
	renderObject( icon, iconID, xOffset, yOffset );
	renderObject( chassisName, chassisNameID, xOffset, yOffset );
	renderObject( pilotName, pilotID, xOffset, yOffset );
	renderObject( iconConnector, iconConnectorID, xOffset, yOffset );
	renderObject( icon, iconID, xOffset, yOffset );

	if ( pMech && pMech->getPilot() )
		pilotIcon.render(xOffset, yOffset );
	
}

void LogisticsMechIcon::renderObject( aObject& obj, long animIndex, long xOffset, long yOffset )
{
	long color = 0xffffffff;
	/*
	if ( animIndex != -1 )
	{
		color = animations[animIndex].getCurrentColor(animations[animIndex].getState());
	}
	*/ //magic 22082012 disabled stupid orange animation
	obj.setColor( color );
	obj.render( xOffset, yOffset );
}

void LogisticsMechIcon::update()
{
	bJustSelected = 0;

	if ( !pMech )
		return;

	long x = userInput->getMouseX();
	long y = userInput->getMouseY();

	for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
	{
		animations[i].update();
	}

	if ( outline.pointInside( x, y ) )
	{
		if ( (userInput->isLeftClick() || userInput->isLeftDoubleClick())
			&& getMech() )
		{
		
			for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
			{
				animations[i].setState( aAnimGroup::PRESSED );
			}

			bJustSelected = true;
			if ( state != aListItem::SELECTED )
				soundSystem->playDigitalSample( LOG_SELECT );
			state = aListItem::SELECTED;
		}

		else if ( state == aListItem::ENABLED || state == aListItem::HIGHLITE )
		{
			for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
			{
				animations[i].setState( aAnimGroup::HIGHLIGHT );
			}
			if ( state != aListItem::HIGHLITE )
			{
				soundSystem->playDigitalSample( LOG_HIGHLIGHTBUTTONS );
			}
			state = aListItem::HIGHLITE;
		}

		::helpTextID = helpID;

	}
	else if ( state != aListItem::SELECTED )
	{
		for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			animations[i].setState( aAnimGroup::NORMAL );
		}

		state = aListItem::ENABLED;
	}

	//magic 21082012
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

		icon.doDraw2( newDamage );
	//magic 21082012 end
	
}

void LogisticsMechIcon::move( long x, long y )
{
	outline.move(x, y);	
}

void LogisticsMechIcon::select( bool bSelect )
{
	if ( !bSelect || bDisabled )
	{
		state = aListItem::ENABLED;
		for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
			animations[i].setState( aAnimGroup::NORMAL );
	}
	else
	{
		state = aListItem::SELECTED;
		for ( int i = 0; i < ICON_ANIM_COUNT; i++ )
			animations[i].setState( aAnimGroup::PRESSED );

		
		bJustSelected = true;
	}
}

bool LogisticsMechIcon::isSelected()
{
	return state == aListItem::SELECTED;
}

LogisticsPilot* LogisticsMechIcon::getPilot()
{
	if ( pMech )
		return pMech->getPilot();

	return NULL;
}

void LogisticsMechIcon::setPilot( LogisticsPilot* pPilot )
{
	if ( pMech )
	{
		pMech->setPilot( pPilot );
		
		if ( pPilot )
		{
			long x = pilotIcon.globalX();
			long y = pilotIcon.globalY();
			LogisticsPilotListBox::makeUVs( pPilot, pilotIcon );
			pilotIcon.moveTo( x, y );
			pilotName.setText( pPilot->getName() );
			pPilot->setUsed( true );

		}
		else
		{
			pilotIcon.setUVs( 0, 0, 0, 0 );
			pilotName.setText( IDS_NOPILOT );
		}
	}
	else
		pilotIcon.setUVs( 0, 0, 0, 0 );
}

void LogisticsMechIcon::dimPilot( bool bDim )
{
	if ( bDim )
	{
		pilotIcon.setColor( 0x7f000000 );
	}
	else
		pilotIcon.setColor( 0xffffffff );
}

//*************************************************************************************************
// end of file ( LogisticsMechIcon.cpp )
