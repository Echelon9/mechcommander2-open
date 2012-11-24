#define LOGISTICSVARIANT_CPP
/*************************************************************************************************\
LogisticsVariant.cpp			: Implementation of the LogisticsVariant component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "LogisticsVariant.h"
#include "mclib.h"
#include "LogisticsData.h"
#include "..\resource.h"
#include "cmponent.h" 
#include "LogisticsErrors.h"


int LogisticsChassis::weightClasses[5] = { 0, 35, 55, 75, 100};
/*[0- 34	Light
35-55		Medium 
56-75		Heavy
76-100		Assualt
*/


LogisticsChassis::LogisticsChassis()
{
	appearance = 0;
	chassisNameID = 0;
	baseCost = 0;
	maxWeight = 0;
	refCount = 0;
	canHaveActiveProbe = 
	canHaveAdvSensor = 
	canHaveExtendedSensor = 
	canHaveECM = 
	canHaveGlobalSensor = 
	canHaveOptics = 
	canHaveJumpJets = 0;

}

int LogisticsChassis::init( CSVFile* file, int chassisID )
{
	ID = chassisID;
	char buffer[256];
	
	fileName = file->getFilename();
	fileName.MakeLower();

	long result = file->readLong( 11, 5, baseCost );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 5, 2, chassisNameID );
	gosASSERT( result == NO_ERR );
	
	//magic 31102011 begin
	result = file->readString( 3, 2, buffer, 256 );
	gosASSERT( result == NO_ERR );
	chassisName = buffer;
	//magic 31102011 end

	result = file->readFloat(  3, 5, maxWeight );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 12, 5, maxArmor );
	gosASSERT( result == NO_ERR );
	
	result = file->readLong( 8,2, helpID );
	gosASSERT( result == NO_ERR );
	
	result = file->readLong( 6, 2,  encyclopediaID );
	gosASSERT( result == NO_ERR );

	file->readLong( 12, 2, iconPictureIndex );
	gosASSERT( result == NO_ERR );

	
	houseID;
	result = file->readLong( 4, 2, houseID );
	ID |= ((houseID << 8) & 0x0000ff00);
	gosASSERT( result == NO_ERR );

	for ( int row = 9; row < 12; ++row )
	{
		result = file->readString( row, 2, buffer, 256 );
		gosASSERT( result == NO_ERR );
		iconFileNames[row - 9] = buffer;
	}

	result = file->readBoolean(  3, 8, canHaveAdvSensor );
	gosASSERT( result == NO_ERR );
	
	result = file->readBoolean(4, 8, canHaveOptics );
	gosASSERT( result == NO_ERR );
	
	result = file->readBoolean( 5, 8, canHaveGlobalSensor );
	gosASSERT( result == NO_ERR );
	
	result = file->readBoolean( 6, 8, canHaveECM );
	gosASSERT( result == NO_ERR );
	
	result = file->readBoolean( 7, 8, canHaveActiveProbe );
	gosASSERT( result == NO_ERR );

	result = file->readBoolean( 8,8, canHaveJumpJets );
	 gosASSERT( result == NO_ERR );

 	result = file->readBoolean( 9,8, canHaveExtendedSensor );
	 gosASSERT( result == NO_ERR );


	result = file->readFloat(  7, 5, speed );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 4, 5, maxHeat );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 6, 5, baseArmor );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 13, 2, componentAreaWidth );
	gosASSERT( result == NO_ERR );

	result = file->readLong( 14, 2, componentAreaHeight );
	gosASSERT( result == NO_ERR );

	int ID = IDS_VERY_LIGHT;
	for ( int i = 4; i > -1; --i )
	{
		if ( maxWeight > weightClasses[i] )
		{
			ID = IDS_VERY_LIGHT + i + 1;
			break;
		}		
	}

	char tmpWeightClass[256];

	cLoadString( ID, tmpWeightClass, 256 );

	mechClass = tmpWeightClass;

	//magic 03052012 begin
	long stupidNewSageColumns[8] = 
	{
	 2,7,5,6,3,4,8,9
	};

	for (long curLocation = 0; curLocation < NUM_BODY_LOCATIONS; curLocation++) 
	{
		result = file->readUChar(17,stupidNewSageColumns[curLocation],maxInternalPoints[curLocation]);
		gosASSERT( result == NO_ERR );
		//curInternalPoints[curLocation] = (float)maxInternalPoints[curLocation];
	}

	file->readFloat(18,2,baseArmorPoints[0]); //MECH_ARMOR_LOCATION_HEAD
	file->readFloat(18,7,baseArmorPoints[1]); //MECH_ARMOR_LOCATION_CTORSO
	file->readFloat(18,5,baseArmorPoints[2]); //MECH_ARMOR_LOCATION_LTORSO
	file->readFloat(18,6,baseArmorPoints[3]);//MECH_ARMOR_LOCATION_RTORSO
	file->readFloat(18,3,baseArmorPoints[4]);//MECH_ARMOR_LOCATION_LARM
	file->readFloat(18,4,baseArmorPoints[5]);//MECH_ARMOR_LOCATION_RARM
	file->readFloat(18,8,baseArmorPoints[6]);//MECH_ARMOR_LOCATION_LLEG
	file->readFloat(18,9,baseArmorPoints[7]);//MECH_ARMOR_LOCATION_RLEG
	file->readFloat(18,12,baseArmorPoints[8]);//MECH_ARMOR_LOCATION_RCTORSO
	file->readFloat(18,10,baseArmorPoints[9]);//MECH_ARMOR_LOCATION_RLTORSO
	file->readFloat(18,11,baseArmorPoints[10]);//MECH_ARMOR_LOCATION_RRTORSO

	//for (int i=0; i<11; i++)
	//	curArmorPoints[i] = baseArmorPoints[i];
	//magic 03052012 end //magic 12052012 read only maxinternal and baseArmor
		
	return 0;
	
	
}

