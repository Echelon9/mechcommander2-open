#ifndef LOGISTICSVARIANT_H
#define LOGISTICSVARIANT_H
/*************************************************************************************************\
LogisticsVariant.h			: Interface for the LogisticsVariant component.  This thing
					is a simplified mech/vehicle object without AI.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

// forward declarations
class MechAppearance;
class LogisticsComponent;
class CSVFile;
class FitIniFile;

#include "EString.h"

#pragma warning (disable:4514)


enum LOGISTICS_COMPONENT_LOCATION
{
	HEAD = 0,
	CHEST = 1,
	LEGS = 2,
	RARM = 3,
	LARM = 4,
	RTORSO = 5,
	LTORSO = 6,
	CTORSO = 7
};


class LogisticsChassis
{
public:

	LogisticsChassis( );
	int				init( CSVFile* fileName, int chassisID );
	void			setFitID( int id ){ fitID = id; }
	int				getNameID() const { return chassisNameID; }
	int				getEncyclopediaID() const { return encyclopediaID; }
	const EString& getFileName() const { return fileName; }
	int				getHouseID() const{ return houseID; }
	float			getMaxWeight() const{ return maxWeight; }
	const EString&	getMechClass() const  { return mechClass; }
	long			getArmor() const { return baseArmor; }
	int				getArmorClass() const;
	float			getSpeed() const { return speed; }
	int				getDisplaySpeed() const;
	void			setScale( float newScale ) { scale = newScale; }
	float			getScale() const { return scale; }
	long			getMaxArmor() const { return maxArmor; }
	int				getSensorID() const;
	int				getECM() const;
	bool			jumpJetsAllowed() const { return canHaveJumpJets; }

protected:

	int					refCount;
	
	float				maxWeight;
	long				chassisNameID;	// resource ID
	//magic 31102011 begin
	EString				chassisName;
	//magic 31102011 end
	long				baseCost;
	long				maxHeat;
	long				baseArmor;
	long				maxArmor;

	float					speed;
	float				scale;

	long				iconPictureIndex;
	long				ID;
	EString				fileName;
	EString				mechClass;
	EString				iconFileNames[3];

	long				componentAreaWidth;
	long				componentAreaHeight;

	bool canHaveAdvSensor;//can Mech Have Advanced Sensor installed.
	bool canHaveOptics;//can Mech Have Optics Installed.
	bool canHaveGlobalSensor;//can Mech have Global Sensor installed.
	bool canHaveECM;//can Mech have ECM Installed.
	bool canHaveActiveProbe;//can Mech have Active Probe Installed.
	bool canHaveJumpJets;
	bool canHaveExtendedSensor;

	//Magic 03052012 begin
	unsigned char maxInternalPoints[8];
	//float curInternalPoints[8]; //magic 12052012 moved to variant

	float baseArmorPoints[11];
	//float curArmorPoints[11]; //magic 12052012 moved to variant
	//Magic 03052012 end
	/*

	//magic 25012011 begin
	long rightArmWidth;
	long rightArmHeight;
	long rightArmType;
	long leftArmWidth;
	long leftArmHeight;
	long leftArmType;
	long rightTorsoWidth;
	long rightTorsoHeight;
	long rightTorsoType;
	long leftTorsoWidth;
	long leftTorsoHeight;
	long leftTorsoType;
	long centerTorsoWidth;
	long centerTorsoHeight;
	long centerTorsoType;
	//magic 25012011 end */

	long				encyclopediaID;
	long				helpID;
	long				houseID;

	long				fitID;

	MechAppearance*		appearance; 

	friend class		LogisticsVariant;
	friend class		LogisticsMech;

	static int			weightClasses[]; // mech weights

	friend class LogisticsData;

	struct ComponentInfo
	{
		long										xCoord;
		long										yCoord;
		LOGISTICS_COMPONENT_LOCATION location;
		LogisticsComponent*				component;
		//bool destroyed; //magic 03052012 false weapon OK, true weapon destroyed
	};
	


};

class LogisticsVariant
{
protected:

	public:

		LogisticsVariant();
		LogisticsVariant( const LogisticsChassis* pChassis, bool isDesignerMech );
		LogisticsVariant( const LogisticsVariant&  ); // going to need copy c'tor
		virtual ~LogisticsVariant();
		
		int init( CSVFile* fileName, LogisticsChassis*, int blockID );

		int		compareWeight( LogisticsVariant* );
		
		inline bool			isAvailable() const { return (availableToUser && !bHidden); }
		unsigned long	getID() const { return ID; }
		float			getMaxWeight() const { return chassis->maxWeight; }
		long			getChassisName() const { return chassis->chassisNameID; }
		//magic 31102011 begin
		const EString&	getRealChassisName() const { return chassis->chassisName; }
		//magic 31102011 end
		const EString&		getName() const { return variantName; }
		long			getEncyclopediaID() const { return chassis->encyclopediaID; }
		long			getHelpID() const { return chassis->helpID; }
		long			getBaseCost() const { return chassis->baseCost; }
		long			getComponentCount() const { return componentCount; }
		int				canAddComponent( LogisticsComponent* pComponent, long& x, long& y ) const;
		int				getCost() const;
		//magic 25092012 begin
		int				getWeaponsCost() const;
		//magic 25092012 end
		int				getWeight() const;
		const EString&		getMechClass() const;
		int				getChassisID() const { return chassis->ID; }
		int				getArmor() const;
		int				getArmorClass() { return chassis->getArmorClass(); }
		int				getJumpRange() const;
		int				getMaxJumpRange() const;
		int				getSpeed() const;
		int				getDisplaySpeed() const;

