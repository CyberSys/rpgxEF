/**********************************************************************
	UI_msd.C

	User interface trigger from within game
	**********************************************************************/
#include "ui_local.h"
#include "ui_logger.h"

typedef struct //static
{
	menuframework_s menu;
	menubitmap_s	quitmenu;

	int32_t				maxhull;
	int32_t				currhull;
	int32_t				maxshield;
	int32_t				currshield;
	int32_t				shieldstate;
	int32_t				warpstate;
	int32_t				turbostate;
	int32_t				transstate;
	int32_t				alertstate;
	const char		*model;
} msd_t;

msd_t	s_msd;

//s_msd.maxhull

enum ui_msdIDs {
	ID_QUIT = 10
};

void UI_msdMenu_Cache(void);

/*
=================
M_msd_Event
=================
*/
static void M_msd_Event(void* ptr, int32_t notification)
{
	UI_LogFuncBegin();
	int32_t	id;
	//menubitmap_s	*holdLocation;
	//menubitmap_s	*holdServer;

	id = ((menucommon_s*)ptr)->id;

	/*if ( notification != QM_ACTIVATED )
	{
	return;
	}*/

	if ((id == ID_QUIT) && (notification == QM_ACTIVATED))
		UI_PopMenu();
	UI_LogFuncEnd();
}

