#ifndef LOGISTICSMECH_H
#define LOGISTICSMECH_H
//===========================================================================//
//LogisticsMech.h			: Interface for the LogisticsMech component.     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

class LogisticsPilot;

#include "LogisticsVariant.h"

class LogisticsMech
{
	public:

		LogisticsMech( LogisticsVariant*, int count );
		~LogisticsMech();

		inline int getID(){ return ID; }
		inline int getInstanceID() { return ID >> 24; }
		void	setPilot( LogisticsPilot* pPilot );
		inline LogisticsPilot* getPilot( ) { return pilot; }
		inline int getForceGroup() { return forceGroup; }
		inline void setForceGroup( int bInForceGroup ) { forceGroup = bInForceGroup; }
		inline const char* getFileName(){ return pVariant->chassis->fileName; }
		inline bool		isAvailable() const { return pVariant->availableToUser; }
		unsigned long	getID() const { return pVariant->ID; }
		float			getMaxWeight() const { return pVariant->chassis->maxWeight; }
		long			getChassisName() const { return pVariant->chassis->chassisNameID; }
		//magic 31102011 begin
		const EString&	getRealChassisName() const { return pVariant->chassis->chassisName; }
		//magic 31102011 end
		const EString&		getName() const { return pVariant->variantName; }
		long			getEncyclopediaID() const { return pVariant->chassis->encyclopediaID; }
		long			getHelpID() const { return pVariant->chassis->helpID; }
		long			getBaseCost() const { return pVariant->chassis->baseCost; }
		long			getComponentCount() const { return pVariant->componentCount; }
		int				canAddComponent( LogisticsComponent* pComponent, long x, long y ) const
		{ return pVariant->canAddComponent( pComponent, x, y ); }
		//int				getCost() const { return pVariant->getCost(); } //magic 12052012 disabled
		int				getWeight() const { return pVariant->getWeight(); }
		const char*		getMechClass() const { return pVariant->getMechClass(); }
		int				getChassisID() const { return pVariant->chassis->ID; }
		int				getArmor() const { return pVariant->getArmor(); }
		int				getJumpRange() const { return pVariant->getJumpRange(); }
		int				getSpeed() const { return pVariant->getSpeed(); }
		int				getDisplaySpeed() const { return pVariant->getDisplaySpeed(); }
		int				getVariantID() const { return pVariant->ID; }
		const EString&		getSmallIconFileName() const { return pVariant->chassis->iconFileNames[0]; }
		const EString&		getMediumIconFileName() const { return pVariant->chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return pVariant->chassis->iconFileNames[2]; }
		int				getComponentsWithLocation( long& count, long* IDArray, long* xLocationArray, long* yLocationArray )
		{
			return pVariant->getComponentsWithLocation( count, IDArray, xLocationArray, yLocationArray );
		}

		int				getComponents( long& count, long* IDArray )
		{
			return pVariant->getComponents( count, IDArray );
		}
		//magic 12052012 begin
		float*	getCurInternalArray()
		{
			//return pVariant->getCurInternalPoints();
			return curInternalPoints; //magic 12052012
		}

		float*	getCurArmorArray()
		{
			//return pVariant->getCurArmorPoints();
			return curArmorPoints; //magic 12052012
		}

		unsigned char*	getMaxInternalArray()
		{
			return pVariant->getMaxInternalPoints();
		}

		float*	getBaseArmorArray()
		{
			return pVariant->getBaseArmorPoints();
		}

		void	setCurInternalArray(float* newCurInternalPoints);
		void	setCurArmorArray(float* newCurArmorPoints);
		void	loadCurArmorArray(float* newCurArmorPoints);

		long	repairInternalPoints();
		long	repairArmorPoints();
		long	getInternalDamage();
		long	getArmorDamage();
		void	overwriteCurArmorPoints( float* newCurArmorPoints);
		int		getCost()const;
		//magic 12052012 end

		void			setVariant( LogisticsVariant* pVariant );

		int				getFitID() const { return pVariant->chassis->fitID; }
		long			save( FitIniFile& file, long count );
		long			savePurchase( FitIniFile& file, long counter );//Magic

		void			setColors( unsigned long base, unsigned long high1, unsigned long high2 );
		void			getColors( unsigned long& base, unsigned long& high1, unsigned long& high2 ) const;
		int				getIconIndex() const { return pVariant->chassis->iconPictureIndex; }

		LogisticsVariant* getVariant() { return pVariant; }

	

	private:
		LogisticsVariant* pVariant;

		unsigned long	pilotID;
		int				forceGroup; // 0 if none, 1 if otherwise
		LogisticsPilot*	pilot;
		int				ID;
		long			baseColor;
		long			highlightColor1;
		long			highlightColor2;

		//Magic 12052012 begin
		//unsigned char maxInternalPoints[8];
		float curInternalPoints[8];

		//float baseArmorPoints[11];
		float curArmorPoints[11];
		//Magic 12052012 end
		// DATA

		// HELPER FUNCTIONS

		// suppress these puppies
		LogisticsMech();
		LogisticsMech& operator=( const LogisticsMech& );

};


//*************************************************************************************************
#endif  // end of file ( LogisticsMech.h )