		int				getVariantID() const { return ID & 0x00ffffff; }
		const EString&		getSmallIconFileName() const { return chassis->iconFileNames[0]; }
		const EString&	getMediumIconFileName() const { return chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return chassis->iconFileNames[2]; }
		int				getComponentsWithLocation( long& count, long* IDArray, long* xLocationArray, long* yLocationArray );
		int				removeComponent( long xCoord, long yCoord );
		int				addComponent( LogisticsComponent*, long& xCoord, long& yCoord );
		inline			bool canDelete( ) const { return !isDesignerMech(); }
		inline			bool isDesignerMech() const { return bDesignerMech; }
		int				getComponents( long& count, long* array );
		int				getComponents( long& count, LogisticsComponent** array );

		int				getHeat() const;
		int				getMaxHeat() const;
		LogisticsVariant& operator=( const LogisticsVariant&  );

		bool			operator==( const LogisticsVariant& src );
		const EString&		getFileName(){ return chassis->fileName; }

		long			save( FitIniFile& file, long counter );

		const LogisticsChassis* getChassis() const { return chassis; }
		bool			addComponent( int idFromFitFile, long& x, long& y );
		void			setName( const char* name ); // will allocate for you

		void		setAvailable( bool available );
		bool		allComponentsInInventory(LogisticsComponent** pComps, int& maxCount) const;//Magic

		int			getIconIndex() const { return chassis->iconPictureIndex; }

		int			getComponentAreaWidth() const{ return chassis->componentAreaWidth; } 
		int			getComponentAreaHeight() const { return chassis->componentAreaHeight; } 

		LogisticsComponent*	getCompAtLocation( int i, int j, long& realI, long& realJ );
		int			getComponentLocation( LogisticsComponent* pComp, long& i, long& j );

		int			getOptimalRangeString( long& color ) const;
		long		getHouseID(){ return chassis->houseID; }
		long		getMaxArmor() const { return chassis->maxArmor; }
//		int			getSensorID() const { return chassis->getSensorID(); }//Magic 61 disabled
		int			getSensorID(); //Magic 61
		void		setSensorID( int sensID ); // magic 61
		//int			getECM() const { return chassis->getECM(); }
		int			getECM() const; //magic 09022012
		bool		allComponentsAvailable() const;

		int			getFileID() const { return fileID; }
		/*//magic 03052012 begin
		bool	getWeaponStatus( long weaponID );
		int	setWeaponStatus( long weaponID, bool status = false );
		//magic 03052012 end*/

		//magic 12052012 begin
		unsigned char*	getMaxInternalPoints() { return chassis->maxInternalPoints; }
		float*	getCurInternalPoints() { return curInternalPoints; }
		float*	getBaseArmorPoints() { return chassis->baseArmorPoints; }
		float*	getCurArmorPoints() { return curArmorPoints; }
		//magic 12052012 end
		
		void	setCurInternalPoints( float* newCurInternalPoints);
		void	setCurArmorPoints( float* newCurArmorPoints);
		void	loadCurArmorPoints( float* newCurArmorPoints);
		
		long	repairInternalPoints();
		long	repairArmorPoints();
		//magic 05052012 end
		//magic 07052012 begin
		long	getInternalDamage();
		long	getArmorDamage();
		//magic 07052012 end
		//magic 08052012 begin
		void	overwriteCurArmorPoints( float* newCurArmorPoints);
		//magic 08052012 end
		//magic 12052012 begin
		void	maximizeInternalPoints();
		void	maximizeArmorPoints();
		//magic 12052012 end

	protected:

	
		const LogisticsChassis::ComponentInfo* getComponentAtLocation(long x, long y) const;

		
		unsigned long		ID; // bottom 8 bits = mech chassis, next 8 = number within
		int					fileID; // in csv file
		
		LogisticsChassis*	chassis;
		EString				variantName;	// in file
		
		long				componentCount;
		LogisticsChassis::ComponentInfo		components[54]; // I count a max of 54 components, might want to check though

		bool				availableToUser;
		bool				bDesignerMech;
		bool				bHidden;
		long				sensorID;//Magic 61
		//Magic 12052012 begin
		//unsigned char maxInternalPoints[8];
		float curInternalPoints[8]; //magic 12052012 moved to variant

		//float baseArmorPoints[11];
		float curArmorPoints[11]; //magic 12052012 moved to variant
		//Magic 12052012 end

		// HELPERS
		bool				hasJumpJets() const;
		bool				hasECM() const;
		bool				hasSensor() const;

		// SUPPRESSED
		
		friend class LogisticsMech;


};

class LogisticsVehicle : public LogisticsChassis
{

public:
	void init( FitIniFile& file );
	int  getComponents( long& count, LogisticsComponent** array );

protected:
	long				componentCount;
	ComponentInfo		components[54]; // I count a max of 54 components, might want to check though

};


//*************************************************************************************************
#endif  // end of file ( LogisticsVariant.h )
