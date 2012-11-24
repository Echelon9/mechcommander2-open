#define LOGISTICSPILOT_CPP

//===========================================================================//
// LogisticsPilot.cpp		: Implementation of the LogisticsPilot component.//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "McLib.h"
#include "LogisticsPilot.h"
#include "LogisticsErrors.h"
#include "LogisticsData.h"

#include "warrior.h"
#include "..\resource.h"
#include "MechIcon.h"
#include "objmgr.h"

//char	LogisticsPilot::skillTexts[NUM_SPECIALTY_SKILLS][255] = {0};//Magic 79 disabled
char	LogisticsPilot::skillTexts[NUM_SPECIALTY_SKILLS][255] = {
	"Light Mech Specialist",
	"Laser Specialist",
	"Light AC Specialist",
	"Medium AC Specialist",
	"SRM Specialist",
	"Small Arms Specialist",
	"Toughness Specialist",			//Thoughness Specialty
	"Medium Mech Specialist",
	"Pulse Laser Specialist",
	"ER Laser Specialist",
	"LRM Specialist",
	"Scouting Specialist",						//Scouting Specialty
	"Heavy Mech Specialist",			//Heavy mech Specialty
	"PPC Specialist",
	"Heavy AC Specialist",
	"Short Range Specialist",
	"Medium Range Specialist",
	"Long Range Specialist",
	"Assault Mech Specialist",
	"Gauss Specialist",
	"Sharp Shooter",	
};

extern char* SpecialtySkillsTable[NUM_SPECIALTY_SKILLS];


LogisticsPilot::LogisticsPilot()
{
	bIsUsed = 0;
	bJustDied = 0;

	mechKills = vehicleKills = infantryKills =  0;
	missionsCompleted = 0;

	memset(missionsPlayed,0,sizeof(unsigned char) * MAX_MISSIONS);

	bDead = 0;
	bAvailable = 0;
	newPiloting = 0;
	newGunnery = 0;
	newSensorSkill = 0;//Magic 55
	newJumping = 0;//Magic 55
	memset( specialtySkills, 0, sizeof( bool ) * NUM_SPECIALTY_SKILLS );

	/* Magic 79
	if ( !strlen( skillTexts[0] ) )
	{
		char tmp[256];
		for ( int i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
		{
			cLoadString( IDS_SPECIALTY + i, tmp, 255 );
			strcpy( skillTexts[i], tmp );
		}
	}*/

	memset( medals, 0, sizeof( bool ) * MAX_MEDAL );

}

LogisticsPilot::~LogisticsPilot()
{
	
}

int LogisticsPilot::init( char* pilotFileName )
{
	fileName = pilotFileName;
	
	char path[256];
	strcpy( path, warriorPath );
	strcat( path, fileName );
	strcat( path, ".fit" );

	FitIniFile pilotFile;
	if ( NO_ERR != pilotFile.open( path ) )
	{
		char errorString[256];
		sprintf( errorString, "Couldn't open file %s", fileName);
		Assert( 0, 0, errorString );
		return -1;
	}

	// if we got this far we have a file, make a pilot
	int result = pilotFile.seekBlock( "General" );
	gosASSERT( result == 0 );

	long tmp;
	result = pilotFile.readIdLong( "descIndex", tmp );
	gosASSERT( result == NO_ERR );

	cLoadString( tmp, path, 256 );

	name = path;

	result = pilotFile.readIdLong("PictureIndex", photoIndex);
	if (result != NO_ERR)
		photoIndex = 0;

	
	result = pilotFile.readIdLong( "Rank", rank );
	gosASSERT( result == 0 );
//	result = pilotFile.readIdULong( "ID", id );
//	gosASSERT( result == 0 );
	result = pilotFile.readIdLong( "FlavorText", flavorTextID );
	
	result = pilotFile.readIdString( "pilotAudio", path, 256 );
	gosASSERT( result == 0 );
	audio = path;
	
	result = pilotFile.readIdString( "pilotVideo", path, 256 );
	gosASSERT( result == 0 );
	video = path;

	result = pilotFile.readIdString( "Picture", path, 256 );
	iconFile = artPath;
	iconFile += path;
	
	pilotFile.seekBlock( "Skills" );

	char tPilot, tGunnery, tSensor, tJumping;
	result = pilotFile.readIdChar( "Piloting", tPilot );
	gosASSERT( result == NO_ERR );
	pilotFile.readIdChar( "Sensors", tSensor );//Magic 55
	pilotFile.readIdChar( "Gunnery", tGunnery );
	pilotFile.readIdChar( "Jumping", tJumping );//Magic 55

	piloting = tPilot;
	gunnery = tGunnery;
	sensorSkill = tSensor;//Magic 55
	jumping = tJumping;//Magic 55

	result = pilotFile.seekBlock("SpecialtySkills");
	if (result == NO_ERR)
	{
		for (int i = 0; i < NUM_SPECIALTY_SKILLS; i++) 
		{
			char tmpChar;
			result = pilotFile.readIdChar(SpecialtySkillsTable[i], tmpChar);
			if (result == NO_ERR)
				specialtySkills[i] = (tmpChar == 1);
		}
	}

	rank = turnAverageIntoRank( (gunnery + piloting + sensorSkill + jumping)/4.f );

	return 0;

}