int LogisticsChassis::getArmorClass() const
{
	if (baseArmor < 96)
		return IDS_ARMOR0;
	else if (baseArmor < 113)
		return IDS_ARMOR1;
	else if (baseArmor < 195)
		return IDS_ARMOR2;
	else if (baseArmor < 304)
		return IDS_ARMOR3;
	else
		return IDS_ARMOR4;
}


int LogisticsChassis::getDisplaySpeed() const
{
/*	switch( (long)speed )
	{
	case 5:
		return 17;
		break;
	case 10:
		return 34;
		break;
	case 15:
		return 54;
		break;
	case 19:
		return 65;
		break;
	case 23:
		return 86;
		break;
	case 27:
		return 97;
		break;
	case 31:
		return 119;
		break;
	case 35:
		return 130;
		break;
	}
*/
	/*//magic 15092011 begin
	switch( (long)speed )
	{
	case 5:
		return 17;
		break;
	case 6:
		return 20;
		break;
	case 7:
		return 23;
		break;
	case 8:
		return 26;
		break;
	case 9:
		return 30;
		break;
	case 10:
		return 34;
		break;
	case 11:
		return 38;
		break;
	case 12:
		return 42;
		break;
	case 13:
		return 46;
		break;
	case 14:
		return 50;
		break;
	case 15:
		return 54;
		break;
	case 16:
		return 56;
		break;
	case 17:
		return 59;
		break;
	case 18:
		return 62;
		break;
	case 19:
		return 65;
		break;
	case 20:
		return 70;
		break;
	case 21:
		return 75;
		break;
	case 22:
		return 80;
		break;
	case 23:
		return 86;
		break;
	case 24:
		return 88;
		break;
	case 25:
		return 91;
		break;
	case 26:
		return 94;
		break;
	case 27:
		return 97;
		break;
	case 28:
		return 103;
		break;
	case 29:
		return 108;
		break;
	case 30:
		return 114;
		break;
	case 31:
		return 119;
		break;
	case 32:
		return 122;
		break;
	case 33:
		return 125;
		break;
	case 34:
		return 128;
		break;
	case 35:
		return 130;
		break;
	}
	//magic 15092011 end*/
	//magic 28022012 begin
	//speed factor average from above 3.7
	//if ((speed > 5) && (speed < 35))
		return ((long)(speed * 3.7));
	//else
	//magic 28022012 end
	//return -1;
}

int	LogisticsChassis::getSensorID() const
{

	if ( canHaveAdvSensor )
		return 15;
	else if ( canHaveExtendedSensor )
		return 14;
	else if ( canHaveGlobalSensor )
		return 17;
	else 
		return -1;

	return -1;
}

int LogisticsChassis::getECM() const
{
	if ( canHaveECM )
		return 38;

	return -1;
}


LogisticsVariant::LogisticsVariant()
{
	componentCount = 0;
	ID = -1;
	chassis = 0;
	availableToUser = 0;

	memset( components, 0, sizeof( components ) );
	bDesignerMech = 0;
	bHidden = 0;

	fileID = 0;
}

LogisticsVariant::LogisticsVariant( const LogisticsChassis* pChassis , bool Designer)
{
	chassis = const_cast<LogisticsChassis*>(pChassis);
	availableToUser = 1;
	componentCount = 0;
	ID = (pChassis->refCount << 16) | pChassis->ID;
	chassis->refCount++;
	bDesignerMech = Designer;
	bHidden = 0;
	fileID = 0;


}
LogisticsVariant::~LogisticsVariant()
{
	chassis->refCount--;

	if ( chassis->refCount == 0 )
		delete chassis;

	chassis = NULL;
}

LogisticsVariant::LogisticsVariant( const LogisticsVariant& src )
{
	chassis = src.chassis;
	availableToUser = src.availableToUser;
	componentCount = src.componentCount;
	
	for ( int i = 0; i < componentCount; ++i )
	{
		components[i] = src.components[i];
	}	

	variantName = src.variantName;
	sensorID = src.sensorID;
	
	ID = (chassis->refCount << 16) | chassis->ID;
	chassis->refCount++;

	bDesignerMech = 0;
	bHidden= 0;
	fileID = src.fileID;
}

LogisticsVariant& LogisticsVariant::operator=( const LogisticsVariant& src )
{
	if ( &src != this )
	{
		chassis = src.chassis;
		availableToUser = src.availableToUser;
		componentCount = src.componentCount;
		
		for ( int i = 0; i < componentCount; ++i )
		{
			components[i] = src.components[i];
		}	

		variantName = src.variantName;
		sensorID = src.sensorID;
		
		ID = (chassis->refCount << 16) | chassis->ID;
		chassis->refCount++;

		bDesignerMech = 0;

		fileID = src.fileID;
	}

	return *this;
}

