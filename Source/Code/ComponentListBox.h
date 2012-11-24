#ifndef COMPONENTLISTBOX_H
#define COMPONENTLISTBOX_H
/*************************************************************************************************\
ComponentListBox.h			: Interface for the ComponentListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef ALISTBOX_H
#include "aListBox.h"
#endif

#ifndef AANIM_H
#include "aanim.h"
#endif

#ifndef ELIST_H
#include "eList.h"
#endif

class LogisticsVariant;
class LogisticsComponent;
class LogisticsVehicle;


#define COMP_ANIMATION_COUNT 5


//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
ComponentListBox:
**************************************************************************************************/
// complex list box items, icons and all
class ComponentListItem : public aListItem
{
	public:

		ComponentListItem( LogisticsComponent* pComp );
		virtual ~ComponentListItem();

		static int init( FitIniFile& file );

		LogisticsComponent* getComponent() { return pComponent; }

		virtual void render();

		void	update();

	private:

		friend class ComponentIconListBox;

		static ComponentListItem* s_templateItem;

		aText		name;
		aText		costText;
		aText		heatText;
		aObject		costIcon;
		aObject		heatIcon;
		aObject		icon;
		aRect		iconOutline;
		aRect		outline;
		aText		disabledText;

		aAnimGroup	animations[COMP_ANIMATION_COUNT];

		aAnimGroup*	pChildAnims[6];

		LogisticsComponent* pComponent;

		
		static void assignAnimation( FitIniFile& file,
			int whichChild, char animNames[COMP_ANIMATION_COUNT][32], aObject* pObject );

		void doAdd();
		void setComponent();
		void startDrag();


};

class ComponentIconListBox : public aListBox
{

public:
	ComponentIconListBox();
	virtual ~ComponentIconListBox();
	void setType( int Type, int orThisType, int orThis);
	void setType2( int Type, int Type2, int Type3, int Type4, int Type5, int Type6); //magic 08022012
	virtual void update();

	LogisticsComponent* getComponent();

	int selectFirstAvailableComponent();

	static ComponentIconListBox* s_instance;
		//Magic begin
	void removeIComponent( LogisticsComponent* pComp );
	void addIComponent( LogisticsComponent* pComp );
	void reType();
	void getIComponent( LogisticsComponent** pComp, int& maxCount );
	void getCinv( LogisticsComponent** pComp, int& maxCount );
	void saveInv( bool change );
	void savePurInv( bool change );
	void initComponents();
	void initPurComponents();
	int selectFirstPurComponent();
	void disableCThatCostMoreThanRP();
	long getCostOfSelected();
//Magic end	

	private:

		ComponentIconListBox( const ComponentIconListBox& src );
		ComponentIconListBox& operator=( const ComponentIconListBox& src );

		EList<ComponentListItem*, ComponentListItem*>	masterComponentList;
		LogisticsComponent*	Cinv[256];//Magic
		int	MaxCinv;//Magic
		LogisticsComponent* pIComp[256]; //Magic
		int pIcount; //Magic
		bool firstpass;

		int type;

		void addSortedItem( ComponentListItem* pItem );


};


//*************************************************************************************************
#endif  // end of file ( ComponentListBox.h )
