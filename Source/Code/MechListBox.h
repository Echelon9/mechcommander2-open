#ifndef MECHLISTBOX_H
#define MECHLISTBOX_H
/*************************************************************************************************\
MechListBox.h			: Interface for the MechListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef ALISTBOX_H
#include "aListBox.h"
#endif

#ifndef AANIM_H
#include "aAnim.h"
#endif

class FitIniFile;
class LogisticsMech;


//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MechListBox:
**************************************************************************************************/

#define ANIMATION_COUNT 4
class MechListBoxItem : public aListItem
{
public:
	
	virtual ~MechListBoxItem();
	MechListBoxItem( LogisticsMech* pMech, long count = 1 );

	static void init( FitIniFile& file );

	virtual void update();
	virtual void render();

	LogisticsMech* getMech( ){ return pMech; }
	void	resetMech( ) {pMech = NULL; }
	virtual bool		pointInside(long xPos, long yPos) const;

	bool		bOrange;
	static bool	bAddCalledThisFrame;

	//magic 08052012 begin
	bool	isArmorDamaged();
	bool	isInternalDamaged();
	//magic 08052012 end

	
private:

	static MechListBoxItem* s_templateItem;
	
	aAnimGroup		animations[2][ANIMATION_COUNT];
	
	aObject		mechIcon;
	aText		chassisName;
	aText		variantName;
	aText		countText;
	aText		weightText;
	aObject		weightIcon;
	aText		costText;
	aObject		costIcon;
	aRect		outline;
	aRect		line;

	long		animationIDs[9];

	bool		drawCBills;
	bool		bIncludeForceGroup;

	LogisticsMech* pMech;

	long mechCount;

	MechListBoxItem& operator=( const MechListBoxItem& src );

	friend class MechListBox;

	void setMech();
	void doAdd();
	void doAddMechBay(); //Magic 72
	void startDrag();
	static void assignAnimation( FitIniFile& file, long& curAnimation );


	bool bDim;

	float animTime;
	/*
	//magic 11052012 begin
	char	damage[8];
	//magic 11052012 end
	*/ //magic 21082012 disabled

};


class MechListBox: public aListBox
{
public:

	MechListBox(bool deleteIfNoInventory, bool bIncludeForceGroup);

	
	void	setScrollBarOrange();
	void	setScrollBarGreen();

	virtual ~MechListBox();
	
	static int		init();
	void	drawCBills( bool bDraw );

	virtual void update();

	LogisticsMech* getCurrentMech();
	LogisticsMech* getListMech(int itemNum);

	static void initIcon( LogisticsMech* pMec, aObject& icon );
	static void initIcon2( long index, aObject& icon, char* newDamage, char* oldDamage ); //magic 12052012

	virtual long		AddItem(aListItem* itemString);
	virtual long		RemoveMBItem(aListItem* itemString); //magic 15022011

	void	dimItem( LogisticsMech* pMech, bool bDim );
	void	undimAll();
	void	disableItemsThatCostMoreThanRP();
	void	disableItemsThatCanNotGoInFG();
	void	disableItemsWith0Q();

	void	setOrange( bool bOrange );
	long	getNumVariants(LogisticsMech* pMech ); //M12
	void	incMechCount( LogisticsMech* pMech ); //M12
	void	decMechCount( LogisticsMech* pMech ); //M12
	long	getMechCount( LogisticsMech* pMech );//M12

	

private:
	
	static bool	s_DrawCBills;
	
	MechListBox( const MechListBox& src );
	MechListBox& operator=( const MechListBox& src );

	friend class MechListBoxItem;

	bool		bDeleteIfNoInventory;
	bool		bIncludeForceGroup;

	bool		bOrange;



	

};


//*************************************************************************************************
#endif  // end of file ( MechListBox.h )