bool LogisticsVariant::operator==( const LogisticsVariant& src )
{
		if ( chassis->chassisNameID != src.chassis->chassisNameID )
			return 0;
		
		if ( availableToUser != src.availableToUser )
			return 0;

		if ( componentCount != src.componentCount )
			return 0;
		
		for ( int i = 0; i < componentCount; ++i )
		{
			if ( components[i].component->getID() != src.components[i].component->getID() )
			{
				return 0;
			}
		}	

//		if ( variantName != src.variantName )
//			return 0;
		
		return 1;

}


int LogisticsVariant::init( CSVFile* file, LogisticsChassis* pChassis, int Variant )
{
	bDesignerMech = true;
	
	int offset = 97 * Variant;

	fileID = Variant;
	
	gosASSERT( pChassis );
	chassis = pChassis;
	
	ID = (pChassis->refCount << 16) | pChassis->ID;\
	pChassis->refCount++;

	file->readBoolean( 21 + offset, 4, bHidden );

	char buffer[256];
	if ( NO_ERR == file->readString( 23 + offset, 2, buffer, 256 ) )
	{
		variantName = buffer;
	}

	LogisticsComponent*  pComps[128];
	long xLocs[128];
	long yLocs[128];
	memset( pComps, 0, sizeof( LogisticsComponent* ) * 128 );
	long componentCount = 0;
	
	for ( int i =26; i < 97; i++ )
	{
		long componentID;
		//if ( NO_ERR == file->readLong( offset + i, 5, componentID ) && componentID != 0xff )
		if ( NO_ERR == file->readLong( offset + i, 5, componentID ) && componentID != 0xff && componentID < 198 ) //magic 09022012
		//AMMO FROM CSV WILL NOT SHOW INLOGISTICS - FOR NOW
		{
			LogisticsComponent* pComp = LogisticsData::instance->getComponent(componentID );
			if ( pComp )
			{
				file->readLong( offset + i, 4, yLocs[componentCount] );
				file->readLong( offset + i, 3, xLocs[componentCount] );
				pComps[componentCount++] = pComp;
			}
		}
	}
	//magic ovde dodati za svaku komponentu i=26, i<x za rightarm,...
	//Magic 61 begin
		long sensorComponentID;
		if ( NO_ERR == file->readLong( offset + 38, 5, sensorComponentID ) && sensorComponentID != 0xff )
		{
			sensorID = sensorComponentID;
		}
	//Magic 61 end

	for ( int j = 4; j > 0; j-- )
	{
		for ( int k = 3; k >0; k-- )
		{
			for ( i = 0; i < componentCount; i++ )
			{
				LogisticsComponent*
				pComponent = pComps[i];

				if ( pComponent&& pComponent->getComponentWidth() == k 
					&& pComponent->getComponentHeight() == j )
				{
					if ( !addComponent( pComps[i]->getID(), xLocs[i], yLocs[i] ) )
					{
						char errorString[256];
						sprintf( errorString, "Couldn't add component with id %ld", 
							pComponent->getID() );
					}
				}
			}
		}

	}

	//magic 08052012 begin
	long stupidNewSageColumns[8] = 
	{
	 2,7,5,6,3,4,8,9
	};
	long result = 0;
	unsigned char curIntPointsS[8];
	//float curIntPointsF[8];
	//float curArmPoints[11];
	for (long curLocation = 0; curLocation < NUM_BODY_LOCATIONS; curLocation++) 
	{
		result = file->readUChar(114 + offset,stupidNewSageColumns[curLocation],curIntPointsS[curLocation]);
		gosASSERT( result == NO_ERR );
	}
	for (int i=0; i<8; i++)
		curInternalPoints[i] = (float)curIntPointsS[i];
	//setCurInternalPoints( curIntPointsF);

	file->readFloat(115 + offset,2,curArmorPoints[0]); //MECH_ARMOR_LOCATION_HEAD
	file->readFloat(115 + offset,7,curArmorPoints[1]); //MECH_ARMOR_LOCATION_CTORSO
	file->readFloat(115 + offset,5,curArmorPoints[2]); //MECH_ARMOR_LOCATION_LTORSO
	file->readFloat(115 + offset,6,curArmorPoints[3]);//MECH_ARMOR_LOCATION_RTORSO
	file->readFloat(115 + offset,3,curArmorPoints[4]);//MECH_ARMOR_LOCATION_LARM
	file->readFloat(115 + offset,4,curArmorPoints[5]);//MECH_ARMOR_LOCATION_RARM
	file->readFloat(115 + offset,8,curArmorPoints[6]);//MECH_ARMOR_LOCATION_LLEG
	file->readFloat(115 + offset,9,curArmorPoints[7]);//MECH_ARMOR_LOCATION_RLEG
	file->readFloat(115 + offset,12,curArmorPoints[8]);//MECH_ARMOR_LOCATION_RCTORSO
	file->readFloat(115 + offset,10,curArmorPoints[9]);//MECH_ARMOR_LOCATION_RLTORSO
	file->readFloat(115 + offset,11,curArmorPoints[10]);//MECH_ARMOR_LOCATION_RRTORSO

	//overwriteCurArmorPoints(curArmPoints);
	//magic 08052012 end

	return 0;


}

