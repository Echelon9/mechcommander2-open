#ifndef MECHPURCHASESCREEN_H
#define MECHPURCHASESCREEN_H
/*************************************************************************************************\
MechPurchaseScreen.h			: Interface for the MechPurchaseScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "LogisticsScreen.h"
#endif

#include "MechListBox.h"
#include "LogisticsMech.h"
#include "EList.h"
#include "LogisticsMechDisplay.h"

class LogisticsMech;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MechPurchaseScreen:
**************************************************************************************************/
class MechPurchaseScreen: public LogisticsScreen
{
	public:

		static MechPurchaseScreen* instance() { return s_instance; }

		MechPurchaseScreen();
		virtual				~MechPurchaseScreen();

		int					init( FitIniFile& file );

		virtual void		begin();
		virtual void		end();
		virtual void		update();
		virtual void		update1();
		virtual void		update2();
		virtual void		render(int xOffset, int yOffset);

		void				setMech( LogisticsMech* pMech, bool bFromLB = 0 );
		void				beginDrag( LogisticsMech* pMech );
		virtual int			handleMessage( unsigned long, unsigned long );
		void				setSelectedMech( LogisticsMech* pMech) { pSelectedMech = pMech; }//magic 13022011
		void				setActionClick( ) { actionClickFromLB = true; }//magic 13022011
		void				reLoadPurchase(); //magic 19032011

	private:
	
		MechPurchaseScreen( const MechPurchaseScreen& src );
		MechPurchaseScreen& operator=( const MechPurchaseScreen& echPurchaseScreen );

		MechListBox			inventoryListBox;
		MechListBox			variantListBox;
		LogisticsMechDisplay	mechDisplay;

		LogisticsMech*		pDragMech;
		LogisticsMech*		pSelectedMech;
		LogisticsMech*		purMech;//magic 13020211
		LogisticsMech*		selMech;//magic 13022011
		aObject				dragIcon;

		bool				acceptPressed;
	//Magic 72 begin
		bool purFirst;
		bool invFirst;
	//Magic 72 end
		bool actionClickFromLB;//magic 13020211


		static MechPurchaseScreen* s_instance;


		bool				dragStartLeft;
		LogisticsVariant* purchaseVariants[256]; //M11
		int maxPurchaseVariants; //M11

		void removePurVar( LogisticsVariant* pVar ); //M11
		void addPurVar( LogisticsVariant* pVar );//M11
		long getPurVariants( LogisticsVariant* pVar );//M12
		long getInvVariants( LogisticsVariant* pVar );//M12
		void savePurVarChanges(LogisticsVariant** pVar, int& maxCount);//M16

		
		typedef EList<LogisticsMech, LogisticsMech > MECH_LIST;	
		MECH_LIST		prevInventory;

		float				countDownTime;
		float				curCount;
		float				previousAmount;
		float				oldCBillsAmount;

		float				flashTime;


	// HELPER FUNCTIONS

		void addSelectedMech();
		void removeSelectedMech();
		void addMech( LogisticsMech* pMech );
		void removeMech( LogisticsMech* pMech );
		void endDrag();
		bool selectFirstBuyableMech();



};


//*************************************************************************************************
#endif  // end of file ( MechPurchaseScreen.h )
