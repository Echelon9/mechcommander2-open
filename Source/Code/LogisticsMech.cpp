#define LOGISTICSMECH_CPP
/*************************************************************************************************\
LogisticsMech.cpp			: Implementation of the LogisticsMech component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "LogisticsMech.h"
#include "LogisticsData.h"
#include "IniFile.h"

LogisticsMech::LogisticsMech( LogisticsVariant* pVar, int Count ) : pVariant( pVar )
{
	ID = (Count << 24 | pVar->getID() );
	pilot = 0;
	forceGroup = 0;
	//magic 12052012 begin
	float* curArmPoints = pVariant->getCurArmorPoints();
	for (int i=0; i<11; i++)
		curArmorPoints[i] = curArmPoints[i];

	float* curIntPoints = pVariant->getCurInternalPoints();
	for (int i=0; i<8; i++)
		curInternalPoints[i] = curIntPoints[i];
	//magic 12052012 end
}

LogisticsMech::~LogisticsMech()
{
	if ( pilot )
		pilot->setUsed( 0 );

	pilot = NULL;
}

void LogisticsMech::setPilot( LogisticsPilot* pPilot )
{
	if ( pilot )
		pilot->setUsed( 0 );

	pilot = pPilot;
	if ( pilot )
		pilot->setUsed( 1 );

}

void LogisticsMech::setVariant( LogisticsVariant* pVar )
{
	pVariant = pVar;
	ID = ( LogisticsData::instance->createInstanceID( pVar ) << 24 | pVar->getID() );

}


long LogisticsMech::save( FitIniFile& file, long counter )
{
	char tmp[256];
	sprintf( tmp, "Inventory%ld", counter );

	file.writeBlock( tmp );

	file.writeIdString( "Chassis", pVariant->getFileName() );
	file.writeIdString( "Variant", pVariant->getName() );
	if ( pilot )
		file.writeIdString( "Pilot", pilot->getFileName() );
	file.writeIdBoolean( "Designer", pVariant->isDesignerMech() );
	//magic 12052012 begin
	//float* curInteranalPoints =  pVariant->getCurInternalPoints(); //magic 12052012 disabled
	file.writeIdFloat( "CurInternalHead", curInternalPoints[0] );
	file.writeIdFloat( "CurInternalCTorso", curInternalPoints[1] );
	file.writeIdFloat( "CurInternalLTorso", curInternalPoints[2] );
	file.writeIdFloat( "CurInternalRTorso", curInternalPoints[3] );
	file.writeIdFloat( "CurInternalLArm", curInternalPoints[4] );
	file.writeIdFloat( "CurInternalRArm", curInternalPoints[5] );
	file.writeIdFloat( "CurInternalLLeg", curInternalPoints[6] );
	file.writeIdFloat( "CurInternalRLeg", curInternalPoints[7] );

	//float* curArmorPoints =  pVariant->getCurArmorPoints(); //magic 12052012 disabled
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

void	LogisticsMech::setColors( unsigned long base, unsigned long high1, unsigned long high2 )
{
	baseColor = base;
	highlightColor1 = high1;
	highlightColor2 = high2;
}

void	LogisticsMech::getColors( unsigned long& base, unsigned long& high1, unsigned long& high2 ) const
{
	base = baseColor;
	high1 = highlightColor1;
	high2 = highlightColor2;
}

long LogisticsMech::savePurchase( FitIniFile& file, long counter )
{
	char tmp[256];
	sprintf( tmp, "MechPurchase%ld", counter );

	file.writeBlock( tmp );

	file.writeIdString( "Chassis", pVariant->getFileName() );
	file.writeIdString( "Variant", pVariant->getName() );
	if ( pilot )
		file.writeIdString( "Pilot", pilot->getFileName() );
	file.writeIdBoolean( "Designer", pVariant->isDesignerMech() );
	//magic 12052012 begin
	//float* curInteranalPoints =  pVariant->getCurInternalPoints(); //magic 12052012 disabled
	file.writeIdFloat( "CurInternalHead", curInternalPoints[0] );
	file.writeIdFloat( "CurInternalCTorso", curInternalPoints[1] );
	file.writeIdFloat( "CurInternalLTorso", curInternalPoints[2] );
	file.writeIdFloat( "CurInternalRTorso", curInternalPoints[3] );
	file.writeIdFloat( "CurInternalLArm", curInternalPoints[4] );
	file.writeIdFloat( "CurInternalRArm", curInternalPoints[5] );
	file.writeIdFloat( "CurInternalLLeg", curInternalPoints[6] );
	file.writeIdFloat( "CurInternalRLeg", curInternalPoints[7] );

	//float* curArmorPoints =  pVariant->getCurArmorPoints(); //magic 12052012 disabled
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

//magic 12052012 begin
void	LogisticsMech::setCurInternalArray( float* newCurInternalPoints)
{
	for (int i=0; i<8; i++)
		curInternalPoints[i] = newCurInternalPoints[i]; 
}
void	LogisticsMech::setCurArmorArray( float* newCurArmorPoints)
//newCurArmorPoints contain damage from the mission
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] -= newCurArmorPoints[i];
		if (curArmorPoints[i] < 0.f)
			curArmorPoints[i] = 0.f;
	}
}
void	LogisticsMech::loadCurArmorArray( float* newCurArmorPoints)
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] = newCurArmorPoints[i];
		if (curArmorPoints[i] < 0.f)
			curArmorPoints[i] = 0.f;
	}
}

long	LogisticsMech::repairInternalPoints()
{
	unsigned char* maxIntPoints = pVariant->getMaxInternalPoints();
	float sumCurInternalPoints = 0.f;
	float sumMaxInternalPoints = 0.f;
	float pointsToRepair = 0.f;
	for (int i=0; i<8; i++)
	{
		sumCurInternalPoints += curInternalPoints[i];
		sumMaxInternalPoints += (float)maxIntPoints[i];
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
				curInternalPoints[i] = (float)maxIntPoints[i];
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
					if (curInternalPoints[i] < (float)maxIntPoints[i])
					{
						curInternalPoints[i] += 0.25f;
						pointsToRepair -= 0.25f;
						if (curInternalPoints[i] > (float)maxIntPoints[i])
							curInternalPoints[i] = (float)maxIntPoints[i];
					}
				}
			}
		}
	}
	return (long)pointsToRepair;

}

long	LogisticsMech::repairArmorPoints()
{
	float* baseArmPoints = pVariant->getBaseArmorPoints();
	float sumCurArmorPoints = 0;
	float sumBaseArmorPoints = 0;
	float pointsToRepair = 0.f;
	for (int i=0; i<11; i++)
	{
		sumCurArmorPoints += curArmorPoints[i];
		sumBaseArmorPoints += baseArmPoints[i];
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
				curArmorPoints[i] = baseArmPoints[i];
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
					if (curArmorPoints[i] < baseArmPoints[i])
					{
						curArmorPoints[i] += 0.25f;
						pointsToRepair -= 0.25f;
						if (curArmorPoints[i] > baseArmPoints[i])
							curArmorPoints[i] = baseArmPoints[i];
					}
				}
			}
		}
	}
							/*//Magic Debug file open begin
							char tmp2[256];
							FitIniFile MtestFile;
							MtestFile.open("data\\repairArmor.fit", CREATE);
							//Magic Debug file write begin
							sprintf( tmp2, "max0 - cur0: %f - %f", chassis->baseArmorPoints[0],chassis->curArmorPoints[0]);
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max1- cur1: %f - %f", chassis->baseArmorPoints[1], chassis->curArmorPoints[1] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max2- cur2: %f - %f", chassis->baseArmorPoints[2], chassis->curArmorPoints[2] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max3- cur3: %f - %f", chassis->baseArmorPoints[3], chassis->curArmorPoints[3] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max4- cur4: %f - %f", chassis->baseArmorPoints[4], chassis->curArmorPoints[4]);
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max5- cur5: %f - %f", chassis->baseArmorPoints[5], chassis->curArmorPoints[5] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max6- cur6: %f - %f", chassis->baseArmorPoints[6], chassis->curArmorPoints[6] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max7- cur7: %f - %f", chassis->baseArmorPoints[7], chassis->curArmorPoints[7] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max8- cur8: %f - %f", chassis->baseArmorPoints[8], chassis->curArmorPoints[8] );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max9- cur9: %f - %f", chassis->baseArmorPoints[9], chassis->curArmorPoints[9]  );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "max10- cur10: %f - %f", chassis->baseArmorPoints[10], chassis->curArmorPoints[10]  );
							MtestFile.writeBlock( tmp2 );
							sprintf( tmp2, "base armor sum - cur armor sum: %f - %f = %f", sumBaseArmorPoints, sumCurArmorPoints, pointsToRepair );
							MtestFile.writeBlock( tmp2 );
							MtestFile.close();
							//magic debug file write end*/

		return (long)pointsToRepair;
}
long	LogisticsMech::getInternalDamage()
{
	unsigned char* maxIntPoints = pVariant->getMaxInternalPoints();
	float sumCurInternalPoints = 0.f;
	float sumMaxInternalPoints = 0.f;
	float pointsToRepair = 0.f;
	for (int i=0; i<8; i++)
	{
		sumCurInternalPoints += curInternalPoints[i];
		sumMaxInternalPoints += (float)maxIntPoints[i];
	}
	pointsToRepair = sumMaxInternalPoints - sumCurInternalPoints;

	return (long)pointsToRepair;

}