bool LogisticsVariant::addComponent( int idFromFitFile, long& x, long& y )
{
	LogisticsComponent*
		pComponent = LogisticsData::instance->getComponent( idFromFitFile );


	if ( !pComponent )
	{
	//	Assert( 0, idFromFitFile, "couldn't find the component in the fit file\n" );
		delete pComponent;
		return false;
	}

	if ( pComponent->getType() == COMPONENT_FORM_JUMPJET )
	{
		components[componentCount].location = LEGS;
		x = y = -2;
	}

	//else if ( pComponent->getType() == COMPONENT_FORM_ECM || pComponent->getType() == COMPONENT_FORM_SENSOR )
	else if ( pComponent->getType() == COMPONENT_FORM_SENSOR ) //magic 09022012
	{
		components[componentCount].location = HEAD;
		x = y = -3;
	}

	else
	{


		// need to see if this thing will fit
		long componentWidth = pComponent->getComponentWidth();
		long componentHeight = pComponent->getComponentHeight();

		if ( x == -1 && y == -1 )
		{
			for ( int j = 0; j < chassis->componentAreaHeight && x == -1; j++ )
			{
				for ( int i = 0; i < chassis->componentAreaWidth && x == -1; i++ )
				{
					bool bAdd = true;
					for ( int l = 0; l < componentHeight; ++l )
					{
						for ( int k =0; k < componentWidth; ++k )
						{

 							if ( getComponentAtLocation( i +k, j + l ) 
								|| ( i + k >= chassis->componentAreaWidth )
								|| ( j +l >= chassis->componentAreaHeight ) )
							{
								bAdd = false;
								break;
								//break; //magic 19022012 double break
							}
						}
					}

					if ( bAdd )
					{
						x = i;
						y = j;
			
					}
				}
			}
		}

		if ( x == -1 && y == -1 )
			return 0;

		if ( x > -1 )
		{
			for ( int i =0; i < componentWidth; ++i )
			{
				for ( int j = 0; j < componentHeight; ++j )
				{
 					if ( getComponentAtLocation( x +i, y + j ) )
					{
						char errorString[1024];
						sprintf( errorString, "couldn't add component %s to variant %s because another object was in the specified location", 
							pComponent->getName(), this->getName() );
		//				Assert( 0, 0, errorString );
						return false; 
					}
				}
			}
		}

		components[componentCount].location = CHEST;
	}

	components[componentCount].component = pComponent;
	components[componentCount].xCoord = x;
	components[componentCount].yCoord = y;
	componentCount++;
	return true;
}

int LogisticsVariant::getCost( ) const //magic 12052012 cost of chassis plus components, damage is subtracted in logisticsMech
{
	int cost = chassis->baseCost;

	for ( int i = 0; i < componentCount; ++i )
	{
		cost += components[i].component->getCost();
	}

	return cost;
}

//magic 25092012 begin
int LogisticsVariant::getWeaponsCost( ) const //magic 12052012 cost of chassis plus components, damage is subtracted in logisticsMech
{
	int cost = 0;

	for ( int i = 0; i < componentCount; ++i )
	{
		cost += components[i].component->getCost();
	}

	return cost;
}
//magic 25092012 end

int LogisticsVariant::getWeight() const
{
	//Yup.  20% of base weight Plus 7 tons is used by armor and misc components which we do not explicitly track.

	float baseWeight = (chassis->maxWeight * .20) + 7.0;

	for ( int i = 0; i < componentCount; ++i )
	{
		baseWeight += components[i].component->getWeight();
	}

	return baseWeight;
}

const EString& LogisticsVariant::getMechClass() const
{
	return chassis->mechClass;   
}

int			LogisticsVariant::getMaxJumpRange() const
{
	return 5;
}

// BOGUS, this is entirely made up.  There is no real spec for jumpranges yet
int LogisticsVariant::getJumpRange() const
{
	int jumpJetCount = 0;

	for ( int i = 0; i < componentCount; ++i )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_JUMPJET )
			jumpJetCount++;
	}

	if ( jumpJetCount )
		return 5;

	return 0;
}

int LogisticsVariant::getHeat() const
{
	int retVal = 0;
	for ( int i = 0; i < componentCount; ++i )
	{
		retVal += components[i].component->getHeat();
	}

	return retVal;
}

// BOGUS -- range isn't really the right thing to add...
int LogisticsVariant::getArmor( ) const
{
	int retArmor = chassis->baseArmor;

	for ( int i = 0; i < componentCount; ++i )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_BULK )
		{
			//retArmor += 32;			//Magic disabled
			retArmor += components[i].component->getDamage();	
		}
	}

	return retArmor;
}

int LogisticsVariant::getSpeed() const
{
	return chassis->speed;
}

int LogisticsVariant::getDisplaySpeed() const
{
	return chassis->getDisplaySpeed();
}
int		LogisticsVariant::getMaxHeat() const
{
	int heat = 0;
	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_HEATSINK )
		{
			heat += components[i].component->getDamage();
		}
	}

	return heat + chassis->maxHeat;
}