const char* LogisticsPilot::getSkillText( int skillID )
{
	gosASSERT( skillID <= NUM_SKILLS );
	
	if ( skillTexts[skillID] )
	{
		return skillTexts[skillID];
	}

//	char tmp[256];
 
//	cLoadString( IDS_SKILL0 + skillID, tmp, 256 );

//	skillTexts[skillID] = tmp;
	
	return skillTexts[skillID];

	
}

int	LogisticsPilot::getNumberMissions() const
{
	return missionsCompleted;

}

long LogisticsPilot::save( FitIniFile& file, long which )
{
	char tmp[256];
	sprintf( tmp, "Pilot%ld", which );
	file.writeBlock( tmp );

	file.writeIdString( "FileName", fileName );
	file.writeIdLong( "Rank", rank );
	file.writeIdFloat( "Gunnery", gunnery );
	file.writeIdFloat( "Piloting", piloting );
	file.writeIdFloat( "Sensors", sensorSkill );//Magic 55
	file.writeIdFloat( "Jumping", jumping );//Magic 55
	file.writeIdLong( "Kills", mechKills );
	file.writeIdLong( "VehicleKills", vehicleKills );
	file.writeIdLong( "InfantryKills", infantryKills );

	file.writeIdLong( "MissionsCompleted", missionsCompleted );
	file.writeIdUCharArray( "MissionsPlayed", missionsPlayed, MAX_MISSIONS);
	file.writeIdBoolean( "Dead", bDead );

	char buffer[64];
	for ( int i = 0; i < MAX_MEDAL; i++ )
	{
		sprintf( buffer, "Medal%ld", i );
		file.writeIdBoolean( buffer, medals[i] );
	}

	for ( i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		sprintf( buffer, "SpecialtySkill%ld", i );
		file.writeIdBoolean( buffer, specialtySkills[i] );
	}


	return 0;
}

long LogisticsPilot::load( FitIniFile& file )
{
	char tmp[256];
	file.readIdString( "FileName", tmp, 255 );
	fileName = tmp;

	file.readIdLong( "Rank", rank );
	file.readIdFloat( "Gunnery", gunnery );
	file.readIdFloat( "Piloting", piloting );
	file.readIdFloat( "Sensors", sensorSkill );
	file.readIdFloat( "Jumping", jumping );
	file.readIdLong( "Kills", mechKills );
	file.readIdLong( "VehicleKills", vehicleKills );
	file.readIdLong( "InfantryKills", infantryKills );
	file.readIdLong( "MissionsCompleted", missionsCompleted );

	long result = file.readIdUCharArray( "MissionsPlayed", missionsPlayed, MAX_MISSIONS);
	if (result != NO_ERR)
		memset(missionsPlayed,0,sizeof(unsigned char) * MAX_MISSIONS);

	file.readIdBoolean( "Dead", bDead );

	char buffer[64];
	for ( int i = 0; i < MAX_MEDAL; i++ )
	{
		sprintf( buffer, "Medal%ld", i );
		file.readIdBoolean( buffer, medals[i] );
	}

	for ( i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		sprintf( buffer, "SpecialtySkill%ld", i );
		file.readIdBoolean( buffer, specialtySkills[i] );
	}

	rank = turnAverageIntoRank((gunnery + piloting + sensorSkill + jumping)/4.f);


	return 0;
}

void LogisticsPilot::clearIcons()
{
	// clear out the old ones
	for ( EList<ForceGroupIcon*, ForceGroupIcon*>::EIterator iter = 
		killedIcons.Begin(); !iter.IsDone(); iter++ )
	{
		delete (*iter);
	}

	killedIcons.Clear();
}

