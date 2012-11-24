#ifndef COMPONENTPURCHASESCREEN_H
#define COMPONENTPURCHASESCREEN_H
/*************************************************************************************************\
ComponentPurchase.h			: Interface for the ComponentPurchase component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "LogisticsScreen.h"
#endif

#ifndef ATTRIBUTEMETER_H
#include "AttributeMeter.h"
#endif

#ifndef COMPONENTPURBOX_H
#include "ComponentPurBox.h"
#endif

#ifndef SIMPLECAMERA_H
#include "SimpleCamera.h"
#endif

#define COMPUR_ATTRIBUTE_METER_COUNT	9
class FitIniFile;
//class LogisticsVariant;
class LogisticsComponent;
//class LogisticsVariantDialog;
//class LogisticsAcceptVariantDialog;
//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
ComponentPurchase:
**************************************************************************************************/
class ComponentPurchaseScreen: public LogisticsScreen
{
	public:

		ComponentPurchaseScreen();
		virtual ~ComponentPurchaseScreen();
		int init( FitIniFile& );
		virtual void begin();
		virtual void end();
		virtual void update();
		virtual void render(int xOffset, int yOffset);
		virtual int	handleMessage( unsigned long, unsigned long );
		void	setComponent( LogisticsComponent* pComponent, bool bMessageFromLB = 0 );
		static ComponentPurchaseScreen* instance() { return s_instance;}

		void	removeComponentListToDrag(LogisticsComponent* pComponent); //Magic
		int		addComponentListFromDrag( LogisticsComponent* pComponent ); //Magic
		void	buttonAddPressed( bool p1 );//Magic
		int		selectFirstINComponent();//Magic

	private:

		ComponentPurchaseScreen( const ComponentPurchaseScreen& src );
		ComponentPurchaseScreen& operator=( const ComponentPurchaseScreen& cPurScreen );
		static ComponentPurchaseScreen*	s_instance;
		
		LogisticsComponent*	pSelectedComponent;
		//LogisticsComponent*	pPurSelectedComponent;//Magic
		//LogisticsComponent*	pInvSelectedComponent;//Magic
		bool purFirst;
		bool invFirst;
		long				selI;
		long				selJ;
		long				dragX; //M
		long				dragY; //M

		ComponentIconPurBox	purcomponentListBox;
		ComponentIconPurBox	invcomponentListBox;

		AttributeMeter attributeMeters[COMPUR_ATTRIBUTE_METER_COUNT];

		aObject				purcomponentIcons[128];
		aObject				invcomponentIcons[128];//M19
		long				purcomponentCount;
		long				invcomponentCount;

		aObject				payloadIcon;

		aObject				dragIcon;

		//LogisticsComponent*	pDragComponent;
		LogisticsComponent*	pCurComponent;

		void showJumpJetItems( bool bShow );
		
		float originalCost;

		aObject				selPRects[2][5];
		aObject				selPJumpJetRect;
		aObject*			selPRect;
		
		bool				bSaveDlg;
		bool				bDragLeft ;

		bool				inPurchaseList; //Magic

		SimpleCamera		camera;

		EString				varName;

		static				GUI_RECT	sensorRects[4];
		static				long		sensorHelpIDs[4];

		float				countDownTime;
		float				curCount;
		float				previousAmount;
		float				oldCBillsAmount;

		float				oldHeat;
		float				heatTime;
		bool				flashHeatRange;

		float				oldArmor;
		float				newArmor;
		float				armorTime;

		bool				bErrorDlg;


		// HELPER FUNCTIONS
//		void getMouseDiagramCoords( long& x, long& y );


		int		selectFirstLBComponent(); 


};


//*************************************************************************************************
#endif  // end of file ( ComponentPurchaseScreen.h )