// if you pass in -1's for x and y, we'll figure out where it can go, and return where it went
int LogisticsVariant::canAddComponent( LogisticsComponent* pComponent, long& x, long& y ) const
{
	/*  weight no longer matters
	int weight = getWeight();
	if ( weight + pComponent->getWeight() > chassis->maxWeight )
		return COMPONENT_TOO_HEAVY;*/

	if ( getHeat() + pComponent->getHeat() > getMaxHeat() )
		return COMPONENT_TOO_HOT;

	if ( pComponent->getType() == COMPONENT_FORM_JUMPJET  )
	{
		if ( !chassis->canHaveJumpJets )
			return JUMPJETS_NOT_ALLOWED;
		else if ( hasJumpJets() )
			return ONLY_ONE_JUMPJET_ALLOWED;
		else if ( ( x!= -1 && x != -2)
					|| ( y != -1 && y != -2 ) )
			return COMPONENT_SLOT_FULL;

		return 0;
	}
	else if ( x == -2 && y == -2 ) // trying to put something illegal in jump jet slot
		return COMPONENT_SLOT_FULL;
		 

	if ( x!= -1 && y != -1 )
	{
		for ( int i = 0; i < pComponent->getComponentWidth(); i++ )
		{
			for ( int j = 0; j < pComponent->getComponentHeight(); j++ )
			{
				if ( getComponentAtLocation( x + i, y  + j)
					|| x + i >= chassis->componentAreaWidth
					|| j + y >= chassis->componentAreaHeight )
					return COMPONENT_SLOT_FULL;
			}  
		}
	}
	else
	{
		for ( int j = 0; j < chassis->componentAreaHeight && x == -1; j++ )
		{
			for ( int i = 0; i < chassis->componentAreaWidth && x == -1; i++ )
			{
				if ( !getComponentAtLocation( i, j ) )
				{
					bool bAdd = true;
					for ( int l = 0; l < pComponent->getComponentHeight(); ++l )
					{
						for ( int k =0; k < pComponent->getComponentWidth(); ++k )
						{

 							if ( getComponentAtLocation( i +k, j + l ) 
								|| ( i + k >= chassis->componentAreaWidth )
								|| ( j +l >= chassis->componentAreaHeight ) )
							{
								bAdd = false;
								break;
								break;
							}
						}
					}

					if ( bAdd )
					{
						x = i;
						y = j;
			
					}
				}

				
			}
		}

		if ( x == -1 || y == -1 )
		{
			return ADD_COMPONENT_FAILED;
		}
	}

	if ( pComponent->getType() == COMPONENT_FORM_BULK )
	{
		//if ( getArmor() + 32 > getMaxArmor() ) //Magic disabled
		if ( getArmor() + pComponent->getDamage() > getMaxArmor() )
			return NO_MORE_ARMOR;
	}

	if ( pComponent->getType() == COMPONENT_FORM_SENSOR )
	{
		//if ( !chassis->canHaveAdvSensor ) //Magic 61 disabled
			//return SENSORS_NOT_ALLOWED; //Magic 61 disabled
		//else if ( hasSensor() )
		if ( hasSensor() )
			return ONLY_ONE_SENSOR_ALLOWED;
	}

	if ( pComponent->getType() == COMPONENT_FORM_ECM  )
	{
		if ( !chassis->canHaveECM )
			return ECM_NOT_ALLOWED;
		else if ( hasECM() )
			return ONLY_ONE_ECM_ALLOWED;
	}

	

	return 0;

}

const LogisticsChassis::ComponentInfo* LogisticsVariant::getComponentAtLocation( long x, long y ) const
{
	if ( x == -1 && y == -1 )
		return NULL;

	for ( int i = 0; i < componentCount; ++i )
	{
		LogisticsComponent* pComponent = components[i].component;

		for ( int j = 0; j < pComponent->getComponentHeight(); ++j )
		{
			for ( int k = 0; k < pComponent->getComponentWidth(); ++k )
			{
				if ( components[i].xCoord + k == x
					 && components[i].yCoord + j == y )
				{
					return &components[i];
				}
			}
		}
	}

	return NULL;
}
//magic 05052012 begin
void	LogisticsVariant::setCurInternalPoints( float* newCurInternalPoints)
{
	for (int i=0; i<8; i++)
		//chassis->curInternalPoints[i] = newCurInternalPoints[i];
		curInternalPoints[i] = newCurInternalPoints[i]; //magic 12052012
}
void	LogisticsVariant::setCurArmorPoints( float* newCurArmorPoints)
//newCurArmorPoints contain damage from the mission input array is max armor - cur armor
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] -= newCurArmorPoints[i];
		if (curArmorPoints[i] < 0.f)
			curArmorPoints[i] = 0.f;
	}
}
void	LogisticsVariant::loadCurArmorPoints( float* newCurArmorPoints)
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] = newCurArmorPoints[i];
		if (curArmorPoints[i] < 0.f)
			curArmorPoints[i] = 0.f;
	}
}

long	LogisticsVariant::repairInternalPoints()
{
	float sumCurInternalPoints = 0.f;
	float sumMaxInternalPoints = 0.f;
	float pointsToRepair = 0.f;
	for (int i=0; i<8; i++)
	{
		sumCurInternalPoints += curInternalPoints[i];
		sumMaxInternalPoints += (float)chassis->maxInternalPoints[i];
	}
	pointsToRepair = sumMaxInternalPoints - sumCurInternalPoints;
	if (pointsToRepair > 1.0f) 
	{
		long moneyForRepair = (long)pointsToRepair * 200; //8; //magic 10052012
		if (moneyForRepair <= LogisticsData::instance->getCBills())
		{
			LogisticsData::instance->addCBills(-moneyForRepair);
			for (int i=0; i<8; i++)
			{
				curInternalPoints[i] = (float)chassis->maxInternalPoints[i];
			}
		}
		else
		{
			long availableMoney = LogisticsData::instance->getCBills();
			pointsToRepair = (float)availableMoney/200; //8; //magic 10052012
			LogisticsData::instance->addCBills(-availableMoney);
			while (pointsToRepair > 0.25f)
			{
				for (int i=0; i<8; i++)
				{
					if (curInternalPoints[i] < (float)chassis->maxInternalPoints[i])
					{
						curInternalPoints[i] += 0.25f;
						pointsToRepair -= 0.25f;
						if (curInternalPoints[i] > (float)chassis->maxInternalPoints[i])
							curInternalPoints[i] = (float)chassis->maxInternalPoints[i];
					}
				}
			}
		}
	}
	return (long)pointsToRepair;

}