/*
=================
msdMenu_Key
=================
*/
sfxHandle_t msdMenu_Key(int32_t key)
{
	return (Menu_DefaultKey(&s_msd.menu, key));
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;
qhandle_t					msd_loading1;
qhandle_t					msd_loading2;
qhandle_t					msd_loading3;
qhandle_t					msd_loading4;
qhandle_t					msd_loading5;
qhandle_t					redalert;
qhandle_t					yellowalert;
qhandle_t					bluealert;
qhandle_t					model;

/*
=================
M_msdMenu_Graphics
=================
*/
static void M_msdMenu_Graphics(void)
{
	UI_LogFuncBegin();
	/*Notes:
	Color-Types in Frame:
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	Blue Alert:
	trap_R_SetColor( colorTable[CT_BLUE] );
	Yellow Alert:
	trap_R_SetColor( colorTable[CT_YELLOW] );
	Red Alert:
	trap_R_SetColor( colorTable[CT_RED] );

	*/
	//menubitmap_s	*holdDeck;
	//int32_t		i;
	int32_t length, xTurboStart, lengthshield, lengthhull;
	//int32_t		numColor, roundColor;

	// Draw the basic screen frame

	// Upper corners
	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(20, 24, 64, 32, corner_ul_24_60);		// Upper corner

	// Lower corners
	//colorTable[CT_VDKPURPLE2]
	trap_R_SetColor(colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic(20, 440, 64, 16, corner_ll_12_60);		// 

	xTurboStart = 604;
	length = UI_ProportionalStringWidth("MASTER SYSTEMS DISPLAY", UI_BIGFONT);
	length += 4;

	// Upper half
	trap_R_SetColor(colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic(79, 24, xTurboStart - (79 + length), PROP_BIG_HEIGHT, uis.whiteShader);	// Top left line
	UI_DrawHandlePic(20, 60, 60, 40, uis.whiteShader);		// 
	trap_R_SetColor(colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic(20, 106, 60, 11, uis.whiteShader);		// 
	if (s_msd.alertstate == 2){
		UI_DrawHandlePic(20, 123, 60, 250, redalert);			//  Alert Fade Red
	}
	else if (s_msd.alertstate == 1){
		UI_DrawHandlePic(20, 123, 60, 250, yellowalert);		//  Alert Fade Yellow
	}
	else if (s_msd.alertstate == 3){
		UI_DrawHandlePic(20, 123, 60, 250, bluealert);		//  Alert Fade Blue
	}
	else{
		trap_R_SetColor(colorTable[CT_DKPURPLE1]);  //DKGOLD1
		UI_DrawHandlePic(20, 123, 60, 250, uis.whiteShader);		//  Left hand column
	}

	// Lower half
	trap_R_SetColor(colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(20, 380, 60, 70, uis.whiteShader);		// Left Column

	// Bottom line
	trap_R_SetColor(colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(69, 443, 287, 12, uis.whiteShader);		// 
	trap_R_SetColor(colorTable[CT_DKPURPLE1]); //colorTable[CT_DKGOLD1]
	UI_DrawHandlePic(364, 443, 260, 12, uis.whiteShader);		// Bottom line

	if (s_msd.alertstate == 2)
		UI_DrawProportionalString(xTurboStart, 24,
		                          "MASTER SYSTEMS DISPLAY", UI_BIGFONT | UI_RIGHT, colorTable[CT_RED]);
	else if (s_msd.alertstate == 1)
		UI_DrawProportionalString(xTurboStart, 24,
		                          "MASTER SYSTEMS DISPLAY", UI_BIGFONT | UI_RIGHT, colorTable[CT_YELLOW]);
	else if (s_msd.alertstate == 3)
		UI_DrawProportionalString(xTurboStart, 24,
		                          "MASTER SYSTEMS DISPLAY", UI_BIGFONT | UI_RIGHT, colorTable[CT_BLUE]);
	else
		UI_DrawProportionalString(xTurboStart, 24,
		                          "MASTER SYSTEMS DISPLAY", UI_BIGFONT | UI_RIGHT, colorTable[CT_WHITE]);

	trap_R_SetColor(colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic(607, 24, -16, 32, leftRound);

	// MSD Model
	UI_DrawHandlePic(100, 60, 500, 250, model);

	// health Bars
	lengthshield = (int32_t)(498 * s_msd.currshield / s_msd.maxshield);
	lengthhull = (int32_t)(498 * s_msd.currhull / s_msd.maxhull);

	trap_R_SetColor(colorTable[CT_BLACK]);
	UI_DrawHandlePic(100, 320, 500, 49, uis.whiteShader);		//background
	trap_R_SetColor(colorTable[CT_VDKBLUE1]);
	//trap_R_SetColor( colorTable[CT_VDKPURPLE2] ); 
	UI_DrawHandlePic(101, 321, 498, 23, uis.whiteShader);		//Shield Bar BG
	trap_R_SetColor(colorTable[CT_VDKRED1]);
	//trap_R_SetColor( colorTable[CT_VDKORANGE] ); 
	UI_DrawHandlePic(101, 345, 498, 23, uis.whiteShader);		//Health Bar BG

	//Shield Bar
	if (s_msd.shieldstate == 1){
		trap_R_SetColor(colorTable[CT_DKBLUE1]);
		UI_DrawHandlePic(101, 321, lengthshield, 23, uis.whiteShader);
		UI_DrawProportionalString(103, 323, va("Shields at %.1f Percent", (lengthshield / 4.98)), UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == 0){
		UI_DrawProportionalString(103, 323, "Shields are on standby", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == -2){
		UI_DrawProportionalString(103, 323, "Shields are offline", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == -1){
		UI_DrawProportionalString(103, 323, "Shields are inoperable", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	trap_R_SetColor(colorTable[CT_DKRED1]);
	UI_DrawHandlePic(101, 345, lengthhull, 23, uis.whiteShader);		//Health Bar
	UI_DrawProportionalString(103, 347, va("Structural Integity at %.1f Percent", (lengthhull / 4.98)), UI_TINYFONT, colorTable[CT_MDGREY]);

	//Sub System indicators
	//Shields
	trap_R_SetColor(colorTable[CT_WHITE]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(100, 380, 90, 45, uis.whiteShader);
	if (s_msd.shieldstate == 1){
		trap_R_SetColor(colorTable[CT_GREEN]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(101, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(101, 382, "Shields are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(101, 397, "online", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == 0){
		trap_R_SetColor(colorTable[CT_YELLOW]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(101, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(101, 382, "Shields are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(101, 397, "on standby", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == -2){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(101, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(101, 382, "Shields are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(101, 397, "offline", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.shieldstate == -1){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(101, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(101, 382, "Shields are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(101, 397, "inoperable", UI_TINYFONT, colorTable[CT_MDGREY]);
	}

	//Warp Drive
	trap_R_SetColor(colorTable[CT_WHITE]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(195, 380, 90, 45, uis.whiteShader);
	if (s_msd.warpstate == 1){
		trap_R_SetColor(colorTable[CT_GREEN]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(196, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(197, 382, "Warp Drive is", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(197, 397, "online", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.warpstate == 0){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(196, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(197, 382, "Warp Drive is", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(197, 397, "offline", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.warpstate == 2){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(196, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(197, 382, "Warp Core is", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(197, 397, "ejected", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.warpstate == -2){//-2, no info available
		trap_R_SetColor(colorTable[CT_YELLOW]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(196, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(197, 382, "Warp-Core-ODN-", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(197, 397, "Update required", UI_TINYFONT, colorTable[CT_MDGREY]);
	}

	//Turbolift
	trap_R_SetColor(colorTable[CT_WHITE]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(290, 380, 90, 45, uis.whiteShader);
	if (s_msd.turbostate == 1){
		trap_R_SetColor(colorTable[CT_GREEN]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(291, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(292, 382, "Turbolifts are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(292, 397, "online", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.turbostate == 0){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(291, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(292, 382, "Turbolifts are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(292, 397, "offline", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.turbostate == -2){//-2, no info available
		trap_R_SetColor(colorTable[CT_YELLOW]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(291, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(292, 382, "Turbolift-ODN-", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(292, 397, "Update required", UI_TINYFONT, colorTable[CT_MDGREY]);
	}

	//Transporter
	trap_R_SetColor(colorTable[CT_WHITE]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic(385, 380, 90, 45, uis.whiteShader);
	if (s_msd.transstate == 1){
		trap_R_SetColor(colorTable[CT_GREEN]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(386, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(387, 382, "Transporters are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(387, 397, "online", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.transstate == 0){
		trap_R_SetColor(colorTable[CT_RED]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(386, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(387, 382, "Transporters are", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(387, 397, "offline", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	else if (s_msd.transstate == -2){//-2, no info available
		trap_R_SetColor(colorTable[CT_YELLOW]); //colorTable[CT_VDKPURPLE2]
		UI_DrawHandlePic(386, 381, 88, 43, uis.whiteShader);
		UI_DrawProportionalString(386, 382, "Transporter-ODN-", UI_TINYFONT, colorTable[CT_MDGREY]);
		UI_DrawProportionalString(387, 397, "Update required", UI_TINYFONT, colorTable[CT_MDGREY]);
	}
	UI_LogFuncEnd();

}

/*
===============
msdMenu_Draw
===============
*/
static void msdMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	UI_LogFuncBegin();
	M_msdMenu_Graphics();

	Menu_Draw(&s_msd.menu);
	UI_LogFuncEnd();
}

/*
===============
UI_msdMenu_Cache
===============
*/
void UI_msdMenu_Cache(void)
{
	UI_LogFuncBegin();
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
	msd_loading1 = trap_R_RegisterShaderNoMip("menu/new/nav_y.tga");
	msd_loading2 = trap_R_RegisterShaderNoMip("menu/new/nav_mb.tga");
	msd_loading3 = trap_R_RegisterShaderNoMip("menu/new/nav_lb.tga");
	msd_loading4 = trap_R_RegisterShaderNoMip("menu/new/nav_db.tga");
	msd_loading5 = trap_R_RegisterShaderNoMip("menu/new/nab_o.tga");
	redalert = trap_R_RegisterShaderNoMip("menu/alert/red");
	yellowalert = trap_R_RegisterShaderNoMip("menu/alert/yellow");
	bluealert = trap_R_RegisterShaderNoMip("menu/alert/blue");
	model = trap_R_RegisterShaderNoMip(s_msd.model);
	UI_LogFuncEnd();
}

/*
===============
msdMenu_Init
===============
*/
void msdMenu_Init(void)
{
	UI_LogFuncBegin();
	s_msd.menu.m_ItemCount = 0;
	s_msd.menu.OnDraw = msdMenu_Draw;
	s_msd.menu.OnKey = msdMenu_Key;
	s_msd.menu.m_WrapAround = true;
	s_msd.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_msd.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y } };

	s_msd.quitmenu.generic.m_Type = EMenuItemType::Bitmap;
	s_msd.quitmenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_msd.quitmenu.generic.x = 490;
	s_msd.quitmenu.generic.y = 380;
	s_msd.quitmenu.generic.name = GRAPHIC_BUTTONLEFT;
	s_msd.quitmenu.generic.id = ID_QUIT;
	s_msd.quitmenu.generic.callback = M_msd_Event;
	s_msd.quitmenu.width = MENU_BUTTON_MED_WIDTH - 20;
	s_msd.quitmenu.height = PROP_BIG_HEIGHT;
	s_msd.quitmenu.color = CT_DKPURPLE1;
	s_msd.quitmenu.color2 = CT_LTPURPLE1;
	s_msd.quitmenu.textX = MENU_BUTTON_TEXT_X;
	s_msd.quitmenu.textY = 12;
	s_msd.quitmenu.textEnum = MBT_RETURNMENU;
	s_msd.quitmenu.textcolor = CT_BLACK;
	s_msd.quitmenu.textcolor2 = CT_WHITE;
	s_msd.quitmenu.textStyle = UI_TINYFONT;

	Menu_AddItem(&s_msd.menu, &s_msd.quitmenu);
	UI_LogFuncEnd();
}

/*
===============
UI_msdMenu
===============
*/
void UI_msdMenu(int32_t maxhull, int32_t currhull, int32_t maxshield, int32_t currshield, int32_t shieldstate, int32_t warpstate, int32_t turbostate, int32_t transstate, int32_t alertstate, char *model)
{
	UI_LogFuncBegin();
	memset(&s_msd, 0, sizeof(s_msd));

	s_msd.maxhull = maxhull;
	s_msd.currhull = currhull;
	s_msd.maxshield = maxshield;
	s_msd.currshield = currshield;
	s_msd.shieldstate = shieldstate;
	s_msd.warpstate = warpstate;
	s_msd.turbostate = turbostate;
	s_msd.transstate = transstate;
	s_msd.alertstate = alertstate;
	s_msd.model = model;

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_msdMenu_Cache();

	msdMenu_Init();

	UI_PushMenu(&s_msd.menu);

	Menu_AdjustCursor(&s_msd.menu, 1);
	UI_LogFuncEnd();
}