long	LogisticsMech::getArmorDamage()
{
	float* baseArmPoints = pVariant->getBaseArmorPoints();
	float sumCurArmorPoints = 0.f; //magic 02102012
	float sumBaseArmorPoints = 0.f; //magic 02102012
	float pointsToRepair = 0.f;
	for (int i=0; i<11; i++)
	{
		sumCurArmorPoints += curArmorPoints[i];
		sumBaseArmorPoints += baseArmPoints[i];
	}
	
	pointsToRepair = sumBaseArmorPoints - sumCurArmorPoints;

	return (long)pointsToRepair;
}
void	LogisticsMech::overwriteCurArmorPoints( float* newCurArmorPoints)
{
	for (int i=0; i<11; i++)
	{
		curArmorPoints[i] = newCurArmorPoints[i];
	}
}

int LogisticsMech::getCost( ) const 
{
	float* baseArmPoints = pVariant->getBaseArmorPoints();
	int cost = pVariant->getCost();
	
	float sumCurArmorPoints = 0;
	float sumBaseArmorPoints = 0;
	float pointsToRepair = 0.f;
	for (int i=0; i<11; i++)
	{
		sumCurArmorPoints += curArmorPoints[i];
		sumBaseArmorPoints += baseArmPoints[i];
	}
	
	pointsToRepair = sumBaseArmorPoints - sumCurArmorPoints;
	long armDamage = (long)pointsToRepair;
	
	//long intDamage = getInternalDamage();
	unsigned char* maxIntPoints = pVariant->getMaxInternalPoints();
	float sumCurInternalPoints = 0.f;
	float sumMaxInternalPoints = 0.f;
	pointsToRepair = 0.f;
	for (int i=0; i<8; i++)
	{
		sumCurInternalPoints += curInternalPoints[i];
		sumMaxInternalPoints += (float)maxIntPoints[i];
	}
	pointsToRepair = sumMaxInternalPoints - sumCurInternalPoints;
	long intDamage = (long)pointsToRepair;

	cost -= armDamage*25;
	cost -= intDamage*100;
	//magic 10052012 end

	return cost;
}

//magic 12052012 end
//*************************************************************************************************
// end of file ( LogisticsMech.cpp )