long	LogisticsVariant::repairArmorPoints()
{
	float sumCurArmorPoints = 0;
	float sumBaseArmorPoints = 0;
	float pointsToRepair = 0.f;
	for (int i=0; i<11; i++)
	{
		sumCurArmorPoints += curArmorPoints[i];
		sumBaseArmorPoints += chassis->baseArmorPoints[i];
	}
	
	pointsToRepair = sumBaseArmorPoints - sumCurArmorPoints;
	if (pointsToRepair > 1.0f) 
	{
		long moneyForRepair = (long)pointsToRepair * 50; //4; //magic 10052012
		if (moneyForRepair <= LogisticsData::instance->getCBills())
		{
			LogisticsData::instance->addCBills(-moneyForRepair);
			for (int i=0; i<11; i++)
			{
				curArmorPoints[i] = chassis->baseArmorPoints[i];
			}
		}
		else
		{
			long availableMoney = LogisticsData::instance->getCBills();
			pointsToRepair = (float)availableMoney / 50; //4; //magic 10052012
			LogisticsData::instance->addCBills(-availableMoney);
			while (pointsToRepair > 0.25f)
			{
				for (int i=0; i<11; i++)
				{
					if (curArmorPoints[i] < chassis->baseArmorPoints[i])
					{
						curArmorPoints[i] += 0.25f;
						pointsToRepair -= 0.25f;
						if (curArmorPoints[i] > chassis->baseArmorPoints[i])
							curArmorPoints[i] = chassis->baseArmorPoints[i];
					}
				}
			}
		}
	}
						

		return (long)pointsToRepair;
}
//magic 05052012 end
//magic 07052012 begin
long	LogisticsVariant::getInternalDamage()
{
	float sumCurInternalPoints = 0.f;
	float sumMaxInternalPoints = 0.f;
	float pointsToRepair = 0.f;
	for (int i=0; i<8; i++)
	{
		sumCurInternalPoints += curInternalPoints[i];
		sumMaxInternalPoints += (float)chassis->maxInternalPoints[i];
	}
	pointsToRepair = sumMaxInternalPoints - sumCurInternalPoints;

	return (long)pointsToRepair;

}

long	LogisticsVariant::getArmorDamage()
{
	float sumCurArmorPoints = 0;
	float sumBaseArmorPoints = 0;
	float pointsToRepair = 0.f;
	for (int i=0; i<11; i++)
	{
		sumCurArmorPoints += curArmorPoints[i];
		sumBaseArmorPoints += chassis->baseArmorPoints[i];
	}
	
	pointsToRepair = sumBaseArmorPoints - sumCurArmorPoints;

	return (long)pointsToRepair;
}
//magic 07052012 end
//magic 08052012 begin
void	LogisticsVariant::overwriteCurArmorPoints( float* newCurArmorPoints)
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] = newCurArmorPoints[i];
	}
}
//magic 08052012 end //magic 12052012 all of the above are now in logistics mech
//magic 12052012 begin
void	LogisticsVariant::maximizeInternalPoints()
{
	for (int i=0; i<8; i++)
	{
		curInternalPoints[i] = (float)chassis->maxInternalPoints[i];
	}
}

void	LogisticsVariant::maximizeArmorPoints()
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] = chassis->baseArmorPoints[i];
	}
}
//magic 12052012 end
bool LogisticsVariant::hasJumpJets() const
{
	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_JUMPJET )
			return true;
	}

	return false;
}
bool LogisticsVariant::hasECM() const
{
	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_ECM )
			return true;
	}

	return false;

}

int LogisticsVariant::getECM() const //magic 09022012 new function
{
	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_ECM )
			return components[i].component->getID();
	}

	return -1;

}

bool LogisticsVariant::hasSensor() const
{
	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_SENSOR )
			return true;
	}

	return false;

}


int	LogisticsVariant::getComponentsWithLocation( long& count, long* IDArray, long* xLocationArray, long* yLocationArray )
{
	if ( count < componentCount ) 
		return NEED_BIGGER_ARRAY;

	for ( int i = 0; i < componentCount; ++i )
	{
		IDArray[i] = components[i].component->getID();
		xLocationArray[i] = components[i].xCoord;
		yLocationArray[i] = components[i].yCoord;
	}

	count = componentCount;

	return 0;
}

int	LogisticsVariant::getComponents( long& count, long* array )
{
 	for ( int i = 0; i < componentCount; ++i )
	{
		array[i] = components[i].component->getID();

	}

	count = componentCount;

	return count;

}

int		LogisticsVariant::removeComponent( long xCoord, long yCoord )
{
	LogisticsChassis::ComponentInfo* info = const_cast<LogisticsChassis::ComponentInfo*>(getComponentAtLocation( xCoord, yCoord ));

	if ( !info )
		return -1;

	if ( info->component && info->component->getType() == COMPONENT_FORM_HEATSINK )
	{
		if ( getMaxHeat() - info->component->getDamage() < getHeat() )
			return INSUFFICIENT_HEAT;
	}

	
	if ( !info )
		return INVALID_LOCATION;

	for ( int i = 0; i < componentCount; i++ )
	{
		if ( &components[i] == info )
		{
			memmove( &components[i], &components[i + 1], (componentCount - ( i + 1 )) * sizeof( LogisticsChassis::ComponentInfo ) );
			break;
		}
	}

	componentCount --;

	return 0;
}

