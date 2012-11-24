#ifndef LOGISTICSDATA_H
#define LOGISTICSDATA_H
/*************************************************************************************************\
LogisticsData.h			: Interface for the LogisticsData component.  This thing holds the inventory
							lists and the component lists
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stuff\stuff.hpp"
#include <EList.h>

#include "LogisticsMech.h"
#include "LogisticsComponent.h"
#include "LogisticsPilot.h"

class FitIniFile;
class LogisticsMissionInfo;
class PacketFile;

class LogisticsData
{

public:

	struct Building;

	LogisticsData();
	~LogisticsData();
	void			init();

	bool			rpJustAdded;
	bool			loadFromSave;//Magic 63
	bool			loadSoloMission;//Magic 63

	static LogisticsData*		instance;

	LogisticsComponent* getComponent( int componentID );
	LogisticsPilot*		getPilot( int pilotID );
	LogisticsPilot*		getPilot( const char* pilotName );
	int					getPilotCount();
	int					getPilots( LogisticsPilot**, long& count );
	const char*			getBestPilot( long mechWeight );
	bool				gotPilotsLeft();

	void				setPilotUnused( const char* pPilot );

	int					getVariantsInInventory( LogisticsVariant* pVar, bool bIncludeForceGroup );
	int					getPlayerVariantNames( const char**, int& count );
	int					getChassisVariants( const LogisticsChassis* pChassis, LogisticsVariant** pVar, int& count );
	
	LogisticsVariant*	getVariant( int ID );
	LogisticsVariant*	getVariant( const char* mechName );
	LogisticsVariant*	getVariant( const char* pCSVFileNaem, int VariantNum );
	int					removeVariant( const char* varName );


	LogisticsMech*		getMech( int ID );
	LogisticsMech*		getMech( const char* MechName, const char* pilotName );
	LogisticsMech*		getMechWithoutForceGroup( LogisticsMech* clone );

	void				addMechToInventory( LogisticsVariant* pVar, LogisticsPilot* pPilot, int ForceGrup,
											bool bSubtractPts = 0);




	int createInstanceID( LogisticsVariant* pVar );
	LogisticsPilot*		getFirstAvailablePilot();
	void				getForceGroup( EList<LogisticsMech*, LogisticsMech*>& newList );
	void				getInventory( EList<LogisticsMech*, LogisticsMech*>& newList );
	void				getComponentsInventory( EList<LogisticsComponent*, LogisticsComponent*>& newList );

	void				removeMechsInForceGroup(); // takes mechs that are in the force group out of the inventory
	void				addMechToInventory( LogisticsVariant* pVar,
											int addToForceGroup, LogisticsPilot*,
											unsigned long color1 = 0xffffffff, 
											unsigned long color2 = 0xffffffff,
											unsigned long color3 = 0xffffffff);
	void				removeMechFromInventory( const char* mechName, const char* pilotName );
	void				removeComponentFromInventory( int weaponID ); //M
	void				removeComponentFromInventory( LogisticsComponent* mComp ); //M

	int					addMechToForceGroup( LogisticsMech* pMech, int slot );
	int					removeMechFromForceGroup( LogisticsMech* pMech, bool bRemovePilot );
	int					removeMechFromForceGroup( int slot );

	bool				canAddMechToForceGroup( LogisticsMech* pMech );


	long				save( FitIniFile& file );
	long				load( FitIniFile& file );

	void				setResourcePoints(long rp) {resourcePoints = rp; rpJustAdded = 0;}
	int					getResourcePoints() { return resourcePoints; }
	void				addResourcePoints( int amount ){ resourcePoints+= amount; rpJustAdded = true;}
	void				decrementResourcePoints( int amount ){ resourcePoints -= amount; }

	int					getCBills();
	void				addCBills( int amount );
	void				decrementCBills( int amount );

	int					comparePilots( LogisticsPilot* p1, LogisticsPilot* p2, long weight );
	int					getCurrentDropWeight() const;
	int					getMaxDropWeight() const;
	//magic 02062012 begin
	int					setMaxDropWeight(int amount);
	int					getCurrentRP() const;
	int					addRP( int amount);
	int					setRP( int amount);
	//magic 02062012 end
	int					incrementMaxDropWeight( int amount); //M17
	int					decrementMaxDropWeight( int amount); //M17

	long				loadMech( FitIniFile& file, int& count );
	long				loadComponent( FitIniFile& file, int& count );

	void				setMissionCompleted();
	long				updateAvailability();

	const EString&		getCurrentMission() const;
	const EString&		getLastMission() const;
	int					setCurrentMission( const EString& missionName );
	void				setSingleMission( const char* pName );
	bool				isSingleMission();
	long				getCurrentMissionTune();
	const char *		getCurrentABLScript() const;
	long 				getCurrentMissionId();

	void				clearInventory();
	void				clearComponentsInventory();
	//magic 04092012 begin
	void				clearSalvageComponents() { salvageComponents.Clear(); }
	long				quickSaveSalvageComponents( FitIniFile& file );
	long				quickLoadSalvageComponents( FitIniFile& file );
	//magic 04022012 end

	int					getPurchasableMechs( LogisticsVariant**, int& count );
	int					canPurchaseMech( LogisticsVariant* pVar );
	int					purchaseMech( LogisticsVariant* pVar );
	int					addComponentToInventory( LogisticsComponent* iComp ); //M
	int					sellMech( LogisticsMech* pVar );
	int					getEncyclopediaMechs( const LogisticsVariant**, int& count );
	int					getHelicopters( const LogisticsVariant**, int& count );
	
	int					getVehicles( const LogisticsVehicle**, int& count );
	LogisticsVehicle*	getVehicle( const char* pName );


	int					setMechToModify( LogisticsMech* pVariant );
	LogisticsMech*		getMechToModify( ){ return currentlyModifiedMech; }
	int					acceptMechModifications( const char* pNewVariantName );
	int					cancelMechModfications();
	bool				canReplaceVariant( const char* name );
	bool				canDeleteVariant( const char* name );

	void				setCurrentMissionNum (long cMission);
	long				getCurrentMissionNum (void);

	int					getAvailableComponents( LogisticsComponent** pComps, int& maxCount );
	int					getAllComponents( LogisticsComponent** pComps, int& maxCount );
	int					getAllInventoryComponents( LogisticsComponent** pComps, int& maxCount ); //M
	const char*			getCurrentOperationFileName();
	const char*			getCurrentVideoFileName();
	const char*			getCurrentMissionDescription();
	int					getAvailableMissions( const char** missionNames, long& count );
	int					getCurrentMissions( const char** missionNames, long& count );
	bool				getMissionAvailable( const char* missionName );
	const char*			getCurrentMissionFriendlyName( );
	const char*			getMissionFriendlyName( const char* missionName );
	long				getMaxTeams() const;
	long				getMaxPlayers() const;

	int					acceptMechModificationsUseOldVariant( const char* name );

	const EString&		getCampaignName() const;




	int					setCurrentMission( const char* missionName );

	void				startNewCampaign( const char* fileName = "campaign");
	void				startMultiPlayer();
	void				setPurchaseFile( const char* fileName );

	Building*			getBuilding( int nameID );
	int					getBuildings( Building** bdgs, int& count );

	bool				campaignOver();
	const char*			getCurrentBigVideo() const;
	const char*			getFinalVideo() const;

	bool				newMechsAvailable() { return bNewMechs; }
	bool				newPilotsAvailable() { return bNewPilots; }
	bool				newWeaponsAvailable() { return bNewWeapons; }

	void				setNewPilotsAcknowledged(){ bNewPilots = 0; }
	void				setNewWeaponsAcknowledged(){ bNewWeapons = 0; }
	void				setNewMechsAcknowledged(){ bNewMechs = 0; }

	void				addNewBonusPurchaseFile( const char* pFileName ); 
	void				addNewComponentSalvage( const char* pFileName ); //magic 03092012

	void				appendAvailability(const char* pFileName, bool* availableArray );

	bool				skipLogistics();
	bool				showChooseMission();
	bool				skipPilotReview();
	bool				skipSalvageScreen();
	bool				skipPurchasing();

	bool				canHaveSalavageCraft();
	bool				canHaveRepairTruck();
	bool				canHaveScoutCopter();
	bool				canHaveArtilleryPiece();
	bool				canHaveAirStrike();
	bool				canHaveSensorStrike();
	bool				canHaveMineLayer();
	//magic 15072011 begin
	bool				toggleRepairTruck();
	bool				toggleSalvageCraft(); //magic 29082011
	//magic 15072011 end
	//magic 25052012 begin
	bool				toggleSensorStrike();
	bool				toggleArtilleryPiece();
	bool				toggleAirStrike();
	//magic 25052012 end
	bool				getVideoShown();
	void				setVideoShown();

	void	UpdateCinventory(LogisticsComponent** pComp, int& maxCount);//Magic
	void	UpdatePinventory(LogisticsComponent** pComp, int& maxCount);//Magic
	long	LoadMechsForPurchase(LogisticsVariant** PurVar, int& PurVarCount);//Magic
	LogisticsVariant* getPurVariant( const char* pCSVFileName, int VariantNum ); //Magic
	long				initPurVariants(); //Magic
	void				UpdatePurVariants(LogisticsVariant** pVar, int& maxCount); //Magic
	void	getPurVariants( EList<LogisticsVariant*, LogisticsVariant*>& newList ); //Magic
	void	getAllVariants( EList<LogisticsVariant*, LogisticsVariant*>& newList ); //magic 18122010
	long	loadPurMech( FitIniFile& file, int& count ); //Magic
	int		getAllPurchaseComponents( LogisticsComponent** pComps, int& maxCount ); //Magic
	bool	isLoadFromSave() { return loadFromSave; }//Magic 63
	//void	setLoadFromSave( bool myVar );// { loadFromSave = myVar; }//Magic 63
	bool	isLoadSolo() { return loadSoloMission; }//Magic 63
	//void	setLoadSolo( bool myVar );// { loadSoloMission = myVar; }//Magic 63
	EString getPurchaseFileName();




	struct Building
	{
		long nameID;
		long weight;
		long encycloID;
		long componentIDs[4];
		char fileName[64];
		float scale;
	};




private:

	bool		bNewMechs;
	bool		bNewPilots;
	bool		bNewWeapons;

	typedef EList< LogisticsComponent, const LogisticsComponent& >	COMPONENT_LIST;
	typedef EList< LogisticsComponent*, LogisticsComponent* >		ICOMPONENT_LIST;//Magic
	typedef EList< LogisticsPilot, const LogisticsPilot& >			PILOT_LIST;
	typedef EList< LogisticsVariant*, LogisticsVariant* >			VARIANT_LIST;
	typedef EList< LogisticsMech*, LogisticsMech* >					MECH_LIST;
	typedef EList< LogisticsVehicle*, LogisticsVehicle* >			VEHICLE_LIST;
	typedef EList< Building, const Building& >						BUILDING_LIST;
	
	VARIANT_LIST									variants;
	VARIANT_LIST									purvariants; //Magic
	MECH_LIST										inventory;
	COMPONENT_LIST									components;
	ICOMPONENT_LIST									icomponents;//Magic
	ICOMPONENT_LIST									purcomponents;//Magic
	ICOMPONENT_LIST									salvageComponents;//Magic 04092012

	PILOT_LIST										pilots;
	VEHICLE_LIST									vehicles;
	BUILDING_LIST									buildings;
	
	LogisticsMissionInfo*							missionInfo;
	

	LogisticsMech*									currentlyModifiedMech;
	LogisticsVariant*								oldVariant;
		
	
	int												resourcePoints; // C-Bills for buying mechs

	long	loadVariant( FitIniFile& file );


	

	// HELPERS
	void RegisterFunctions();
	void UnRegisterFunctions();

	void initComponents();
	void initPilots();
	void initVariants();

	void addVehicle( long fitID, PacketFile& objectFile, float scale );
	int  addBuilding( long fitID, PacketFile& objectFile, float scale );
	void removeDeadWeight();
	void clearVariants();




	

};


//*************************************************************************************************
#endif  // end of file ( LogisticsData.h )