long LogisticsPilot::update( MechWarrior* pWarrior )
{
#ifndef VIEWER
	
	// clear out the old ones
	for ( EList<ForceGroupIcon*, ForceGroupIcon*>::EIterator iter = 
		killedIcons.Begin(); !iter.IsDone(); iter++ )
	{
		delete (*iter);
	}

	killedIcons.Clear();

	missionsCompleted++;
	long missionJustPlayed = LogisticsData::instance->getCurrentMissionId();
	if ((missionJustPlayed < 0) || (missionJustPlayed > MAX_MISSIONS))
		STOP(("Logistics thinks last mission played was %d",missionJustPlayed));

	missionsPlayed[missionJustPlayed] = 1;

	if ( pWarrior->getStatus() == WARRIOR_STATUS_DEAD )
	{
		bDead = true;
		bJustDied = true;
	}

	setUsed( 1 );


	pWarrior->updateMissionSkills();

	newGunnery =  pWarrior->skillRank[MWS_GUNNERY] - gunnery;
	newPiloting =  pWarrior->skillRank[MWS_PILOTING] - piloting;
	newSensorSkill =  pWarrior->skillRank[MWS_SENSORS] - sensorSkill;
	newJumping =  pWarrior->skillRank[MWS_JUMPING] - jumping;

	gunnery = pWarrior->skillRank[MWS_GUNNERY];
	piloting = pWarrior->skillRank[MWS_PILOTING];
	sensorSkill = pWarrior->skillRank[MWS_SENSORS];
	jumping = pWarrior->skillRank[MWS_JUMPING];

	// make sure no more than 3 points per mission
	if ( newGunnery > 3.f)
	{
		gunnery -= newGunnery;
		newGunnery = 3.f;
		gunnery += newGunnery;
	}

	if ( newPiloting > 3.f )
	{
		piloting -= newPiloting;
		newPiloting = 3.f;
		piloting += newPiloting;
	}

	if ( newSensorSkill > 3.f )
	{
		sensorSkill -= newSensorSkill;
		newSensorSkill = 3.f;
		sensorSkill += newSensorSkill;
	}

	if ( newJumping > 3.f )
	{
		jumping -= newJumping;
		newPiloting = 3.f;
		jumping += newJumping;
	}

	infantryKills += pWarrior->numMechKills[VEHICLE_CLASS_ELEMENTAL][COMBAT_STAT_MISSION];

	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_LIGHTMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_MEDIUMMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_HEAVYMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_ASSAULTMECH][COMBAT_STAT_MISSION];

	vehicleKills += pWarrior->numMechKills[VEHICLE_CLASS_GROUND][COMBAT_STAT_MISSION];

	rank = pWarrior->getRank();

	long deadMechCount = 0;
	for ( int i = 0; i < pWarrior->numKilled; i++ )
	{
		GameObject* pDead = ObjectManager->getByWatchID(pWarrior->killed[i]);

		if ( pDead->getObjectClass() == BATTLEMECH )
		{
			MechIcon* pIcon = new MechIcon();
			pIcon->swapResolutions(0);
			killedIcons.Append( pIcon );
			pIcon->init( (Mover*)pDead );
			pIcon->update();
			if (((MoverPtr)pDead)->getMoveType() != MOVETYPE_AIR)
				deadMechCount++;
		}
		else if ( pDead->getObjectClass() == GROUNDVEHICLE )
		{
			VehicleIcon* pIcon = new VehicleIcon();
			pIcon->swapResolutions(0);
			killedIcons.Append( pIcon );
			pIcon->init( (Mover*)pDead );
			pIcon->update();
		}
	}

	memset( medalsLastMission, 0, sizeof( bool ) * MAX_MEDAL );

	if ( deadMechCount >= 7 )
	{
		medalsLastMission[UNCOMMON_VALOR] = true;
		medals[UNCOMMON_VALOR] = true;
	}

	if ( (deadMechCount >= 3) && (deadMechCount < 7) ) 
	{
		medalsLastMission[VALOR] = true;
		medals[VALOR] = true;
	}

	if ( pWarrior->getWounds()  )
	{
		if ( !medals[PURPLE_HEART] )
			medalsLastMission[PURPLE_HEART] = true;


		medals[PURPLE_HEART] = true;
	}

	//Check for the campaign ribbons and medals.
	// NOTE: NONE of these should be awarded UNLESS we are playing the shipping campaign.
	long anySteinerPlayed = missionsPlayed[0] + 
							missionsPlayed[1] +  
							missionsPlayed[2] +  
							missionsPlayed[3] +  
							missionsPlayed[4] +  
							missionsPlayed[5] +  
							missionsPlayed[6] +  
							missionsPlayed[7] +  
							missionsPlayed[8];

	long anyLiaoPlayed = missionsPlayed[9] +
						missionsPlayed[10] +  
						missionsPlayed[11] +  
						missionsPlayed[12] +  
						missionsPlayed[13] +  
						missionsPlayed[14] +  
						missionsPlayed[15] +  
						missionsPlayed[16];

	long anyDavionPlayed = missionsPlayed[17] +
							missionsPlayed[18] +  
							missionsPlayed[19] +  
							missionsPlayed[20] +  
							missionsPlayed[21] +  
							missionsPlayed[22] +  
							missionsPlayed[23];

	bool allSteinerPlayed = (anySteinerPlayed == 9);
	bool allLiaoPlayed = (anyLiaoPlayed == 8);
	bool allDavionPlayed = (anyDavionPlayed == 7);

	if (anySteinerPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON1])
			medalsLastMission[CAMPAIGN_RIBBON1] = true;

		medals[CAMPAIGN_RIBBON1] = true;
	}

	if (anyLiaoPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON2])
			medalsLastMission[CAMPAIGN_RIBBON2] = true;

		medals[CAMPAIGN_RIBBON2] = true;
	}

	if (anyDavionPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON3])
			medalsLastMission[CAMPAIGN_RIBBON3] = true;

		medals[CAMPAIGN_RIBBON3] = true;
	}

	if (allSteinerPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON1])
			medalsLastMission[CAMPAIGN_FULLRIBBON1] = true;

		medals[CAMPAIGN_FULLRIBBON1] = true;
	}

	if (allLiaoPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON2])
			medalsLastMission[CAMPAIGN_FULLRIBBON2] = true;

		medals[CAMPAIGN_FULLRIBBON2] = true;
	}

	if (allDavionPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON3])
			medalsLastMission[CAMPAIGN_FULLRIBBON3] = true;

		medals[CAMPAIGN_FULLRIBBON3] = true;
	}

	if (missionsPlayed[8])
	{
		if (!medals[STEINER_MEDAL])
			medalsLastMission[STEINER_MEDAL] = true;

		medals[STEINER_MEDAL] = true;
	}

	if (missionsPlayed[12])
	{
		if (!medals[LIAO_MEDAL])
			medalsLastMission[LIAO_MEDAL] = true;

		medals[LIAO_MEDAL] = true;
	}

	if (missionsPlayed[21])
	{
		if (!medals[DAVION_MEDAL])
			medalsLastMission[DAVION_MEDAL] = true;

		medals[DAVION_MEDAL] = true;
	}

	for ( i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		if ( pWarrior->specialtySkills[i] )
		{
			specialtySkills[i] = true;
		}

	}