// if you pass in -1's for x and y, we'll figure out where it can go, and return where it went
int		LogisticsVariant::addComponent( LogisticsComponent* pComponent, long& xCoord, long& yCoord )
{
	int retVal = canAddComponent( pComponent, xCoord, yCoord );

	if ( retVal == 0 )
	{
		if ( addComponent( pComponent->getID(), xCoord, yCoord ) )
			return 0;
		else
			return ADD_COMPONENT_FAILED;
	}

	return retVal;
}

long LogisticsVariant::save( FitIniFile& file, long counter )
{
	char tmp[256];
	char tmp2[256];
	sprintf( tmp, "Variant%ld", counter );
	file.writeBlock( tmp );
	
	file.writeIdString( "Chassis", chassis->fileName );
	file.writeIdString( "VariantName", variantName );
	file.writeIdLong( "VariantSensor", sensorID );
	file.writeIdLong( "ComponentCount", componentCount );

	for ( int i = 0; i < componentCount; i++ )
	{
		sprintf( tmp, "Component%ld", i );
		file.writeIdLong(tmp, components[i].component->getID() );
		
		strcpy( tmp2, tmp );
		strcat( tmp2, "x" );
		file.writeIdLong( tmp2, components[i].xCoord );

		strcpy( tmp2, tmp );
		strcat( tmp2, "y" );
		file.writeIdLong( tmp2, components[i].yCoord );

		strcpy( tmp2, tmp );
		strcat( tmp2, "Location" );
		file.writeIdLong( tmp2, components[i].location );
	}
	//magic 12052012 begin
	file.writeIdFloat( "CurInternalHead", curInternalPoints[0] );
	file.writeIdFloat( "CurInternalCTorso", curInternalPoints[1] );
	file.writeIdFloat( "CurInternalLTorso", curInternalPoints[2] );
	file.writeIdFloat( "CurInternalRTorso", curInternalPoints[3] );
	file.writeIdFloat( "CurInternalLArm", curInternalPoints[4] );
	file.writeIdFloat( "CurInternalRArm", curInternalPoints[5] );
	file.writeIdFloat( "CurInternalLLeg", curInternalPoints[6] );
	file.writeIdFloat( "CurInternalRLeg", curInternalPoints[7] );

	file.writeIdFloat( "CurArmorHead", curArmorPoints[0] );
	file.writeIdFloat( "CurArmorCTorso", curArmorPoints[1] );
	file.writeIdFloat( "CurArmorLTorso", curArmorPoints[2] );
	file.writeIdFloat( "CurArmorRTorso", curArmorPoints[3] );
	file.writeIdFloat( "CurArmorLArm", curArmorPoints[4] );
	file.writeIdFloat( "CurArmorRArm", curArmorPoints[5] );
	file.writeIdFloat( "CurArmorLLeg", curArmorPoints[6] );
	file.writeIdFloat( "CurArmorRLeg", curArmorPoints[7] );
	file.writeIdFloat( "CurArmorRCTorso", curArmorPoints[8] );
	file.writeIdFloat( "CurArmorRLTorso", curArmorPoints[9] );
	file.writeIdFloat( "CurArmorRRTorso", curArmorPoints[10] );

	//magic 12052012 end
	return 0;
}

void LogisticsVariant::setName( const char* pName )
{
	variantName = pName;
}

int	LogisticsVariant::getComponents( long& count, LogisticsComponent** array )
{
	if ( count < componentCount ) 
		return NEED_BIGGER_ARRAY;

	for ( int i = 0; i < componentCount; ++i )
	{
		array[i] = components[i].component;
	}

	count = componentCount;

	return 0;

}

LogisticsComponent*	LogisticsVariant::getCompAtLocation( int i, int j, long& realI, long& realJ )
{
	realI = realJ = -1;
	const LogisticsChassis::ComponentInfo* pInfo = getComponentAtLocation( i, j );

	if ( pInfo )
	{
		realI = pInfo->xCoord;
		realJ = pInfo->yCoord;
		return pInfo->component;
	}

	return 0;
}

int	LogisticsVariant::getComponentLocation( LogisticsComponent* pComp, long& x, long& y )
{
	const LogisticsChassis::ComponentInfo* pInfo = getComponentAtLocation( x, y );
	x = y = -1;

	if ( pInfo )
	{
		x = pInfo->xCoord;
		y = pInfo->yCoord;
	}
	else
	{
		for ( int i = 0; i < componentCount; i++ )
		{
			if ( (components[i].component) == pComp )
			{
				x = components[i].xCoord;
				y = components[i].yCoord;
				return 0;
			}
		}
		return COMPONENT_NOT_FOUND;
	}
	
	return 0;

}

