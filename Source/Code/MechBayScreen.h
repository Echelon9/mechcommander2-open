#ifndef MECHBAYSCREEN_H
#define MECHBAYSCREEN_H

/*************************************************************************************************\
MechBayScreen.h : Header file for mech selection
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "LogisticsScreen.h"
#include "AttributeMeter.h"
#include "MechListBox.h"
#include "SimpleComponentListBox.h"


class LogisticsMech;
class LogisticsMechIcon;
class SimpleCamera;

#define ICON_COUNT		12
#define ICON_COUNT_X	4
#define ICON_COUNT_Y	3


#define MB_MSG_NEXT 50
#define MB_MSG_PREV 51
#define MB_MSG_ADD 52
#define MB_MSG_REMOVE 53
#define MB_MSG_BUY 54
#define MB_MSG_CHANGE_LOADOUT 55
#define MB_MSG_BUY_SELL 56
#define MB_MSG_MAINMENU	57
#define MB_MSG_ADDWEIGHT	58
#define MB_MSG_DROPWEIGHT	59
//magic 07052012 begin
#define MB_MSG_ADDRP	60
#define MB_MSG_DROPRP	61
#define MB_MSG_REPAIR__ALL	62
#define MB_MSG_REPAIR__INT	63
#define MB_MSG_REPAIR__ARM	64
//magic 07052012 end

class MechBayScreen : public LogisticsScreen
{

public:

	static MechBayScreen* instance(){ return s_instance; }
	MechBayScreen();
	virtual ~MechBayScreen();

	void init(FitIniFile* file);
	virtual void render(int xOffset, int yOffset);
	virtual void update();
	//virtual void update2();//Magic 73
	virtual void update3();//Magic 73
	virtual void begin();
	virtual void end();
	
	virtual int			handleMessage( unsigned long, unsigned long );
	void setMech( LogisticsMech* pMech, bool bCommandFromLB = true );	
	void beginDrag( LogisticsMech* pMech );
	void reLoadMechListBox();//Magic
	//magic 09052012 begin
	bool	isInternalDamaged(LogisticsMech* pMech);
	//magic 09052012 end

	
private:

	LogisticsMech*		pCurMech;
	LogisticsMech*		pDragMech;
	LogisticsMechIcon*	pIcons;
	aObject				dragIcon;
	bool				dragLeft;
	long				forceGroupCount;

	AttributeMeter		attributeMeters[3];
	MechListBox			mechListBox;
	aListBox			componentListBox;
	aObject				dropWeightMeter;
	aAnimation			addWeightAnim;
	long				addWeightAmount;
	aAnimation			removeWeightAnim;
	long				removeWeightAmount;

	long				weightCenterX;
	long				weightCenterY;
	long				weightStartColor;
	long				weightEndColor;

	static	MechBayScreen*	s_instance;

	SimpleCamera*		mechCamera;
	ComponentListBox	loadoutListBox;

	void removeSelectedMech();
	void addSelectedMech();
	
	void drawWeightMeter(long xOffset, long yOffset);
	void reinitMechs();


	MechBayScreen( const MechBayScreen& );
	MechBayScreen& operator=( const MechBayScreen& );

	void				unselectDeploymentTeam();
	bool				selectFirstFGItem();
	bool				selectFirstViableLBMech();
	LogisticsMech*		getFGSelMech();
	//magic 09052012 end
	bool bErrorDlg;
	//magic 09052012 end



};

#endif