#endif
	return 0;
}


bool	LogisticsPilot::promotePilot()
{
	
	float oldGunnery = gunnery - newGunnery;
	float oldPiloting = piloting - newPiloting;
	float oldSensorSkill = sensorSkill - newSensorSkill;
	float oldJumping = jumping - newJumping;
	float oldAvg = (oldGunnery + oldPiloting + oldSensorSkill + oldJumping)/4.f;

	float newAvg = (gunnery + piloting + sensorSkill + jumping)/4.f;

	
	int oldRank = turnAverageIntoRank( oldAvg );
	int newRank = turnAverageIntoRank( newAvg );

	//Magic 61 begin
	if ( oldRank < newRank )
	{
		// go ahead and set that rank
		rank = newRank;
		return true;
	}
	//Magic 61 end
/*Magic61	if ( rank != newRank )
	{
		// go ahead and set that rank
		rank = newRank;
	}

	if ( oldRank != newRank )
	{
		gosASSERT( newRank > oldRank ); // bad to demote

		if ( rank > WARRIOR_RANK_GREEN )
			return true;
		return false;
	}*/ //Magic 61

	// temporary for testing change to false
	return false;
}

int LogisticsPilot::turnAverageIntoRank( float avg)
{
	if ( avg > 79.f )
		return WARRIOR_RANK_ACE;
	else if ( avg > 70.f )
		return WARRIOR_RANK_ELITE;
	else if ( avg > 60.f )
		return WARRIOR_RANK_VETERAN;
	else if ( avg > 50.f )
		return WARRIOR_RANK_REGULAR;

	return WARRIOR_RANK_GREEN;
}

int			LogisticsPilot::getSpecialtySkillCount() const
{
	int count = 0;
	for ( int i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		if ( specialtySkills[i] )
		count++;
	}

	return count;
}
int	LogisticsPilot::getSpecialtySkills( const char** array, int& count )
{
	int max = count;
	count = 0;
	for ( int i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		if ( count >= max )
		{
			return NEED_BIGGER_ARRAY;
		}
		if ( specialtySkills[i] )
		{
			array[count] = skillTexts[i];
			count++;
		}
	}

	return 0;

}

int		LogisticsPilot::getSpecialtySkills( int* array, int& count )
{
	int max = count;
	count = 0;
	for ( int i = 0; i < NUM_SPECIALTY_SKILLS; i++ )
	{
		if ( i >= max )
		{
			return NEED_BIGGER_ARRAY;
		}
		if ( specialtySkills[i] )
		{
			array[count] = i;
			count++;
		}
	}

	return 0;

}
		

void	LogisticsPilot::setSpecialtySkill( int skill, bool set )
{
	if ( skill >= NUM_SPECIALTY_SKILLS )
	{
		gosASSERT( 0 );
		return;
	}

	specialtySkills[skill] = set;
}



//*************************************************************************************************
// end of file ( LogisticsPilot.cpp )