int			LogisticsVariant::getOptimalRangeString( long& color ) const
{
	float rangeDamage[3];

	long rangeColors[3] = {0xff6e7c00, 0xff005392,0xffa21600  };


	memset( rangeDamage, 0, sizeof ( float ) * 3 );

	float maxDamage = -1.f;

	for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->isWeapon() )
		{
			float damageTimeRating = components[i].component->getDamage();
			if ( components[i].component->getRecycleTime() )
				damageTimeRating = components[i].component->getDamage() / components[i].component->getRecycleTime();	
			rangeDamage[components[i].component->getRangeType()] += damageTimeRating;

			if ( rangeDamage[components[i].component->getRangeType()] > maxDamage )
				maxDamage = rangeDamage[components[i].component->getRangeType()];
	
		}
	}

	for ( i = 0; i < 3; i++ )
	{
		if ( rangeDamage[i] == maxDamage )
		{
			color = rangeColors[i];
			return IDS_HOTKEY1 + i;
		}
	}

	return IDS_HOTKEY1;

	
}

bool		LogisticsVariant::allComponentsAvailable() const
{

	// 06/05/01 hack, we wan't to hide this atlas for the whole game.
	if ( stricmp( "Renard's Atlas", variantName ) == 0 )
		return 0;

	for ( int i = 0; i < componentCount; i++ )
	{
		if ( !components[i].component->isAvailable() )
			return false;
	}

	return true;

}

void		LogisticsVariant::setAvailable( bool available ) 
{ 
	// 06/05/01 hack, we wan't to hide this atlas for the whole game.
	if ( stricmp( "Renard's Atlas", variantName ) == 0 )
		availableToUser = 0;

	else
		availableToUser = available; 
}

bool		LogisticsVariant::allComponentsInInventory(LogisticsComponent** pComps, int& maxCount) const
{
	//Magic 52 begin
	long pCompsID[300];
	for (int i = 0; i < maxCount; i++)
		pCompsID[i] = pComps[i]->getID();
	
	for ( int i = 0; i < componentCount; i++ )
	{
		int j = 0;
		bool isIn = false;
		while ((j < maxCount) && (!isIn))
		{
			if ( components[i].component->getID() == pCompsID[j] )
			{
				isIn = true;
				pCompsID[j] = 0;
			}
			j++;
		}
		if (!isIn)
			return false;
	}

	//Magic 52 end

	return true;

}

int		LogisticsVariant::getSensorID() 
{ 
	/*for ( int i = 0; i < componentCount; i++ )
	{
		if ( components[i].component->getType() == COMPONENT_FORM_SENSOR )
			return (components[i].component->getID());
	}
	return 0;*/
	return sensorID;
}


void	LogisticsVariant::setSensorID( int sensID )
		{
			sensorID = sensID;
		}

void LogisticsVehicle::init( FitIniFile& file )
{
	componentCount = 0;
	
	file.seekBlock( "ObjectType" );
	char tmp[256];
	file.readIdString( "AppearanceName", tmp, 255 );
	fileName = tmp;

	file.seekBlock( "General" );
	file.readIdLong( "DescIndex", chassisNameID );
	file.readIdFloat( "CurTonnage", maxWeight );

	char tmpWeightClass[256];

	cLoadString( IDS_VEHICLE_CLASS, tmpWeightClass, 256 );

	mechClass = tmpWeightClass;

	if ( NO_ERR !=file.readIdLong( "HouseID", houseID ) )
	{
		houseID = -1;
	}
	if ( NO_ERR != file.readIdLong( "EncyclopediaID", encyclopediaID ) )
	{
		encyclopediaID = IDS_VEHICLE_DESCRIPTION_0;
	}

	file.seekBlock( "VehicleDynamics" );
	file.readIdFloat( "MaxVelocity", speed );

	char* parts[5] = { "Front", "Left", "Right", "Rear", "Turret" };

	baseArmor = 0;

	unsigned char pts;
	for ( int i = 0; i < 5; i++ )
	{
		file.seekBlock( parts[i] );
		file.readIdUChar( "MaxArmorPoints", pts );
		baseArmor += pts;
		file.readIdUChar( "CurInternalStructure", pts );
		baseArmor += pts;

	}

	file.seekBlock( "InventoryInfo" );
	file.readIdUChar( "NumWeapons", pts );

	char blockName[256];
	for ( i = 4; i < 4 + pts; i++ )
	{
		sprintf( blockName, "Item:%ld", i );
		if ( NO_ERR == file.seekBlock( blockName ) )
		{
			
			unsigned char fitID;
			file.readIdUChar( "MasterID", fitID );

			LogisticsComponent* pComponent = LogisticsData::instance->getComponent( fitID );
			if ( pComponent )
			{
			
				components[componentCount].component = pComponent;
				components[componentCount].xCoord = 0;
				components[componentCount].yCoord = 0;
				componentCount++;
			}
		
		}
	}
}

int LogisticsVehicle::getComponents( long& count, LogisticsComponent** array )
{
	gosASSERT( componentCount <= count );
	
	for ( int i = 0; i < componentCount; ++i )
	{
		array[i] = components[i].component;

	}

	count = componentCount;

	return count;
}
/*
bool	LogisticsVariant::getWeaponStatus( long weaponID ) //new function magic 03052012
{
 	for ( int i = 0; i < componentCount; ++i )
	{
		if (components[i].component->getID() == weaponID)
			return components[i].destroyed;

	}

	return true;

}

int	LogisticsVariant::setWeaponStatus( long weaponID, bool status ) //new function magic 03052012
{
 	for ( int i = 0; i < componentCount; ++i )
	{
		if (components[i].component->getID() == weaponID)
		{
			components[i].destroyed = status;
			return 1; //weapon found and satus set
		}
	}

	return 0; //error weapon not found

}
*/
//*************************************************************************************************
// end of file ( LogisticsVariant.cpp )
