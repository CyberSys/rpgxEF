/*
======================================================

RPG-X Admin
By TiM
28-6-2006

This menu is designed to allow admins to choose commands
ingame.

======================================================
*/

#include "ui_local.h"
#include "ui_logger.h"
// Defines
//general
static const char PIC_ARROW_UP[] = "menu/common/arrow_up_16.tga";
static const char PIC_ARROW_DOWN[] = "menu/common/arrow_dn_16.tga";

// Head Menu (also includes pro forma defines)
enum ui_adminIDs_e {
	ID_MAINMENU = 10,
	ID_ADMIN_MAIN,
	ID_ADMIN_CLIENTS,
	ID_ADMIN_AUDIO,
	ID_LOGIN_MENU,

	//admin Main
	ID_GOD = 1111,
	ID_NOCLIP,
	ID_CLOAK,
	ID_FLIGHT,

	ID_GIVE = 1121,
	ID_REVIVE,
	ID_MESSAGE,
	ID_ENTITY,

	ID_BEAM_LOC = 1131,
	ID_BEAM_PLAYER,

	ID_FXGUNLIST = 1141,
	ID_FXGUN,

	ID_SPAWNCHAR = 1151,
	ID_REMOVECHAR,

	ID_BIND_SPAWN = 1161,
	ID_BIND_USEENT,
	ID_BIND_KICK,

	// admin client
	ID_CLIENT_SELECT = 1211,
	ID_CLIENT_KICK,
	ID_CLIENT_FORCE,

	/* menu choices defines */
	ID_SONGBUTTON0 = 13100,
	ID_SONGBUTTON1,
	ID_SONGBUTTON2,
	ID_SONGBUTTON3,
	ID_SONGBUTTON4,
	ID_SONGBUTTON5,
	ID_SONGBUTTON6,

	ID_PLAYSONG = 1311,
	ID_STOPSONG,
	ID_SONGUP,
	ID_SONGDN,

	ID_SNDBUTTON0 = 13200,
	ID_SNDBUTTON1,
	ID_SNDBUTTON2,
	ID_SNDBUTTON3,
	ID_SNDBUTTON4,
	ID_SNDBUTTON5,
	ID_SNDBUTTON6,

	ID_PLAYSND = 1321,
	ID_SNDUP,
	ID_SNDDN,

	//Login Menu
	ID_ADMIN = 1411,
	ID_SQL
};

// admin audio
enum ui_adminLimits_e {
	MAX_BUTTONS = 7,
	MAX_AUDIO_LOCATIONS = 32,
	MAX_SOUNDS_ = 1024,
	MAX_SONGS = 2048
};

static void AdminMenu_Event(void* ptr, int32_t event);
void UI_ClientAdminMenu(void);
void UI_AdminAudioMenu(void);

typedef enum {
	CHOICE_MYSELF,
	CHOICE_EVERYONE
} adminChoics_n;

/*typedef enum {
	CHOICE_MINE,
	CHOICE_ALL
	};*/

static int32_t s_admin_reviveList[] =
{
	MNT_MYSELF,
	MNT_EVERYONE,
	0
};

static int32_t s_admin_spawnList[] =
{
	MNT_MINE,
	MNT_ALL,
	0
};

typedef struct {
	char		*name;
	int32_t			arg1Flags;
	int32_t			arg2Flags;
	int32_t			arg3Flags;
	int32_t			arg1Desc;
	int32_t			arg2Desc;
	int32_t			arg3Desc;
} fxData_t;

const fxData_t s_admin_fxData[] = {
	{ "default", (QMF_INACTIVE | QMF_GRAYED), (QMF_INACTIVE | QMF_GRAYED), (QMF_INACTIVE | QMF_GRAYED), -1, -1, -1 },
	{ "detpack", (QMF_INACTIVE | QMF_GRAYED), (QMF_INACTIVE | QMF_GRAYED), (QMF_INACTIVE | QMF_GRAYED), -1, -1, -1 },
	{ "chunks", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, MNT_CHUNKTYPE, -1 },
	{ "sparks", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_SPARKINTERVAL, MNT_TIMELENGTH, -1 },
	{ "steam", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_TIMELENGTH, -1, -1 },
	{ "drips", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, MNT_DRIPTYPE, MNT_DRIPINTENSITY, MNT_TIMELENGTH },
	{ "smoke", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, MNT_TIMELENGTH, -1 },
	{ "surf_explosion", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, MNT_SHAKEINTENSITY, -1 },
	{ "elec_explosion", QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, -1, -1 },
	{ "fire", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, MNT_TIMELENGTH, -1 },
	{ "shake", QMF_HIGHLIGHT_IF_FOCUS, QMF_HIGHLIGHT_IF_FOCUS, (QMF_INACTIVE | QMF_GRAYED), MNT_RADIUS, MNT_SHAKEINTENSITY, -1 },
	{ NULL, 0, 0, 0, 0, 0, 0 }
};

const char* s_admin_forceParams[] = {
	"name",
	"model",
	"class",
	"rank",
	"height",
	"weight",
	"age",
	"race",
	0
};

const char* s_admin_bindCmds[] = {
	"spawnChar",
	"useEnt",
	"kickTarget",
	0
};

typedef struct {
	menuframework_s		menu;

	menubitmap_s		mainMenu;		/* Return to game ( or main menu ) */
	menubitmap_s		adminMain;		/* Self Admin Commands */
	menubitmap_s		adminClients;		/* Others Admin Commands */
	menubitmap_s		adminAudio;		/* Audio related commands (coz they need teh space) */

	/* admin main */
	menuaction_s		god;
	menuaction_s		noclip;
	menuaction_s		cloak;
	menuaction_s		flight;

	menuaction_s		give;
	menulist_s		giveList;

	menuaction_s		msg;
	menufield_s		msgArg;

	menubitmap_s		flushTripmines;
	menulist_s		tripminesList;

	menuaction_s		revive;
	menulist_s		reviveList;

	menuaction_s		useEnt;
	menufield_s		useEntField;
	menuaction_s		useEntKey;

	menuaction_s		beamToLoc;
	menulist_s		beamLocList;

	menuaction_s		beamToPlayerExecute;
	menulist_s		beamPlayerList;

	menuaction_s		fxGun;
	menulist_s		fxGunList;
	menufield_s		fxArg1;
	menufield_s		fxArg2;
	menufield_s		fxArg3;

	menuaction_s		spawnChar;

	menulist_s		flushList;
	menuaction_s		flushChars;

	menuaction_s		spawnCharBind;
	menuaction_s		useEntBind;
	menuaction_s		kickTargetBind;

	int32_t			keyBindActive;	/* index of whichever bind is active */
	int32_t			binds[3];	/* stores the indices of each bind button */

	/* menu arrows */
	menubitmap_s		upArrow;
	menubitmap_s		dnArrow;

	qhandle_t		corner_ll_4_18;
	qhandle_t		corner_ll_4_4;

	/* ie this menu was called via the console ( ie a key bind instead of from the main menu) */
	qboolean		fromConsole;
} admin_t;

admin_t s_admin;

adminGeneric_t	s_adminGeneric;

static char				*fxList[10];

/*
===============
AdminGeneric_InitLists

TiM: Init global arrays filling
them in with generic info
we'll need for different menus
===============
*/
void AdminGeneric_InitLists(void) {
	UI_LogFuncBegin();
	int32_t		i;
	char	info[MAX_TOKEN_CHARS];

	memset(&s_adminGeneric, 0, sizeof(s_adminGeneric));

	/* point the items array to our main item list */
	for (i = 0; i < bg_numGiveItems; i++) {
		s_adminGeneric.giveItems[i] = bg_giveItem[i].consoleName;
	}
	s_adminGeneric.giveItems[i] = 0;

	/* build location list */
	for (i = 1; i < MAX_LOCATIONS; i++) {
		trap_GetConfigString(CS_LOCATIONS + i, info, MAX_INFO_STRING);

		if (!info[0])
			break;

		Q_strncpyz(s_adminGeneric.locList[s_adminGeneric.numLocs], info, sizeof(s_adminGeneric.locList[s_adminGeneric.numLocs]));
		/* omfg suxx */
		s_adminGeneric.locListPtr[s_adminGeneric.numLocs] = s_adminGeneric.locList[s_adminGeneric.numLocs];
		s_adminGeneric.numLocs++;
	}
	s_adminGeneric.locListPtr[s_adminGeneric.numLocs] = 0; /* Null terminate */

	/* build player list */
	for (i = 0; i < MAX_CLIENTS; i++) {
		trap_GetConfigString(CS_PLAYERS + i, info, MAX_INFO_STRING);

		if (!info[0] || !strlen(Info_ValueForKey(info, "n")))
			continue;

		Q_strncpyz(s_adminGeneric.charList[s_adminGeneric.numChars].name, Info_ValueForKey(info, "n"), sizeof(s_adminGeneric.charList[s_adminGeneric.numChars].name));
		s_adminGeneric.charList[s_adminGeneric.numChars].id = i;

		/* omfg ghey */
		s_adminGeneric.charListPtr[s_adminGeneric.numChars] = s_adminGeneric.charList[s_adminGeneric.numChars].name; //va( "%s", /*s_adminGeneric.charList[s_adminGeneric.numChars].id,*/ s_adminGeneric.charList[s_adminGeneric.numChars].name);
		s_adminGeneric.numChars++;
	}
	s_adminGeneric.charListPtr[s_adminGeneric.numChars] = 0;
	UI_LogFuncEnd();

}

/*
===============
AdminMenu_DrawLCARSBox

TiM: Since all the elements are
drawn in separate LCARS boxes of dif
dimensions, but ultimately the same
design, we can make one function to draw
all of them. :)
===============
*/

static void AdminMenu_DrawLCARSBox(int32_t x, int32_t y, int32_t w, int32_t h, vec4_t color, int32_t title) {
	UI_LogFuncBegin();

	trap_R_SetColor(color);

	/* draw upper box end curve */
	UI_DrawHandlePic(x + w - 18, y, -18, 18, uis.graphicButtonLeftEnd);

	/* draw upper main box */
	UI_DrawHandlePic(x + 10, y, w - 20, 18, uis.whiteShader);

	/* draw left brace */
	UI_DrawHandlePic(x, y + 20, 4, h - 26, uis.whiteShader);

	/* draw top left corner GFX */
	UI_DrawHandlePic(x, y - 12, 8, -32, s_admin.corner_ll_4_18);

	/* lower left end curve */
	UI_DrawHandlePic(x, y + h - 6, 8, 8, s_admin.corner_ll_4_4);

	/* teeny weeny end piece for lower bar */
	UI_DrawHandlePic(x + w - 4, y + h - 4, -4, 4, uis.graphicButtonLeftEnd);

	/* lower bar */
	UI_DrawHandlePic(x + 9, y + h - 4, w - 11, 4, uis.whiteShader);

	/* if possible, add a title */
	if (menu_normal_text[title]) {
		UI_DrawProportionalString(x + 15, y + 1, menu_normal_text[title], UI_LEFT | UI_SMALLFONT, colorTable[CT_BLACK]);
	}
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_DrawCommon
===============
*/

static void AdminMenu_DrawCommon(void) {
	UI_LogFuncBegin();

	/* Left side LCARS bars */
	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 70, uis.whiteShader);

	trap_R_SetColor(colorTable[CT_LTGOLD1]);
	UI_DrawHandlePic(30, 276, 47, 78, uis.whiteShader);

	trap_R_SetColor(colorTable[CT_DKRED1]);
	UI_DrawHandlePic(30, 357, 47, 32, uis.whiteShader);

	/* LCARS Numbers */
	/*UI_DrawProportionalString(  74,  66, "51876",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	  UI_DrawProportionalString(  74,  84, "4912",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);*/
	UI_DrawProportionalString(74, 188, "2134", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(74, 206, "3754", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 279, "7535", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 360, "12343", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(74, 395, "5356", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	/*UI_DrawProportionalString( 584, 142, "43245", UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);*/

	/* Admin Buttons line */
	trap_R_SetColor(colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(81, 25, 4, 74, uis.whiteShader);
	UI_DrawHandlePic(88, 100, 94, 18, uis.whiteShader);

	if (s_admin.corner_ll_4_18)
		UI_DrawHandlePic(81, 98, 8, 32, s_admin.corner_ll_4_18);

	/* trap_R_SetColor( colorTable[CT_LTGOLD1]); */
	UI_DrawHandlePic(185, 100, 179, 18, uis.whiteShader);

	trap_R_SetColor(colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(367, 100, 94, 18, uis.whiteShader);

	UI_DrawHandlePic(592, 100, -18, 18, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(s_admin.adminMain.generic.x - 14, s_admin.adminMain.generic.y, 18, 18, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_admin.adminClients.generic.x - 14, s_admin.adminClients.generic.y, 18, 18, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_admin.adminAudio.generic.x - 14, s_admin.adminAudio.generic.y, 18, 18, uis.graphicButtonLeftEnd);
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_InitButtons
===============
*/

static void AdminMenu_InitButtons(menuframework_s *menu) {
	UI_LogFuncBegin();

	s_admin.mainMenu.generic.m_Type = EMenuItemType::Bitmap;
	s_admin.mainMenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.mainMenu.generic.x = 464;
	s_admin.mainMenu.generic.y = 100;
	s_admin.mainMenu.generic.name = GRAPHIC_SQUARE;
	s_admin.mainMenu.generic.id = ID_MAINMENU;
	s_admin.mainMenu.generic.callback = AdminMenu_Event;
	s_admin.mainMenu.width = MENU_BUTTON_MED_WIDTH + 2;
	s_admin.mainMenu.height = MENU_BUTTON_MED_HEIGHT;
	s_admin.mainMenu.color = CT_DKPURPLE1;
	s_admin.mainMenu.color2 = CT_LTPURPLE1;
	s_admin.mainMenu.textX = MENU_BUTTON_TEXT_X;
	s_admin.mainMenu.textY = MENU_BUTTON_TEXT_Y;
	if (!ingameFlag || !s_admin.fromConsole)
		s_admin.mainMenu.textEnum = MBT_MAINMENU;
	else
		s_admin.mainMenu.textEnum = MBT_INGAMERESUME;
	s_admin.mainMenu.textcolor = CT_BLACK;
	s_admin.mainMenu.textcolor2 = CT_WHITE;

	s_admin.adminMain.generic.m_Type = EMenuItemType::Bitmap;
	s_admin.adminMain.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.adminMain.generic.x = 125;
	s_admin.adminMain.generic.y = 66;
	s_admin.adminMain.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_admin.adminMain.generic.callback = AdminMenu_Event;
	s_admin.adminMain.generic.id = ID_ADMIN_MAIN;
	s_admin.adminMain.width = MENU_BUTTON_MED_WIDTH;
	s_admin.adminMain.height = MENU_BUTTON_MED_HEIGHT;
	s_admin.adminMain.color = CT_DKPURPLE1;
	s_admin.adminMain.color2 = CT_LTPURPLE1;
	s_admin.adminMain.textX = MENU_BUTTON_TEXT_X;
	s_admin.adminMain.textY = MENU_BUTTON_TEXT_Y;
	s_admin.adminMain.textEnum = MBT_ADMIN_MAIN;
	s_admin.adminMain.textcolor = CT_BLACK;
	s_admin.adminMain.textcolor2 = CT_WHITE;

	s_admin.adminClients.generic.m_Type = EMenuItemType::Bitmap;
	s_admin.adminClients.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.adminClients.generic.x = 298;
	s_admin.adminClients.generic.y = 66;
	s_admin.adminClients.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_admin.adminClients.generic.callback = AdminMenu_Event;
	s_admin.adminClients.generic.id = ID_ADMIN_CLIENTS;
	s_admin.adminClients.width = MENU_BUTTON_MED_WIDTH;
	s_admin.adminClients.height = MENU_BUTTON_MED_HEIGHT;
	s_admin.adminClients.color = CT_DKPURPLE1;
	s_admin.adminClients.color2 = CT_LTPURPLE1;
	s_admin.adminClients.textX = MENU_BUTTON_TEXT_X;
	s_admin.adminClients.textY = MENU_BUTTON_TEXT_Y;
	s_admin.adminClients.textEnum = MBT_ADMIN_CLIENTS;
	s_admin.adminClients.textcolor = CT_BLACK;
	s_admin.adminClients.textcolor2 = CT_WHITE;

	s_admin.adminAudio.generic.m_Type = EMenuItemType::Bitmap;
	s_admin.adminAudio.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.adminAudio.generic.x = 470;
	s_admin.adminAudio.generic.y = 66;
	s_admin.adminAudio.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_admin.adminAudio.generic.callback = AdminMenu_Event;
	s_admin.adminAudio.generic.id = ID_ADMIN_AUDIO;
	s_admin.adminAudio.width = MENU_BUTTON_MED_WIDTH;
	s_admin.adminAudio.height = MENU_BUTTON_MED_HEIGHT;
	s_admin.adminAudio.color = CT_DKPURPLE1;
	s_admin.adminAudio.color2 = CT_LTPURPLE1;
	s_admin.adminAudio.textX = MENU_BUTTON_TEXT_X;
	s_admin.adminAudio.textY = MENU_BUTTON_TEXT_Y;
	s_admin.adminAudio.textEnum = MBT_ADMIN_AUDIO;
	s_admin.adminAudio.textcolor = CT_BLACK;
	s_admin.adminAudio.textcolor2 = CT_WHITE;

	Menu_AddItem(menu, &s_admin.mainMenu);

	Menu_AddItem(menu, &s_admin.adminMain);
	Menu_AddItem(menu, &s_admin.adminClients);
	Menu_AddItem(menu, &s_admin.adminAudio);
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_Event
===============
*/
static void AdminMenu_Event(void* ptr, int32_t event) {
	UI_LogFuncBegin();

	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s *)ptr)->id) {
	case ID_ADMIN_MAIN:
		UI_PopMenu();
		UI_AdminMenu(s_admin.fromConsole);
		break;
	case ID_ADMIN_CLIENTS:
		UI_PopMenu();
		UI_ClientAdminMenu();
		break;
	case ID_ADMIN_AUDIO:
		UI_PopMenu();
		UI_AdminAudioMenu();
		break;
	case ID_GOD:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, "god\n");
		break;
	case ID_NOCLIP:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, "noclip\n");
		break;
	case ID_CLOAK:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, "cloak\n");
		break;
	case ID_FLIGHT:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, "flight\n");
		break;

	case ID_GIVE:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("give %s", s_adminGeneric.giveItems[s_admin.giveList.curvalue]));
		break;

	case ID_REVIVE:

		switch (s_admin.reviveList.curvalue) {
		case CHOICE_MYSELF:
			trap_Cmd_ExecuteText(EXEC_APPEND, "revive\n");
			break;
		case CHOICE_EVERYONE:
			trap_Cmd_ExecuteText(EXEC_APPEND, "revive all\n");
			break;
		}
		/*UI_ForceMenuOff();*/
		break;

	case ID_MESSAGE:
		/*UI_ForceMenuOff();*/

		if (strlen(s_admin.msgArg.field.buffer)) {
			trap_Cmd_ExecuteText(EXEC_APPEND, va("msg \"%s\" ", s_admin.msgArg.field.buffer));
		}
		break;

	case ID_ENTITY:
		/*UI_ForceMenuOff();*/

		if (strlen(s_admin.useEntField.field.buffer)) {
			trap_Cmd_ExecuteText(EXEC_APPEND, va("useEnt \"%s\" ", s_admin.useEntField.field.buffer));
		}
		break;

	case ID_BEAM_LOC:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("beamToLocation %i", s_admin.beamLocList.curvalue + 1));
		break;

	case ID_BEAM_PLAYER:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("beamToPlayer %i", s_adminGeneric.charList[s_admin.beamPlayerList.curvalue].id));
		break;

	case ID_FXGUNLIST:
		/* reload the fields to set up input for this effect */
		s_admin.fxArg1.generic.flags = s_admin_fxData[s_admin.fxGunList.curvalue].arg1Flags;
		s_admin.fxArg2.generic.flags = s_admin_fxData[s_admin.fxGunList.curvalue].arg2Flags;
		s_admin.fxArg3.generic.flags = s_admin_fxData[s_admin.fxGunList.curvalue].arg3Flags;
		break;

	case ID_FXGUN:
	{
					 char cmd[64];
					 /*UI_ForceMenuOff();*/

					 Com_sprintf(cmd, 64, "fxGun %s ", s_admin_fxData[s_admin.fxGunList.curvalue].name);

					 /* compile arguments */
					 if (!(s_admin.fxArg1.generic.flags & QMF_INACTIVE)) {
						 if (s_admin.fxArg1.field.buffer[0]) {
							 strcat(cmd, va("%s", s_admin.fxArg1.field.buffer));
						 }
						 else {
							 strcat(cmd, " 0");
						 }
					 }

					 if (!(s_admin.fxArg2.generic.flags & QMF_INACTIVE)) {
						 if (s_admin.fxArg2.field.buffer[0]) {
							 strcat(cmd, va(" %s", s_admin.fxArg2.field.buffer));
						 }
						 else {
							 strcat(cmd, " 0");
						 }
					 }

					 if (!(s_admin.fxArg3.generic.flags & QMF_INACTIVE)) {
						 if (s_admin.fxArg3.field.buffer[0]) {
							 strcat(cmd, va(" %s", s_admin.fxArg3.field.buffer));
						 }
						 else {
							 strcat(cmd, " 0");
						 }
					 }

					 /* UI_Logger( LL_DEBUG, "Cmd: %s\n", cmd ); */

					 trap_Cmd_ExecuteText(EXEC_APPEND, cmd);
	}
		break;

	case ID_SPAWNCHAR:
		/*UI_ForceMenuOff();*/

		trap_Cmd_ExecuteText(EXEC_APPEND, "spawnChar");
		break;

	case ID_REMOVECHAR:
		/*UI_ForceMenuOff();*/

		trap_Cmd_ExecuteText(EXEC_APPEND, va("flushChars %i", s_admin.flushList.curvalue));
		break;

	case ID_BIND_SPAWN:
	case ID_BIND_USEENT:
	case ID_BIND_KICK:
		s_admin.keyBindActive = ((menucommon_s *)ptr)->id - ID_BIND_SPAWN;
		s_admin.menu.m_NoNewSelecting = qtrue;
		break;

	case ID_MAINMENU:
		UI_PopMenu();
		break;
	}
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_Draw
===============
*/
static void AdminMenu_Draw(void) {
	UI_LogFuncBegin();
	vec_t*	color = colorTable[CT_LTORANGE];
	char	msg1[36];
	char	msg2[36];
	char	msg3[36];

	UI_MenuFrame2(&s_admin.menu);

	AdminMenu_DrawCommon();

	/* Draw Boxes for all of the elements */
	/* First Row */
	AdminMenu_DrawLCARSBox(81, 121, 201, 70, color, MNT_MAIN_CMDS);    /* Cheat Buttons */
	AdminMenu_DrawLCARSBox(287, 121, 173, 70, color, MNT_GIVE_CMDS);   /* Give Buttons  */
	AdminMenu_DrawLCARSBox(465, 121, 142, 70, color, MNT_REVIVE_CMDS); /* Revive Button */

	/* Second Row */
	AdminMenu_DrawLCARSBox(81, 194, 339, 70, color, MNT_BROADCAST_CMDS); /* Admin Message */
	AdminMenu_DrawLCARSBox(425, 194, 183, 70, color, MNT_USEENT_CMDS);   /* UseEnt        */

	/* Third Row */
	AdminMenu_DrawLCARSBox(81, 268, 360, 70, color, MNT_BEAM_CMDS);   /* Beaming    */
	AdminMenu_DrawLCARSBox(447, 268, 160, 88, color, MNT_FXGUN_CMDS); /* FX Gun-zor */

	/* Fourth Row */
	AdminMenu_DrawLCARSBox(81, 342, 361, 49, color, MNT_SPAWN_CMDS); /* Char Spawn */

	/* argument descriptions for the FXGun */
	if (s_admin_fxData[s_admin.fxGunList.curvalue].arg1Desc >= 0) {
		Com_sprintf(msg1, sizeof(msg1), "1: %s", menu_normal_text[s_admin_fxData[s_admin.fxGunList.curvalue].arg1Desc]);
		UI_DrawProportionalString(524, 358, msg1, UI_TINYFONT | UI_CENTER, colorTable[CT_WHITE]);
	}

	if (s_admin_fxData[s_admin.fxGunList.curvalue].arg2Desc >= 0) {
		Com_sprintf(msg2, sizeof(msg2), "2: %s", menu_normal_text[s_admin_fxData[s_admin.fxGunList.curvalue].arg2Desc]);

		UI_DrawProportionalString(524, 370, msg2, UI_TINYFONT | UI_CENTER, colorTable[CT_WHITE]);
	}

	if (s_admin_fxData[s_admin.fxGunList.curvalue].arg3Desc >= 0) {
		Com_sprintf(msg3, sizeof(msg3), "2: %s", menu_normal_text[s_admin_fxData[s_admin.fxGunList.curvalue].arg3Desc]);

		UI_DrawProportionalString(524, 382, msg3, UI_TINYFONT | UI_CENTER, colorTable[CT_WHITE]);
	}

	if (s_admin.keyBindActive >= 0) {
		UI_DrawProportionalString(344, 426, menu_normal_text[MNT_WAITINGFORNEWKEY], UI_TINYFONT | UI_CENTER, colorTable[CT_RED]);
	}

	UI_DrawProportionalString(81, 405, menu_normal_text[MNT_KEYBINDINGS], UI_SMALLFONT, colorTable[CT_WHITE]);

	/* Lines for player model frame */
	Menu_Draw(&s_admin.menu);
	UI_LogFuncEnd();
}

static sfxHandle_t AdminMenu_KeyEvent(int32_t key) {
	UI_LogFuncBegin();
	int32_t				i, j;
	menucommon_s	*s;
	char			command[256];

	s = (menucommon_s*)Menu_ItemAtCursor(&s_admin.menu);

	if (s_admin.keyBindActive >= 0) {
		if (key & K_CHAR_FLAG)
			goto end;

		if (key < 1 || key > 256)
			goto end;

		switch (key) {
		case K_ESCAPE:
			s_admin.keyBindActive = -1;
			s_admin.menu.m_NoNewSelecting = qfalse;

			return (menu_out_sound);
		case '`':
			goto end;
			break;
		}

		/* unbind this emote from anything else */
		for (i = 1; i < 256; i++) {
			trap_Key_GetBindingBuf(i, command, 256);

			if (!Q_stricmp(command, s_admin_bindCmds[s_admin.keyBindActive])) {
				trap_Key_SetBinding(i, "");
			}
		}

		/* set the new command */
		trap_Key_SetBinding(key, s_admin_bindCmds[s_admin.keyBindActive]);
		s_admin.binds[s_admin.keyBindActive] = key;

		s_admin.keyBindActive = -1;
		s_admin.menu.m_NoNewSelecting = qfalse;

		/* reget key binds for all the buttons */
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 256; j++) {
				trap_Key_GetBindingBuf(j, command, sizeof(command));

				if (*command == 0)
					continue;

				if (!Q_stricmp(command, s_admin_bindCmds[i])) {
					s_admin.binds[i] = j;
					break;
				}
			}
		}

		return (menu_out_sound);
	}
	else {
		/* unbind the current key */
		if (s->id >= ID_BIND_SPAWN && s->id <= ID_BIND_KICK) {
			int32_t id = s->id - 1161;

			switch (key) {
			case K_BACKSPACE:
			case K_DEL:
			case K_KP_DEL:

				for (i = 1; i < 256; i++) {
					trap_Key_GetBindingBuf(i, command, 256);

					if (!Q_stricmp(command, s_admin_bindCmds[id])) {
						trap_Key_SetBinding(i, "");
						s_admin.binds[id] = -1;
					}
				}
				return (menu_out_sound);
			}
		}
	}

end:
	UI_LogFuncEnd();
	return (Menu_DefaultKey(&s_admin.menu, key));
}

void UI_AdminMenu_Cache(void) {
	UI_LogFuncBegin();
	trap_R_RegisterShaderNoMip(PIC_ARROW_UP);
	trap_R_RegisterShaderNoMip(PIC_ARROW_DOWN);

	s_admin.corner_ll_4_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_admin.corner_ll_4_4 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_DrawButton

TiM: Because normal
buttons are drawn in a
really silly way, I've custom
wrote the way single buttons
are rendered.
===============
*/
static void AdminMenu_DrawButton(void *self) {
	UI_LogFuncBegin();
	qboolean		focus;
	menuaction_s*	action;
	int32_t				color;
	int32_t				textColor;
	int32_t				x;
	int32_t				y;

	action = (menuaction_s *)self;

	if (action->generic.flags & QMF_HIDDEN)
		return;

	x = action->generic.x;
	y = action->generic.y;

	focus = static_cast<qboolean>(Menu_ItemAtCursor(action->generic.parent) == action);

	/* get the needed color data */
	if (action->generic.flags & QMF_GRAYED) {
		color = CT_DKGREY;
		textColor = CT_BLACK;
	}
	else if (focus && action->generic.flags & QMF_HIGHLIGHT_IF_FOCUS) {
		color = action->color2;
		textColor = action->textcolor2;
	}
	else {
		color = action->color;
		textColor = action->textcolor;
	}

	/* draw the menu caption */
	if (menu_button_text[action->textEnum][1] && focus)
	{
		UI_DrawProportionalString(action->generic.parent->m_DescriptionPosition.X(), action->generic.parent->m_DescriptionPosition.Y(), menu_button_text[action->textEnum][1], UI_LEFT | UI_TINYFONT, colorTable[CT_BLACK]);
	}

	/* draw the button elements */
	trap_R_SetColor(colorTable[color]);
	UI_DrawHandlePic(x, y, MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);						/* left */
	UI_DrawHandlePic(x + action->width - MENU_BUTTON_MED_HEIGHT, y, -MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	/* right */
	UI_DrawHandlePic(x + MENU_BUTTON_MED_HEIGHT - 8, y, action->width - (MENU_BUTTON_MED_HEIGHT - 8) - 10, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);

	if (menu_button_text[action->textEnum][0]) {
		UI_DrawProportionalString(x + action->textX, y + action->textY, menu_button_text[action->textEnum][0], UI_SMALLFONT, colorTable[textColor]);
	}
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_DrawBinding
===============
*/
static void AdminMenu_DrawBinding(void *self) {
	UI_LogFuncBegin();
	qboolean		focus;
	menuaction_s	*action;
	int32_t				x, y;
	int32_t				bind;
	char			name[20];
	int32_t				buttonColor;
	int32_t				textColor;
	int32_t				width;

	action = (menuaction_s *)self;

	x = action->generic.x;
	y = action->generic.y;

	focus = static_cast<qboolean>(Menu_ItemAtCursor(action->generic.parent) == action);

	bind = s_admin.binds[action->generic.id - ID_BIND_SPAWN];

	/* Get bind name */
	if (bind <= 0 || bind >= 256) {
		Q_strncpyz(name, "???", sizeof(name));
	}
	else {
		trap_Key_KeynumToStringBuf(bind, name, sizeof(name));
		Q_strupr(name);
	}

	/* get relevant colors */
	if (focus) {
		buttonColor = action->color2;
		textColor = action->textcolor2;
	}
	else {
		buttonColor = action->color;
		textColor = action->textcolor;
	}

	if (focus) {
		if (menu_button_text[action->textEnum][1]) {
			UI_DrawProportionalString(action->generic.parent->m_DescriptionPosition.X(), action->generic.parent->m_DescriptionPosition.Y(), menu_button_text[action->textEnum][1], UI_LEFT | UI_TINYFONT, colorTable[CT_BLACK]);
		}
	}

	width = action->width;

	if (!width) {
		if (menu_button_text[action->textEnum][0])
			width = 19 + (SMALLCHAR_WIDTH * strlen(menu_button_text[action->textEnum][0])) + 19;
		else
			width = 19 + (SMALLCHAR_WIDTH * 11) + 19;
	}

	if (action->generic.flags & QMF_GRAYED)
		trap_R_SetColor(colorMdGrey);
	else
		trap_R_SetColor(colorTable[buttonColor]);

	UI_DrawHandlePic(x, y, 19, 19, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(x + width - 19, y, -19, 19, uis.graphicButtonLeftEnd);	/* right */
	UI_DrawHandlePic(x + 11, y, width - 22, 19, uis.whiteShader);
	trap_R_SetColor(NULL);

	/* button text */
	if (strlen(menu_button_text[action->textEnum][0])) {
		UI_DrawProportionalString(x + action->textX, y + action->textY, menu_button_text[action->textEnum][0], UI_SMALLFONT, colorTable[textColor]);
	}

	/* bind text */
	if (action->generic.flags & QMF_GRAYED)
		textColor = CT_DKGREY;
	else
		textColor = CT_WHITE;
	UI_DrawProportionalString(x + width + 5, y + 1, name, UI_SMALLFONT, colorTable[textColor]);
	UI_LogFuncEnd();
}

/*
===============
AdminMenu_Init
===============
*/
static void AdminMenu_Init(void) {
	UI_LogFuncBegin();
	int32_t i, j;
	char binding[256];

	UI_AdminMenu_Cache();

	/******************************************************************/

	AdminGeneric_InitLists();

	/* point the FX list */
	for (i = 0; i < 10; i++) {
		fxList[i] = s_admin_fxData[i].name;
	}

	/* get key binds for the bound list */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 256; j++) {
			trap_Key_GetBindingBuf(j, binding, sizeof(binding));

			if (*binding == 0)
				continue;

			if (!Q_stricmp(binding, s_admin_bindCmds[i])) {
				s_admin.binds[i] = j;
				break;
			}
		}
	}

	/******************************************************************/

	s_admin.menu.m_WrapAround = true;
	s_admin.menu.m_Fullscreen = qtrue;
	s_admin.menu.OnDraw = AdminMenu_Draw;
  s_admin.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
  s_admin.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_ADMIN_MENU };
	s_admin.menu.m_FootNote = MNT_ADMIN;
	s_admin.menu.OnKey = AdminMenu_KeyEvent;

	s_admin.god.generic.m_Type = EMenuItemType::Action;
	s_admin.god.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.god.generic.callback = AdminMenu_Event;
	s_admin.god.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.god.generic.id = ID_GOD;
	s_admin.god.generic.x = 90;
	s_admin.god.generic.y = 143;
	s_admin.god.textX = 9;
	s_admin.god.textY = 1;
	s_admin.god.textEnum = MBT_ADMIN_GOD;
	s_admin.god.color = CT_DKPURPLE1;
	s_admin.god.color2 = CT_LTPURPLE1;
	s_admin.god.textcolor = CT_BLACK;
	s_admin.god.textcolor2 = CT_WHITE;
	s_admin.god.width = 81;
	s_admin.god.height = 18;

	s_admin.noclip.generic.m_Type = EMenuItemType::Action;
	s_admin.noclip.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.noclip.generic.callback = AdminMenu_Event;
	s_admin.noclip.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.noclip.generic.id = ID_NOCLIP;
	s_admin.noclip.generic.x = 181;
	s_admin.noclip.generic.y = 143;
	s_admin.noclip.textX = 9;
	s_admin.noclip.textY = 1;
	s_admin.noclip.textEnum = MBT_ADMIN_NOCLIP;
	s_admin.noclip.color = CT_DKPURPLE1;
	s_admin.noclip.color2 = CT_LTPURPLE1;
	s_admin.noclip.textcolor = CT_BLACK;
	s_admin.noclip.textcolor2 = CT_WHITE;
	s_admin.noclip.width = 81;
	s_admin.noclip.height = 18;

	s_admin.cloak.generic.m_Type = EMenuItemType::Action;
	s_admin.cloak.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.cloak.generic.callback = AdminMenu_Event;
	s_admin.cloak.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.cloak.generic.id = ID_CLOAK;
	s_admin.cloak.generic.x = 90;
	s_admin.cloak.generic.y = 165;
	s_admin.cloak.textX = 9;
	s_admin.cloak.textY = 1;
	s_admin.cloak.textEnum = MBT_ADMIN_CLOAK;
	s_admin.cloak.color = CT_DKPURPLE1;
	s_admin.cloak.color2 = CT_LTPURPLE1;
	s_admin.cloak.textcolor = CT_BLACK;
	s_admin.cloak.textcolor2 = CT_WHITE;
	s_admin.cloak.width = 81;
	s_admin.cloak.height = 18;

	s_admin.flight.generic.m_Type = EMenuItemType::Action;
	s_admin.flight.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.flight.generic.callback = AdminMenu_Event;
	s_admin.flight.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.flight.generic.id = ID_FLIGHT;
	s_admin.flight.generic.x = 181;
	s_admin.flight.generic.y = 165;
	s_admin.flight.textX = 9;
	s_admin.flight.textY = 1;
	s_admin.flight.textEnum = MBT_ADMIN_FLIGHT;
	s_admin.flight.color = CT_DKPURPLE1;
	s_admin.flight.color2 = CT_LTPURPLE1;
	s_admin.flight.textcolor = CT_BLACK;
	s_admin.flight.textcolor2 = CT_WHITE;
	s_admin.flight.width = 81;
	s_admin.flight.height = 18;

	s_admin.giveList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.giveList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.giveList.generic.x = 298;
	s_admin.giveList.generic.y = 143;
	s_admin.giveList.width = 60;
	s_admin.giveList.height = 18;
	s_admin.giveList.itemnames = (const char**)s_adminGeneric.giveItems;
	s_admin.giveList.color = CT_DKPURPLE1;
	s_admin.giveList.color2 = CT_LTPURPLE1;
	s_admin.giveList.textcolor = CT_BLACK;
	s_admin.giveList.textcolor2 = CT_WHITE;
	s_admin.giveList.textX = 9;
	s_admin.giveList.textY = 1;
	s_admin.giveList.maxChars = 8;
	s_admin.giveList.textEnum = MBT_ADMIN_ITEM;
	/*s_admin.giveList.ignoreList		= qtrue;*/ /*Sad bug in the engine cropping up, so I have to disable this :(*/

	s_admin.give.generic.m_Type = EMenuItemType::Action;
	s_admin.give.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.give.generic.callback = AdminMenu_Event;
	s_admin.give.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.give.generic.id = ID_GIVE;
	s_admin.give.generic.x = 298;
	s_admin.give.generic.y = 165;
	s_admin.give.textX = 9;
	s_admin.give.textY = 1;
	s_admin.give.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.give.color = CT_DKPURPLE1;
	s_admin.give.color2 = CT_LTPURPLE1;
	s_admin.give.textcolor = CT_BLACK;
	s_admin.give.textcolor2 = CT_WHITE;
	s_admin.give.width = 70;
	s_admin.give.height = 18;

	s_admin.reviveList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.reviveList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.reviveList.generic.x = 474;
	s_admin.reviveList.generic.y = 143;
	s_admin.reviveList.width = 62;
	s_admin.reviveList.height = 18;
	s_admin.reviveList.listnames = s_admin_reviveList;
	s_admin.reviveList.color = CT_DKPURPLE1;
	s_admin.reviveList.color2 = CT_LTPURPLE1;
	s_admin.reviveList.textcolor = CT_BLACK;
	s_admin.reviveList.textcolor2 = CT_WHITE;
	s_admin.reviveList.textX = 9;
	s_admin.reviveList.textY = 1;
	s_admin.reviveList.textEnum = MBT_ADMIN_REVIVE;

	s_admin.revive.generic.m_Type = EMenuItemType::Action;
	s_admin.revive.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.revive.generic.callback = AdminMenu_Event;
	s_admin.revive.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.revive.generic.id = ID_REVIVE;
	s_admin.revive.generic.x = 474;
	s_admin.revive.generic.y = 165;
	s_admin.revive.textX = 9;
	s_admin.revive.textY = 1;
	s_admin.revive.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.revive.color = CT_DKPURPLE1;
	s_admin.revive.color2 = CT_LTPURPLE1;
	s_admin.revive.textcolor = CT_BLACK;
	s_admin.revive.textcolor2 = CT_WHITE;
	s_admin.revive.width = 72;
	s_admin.revive.height = 18;

	s_admin.msgArg.generic.m_Type = EMenuItemType::Field;
	s_admin.msgArg.field.widthInChars = 30;
	s_admin.msgArg.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O */
	s_admin.msgArg.generic.x = 90 + 5 +

		UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_MESSAGE][0], UI_SMALLFONT); /* 159; */
	s_admin.msgArg.generic.y = 217;
	s_admin.msgArg.field.style = UI_SMALLFONT;
	s_admin.msgArg.field.titleEnum = MBT_ADMIN_MESSAGE;
	s_admin.msgArg.field.titlecolor = CT_WHITE;
	s_admin.msgArg.field.textcolor = CT_DKGOLD1;
	s_admin.msgArg.field.textcolor2 = CT_LTGOLD1;

	s_admin.msg.generic.m_Type = EMenuItemType::Action;
	s_admin.msg.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.msg.generic.callback = AdminMenu_Event;
	s_admin.msg.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.msg.generic.id = ID_MESSAGE;
	s_admin.msg.generic.x = 90;
	s_admin.msg.generic.y = 238;
	s_admin.msg.textX = 9;
	s_admin.msg.textY = 1;
	s_admin.msg.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.msg.color = CT_DKPURPLE1;
	s_admin.msg.color2 = CT_LTPURPLE1;
	s_admin.msg.textcolor = CT_BLACK;
	s_admin.msg.textcolor2 = CT_WHITE;
	s_admin.msg.width = 100;
	s_admin.msg.height = 18;

	s_admin.useEntField.generic.m_Type = EMenuItemType::Field;
	s_admin.useEntField.field.widthInChars = 11;
	s_admin.useEntField.field.maxchars = 32;
	s_admin.useEntField.generic.x = 434 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_ENTITY][0], UI_SMALLFONT); /* 159; */
	s_admin.useEntField.generic.y = 217;
	s_admin.useEntField.field.style = UI_SMALLFONT;
	s_admin.useEntField.field.titleEnum = MBT_ADMIN_ENTITY;
	s_admin.useEntField.field.titlecolor = CT_WHITE;
	s_admin.useEntField.field.textcolor = CT_DKGOLD1;
	s_admin.useEntField.field.textcolor2 = CT_LTGOLD1;

	s_admin.useEnt.generic.m_Type = EMenuItemType::Action;
	s_admin.useEnt.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.useEnt.generic.callback = AdminMenu_Event;
	s_admin.useEnt.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.useEnt.generic.id = ID_ENTITY;
	s_admin.useEnt.generic.x = 434;
	s_admin.useEnt.generic.y = 238;
	s_admin.useEnt.textX = 9;
	s_admin.useEnt.textY = 1;
	s_admin.useEnt.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.useEnt.color = CT_DKPURPLE1;
	s_admin.useEnt.color2 = CT_LTPURPLE1;
	s_admin.useEnt.textcolor = CT_BLACK;
	s_admin.useEnt.textcolor2 = CT_WHITE;
	s_admin.useEnt.width = 100;
	s_admin.useEnt.height = 18;

	s_admin.beamLocList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.beamLocList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.beamLocList.generic.x = 90;
	s_admin.beamLocList.generic.y = 290;
	s_admin.beamLocList.width = 123;
	s_admin.beamLocList.height = 18;
	s_admin.beamLocList.itemnames = (const char**)s_adminGeneric.locListPtr;
	s_admin.beamLocList.color = CT_DKPURPLE1;
	s_admin.beamLocList.color2 = CT_LTPURPLE1;
	s_admin.beamLocList.textcolor = CT_BLACK;
	s_admin.beamLocList.textcolor2 = CT_WHITE;
	s_admin.beamLocList.textX = 9;
	s_admin.beamLocList.textY = 1;
	s_admin.beamLocList.maxChars = 20;
	s_admin.beamLocList.textEnum = MBT_ADMIN_BEAMLOC;

	s_admin.beamToLoc.generic.m_Type = EMenuItemType::Action;
	s_admin.beamToLoc.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.beamToLoc.generic.callback = AdminMenu_Event;
	s_admin.beamToLoc.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.beamToLoc.generic.id = ID_BEAM_LOC;
	s_admin.beamToLoc.generic.x = 360;
	s_admin.beamToLoc.generic.y = 290;
	s_admin.beamToLoc.textX = 9;
	s_admin.beamToLoc.textY = 1;
	s_admin.beamToLoc.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.beamToLoc.color = CT_DKPURPLE1;
	s_admin.beamToLoc.color2 = CT_LTPURPLE1;
	s_admin.beamToLoc.textcolor = CT_BLACK;
	s_admin.beamToLoc.textcolor2 = CT_WHITE;
	s_admin.beamToLoc.width = 70;
	s_admin.beamToLoc.height = 18;

	if (!s_adminGeneric.numLocs) {
		s_admin.beamLocList.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_admin.beamToLoc.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
	}

	s_admin.beamPlayerList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.beamPlayerList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.beamPlayerList.generic.x = 90;
	s_admin.beamPlayerList.generic.y = 312;
	s_admin.beamPlayerList.width = 123;
	s_admin.beamPlayerList.height = 18;
	s_admin.beamPlayerList.itemnames = (const char**)s_adminGeneric.charListPtr;
	s_admin.beamPlayerList.color = CT_DKPURPLE1;
	s_admin.beamPlayerList.color2 = CT_LTPURPLE1;
	s_admin.beamPlayerList.textcolor = CT_BLACK;
	s_admin.beamPlayerList.textcolor2 = CT_WHITE;
	s_admin.beamPlayerList.textX = 9;
	s_admin.beamPlayerList.textY = 1;
	s_admin.beamPlayerList.textEnum = MBT_ADMIN_BEAMPLAYER;
	s_admin.beamPlayerList.maxChars = 20;

	s_admin.beamToPlayerExecute.generic.m_Type = EMenuItemType::Action;
	s_admin.beamToPlayerExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.beamToPlayerExecute.generic.callback = AdminMenu_Event;
	s_admin.beamToPlayerExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.beamToPlayerExecute.generic.id = ID_BEAM_PLAYER;
	s_admin.beamToPlayerExecute.generic.x = 360;
	s_admin.beamToPlayerExecute.generic.y = 312;
	s_admin.beamToPlayerExecute.textX = 9;
	s_admin.beamToPlayerExecute.textY = 1;
	s_admin.beamToPlayerExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.beamToPlayerExecute.color = CT_DKPURPLE1;
	s_admin.beamToPlayerExecute.color2 = CT_LTPURPLE1;
	s_admin.beamToPlayerExecute.textcolor = CT_BLACK;
	s_admin.beamToPlayerExecute.textcolor2 = CT_WHITE;
	s_admin.beamToPlayerExecute.width = 70;
	s_admin.beamToPlayerExecute.height = 18;

	if (!s_adminGeneric.numChars) {
		s_admin.beamPlayerList.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_admin.beamToPlayerExecute.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
	}

	s_admin.fxGunList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.fxGunList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.fxGunList.generic.callback = AdminMenu_Event;
	s_admin.fxGunList.generic.id = ID_FXGUNLIST;
	s_admin.fxGunList.generic.x = 457;
	s_admin.fxGunList.generic.y = 290;
	s_admin.fxGunList.width = 66;
	s_admin.fxGunList.height = 18;
	s_admin.fxGunList.itemnames = (const char**)fxList;
	s_admin.fxGunList.color = CT_DKPURPLE1;
	s_admin.fxGunList.color2 = CT_LTPURPLE1;
	s_admin.fxGunList.textcolor = CT_BLACK;
	s_admin.fxGunList.textcolor2 = CT_WHITE;
	s_admin.fxGunList.textX = 9;
	s_admin.fxGunList.textY = 1;
	s_admin.fxGunList.textEnum = MBT_ADMIN_FX;

	s_admin.fxArg1.generic.m_Type = EMenuItemType::Field;
	s_admin.fxArg1.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	s_admin.fxArg1.field.widthInChars = 3;
	s_admin.fxArg1.field.maxchars = 8;
	s_admin.fxArg1.generic.x = 457 + 5 +
		UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_PARM1][0], UI_SMALLFONT); /* 159; */
	s_admin.fxArg1.generic.y = 310;
	s_admin.fxArg1.field.style = UI_SMALLFONT;
	s_admin.fxArg1.field.titleEnum = MBT_ADMIN_PARM1;
	s_admin.fxArg1.field.titlecolor = CT_WHITE;
	s_admin.fxArg1.field.textcolor = CT_DKGOLD1;
	s_admin.fxArg1.field.textcolor2 = CT_LTGOLD1;

	s_admin.fxArg2.generic.m_Type = EMenuItemType::Field;
	s_admin.fxArg2.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	s_admin.fxArg2.field.widthInChars = 3;
	s_admin.fxArg2.field.maxchars = 8;
	s_admin.fxArg2.generic.x = 510 + 5 +
		UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_PARM2][0], UI_SMALLFONT); /* 159; */
	s_admin.fxArg2.generic.y = 310;
	s_admin.fxArg2.field.style = UI_SMALLFONT;
	s_admin.fxArg2.field.titleEnum = MBT_ADMIN_PARM2;
	s_admin.fxArg2.field.titlecolor = CT_WHITE;
	s_admin.fxArg2.field.textcolor = CT_DKGOLD1;
	s_admin.fxArg2.field.textcolor2 = CT_LTGOLD1;

	s_admin.fxArg3.generic.m_Type = EMenuItemType::Field;
	s_admin.fxArg3.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	s_admin.fxArg3.field.widthInChars = 3;
	s_admin.fxArg3.field.maxchars = 8;
	s_admin.fxArg3.generic.x = 563 + 5 +
		UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_PARM3][0], UI_SMALLFONT); /* 159; */
	s_admin.fxArg3.generic.y = 310;
	s_admin.fxArg3.field.style = UI_SMALLFONT;
	s_admin.fxArg3.field.titleEnum = MBT_ADMIN_PARM3;
	s_admin.fxArg3.field.titlecolor = CT_WHITE;
	s_admin.fxArg3.field.textcolor = CT_DKGOLD1;
	s_admin.fxArg3.field.textcolor2 = CT_LTGOLD1;

	s_admin.fxGun.generic.m_Type = EMenuItemType::Action;
	s_admin.fxGun.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.fxGun.generic.callback = AdminMenu_Event;
	s_admin.fxGun.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.fxGun.generic.id = ID_FXGUN;
	s_admin.fxGun.generic.x = 457;
	s_admin.fxGun.generic.y = 330;
	s_admin.fxGun.textX = 9;
	s_admin.fxGun.textY = 1;
	s_admin.fxGun.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.fxGun.color = CT_DKPURPLE1;
	s_admin.fxGun.color2 = CT_LTPURPLE1;
	s_admin.fxGun.textcolor = CT_BLACK;
	s_admin.fxGun.textcolor2 = CT_WHITE;
	s_admin.fxGun.width = 70;
	s_admin.fxGun.height = 18;

	s_admin.spawnChar.generic.m_Type = EMenuItemType::Action;
	s_admin.spawnChar.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.spawnChar.generic.callback = AdminMenu_Event;
	s_admin.spawnChar.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.spawnChar.generic.id = ID_SPAWNCHAR;
	s_admin.spawnChar.generic.x = 90;
	s_admin.spawnChar.generic.y = 365;
	s_admin.spawnChar.textX = 9;
	s_admin.spawnChar.textY = 1;
	s_admin.spawnChar.textEnum = MBT_ADMIN_SPAWN;
	s_admin.spawnChar.color = CT_DKPURPLE1;
	s_admin.spawnChar.color2 = CT_LTPURPLE1;
	s_admin.spawnChar.textcolor = CT_BLACK;
	s_admin.spawnChar.textcolor2 = CT_WHITE;
	s_admin.spawnChar.width = 102;
	s_admin.spawnChar.height = 18;

	s_admin.flushList.generic.m_Type = EMenuItemType::SpinControl;
	s_admin.flushList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.flushList.generic.callback = AdminMenu_Event;
	s_admin.flushList.generic.x = 203;
	s_admin.flushList.generic.y = 365;
	s_admin.flushList.width = 102;
	s_admin.flushList.height = 18;
	s_admin.flushList.listnames = s_admin_spawnList;
	s_admin.flushList.color = CT_DKPURPLE1;
	s_admin.flushList.color2 = CT_LTPURPLE1;
	s_admin.flushList.textcolor = CT_BLACK;
	s_admin.flushList.textcolor2 = CT_WHITE;
	s_admin.flushList.textX = 9;
	s_admin.flushList.textY = 1;
	s_admin.flushList.textEnum = MBT_ADMIN_REMOVE;

	s_admin.flushChars.generic.m_Type = EMenuItemType::Action;
	s_admin.flushChars.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.flushChars.generic.callback = AdminMenu_Event;
	s_admin.flushChars.generic.ownerdraw = AdminMenu_DrawButton;
	s_admin.flushChars.generic.id = ID_REMOVECHAR;
	s_admin.flushChars.generic.x = 371;
	s_admin.flushChars.generic.y = 365;
	s_admin.flushChars.textX = 9;
	s_admin.flushChars.textY = 1;
	s_admin.flushChars.textEnum = MBT_ADMIN_EXECUTE;
	s_admin.flushChars.color = CT_DKPURPLE1;
	s_admin.flushChars.color2 = CT_LTPURPLE1;
	s_admin.flushChars.textcolor = CT_BLACK;
	s_admin.flushChars.textcolor2 = CT_WHITE;
	s_admin.flushChars.width = 70;
	s_admin.flushChars.height = 18;

	s_admin.spawnCharBind.generic.m_Type = EMenuItemType::Action;
	s_admin.spawnCharBind.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.spawnCharBind.generic.callback = AdminMenu_Event;
	s_admin.spawnCharBind.generic.ownerdraw = AdminMenu_DrawBinding;
	s_admin.spawnCharBind.generic.id = ID_BIND_SPAWN;
	s_admin.spawnCharBind.generic.x = 149;
	s_admin.spawnCharBind.generic.y = 404;
	s_admin.spawnCharBind.textX = 9;
	s_admin.spawnCharBind.textY = 1;
	s_admin.spawnCharBind.textEnum = MBT_ADMIN_SPAWNB;
	s_admin.spawnCharBind.color = CT_DKPURPLE1;
	s_admin.spawnCharBind.color2 = CT_LTPURPLE1;
	s_admin.spawnCharBind.textcolor = CT_BLACK;
	s_admin.spawnCharBind.textcolor2 = CT_WHITE;
	s_admin.spawnCharBind.width = 112;
	s_admin.spawnCharBind.height = 19;

	s_admin.useEntBind.generic.m_Type = EMenuItemType::Action;
	s_admin.useEntBind.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.useEntBind.generic.callback = AdminMenu_Event;
	s_admin.useEntBind.generic.ownerdraw = AdminMenu_DrawBinding;
	s_admin.useEntBind.generic.id = ID_BIND_USEENT;
	s_admin.useEntBind.generic.x = 301;
	s_admin.useEntBind.generic.y = 404;
	s_admin.useEntBind.textX = 9;
	s_admin.useEntBind.textY = 1;
	s_admin.useEntBind.textEnum = MBT_ADMIN_USEENTB;
	s_admin.useEntBind.color = CT_DKPURPLE1;
	s_admin.useEntBind.color2 = CT_LTPURPLE1;
	s_admin.useEntBind.textcolor = CT_BLACK;
	s_admin.useEntBind.textcolor2 = CT_WHITE;
	s_admin.useEntBind.width = 112;
	s_admin.useEntBind.height = 19;

	s_admin.kickTargetBind.generic.m_Type = EMenuItemType::Action;
	s_admin.kickTargetBind.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_admin.kickTargetBind.generic.callback = AdminMenu_Event;
	s_admin.kickTargetBind.generic.ownerdraw = AdminMenu_DrawBinding;
	s_admin.kickTargetBind.generic.id = ID_BIND_KICK;
	s_admin.kickTargetBind.generic.x = 453;
	s_admin.kickTargetBind.generic.y = 404;
	s_admin.kickTargetBind.textX = 9;
	s_admin.kickTargetBind.textY = 1;
	s_admin.kickTargetBind.textEnum = MBT_ADMIN_KICKB;
	s_admin.kickTargetBind.color = CT_DKPURPLE1;
	s_admin.kickTargetBind.color2 = CT_LTPURPLE1;
	s_admin.kickTargetBind.textcolor = CT_BLACK;
	s_admin.kickTargetBind.textcolor2 = CT_WHITE;
	s_admin.kickTargetBind.width = 112;
	s_admin.kickTargetBind.height = 19;
	s_admin.keyBindActive = -1;	/* Represents a held-down bind */

	/*s_admin.upArrow.generic.type			= EMenuItemType::Bitmap;
	s_admin.upArrow.generic.flags			= QMF_HIDDEN | QMF_INACTIVE | QMF_GRAYED;
	s_admin.upArrow.generic.x				= 242;
	s_admin.upArrow.generic.y				= 185;
	s_admin.upArrow.generic.name			= PIC_ARROW_UP;
	s_admin.upArrow.generic.id				= ID_LIST_UP;
	s_admin.upArrow.generic.callback		= AdminMenu_Event;
	s_admin.upArrow.width  					= 16;
	s_admin.upArrow.height  				= 16;
	s_admin.upArrow.color					= CT_DKPURPLE1;
	s_admin.upArrow.color2					= CT_LTPURPLE1;
	s_admin.upArrow.textX					= MENU_BUTTON_TEXT_X;
	s_admin.upArrow.textY					= MENU_BUTTON_TEXT_Y;
	s_admin.upArrow.textcolor				= CT_BLACK;
	s_admin.upArrow.textcolor2				= CT_WHITE;

	s_admin.dnArrow.generic.type			= EMenuItemType::Bitmap;
	s_admin.dnArrow.generic.flags			= QMF_HIDDEN | QMF_INACTIVE | QMF_GRAYED;
	s_admin.dnArrow.generic.x				= 242;
	s_admin.dnArrow.generic.y				= 407;
	s_admin.dnArrow.generic.name			= PIC_ARROW_DOWN;
	s_admin.dnArrow.generic.id				= ID_LIST_DN;
	s_admin.dnArrow.generic.callback		= AdminMenu_Event;
	s_admin.dnArrow.width  					= 16;
	s_admin.dnArrow.height  				= 16;
	s_admin.dnArrow.color					= CT_DKPURPLE1;
	s_admin.dnArrow.color2					= CT_LTPURPLE1;
	s_admin.dnArrow.textX					= MENU_BUTTON_TEXT_X;
	s_admin.dnArrow.textY					= MENU_BUTTON_TEXT_Y;
	s_admin.dnArrow.textcolor				= CT_BLACK;
	s_admin.dnArrow.textcolor2				= CT_WHITE;*/

	/* Add the common buttons */
	AdminMenu_InitButtons(&s_admin.menu);

	Menu_AddItem(&s_admin.menu, &s_admin.god);
	Menu_AddItem(&s_admin.menu, &s_admin.noclip);
	Menu_AddItem(&s_admin.menu, &s_admin.cloak);
	Menu_AddItem(&s_admin.menu, &s_admin.flight);

	Menu_AddItem(&s_admin.menu, &s_admin.giveList);
	Menu_AddItem(&s_admin.menu, &s_admin.give);

	Menu_AddItem(&s_admin.menu, &s_admin.reviveList);
	Menu_AddItem(&s_admin.menu, &s_admin.revive);

	Menu_AddItem(&s_admin.menu, &s_admin.msgArg);
	Menu_AddItem(&s_admin.menu, &s_admin.msg);

	Menu_AddItem(&s_admin.menu, &s_admin.useEntField);
	Menu_AddItem(&s_admin.menu, &s_admin.useEnt);

	Menu_AddItem(&s_admin.menu, &s_admin.beamLocList);
	Menu_AddItem(&s_admin.menu, &s_admin.beamToLoc);

	Menu_AddItem(&s_admin.menu, &s_admin.beamPlayerList);
	Menu_AddItem(&s_admin.menu, &s_admin.beamToPlayerExecute);

	Menu_AddItem(&s_admin.menu, &s_admin.fxGunList);
	Menu_AddItem(&s_admin.menu, &s_admin.fxArg1);
	Menu_AddItem(&s_admin.menu, &s_admin.fxArg2);
	Menu_AddItem(&s_admin.menu, &s_admin.fxArg3);
	Menu_AddItem(&s_admin.menu, &s_admin.fxGun);

	Menu_AddItem(&s_admin.menu, &s_admin.spawnChar);
	Menu_AddItem(&s_admin.menu, &s_admin.flushList);
	Menu_AddItem(&s_admin.menu, &s_admin.flushChars);

	Menu_AddItem(&s_admin.menu, &s_admin.spawnCharBind);
	Menu_AddItem(&s_admin.menu, &s_admin.useEntBind);
	Menu_AddItem(&s_admin.menu, &s_admin.kickTargetBind);

	/*Menu_AddItem( &s_admin.menu, &s_admin.upArrow );
	Menu_AddItem( &s_admin.menu, &s_admin.dnArrow );*/

	/*
	 * TiM: Ahem... haaxxxx. For some weird reason, these aren't encapsulating right
	 * ... could be the lack of null pointer at the end ROFL
	 */
	s_admin.reviveList.numitems = 2;
	UI_LogFuncEnd();
}

/*
===============
UI_AdminMenu
===============
*/
void UI_AdminMenu(qboolean fromConsole) {
	UI_LogFuncBegin();
	memset(&s_admin, 0, sizeof(s_admin));

	s_admin.fromConsole = fromConsole;
	AdminMenu_Init();

	ingameFlag = qtrue;
	Mouse_Show();

	UI_PushMenu(&s_admin.menu);
	UI_LogFuncEnd();
}

/*========================================================================*/
/*Admin Client Menu*/

typedef struct {
	int32_t		id;
	char	model[MAX_QPATH];
	float	height;
	float	weight;
	char	race[MAX_NAME_LENGTH];
	char	age[MAX_NAME_LENGTH];
	int32_t		classId;
} clientStats_t;

typedef struct {
	menuframework_s		menu;

	menulist_s			clientList;
	int32_t					numClients;

	clientStats_t		clientStats;

	menulist_s			beamToLoc;
	menuaction_s		beamToLocExecute;

	menulist_s			beamToPlayer;
	menuaction_s		beamToPlayerExecute;

	menulist_s			giveItem;
	menuaction_s		giveItemExecute;

	menufield_s			kickReason;
	menuaction_s		kickExecute;

	menulist_s			forcePlayer;
	menufield_s			forceParm;

	menuaction_s		forceExecute;
} clientAdmin_t;

clientAdmin_t	s_clientAdmin;

/*
===================
ClientAdmin_GetClient

TiM: Get all the data
we need to display info about
the currently selected player
===================
*/

static void ClientAdmin_GetClient(int32_t listNum)
{
	UI_LogFuncBegin();
	/* get the currently selected client, and parse their info string */
	char info[MAX_TOKEN_CHARS];

	trap_GetConfigString(CS_PLAYERS + s_adminGeneric.charList[listNum].id, info, sizeof(info));

	/* holy crap this is bad. */
	/* This could happen if a selected player disconnected at exactly the wrong time lol */
	if (!info[0]) {
		UI_Logger(LL_ERROR, "Selected player was not found. Please try again.");
		/* Re-Init. If it were a dud player, this'll probably get rid of them */
		s_clientAdmin.clientList.curvalue = 0;
		AdminGeneric_InitLists();
		/*return;*/
	}

	/* Fill the fields up */
	s_clientAdmin.clientStats.id = s_adminGeneric.charList[s_clientAdmin.clientList.curvalue].id;
	s_clientAdmin.clientStats.height = atof(Info_ValueForKey(info, "height"));
	s_clientAdmin.clientStats.weight = atof(Info_ValueForKey(info, "weight"));
	s_clientAdmin.clientStats.classId = atoi(Info_ValueForKey(info, "p"));
	Q_strncpyz(s_clientAdmin.clientStats.model, Info_ValueForKey(info, "model"), sizeof(s_clientAdmin.clientStats.model));
	Q_strncpyz(s_clientAdmin.clientStats.race, Info_ValueForKey(info, "race"), sizeof(s_clientAdmin.clientStats.race));
	Q_strncpyz(s_clientAdmin.clientStats.age, Info_ValueForKey(info, "age"), sizeof(s_clientAdmin.clientStats.age));
	UI_LogFuncEnd();

}

/*
===================
ClientAdmin_Event
===================
*/

static void ClientAdmin_Event(void *ptr, int32_t event) {
	UI_LogFuncBegin();

	if (event != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s *)ptr)->id) {
	case ID_CLIENT_SELECT:
		ClientAdmin_GetClient(s_adminGeneric.charList[s_clientAdmin.clientList.curvalue].id);
		break;

	case ID_BEAM_LOC:
		/*UI_ForceMenuOff()*/;
		trap_Cmd_ExecuteText(EXEC_APPEND, va("beamToLocation %i %i", s_clientAdmin.clientStats.id, s_clientAdmin.beamToLoc.curvalue + 1));
		break;

	case ID_BEAM_PLAYER:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("beamToPlayer %i %i", s_clientAdmin.clientStats.id, s_adminGeneric.charList[s_clientAdmin.beamToPlayer.curvalue].id));
		break;
	case ID_GIVE:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("give %i %s", s_clientAdmin.clientStats.id, s_adminGeneric.giveItems[s_clientAdmin.giveItem.curvalue]));
		break;
	case ID_CLIENT_KICK:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("kick2 %i %s", s_clientAdmin.clientStats.id, s_clientAdmin.kickReason.field.buffer));
		break;

	case ID_CLIENT_FORCE:
		/*UI_ForceMenuOff();*/

		/* TiM: Manual overrides for class and rank */
		if (!Q_stricmp(s_admin_forceParams[s_clientAdmin.forcePlayer.curvalue], "class"))
		{
			trap_Cmd_ExecuteText(EXEC_APPEND, va("forceClass %i %s", s_clientAdmin.clientStats.id, s_clientAdmin.forceParm.field.buffer));
		}
		else if (!Q_stricmp(s_admin_forceParams[s_clientAdmin.forcePlayer.curvalue], "rank"))
		{
			trap_Cmd_ExecuteText(EXEC_APPEND, va("forceRank %i %s", s_clientAdmin.clientStats.id, s_clientAdmin.forceParm.field.buffer));
		}
		else
		{
			trap_Cmd_ExecuteText(EXEC_APPEND, va("forcePlayer %i %s %s", s_clientAdmin.clientStats.id,
				s_admin_forceParams[s_clientAdmin.forcePlayer.curvalue],
				s_clientAdmin.forceParm.field.buffer));
		}
		break;

	}
	UI_LogFuncEnd();
}

/*
===================
ClientAdmin_Draw
===================
*/
static void ClientAdmin_Draw(void) {
	UI_LogFuncBegin();
	vec_t*	color = colorTable[CT_LTORANGE];
	char	string[MAX_QPATH];
	int32_t		x, y;

	UI_MenuFrame2(&s_clientAdmin.menu);

	/* Main Box */
	AdminMenu_DrawLCARSBox(81, 122, 527, 118, color, MNT_CLIENT_INFO_CMDS);
	/* Beam List */
	AdminMenu_DrawLCARSBox(81, 244, 351, 70, color, MNT_BEAM_CMDS);
	/* Give Item */
	AdminMenu_DrawLCARSBox(435, 244, 173, 70, color, MNT_GIVE_CMDS);
	/* Kick Client */
	AdminMenu_DrawLCARSBox(81, 318, 527, 48, color, MNT_KICK_CLIENT_CMDS);
	/* Force Parm */
	AdminMenu_DrawLCARSBox(81, 370, 527, 48, color, MNT_FORCE_PARAM_CMDS);

	/* Draw Player Data */
	x = 106;
	y = 169;

	/* Player ID */
	Com_sprintf(string, sizeof(string), "%s: %3i", menu_normal_text[MNT_ID], s_clientAdmin.clientStats.id);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Player Model */
	x += 174;
	Com_sprintf(string, sizeof(string), "%s: %s", menu_normal_text[MNT_MODEL], s_clientAdmin.clientStats.model);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Height */
	x += 233; /* big gap for long model names */
	Com_sprintf(string, sizeof(string), "%s: %3.2f", menu_normal_text[MNT_HEIGHT], s_clientAdmin.clientStats.height);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Race */
	x = 106;
	y += 22;
	Com_sprintf(string, sizeof(string), "%s: %s", menu_normal_text[MNT_RACE], s_clientAdmin.clientStats.race);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Class */
	x += 174;
	Com_sprintf(string, sizeof(string), "%s: %s", menu_normal_text[MNT_CLASS], uis.classData[s_clientAdmin.clientStats.classId].classNameFull);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Weight */
	x += 233; /* big gap for long model names */
	Com_sprintf(string, sizeof(string), "%s: %3.2f", menu_normal_text[MNT_WEIGHT], s_clientAdmin.clientStats.weight);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Age */
	x = 106;
	y += 22;
	Com_sprintf(string, sizeof(string), "%s: %s", menu_normal_text[MNT_AGE], s_clientAdmin.clientStats.age);
	UI_DrawProportionalString(x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);

	/* Rank */
	/* *SOBS* Won't work since we can't access the scores from here :S */
	/*x+=174;
	Com_sprintf( string, sizeof( string ), "%s: %s", menu_normal_text[MNT_RANK], "TestRank" );
	UI_DrawProportionalString( x, y, string, UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);*/

	AdminMenu_DrawCommon();

	Menu_Draw(&s_clientAdmin.menu);
	UI_LogFuncEnd();
}

/*
===================
ClientAdmin_Init
===================
*/
static void ClientAdmin_Init(void) {
	UI_LogFuncBegin();

	AdminGeneric_InitLists();

	s_clientAdmin.menu.m_WrapAround = true;
	s_clientAdmin.menu.m_Fullscreen = qtrue;
	s_clientAdmin.menu.OnDraw = ClientAdmin_Draw;
	s_clientAdmin.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_clientAdmin.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CLIENT_MENU };
	s_clientAdmin.menu.m_FootNote = MNT_ADMIN;

	s_clientAdmin.clientList.generic.m_Type = EMenuItemType::SpinControl;
	s_clientAdmin.clientList.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.clientList.generic.x = 96;
	s_clientAdmin.clientList.generic.y = 145;
	s_clientAdmin.clientList.width = 128;
	s_clientAdmin.clientList.height = 18;
	s_clientAdmin.clientList.itemnames = (const char**)s_adminGeneric.charListPtr;
	s_clientAdmin.clientList.color = CT_DKPURPLE1;
	s_clientAdmin.clientList.color2 = CT_LTPURPLE1;
	s_clientAdmin.clientList.textcolor = CT_BLACK;
	s_clientAdmin.clientList.textcolor2 = CT_WHITE;
	s_clientAdmin.clientList.textX = 9;
	s_clientAdmin.clientList.textY = 1;
	s_clientAdmin.clientList.textEnum = MBT_CLIENT_SELECT;
	s_clientAdmin.clientList.generic.callback = ClientAdmin_Event;
	s_clientAdmin.clientList.generic.id = ID_CLIENT_SELECT;

	s_clientAdmin.beamToLoc.generic.m_Type = EMenuItemType::SpinControl;
	s_clientAdmin.beamToLoc.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.beamToLoc.generic.x = 91;
	s_clientAdmin.beamToLoc.generic.y = 266;
	s_clientAdmin.beamToLoc.width = 123;
	s_clientAdmin.beamToLoc.height = 18;
	s_clientAdmin.beamToLoc.itemnames = (const char**)s_adminGeneric.locListPtr;
	s_clientAdmin.beamToLoc.color = CT_DKPURPLE1;
	s_clientAdmin.beamToLoc.color2 = CT_LTPURPLE1;
	s_clientAdmin.beamToLoc.textcolor = CT_BLACK;
	s_clientAdmin.beamToLoc.textcolor2 = CT_WHITE;
	s_clientAdmin.beamToLoc.textX = 9;
	s_clientAdmin.beamToLoc.textY = 1;
	s_clientAdmin.beamToLoc.maxChars = 20;
	s_clientAdmin.beamToLoc.textEnum = MBT_ADMIN_BEAMLOC;

	s_clientAdmin.beamToLocExecute.generic.m_Type = EMenuItemType::Action;
	s_clientAdmin.beamToLocExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.beamToLocExecute.generic.callback = ClientAdmin_Event;
	s_clientAdmin.beamToLocExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_clientAdmin.beamToLocExecute.generic.id = ID_BEAM_LOC;
	s_clientAdmin.beamToLocExecute.generic.x = 355;
	s_clientAdmin.beamToLocExecute.generic.y = 266;
	s_clientAdmin.beamToLocExecute.textX = 9;
	s_clientAdmin.beamToLocExecute.textY = 1;
	s_clientAdmin.beamToLocExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_clientAdmin.beamToLocExecute.color = CT_DKPURPLE1;
	s_clientAdmin.beamToLocExecute.color2 = CT_LTPURPLE1;
	s_clientAdmin.beamToLocExecute.textcolor = CT_BLACK;
	s_clientAdmin.beamToLocExecute.textcolor2 = CT_WHITE;
	s_clientAdmin.beamToLocExecute.width = 70;
	s_clientAdmin.beamToLocExecute.height = 18;

	if (!s_adminGeneric.numLocs) {
		s_clientAdmin.beamToLoc.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_clientAdmin.beamToLocExecute.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
	}

	s_clientAdmin.beamToPlayer.generic.m_Type = EMenuItemType::SpinControl;
	s_clientAdmin.beamToPlayer.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.beamToPlayer.generic.x = 91;
	s_clientAdmin.beamToPlayer.generic.y = 288;
	s_clientAdmin.beamToPlayer.width = 123;
	s_clientAdmin.beamToPlayer.height = 18;
	s_clientAdmin.beamToPlayer.itemnames = (const char**)s_adminGeneric.charListPtr;
	s_clientAdmin.beamToPlayer.color = CT_DKPURPLE1;
	s_clientAdmin.beamToPlayer.color2 = CT_LTPURPLE1;
	s_clientAdmin.beamToPlayer.textcolor = CT_BLACK;
	s_clientAdmin.beamToPlayer.textcolor2 = CT_WHITE;
	s_clientAdmin.beamToPlayer.textX = 9;
	s_clientAdmin.beamToPlayer.textY = 1;
	s_clientAdmin.beamToPlayer.textEnum = MBT_ADMIN_BEAMPLAYER;
	s_clientAdmin.beamToPlayer.maxChars = 20;

	s_clientAdmin.beamToPlayerExecute.generic.m_Type = EMenuItemType::Action;
	s_clientAdmin.beamToPlayerExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.beamToPlayerExecute.generic.callback = ClientAdmin_Event;
	s_clientAdmin.beamToPlayerExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_clientAdmin.beamToPlayerExecute.generic.id = ID_BEAM_PLAYER;
	s_clientAdmin.beamToPlayerExecute.generic.x = 355;
	s_clientAdmin.beamToPlayerExecute.generic.y = 288;
	s_clientAdmin.beamToPlayerExecute.textX = 9;
	s_clientAdmin.beamToPlayerExecute.textY = 1;
	s_clientAdmin.beamToPlayerExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_clientAdmin.beamToPlayerExecute.color = CT_DKPURPLE1;
	s_clientAdmin.beamToPlayerExecute.color2 = CT_LTPURPLE1;
	s_clientAdmin.beamToPlayerExecute.textcolor = CT_BLACK;
	s_clientAdmin.beamToPlayerExecute.textcolor2 = CT_WHITE;
	s_clientAdmin.beamToPlayerExecute.width = 70;
	s_clientAdmin.beamToPlayerExecute.height = 18;

	if (!s_adminGeneric.numChars) {
		s_clientAdmin.beamToPlayer.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_clientAdmin.beamToPlayerExecute.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
	}

	s_clientAdmin.giveItem.generic.m_Type = EMenuItemType::SpinControl;
	s_clientAdmin.giveItem.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.giveItem.generic.x = 446;
	s_clientAdmin.giveItem.generic.y = 266;
	s_clientAdmin.giveItem.width = 60;
	s_clientAdmin.giveItem.height = 18;
	s_clientAdmin.giveItem.itemnames = (const char**)s_adminGeneric.giveItems;
	s_clientAdmin.giveItem.color = CT_DKPURPLE1;
	s_clientAdmin.giveItem.color2 = CT_LTPURPLE1;
	s_clientAdmin.giveItem.textcolor = CT_BLACK;
	s_clientAdmin.giveItem.textcolor2 = CT_WHITE;
	s_clientAdmin.giveItem.textX = 9;
	s_clientAdmin.giveItem.textY = 1;
	s_clientAdmin.giveItem.maxChars = 8;
	s_clientAdmin.giveItem.textEnum = MBT_ADMIN_ITEM;
	/*s_clientAdmin.giveItem.ignoreList			= qtrue;*/ /*Sad bug in the engine cropping up, so I have to disable this :(*/

	s_clientAdmin.giveItemExecute.generic.m_Type = EMenuItemType::Action;
	s_clientAdmin.giveItemExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.giveItemExecute.generic.callback = ClientAdmin_Event;
	s_clientAdmin.giveItemExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_clientAdmin.giveItemExecute.generic.id = ID_GIVE;
	s_clientAdmin.giveItemExecute.generic.x = 446;
	s_clientAdmin.giveItemExecute.generic.y = 288;
	s_clientAdmin.giveItemExecute.textX = 9;
	s_clientAdmin.giveItemExecute.textY = 1;
	s_clientAdmin.giveItemExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_clientAdmin.giveItemExecute.color = CT_DKPURPLE1;
	s_clientAdmin.giveItemExecute.color2 = CT_LTPURPLE1;
	s_clientAdmin.giveItemExecute.textcolor = CT_BLACK;
	s_clientAdmin.giveItemExecute.textcolor2 = CT_WHITE;
	s_clientAdmin.giveItemExecute.width = 70;
	s_clientAdmin.giveItemExecute.height = 18;

	s_clientAdmin.kickReason.generic.m_Type = EMenuItemType::Field;
	s_clientAdmin.kickReason.field.widthInChars = 42;
	s_clientAdmin.kickReason.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O*/
	s_clientAdmin.kickReason.generic.x = 91 + 5 +
		UI_ProportionalStringWidth(menu_button_text[MBT_CLIENT_KICK][0], UI_SMALLFONT); /* 159; */
	s_clientAdmin.kickReason.generic.y = 340;
	s_clientAdmin.kickReason.field.style = UI_SMALLFONT;
	s_clientAdmin.kickReason.field.titleEnum = MBT_CLIENT_KICK;
	s_clientAdmin.kickReason.field.titlecolor = CT_WHITE;
	s_clientAdmin.kickReason.field.textcolor = CT_DKGOLD1;
	s_clientAdmin.kickReason.field.textcolor2 = CT_LTGOLD1;

	s_clientAdmin.kickExecute.generic.m_Type = EMenuItemType::Action;
	s_clientAdmin.kickExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.kickExecute.generic.callback = ClientAdmin_Event;
	s_clientAdmin.kickExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_clientAdmin.kickExecute.generic.id = ID_CLIENT_KICK;
	s_clientAdmin.kickExecute.generic.x = 512;
	s_clientAdmin.kickExecute.generic.y = 340;
	s_clientAdmin.kickExecute.textX = 9;
	s_clientAdmin.kickExecute.textY = 1;
	s_clientAdmin.kickExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_clientAdmin.kickExecute.color = CT_DKPURPLE1;
	s_clientAdmin.kickExecute.color2 = CT_LTPURPLE1;
	s_clientAdmin.kickExecute.textcolor = CT_BLACK;
	s_clientAdmin.kickExecute.textcolor2 = CT_WHITE;
	s_clientAdmin.kickExecute.width = 91;
	s_clientAdmin.kickExecute.height = 18;

	s_clientAdmin.forcePlayer.generic.m_Type = EMenuItemType::SpinControl;
	s_clientAdmin.forcePlayer.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.forcePlayer.generic.x = 91;
	s_clientAdmin.forcePlayer.generic.y = 392;
	s_clientAdmin.forcePlayer.width = 98;
	s_clientAdmin.forcePlayer.height = 18;
	s_clientAdmin.forcePlayer.itemnames = (const char**)s_admin_forceParams;
	s_clientAdmin.forcePlayer.color = CT_DKPURPLE1;
	s_clientAdmin.forcePlayer.color2 = CT_LTPURPLE1;
	s_clientAdmin.forcePlayer.textcolor = CT_BLACK;
	s_clientAdmin.forcePlayer.textcolor2 = CT_WHITE;
	s_clientAdmin.forcePlayer.textX = 9;
	s_clientAdmin.forcePlayer.textY = 1;
	s_clientAdmin.forcePlayer.maxChars = 20;
	s_clientAdmin.forcePlayer.textEnum = MBT_CLIENT_FORCE;

	s_clientAdmin.forceParm.generic.m_Type = EMenuItemType::Field;
	s_clientAdmin.forceParm.field.widthInChars = 15;
	s_clientAdmin.forceParm.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O */
	s_clientAdmin.forceParm.generic.x = 281 + 5 +
		UI_ProportionalStringWidth(menu_button_text[MBT_CLIENT_PARAM][0], UI_SMALLFONT); /* 159; */
	s_clientAdmin.forceParm.generic.y = 392;
	s_clientAdmin.forceParm.field.style = UI_SMALLFONT;
	s_clientAdmin.forceParm.field.titleEnum = MBT_CLIENT_PARAM;
	s_clientAdmin.forceParm.field.titlecolor = CT_WHITE;
	s_clientAdmin.forceParm.field.textcolor = CT_DKGOLD1;
	s_clientAdmin.forceParm.field.textcolor2 = CT_LTGOLD1;

	s_clientAdmin.forceExecute.generic.m_Type = EMenuItemType::Action;
	s_clientAdmin.forceExecute.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_clientAdmin.forceExecute.generic.callback = ClientAdmin_Event;
	s_clientAdmin.forceExecute.generic.ownerdraw = AdminMenu_DrawButton;
	s_clientAdmin.forceExecute.generic.id = ID_CLIENT_FORCE;
	s_clientAdmin.forceExecute.generic.x = 512;
	s_clientAdmin.forceExecute.generic.y = 392;
	s_clientAdmin.forceExecute.textX = 9;
	s_clientAdmin.forceExecute.textY = 1;
	s_clientAdmin.forceExecute.textEnum = MBT_ADMIN_EXECUTE;
	s_clientAdmin.forceExecute.color = CT_DKPURPLE1;
	s_clientAdmin.forceExecute.color2 = CT_LTPURPLE1;
	s_clientAdmin.forceExecute.textcolor = CT_BLACK;
	s_clientAdmin.forceExecute.textcolor2 = CT_WHITE;
	s_clientAdmin.forceExecute.width = 91;
	s_clientAdmin.forceExecute.height = 18;

	AdminMenu_InitButtons(&s_clientAdmin.menu);

	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.clientList);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.beamToLoc);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.beamToLocExecute);

	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.beamToPlayer);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.beamToPlayerExecute);

	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.giveItem);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.giveItemExecute);

	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.kickReason);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.kickExecute);

	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.forcePlayer);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.forceParm);
	Menu_AddItem(&s_clientAdmin.menu, &s_clientAdmin.forceExecute);

	ClientAdmin_GetClient(0);
	UI_LogFuncEnd();
}

/*
===================
UI_ClientAdminMenu
===================
*/

void UI_ClientAdminMenu(void) {
	UI_LogFuncBegin();
	memset(&s_clientAdmin, 0, sizeof(s_clientAdmin));

	ClientAdmin_Init();
	Mouse_Show();

	UI_PushMenu(&s_clientAdmin.menu);
	UI_LogFuncEnd();
}

/*****************************************************************************************/
/* TiM - Admin Audio Menu */

typedef enum
{
	TYPE_WAV,
	TYPE_MP3
} soundFormat_t;

typedef struct
{
	char	fileRoute[MAX_QPATH];
	byte	format;
} soundList_t;

typedef struct {
	menuframework_s		menu;

	/* song list */
	menubitmap_s		songButtons[MAX_BUTTONS];
	menubitmap_s		songUpArrow;
	menubitmap_s		songDnArrow;
	menubitmap_s		playSong;
	menubitmap_s		stopSong;

	/* sound list */
	menubitmap_s		soundButtons[MAX_BUTTONS];
	menubitmap_s		soundUpArrow;
	menubitmap_s		soundDnArrow;
	menubitmap_s		playSound;

	soundList_t			musicList[MAX_SONGS];
	int32_t					numSongs;
	int32_t					songOffset;
	int32_t					selectedSong;

	soundList_t			soundList[MAX_SOUNDS_];
	int32_t					numSounds;
	int32_t					soundOffset;
	int32_t					selectedSound;

} adminAudio_t;

adminAudio_t	s_adminAudio;

/*
====================
AdminAudio_OffsetList

TiM: Offset the list of char pointers
when we click an arrow button
====================
*/
/*char mainArray[][MAX_QPATH]*/

static void AdminAudio_OffsetList(soundList_t *soundList, int32_t *offset, int32_t numSounds, menubitmap_s *btnList)
{
	UI_LogFuncBegin();
	int32_t		i;
	char	*buffer;

	if (*offset < 0)
	{
		*offset = 0;
	}

	if (numSounds > MAX_BUTTONS && (*offset > (numSounds - MAX_BUTTONS)))
	{
		*offset = (numSounds - MAX_BUTTONS);
	}

	for (i = 0; i < MAX_BUTTONS; i++)
	{
		buffer = soundList[*offset + i].fileRoute;

		if (!buffer[0])
		{
			if (btnList[i].generic.flags & (QMF_INACTIVE | QMF_HIDDEN)) {
				continue;
			}
			else {
				btnList[i].generic.flags |= (QMF_INACTIVE | QMF_HIDDEN);
				continue;
			}
		}

		btnList[i].generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		btnList[i].textPtr = buffer;
	}
	UI_LogFuncEnd();
}

/*
====================
AdminAudio_InitFilesList

TiM: Load our dat file,
and see where we need to look
for sound/music files
====================
*/
static void AdminAudio_InitFilesList(void)
{
	UI_LogFuncBegin();
	int32_t				numFiles;
	char			buffer[20000];
	char*			strPtr;
	char*			token;
	int32_t				strLen;
	fileHandle_t	f;
	char			soundDirs[MAX_AUDIO_LOCATIONS][MAX_QPATH];
	char			musicDirs[MAX_AUDIO_LOCATIONS][MAX_QPATH];
	int32_t				numMusicDirs = 0;
	int32_t				numSndDirs = 0;
	int32_t				i, j;

	/* First step: Parse in the config where we store all our file dirs */
	strLen = trap_FS_FOpenFile("ext_data/audioFileRoutes.dat", &f, FS_READ);

	if (strLen <= 0) {
		UI_Logger(LL_ERROR, "ext_data/audioFileRoutes.dat could not be found.\n");
		return;
	}

	trap_FS_Read(buffer, sizeof(buffer), f);

	if (!buffer[0]) {
		UI_Logger(LL_WARN, "could not read ext_data/audio_dirs.dat\n");
		return;
	}

	trap_FS_FCloseFile(f);

	COM_BeginParseSession();

	strPtr = buffer;

	while (1)
	{
		token = COM_Parse(&strPtr);
		if (!token[0])
			break;

		/* music list */
		if (!Q_stricmp("musicFiles", token))
		{
			token = COM_Parse(&strPtr);
			if (!token[0])
				break;

			/* next item should be brackets */
			if (!Q_stricmpn(token, "{", 1))
			{
				/* start loop */
				token = COM_Parse(&strPtr);
				if (!token[0])
					break;

				while (token[0] && Q_stricmpn(token, "}", 1))
				{
					Q_strncpyz(musicDirs[numMusicDirs], token, sizeof(musicDirs[numMusicDirs]));
					numMusicDirs++;

					token = COM_Parse(&strPtr);
					if (!token[0])
						break;
				}
			}
			else
			{
				UI_Logger(LL_ERROR, "musicFiles does not have an opening brace!\n");
				continue;
			}
		}

		/* sound list */
		if (!Q_stricmp("soundFiles", token))
		{
			token = COM_Parse(&strPtr);
			if (!token[0])
				break;

			/* next item should be brackets */
			if (!Q_stricmpn(token, "{", 1))
			{
				/* start loop */
				token = COM_Parse(&strPtr);
				if (!token[0])
					break;

				while (token[0] && Q_stricmpn(token, "}", 1))
				{
					Q_strncpyz(soundDirs[numSndDirs], token, sizeof(soundDirs[numSndDirs]));
					numSndDirs++;

					token = COM_Parse(&strPtr);
					if (!token[0])
						break;
				}
			}
			else
			{
				UI_Logger(LL_ERROR, "soundFiles does not have an opening brace!\n");
				continue;
			}
		}
	}

	/* step two - load in the music dir mp3s */
	memset(&s_adminAudio.musicList, 0, sizeof(s_adminAudio.musicList));

	for (i = 0; i < numMusicDirs; i++)
	{
		/* first, get the file list */
		numFiles = trap_FS_GetFileList(musicDirs[i], ".mp3", buffer, sizeof(buffer));

		if (!numFiles || !buffer[0])
		{
			UI_Logger(LL_WARN, "No songs found in directory: %s\n", musicDirs[i]);
			continue;
		}

		strPtr = buffer;

		j = 0;
		while (j < numFiles)
		{
			if (!strPtr)
				break;

			strLen = strlen(strPtr);

			/*
			 * the API returns files in subfolders too
			 * but we're not interested in that here

			 //Why not?
			 if ( strchr( strPtr, '/' ) != NULL  )
			 {
			 strPtr += strLen + 1;
			 j++;
			 continue;
			 }*/

			if (!Q_stricmp(strPtr + strLen - 4, ".mp3"))
			{
				strPtr[strLen - 4] = '\0';
			}

			Com_sprintf(s_adminAudio.musicList[s_adminAudio.numSongs].fileRoute, sizeof(s_adminAudio.musicList[s_adminAudio.numSongs].fileRoute), "%s/%s", musicDirs[i], strPtr);
			/*UI_Logger( LL_DEBUG, "%s\n", s_adminAudio.musicList[s_adminAudio.numSongs] );*/
			s_adminAudio.musicList[s_adminAudio.numSongs].format = TYPE_MP3;
			s_adminAudio.numSongs++;

			if (s_adminAudio.numSongs >= MAX_SONGS)
				break;

			strPtr += strLen + 1;
			j++;
		}

	}

	/* step three - sound files */
	memset(&s_adminAudio.soundList, 0, sizeof(s_adminAudio.soundList));

	for (i = 0; i < numSndDirs; i++)
	{
		/* first, get the file list for mp3s */
		numFiles = trap_FS_GetFileList(soundDirs[i], ".mp3", buffer, sizeof(buffer));

		strPtr = buffer;

		j = 0;
		while (j < numFiles)
		{
			if (!strPtr)
				break;

			strLen = strlen(strPtr);

			/* no hierarchical files here */
			if (strchr(strPtr, '/') != NULL)
			{
				strPtr += strLen + 1;
				j++;
				continue;
			}

			if (!Q_stricmp(strPtr + strLen - 4, ".mp3"))
			{
				strPtr[strLen - 4] = '\0';
			}

			Com_sprintf(s_adminAudio.soundList[s_adminAudio.numSounds].fileRoute, sizeof(s_adminAudio.soundList[s_adminAudio.numSounds].fileRoute), "%s/%s", soundDirs[i], strPtr);
			/*UI_Logger( LL_DEBUG, "%s\n", s_adminAudio.soundList[s_adminAudio.numSounds] ); */
			s_adminAudio.soundList[s_adminAudio.numSounds].format = TYPE_MP3;
			s_adminAudio.numSounds++;

			if (s_adminAudio.numSounds >= MAX_SONGS)
				break;

			strPtr += strLen + 1;
			j++;
		}

		/*
		 * second - wav files
		 * in EF, MP3 files are usually single FX while wavs are loops
		 * tho custom sounds might not abide by this
		 */
		numFiles = trap_FS_GetFileList(soundDirs[i], ".wav", buffer, sizeof(buffer));

		strPtr = buffer;

		j = 0;
		while (j < numFiles)
		{
			if (!strPtr)
				break;

			strLen = strlen(strPtr);

			/* no hierarchical files here
			if ( strchr( strPtr, '/' ) != NULL  )
			{
			strPtr += strLen + 1;
			j++;
			continue;
			}*/

			if (!Q_stricmp(strPtr + strLen - 4, ".wav"))
			{
				strPtr[strLen - 4] = '\0';
			}

			Com_sprintf(s_adminAudio.soundList[s_adminAudio.numSounds].fileRoute, sizeof(s_adminAudio.soundList[s_adminAudio.numSounds].fileRoute), "%s/%s", soundDirs[i], strPtr);
			/*UI_Logger( LL_DEBUG, "%s\n", s_adminAudio.soundList[s_adminAudio.numSounds] ); */
			s_adminAudio.soundList[s_adminAudio.numSounds].format = TYPE_WAV;
			s_adminAudio.numSounds++;

			if (s_adminAudio.numSounds >= MAX_SONGS)
				break;

			strPtr += strLen + 1;

			j++;
		}
	}
	UI_LogFuncEnd();
}

/*
====================
AdminAudio_Event
====================
*/

static void AdminAudio_Event(void *ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s *)ptr)->id)
	{
		/* Song Buttons */
	case ID_SONGBUTTON0:
	case ID_SONGBUTTON1:
	case ID_SONGBUTTON2:
	case ID_SONGBUTTON3:
	case ID_SONGBUTTON4:
	case ID_SONGBUTTON5:
	case ID_SONGBUTTON6:
	{
						   if (s_adminAudio.selectedSong < 0)
							   s_adminAudio.playSong.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

						   s_adminAudio.selectedSong = s_adminAudio.songOffset + (((menucommon_s *)ptr)->id - 13100);
	}

		break;
	case ID_SONGUP:
		s_adminAudio.songOffset -= 7; /*--;*/
		AdminAudio_OffsetList(s_adminAudio.musicList, &s_adminAudio.songOffset, s_adminAudio.numSongs, s_adminAudio.songButtons);
		break;
	case ID_SONGDN:
		s_adminAudio.songOffset += 7; /*++;*/
		AdminAudio_OffsetList(s_adminAudio.musicList, &s_adminAudio.songOffset, s_adminAudio.numSongs, s_adminAudio.songButtons);
		break;
	case ID_STOPSONG:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, "stopMusic");
		break;
	case ID_PLAYSONG:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("playMusic %s", s_adminAudio.musicList[s_adminAudio.selectedSong].fileRoute));
		break;

		/* Sound Buttons */
	case ID_SNDBUTTON0:
	case ID_SNDBUTTON1:
	case ID_SNDBUTTON2:
	case ID_SNDBUTTON3:
	case ID_SNDBUTTON4:
	case ID_SNDBUTTON5:
	case ID_SNDBUTTON6:
	{
						  if (s_adminAudio.selectedSound < 0)
							  s_adminAudio.playSound.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

						  s_adminAudio.selectedSound = s_adminAudio.soundOffset + (((menucommon_s *)ptr)->id - 13200);
	}
		break;
	case ID_SNDUP:
		s_adminAudio.soundOffset -= 7; /*--;*/
		AdminAudio_OffsetList(s_adminAudio.soundList, &s_adminAudio.soundOffset, s_adminAudio.numSounds, s_adminAudio.soundButtons);
		break;
	case ID_SNDDN:
		s_adminAudio.soundOffset += 7; /*++;*/
		AdminAudio_OffsetList(s_adminAudio.soundList, &s_adminAudio.soundOffset, s_adminAudio.numSounds, s_adminAudio.soundButtons);
		break;
	case ID_PLAYSND:
		/*UI_ForceMenuOff();*/
		trap_Cmd_ExecuteText(EXEC_APPEND, va("playSound %s.%s", s_adminAudio.soundList[s_adminAudio.selectedSound].fileRoute, s_adminAudio.soundList[s_adminAudio.selectedSound].format == TYPE_MP3 ? "mp3" : "wav"));
		break;
	}
	UI_LogFuncEnd();
}

/*
====================
AdminAudio_Draw
====================
*/

static void AdminAudio_Draw(void)
{
	UI_LogFuncBegin();
	int32_t i;
	int32_t x, y, w, h;

	UI_MenuFrame2(&s_adminAudio.menu);

	/*
	 * Music UI Elements
	 * As opposed to the other functions, we need
	 * to custom make this function
	 */

	x = 81;
	y = 121;
	w = 529;
	h = 155;

	trap_R_SetColor(colorTable[CT_LTORANGE]);
	/* draw upper box end curve */
	UI_DrawHandlePic(x + w - 18, y, -18, 18, uis.graphicButtonLeftEnd);
	/* draw upper main box */
	UI_DrawHandlePic(x + 10, y, w - 262, 18, uis.whiteShader);
	/* draw left brace */
	UI_DrawHandlePic(x, y + 20, 4, h - 26, uis.whiteShader);
	/* draw top left corner GFX */
	UI_DrawHandlePic(x, y - 12, 8, -32, s_admin.corner_ll_4_18);
	/* lower left end curve */
	UI_DrawHandlePic(x, y + h - 6, 8, 8, s_admin.corner_ll_4_4);
	/* teeny weeny end piece for lower bar */
	UI_DrawHandlePic(x + w - 4, y + h - 4, -4, 4, uis.graphicButtonLeftEnd);
	/* lower bar */
	UI_DrawHandlePic(x + 9, y + h - 4, w - 11, 4, uis.whiteShader);
	/* if possible, add a title */
	UI_DrawProportionalString(x + 15, y + 1, menu_normal_text[MNT_PLAYMUSIC], UI_LEFT | UI_SMALLFONT, colorTable[CT_BLACK]);

	/* Arrow Buttons */
	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(x + 476, y, 18, 18, uis.whiteShader);
	UI_DrawHandlePic(x + 497, y, 18, 18, uis.whiteShader);

	x = 81;
	y = 279;
	w = 529;
	h = 155;

	trap_R_SetColor(colorTable[CT_LTORANGE]);
	/* draw upper box end curve */
	UI_DrawHandlePic(x + w - 18, y, -18, 18, uis.graphicButtonLeftEnd);
	/* draw upper main box */
	UI_DrawHandlePic(x + 10, y, w - 164, 18, uis.whiteShader);
	/* draw left brace */
	UI_DrawHandlePic(x, y + 20, 4, h - 26, uis.whiteShader);
	/* draw top left corner GFX */
	UI_DrawHandlePic(x, y - 12, 8, -32, s_admin.corner_ll_4_18);
	/* lower left end curve */
	UI_DrawHandlePic(x, y + h - 6, 8, 8, s_admin.corner_ll_4_4);
	/* teeny weeny end piece for lower bar */
	UI_DrawHandlePic(x + w - 4, y + h - 4, -4, 4, uis.graphicButtonLeftEnd);
	/* lower bar */
	UI_DrawHandlePic(x + 9, y + h - 4, w - 11, 4, uis.whiteShader);
	/* if possible, add a title */
	UI_DrawProportionalString(x + 15, y + 1, menu_normal_text[MNT_PLAYSOUND], UI_LEFT | UI_SMALLFONT, colorTable[CT_BLACK]);

	/* Arrow Buttons */
	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(x + 476, y, 18, 18, uis.whiteShader);
	UI_DrawHandlePic(x + 497, y, 18, 18, uis.whiteShader);

	/* TiM - paint the selected menu option white */
	for (i = 0; i < MAX_BUTTONS; i++)
	{
		/* selected song */
		{
			if (i + s_adminAudio.songOffset == s_adminAudio.selectedSong)
			{
				s_adminAudio.songButtons[i].textcolor = CT_WHITE;
				s_adminAudio.songButtons[i].textcolor2 = CT_WHITE;
			}
			else
			{
				if (s_adminAudio.songButtons[i].textcolor == CT_WHITE && s_adminAudio.songButtons[i].textcolor2 == CT_WHITE)
				{
					s_adminAudio.songButtons[i].textcolor = CT_DKGOLD1;
					s_adminAudio.songButtons[i].textcolor2 = CT_LTGOLD1;
				}
			}
		}

		/* selected sound */
		{
		if (i + s_adminAudio.soundOffset == s_adminAudio.selectedSound)
		{
			s_adminAudio.soundButtons[i].textcolor = CT_WHITE;
			s_adminAudio.soundButtons[i].textcolor2 = CT_WHITE;
		}
		else
		{
			if (s_adminAudio.soundButtons[i].textcolor == CT_WHITE && s_adminAudio.soundButtons[i].textcolor2 == CT_WHITE)
			{
				s_adminAudio.soundButtons[i].textcolor = CT_DKGOLD1;
				s_adminAudio.soundButtons[i].textcolor2 = CT_LTGOLD1;
			}
		}
	}
	}

	AdminMenu_DrawCommon();
	Menu_Draw(&s_adminAudio.menu);
	UI_LogFuncEnd();
}

/*static void AdminAudio_Cache( void )
{
trap_R_RegisterShaderNoMip( PIC_ARROW_UP );
trap_R_RegisterShaderNoMip(	PIC_ARROW_DOWN );
}*/

/*
====================
AdminAudio_Init
====================
*/

static void AdminAudio_Init(void)
{
	UI_LogFuncBegin();
	int32_t i, x, y;

	s_adminAudio.selectedSong = -1;
	s_adminAudio.selectedSound = -1;

	AdminAudio_InitFilesList();

	s_adminAudio.menu.m_WrapAround = true;
	s_adminAudio.menu.m_Fullscreen = qtrue;
	s_adminAudio.menu.OnDraw = AdminAudio_Draw;
	s_adminAudio.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_adminAudio.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_AUDIO_MENU };
	s_adminAudio.menu.m_FootNote = MNT_ADMIN;

	x = 93;
	y = 141;
	for (i = 0; i < MAX_BUTTONS; i++)
	{
		s_adminAudio.songButtons[i].generic.m_Type = EMenuItemType::Bitmap;
		s_adminAudio.songButtons[i].generic.flags = QMF_INACTIVE | QMF_HIDDEN;
		s_adminAudio.songButtons[i].generic.x = x;
		s_adminAudio.songButtons[i].generic.y = y;
		s_adminAudio.songButtons[i].generic.callback = AdminAudio_Event;
		s_adminAudio.songButtons[i].generic.id = ID_SONGBUTTON0 + i;
		s_adminAudio.songButtons[i].width = 500; /*129*/
		s_adminAudio.songButtons[i].height = 16;
		s_adminAudio.songButtons[i].color = CT_DKPURPLE1;
		s_adminAudio.songButtons[i].color2 = CT_LTPURPLE1;
		s_adminAudio.songButtons[i].textPtr = NULL;
		s_adminAudio.songButtons[i].textX = 4;
		s_adminAudio.songButtons[i].textY = 1;
		s_adminAudio.songButtons[i].textcolor = CT_DKGOLD1;
		s_adminAudio.songButtons[i].textcolor2 = CT_LTGOLD1;
		s_adminAudio.songButtons[i].textStyle = UI_SMALLFONT;

		y += 18;
	}

	s_adminAudio.playSong.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.playSong.generic.name = GRAPHIC_SQUARE;
	s_adminAudio.playSong.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.playSong.generic.callback = AdminAudio_Event;
	s_adminAudio.playSong.generic.id = ID_PLAYSONG;
	s_adminAudio.playSong.generic.x = 361;
	s_adminAudio.playSong.generic.y = 121;
	s_adminAudio.playSong.width = 95;
	s_adminAudio.playSong.height = 18;
	s_adminAudio.playSong.color = CT_DKPURPLE1;
	s_adminAudio.playSong.color2 = CT_LTPURPLE1;
	s_adminAudio.playSong.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.playSong.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.playSong.textEnum = MBT_AUDIO_PLAYSONG;
	s_adminAudio.playSong.textcolor = CT_BLACK;
	s_adminAudio.playSong.textcolor2 = CT_WHITE;

	s_adminAudio.stopSong.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.stopSong.generic.name = GRAPHIC_SQUARE;
	s_adminAudio.stopSong.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_adminAudio.stopSong.generic.callback = AdminAudio_Event;
	s_adminAudio.stopSong.generic.id = ID_STOPSONG;
	s_adminAudio.stopSong.generic.x = 459;
	s_adminAudio.stopSong.generic.y = 121;
	s_adminAudio.stopSong.width = 95;
	s_adminAudio.stopSong.height = 18;
	s_adminAudio.stopSong.color = CT_DKPURPLE1;
	s_adminAudio.stopSong.color2 = CT_LTPURPLE1;
	s_adminAudio.stopSong.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.stopSong.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.stopSong.textEnum = MBT_AUDIO_STOPSONG;
	s_adminAudio.stopSong.textcolor = CT_BLACK;
	s_adminAudio.stopSong.textcolor2 = CT_WHITE;

	s_adminAudio.songUpArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.songUpArrow.generic.name = PIC_ARROW_UP;
	s_adminAudio.songUpArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.songUpArrow.generic.callback = AdminAudio_Event;
	s_adminAudio.songUpArrow.generic.id = ID_SONGUP;
	s_adminAudio.songUpArrow.generic.x = 558;
	s_adminAudio.songUpArrow.generic.y = 123;
	s_adminAudio.songUpArrow.width = 16;
	s_adminAudio.songUpArrow.height = 16;
	s_adminAudio.songUpArrow.color = CT_DKGOLD1;
	s_adminAudio.songUpArrow.color2 = CT_LTGOLD1;
	s_adminAudio.songUpArrow.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.songUpArrow.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.songUpArrow.textcolor = CT_BLACK;
	s_adminAudio.songUpArrow.textcolor2 = CT_WHITE;

	s_adminAudio.songDnArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.songDnArrow.generic.name = PIC_ARROW_DOWN;
	s_adminAudio.songDnArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.songDnArrow.generic.callback = AdminAudio_Event;
	s_adminAudio.songDnArrow.generic.id = ID_SONGDN;
	s_adminAudio.songDnArrow.generic.x = 579;
	s_adminAudio.songDnArrow.generic.y = 123;
	s_adminAudio.songDnArrow.width = 16;
	s_adminAudio.songDnArrow.height = 16;
	s_adminAudio.songDnArrow.color = CT_DKGOLD1;
	s_adminAudio.songDnArrow.color2 = CT_LTGOLD1;
	s_adminAudio.songDnArrow.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.songDnArrow.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.songDnArrow.textcolor = CT_BLACK;
	s_adminAudio.songDnArrow.textcolor2 = CT_WHITE;

	if (s_adminAudio.numSongs > MAX_BUTTONS) {
		s_adminAudio.songUpArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_adminAudio.songDnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	x = 93;
	y = 299;
	for (i = 0; i < MAX_BUTTONS; i++)
	{
		s_adminAudio.soundButtons[i].generic.m_Type = EMenuItemType::Bitmap;
		s_adminAudio.soundButtons[i].generic.flags = QMF_INACTIVE | QMF_HIDDEN;
		s_adminAudio.soundButtons[i].generic.x = x;
		s_adminAudio.soundButtons[i].generic.y = y;
		s_adminAudio.soundButtons[i].generic.callback = AdminAudio_Event;
		s_adminAudio.soundButtons[i].generic.id = ID_SNDBUTTON0 + i;
		s_adminAudio.soundButtons[i].width = 500; /*129*/
		s_adminAudio.soundButtons[i].height = 16;
		s_adminAudio.soundButtons[i].color = CT_DKPURPLE1;
		s_adminAudio.soundButtons[i].color2 = CT_LTPURPLE1;
		s_adminAudio.soundButtons[i].textPtr = NULL;
		s_adminAudio.soundButtons[i].textX = 4;
		s_adminAudio.soundButtons[i].textY = 1;
		s_adminAudio.soundButtons[i].textcolor = CT_DKGOLD1;
		s_adminAudio.soundButtons[i].textcolor2 = CT_LTGOLD1;
		s_adminAudio.soundButtons[i].textStyle = UI_SMALLFONT;

		y += 18;
	}

	s_adminAudio.playSound.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.playSound.generic.name = GRAPHIC_SQUARE;
	s_adminAudio.playSound.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.playSound.generic.callback = AdminAudio_Event;
	s_adminAudio.playSound.generic.id = ID_PLAYSND;
	s_adminAudio.playSound.generic.x = 459;
	s_adminAudio.playSound.generic.y = 279;
	s_adminAudio.playSound.width = 95;
	s_adminAudio.playSound.height = 18;
	s_adminAudio.playSound.color = CT_DKPURPLE1;
	s_adminAudio.playSound.color2 = CT_LTPURPLE1;
	s_adminAudio.playSound.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.playSound.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.playSound.textEnum = MBT_AUDIO_PLAYSND;
	s_adminAudio.playSound.textcolor = CT_BLACK;
	s_adminAudio.playSound.textcolor2 = CT_WHITE;

	s_adminAudio.soundUpArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.soundUpArrow.generic.name = PIC_ARROW_UP;
	s_adminAudio.soundUpArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.soundUpArrow.generic.callback = AdminAudio_Event;
	s_adminAudio.soundUpArrow.generic.id = ID_SNDUP;
	s_adminAudio.soundUpArrow.generic.x = 558;
	s_adminAudio.soundUpArrow.generic.y = 281;
	s_adminAudio.soundUpArrow.width = 16;
	s_adminAudio.soundUpArrow.height = 16;
	s_adminAudio.soundUpArrow.color = CT_DKGOLD1;
	s_adminAudio.soundUpArrow.color2 = CT_LTGOLD1;
	s_adminAudio.soundUpArrow.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.soundUpArrow.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.soundUpArrow.textcolor = CT_BLACK;
	s_adminAudio.soundUpArrow.textcolor2 = CT_WHITE;

	s_adminAudio.soundDnArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_adminAudio.soundDnArrow.generic.name = PIC_ARROW_DOWN;
	s_adminAudio.soundDnArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_adminAudio.soundDnArrow.generic.callback = AdminAudio_Event;
	s_adminAudio.soundDnArrow.generic.id = ID_SNDDN;
	s_adminAudio.soundDnArrow.generic.x = 579;
	s_adminAudio.soundDnArrow.generic.y = 281;
	s_adminAudio.soundDnArrow.width = 16;
	s_adminAudio.soundDnArrow.height = 16;
	s_adminAudio.soundDnArrow.color = CT_DKGOLD1;
	s_adminAudio.soundDnArrow.color2 = CT_LTGOLD1;
	s_adminAudio.soundDnArrow.textX = MENU_BUTTON_TEXT_X;
	s_adminAudio.soundDnArrow.textY = MENU_BUTTON_TEXT_Y;
	s_adminAudio.soundDnArrow.textcolor = CT_BLACK;
	s_adminAudio.soundDnArrow.textcolor2 = CT_WHITE;

	if (s_adminAudio.numSounds > MAX_BUTTONS)
	{
		s_adminAudio.soundUpArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_adminAudio.soundDnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	AdminMenu_InitButtons(&s_adminAudio.menu);

	for (i = 0; i < MAX_BUTTONS; i++)
		Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.songButtons[i]);

	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.playSong);
	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.stopSong);
	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.songUpArrow);
	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.songDnArrow);

	for (i = 0; i < MAX_BUTTONS; i++)
		Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.soundButtons[i]);

	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.playSound);
	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.soundUpArrow);
	Menu_AddItem(&s_adminAudio.menu, &s_adminAudio.soundDnArrow);

	AdminAudio_OffsetList(s_adminAudio.musicList, &s_adminAudio.songOffset, s_adminAudio.numSongs, s_adminAudio.songButtons);
	AdminAudio_OffsetList(s_adminAudio.soundList, &s_adminAudio.soundOffset, s_adminAudio.numSounds, s_adminAudio.soundButtons);
	UI_LogFuncEnd();
}

/*
====================
UI_AdminAudioMenu
====================
*/

void UI_AdminAudioMenu(void)
{
	UI_LogFuncBegin();
	memset(&s_adminAudio, 0, sizeof(s_adminAudio));

	AdminAudio_Init();
	Mouse_Show();

	UI_PushMenu(&s_adminAudio.menu);
	UI_LogFuncEnd();
}

/**********************************************************************
	UI_LOGIN

	User inferface for user login (sql)
	**********************************************************************/

static byte sqlkey;
uiClientState_t	cs;
char	info[MAX_INFO_STRING];

typedef struct {
	menuframework_s menu;
	sfxHandle_t		openingVoice;
	menuaction_s	admin;
	menuaction_s	sql;
	menubitmap_s	quit;
	menufield_s		apassword;
	menufield_s		username;
	menufield_s		password;
	qboolean		fromConsole;
	qboolean		isAdmin;
	int32_t				isSQL;
} login_t;

login_t s_login;

/*
=================
UI_LoginSetSqlkey
=================
*/
void UI_LoginSetSqlkey(int32_t key) {
	UI_LogFuncBegin();
	sqlkey = (byte)key;
	UI_LogFuncEnd();
}

/*
=================
M_Login_Event
=================
*/
static void M_Login_Event(void* ptr, int32_t notification) {
	UI_LogFuncBegin();
	int32_t id;

	id = ((menucommon_s*)ptr)->id;

	switch (id) {
	case ID_SQL:
		if (notification == QM_ACTIVATED) {
			trap_Cmd_ExecuteText(EXEC_APPEND, va("userlogin %s %s\n", s_login.username.field.buffer, s_login.password.field.buffer));
			UI_PopMenu();
		}
		break;
	case ID_ADMIN:
		if (notification == QM_ACTIVATED) {
			trap_Cmd_ExecuteText(EXEC_APPEND, va("adminlogin %s\n", s_login.apassword.field.buffer));
			UI_PopMenu();
		}
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
LoginMenu_Key
=================
*/
sfxHandle_t LoginMenu_Key(int32_t key) {
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return (Menu_DefaultKey(&s_login.menu, key));
}

/*
===============
LoginMenu_Draw
===============
*/
static void LoginMenu_Draw(void) {
	UI_LogFuncBegin();
	int32_t length;

	AdminMenu_DrawCommon();
	UI_MenuFrame2(&s_login.menu);

	if (s_login.isAdmin || s_login.isSQL > 0){
		length = UI_ProportionalStringWidth("WELCOME TO", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 130, "WELCOME TO", UI_BIGFONT, colorTable[CT_WHITE]);;
		length = UI_ProportionalStringWidth("LCARS ODN BACKEND PROCESSING", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 160, "LCARS ODN BACKEND PROCESSING", UI_BIGFONT, colorTable[CT_WHITE]);
		length = UI_ProportionalStringWidth("ACCESS GRANTED", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 190, "ACCESS GRANTED", UI_BIGFONT, colorTable[CT_GREEN]);
	}
	else{
		length = UI_ProportionalStringWidth("LCARS ODN BACKEND PROCESSING", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 130, "LCARS ODN BACKEND PROCESSING", UI_BIGFONT, colorTable[CT_WHITE]);
		length = UI_ProportionalStringWidth("ACCESS DENIED", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 160, "ACCESS DENIED", UI_BIGFONT, colorTable[CT_RED]);
		length = UI_ProportionalStringWidth("PLEASE ENTER AUTHORISATION CODE", UI_BIGFONT);
		UI_DrawProportionalString(360 - (length / 2), 190, "PLEASE ENTER AUTHORISATION CODE", UI_BIGFONT, colorTable[CT_WHITE]);

		vec_t*	color = colorTable[CT_LTORANGE];
		AdminMenu_DrawLCARSBox(190, 268, 340, 70, color, MNT_BROADCAST_CMDS); /* Admin Login */
		AdminMenu_DrawLCARSBox(190, 342, 340, 91, color, MNT_BROADCAST_CMDS); /* SQL Login */
	}

	Menu_Draw(&s_login.menu);
	UI_LogFuncEnd();
}

/*
===============
LoginMenu_Init
===============
*/
void LoginMenu_Init(void) {
	UI_LogFuncBegin();
	if (s_login.isAdmin || s_login.isSQL > 0){
		UI_AdminMenu_Cache();

		s_login.menu.m_WrapAround = true;
		s_login.menu.m_Fullscreen = qtrue;
		s_login.menu.OnDraw = LoginMenu_Draw;
		s_login.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
		s_login.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_ADMIN_MENU };
		s_login.menu.m_FootNote = MNT_ADMIN;
		s_login.menu.OnKey = LoginMenu_Key;

		AdminMenu_InitButtons(&s_login.menu);
	}
	else{
		UI_AdminMenu_Cache();

		s_login.menu.m_WrapAround = true;
		s_login.menu.m_Fullscreen = qtrue;
		s_login.menu.OnDraw = LoginMenu_Draw;
		s_login.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
		s_login.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_ADMIN_MENU };
		s_login.menu.m_FootNote = MNT_ADMIN;
		s_login.menu.OnKey = LoginMenu_Key;

		s_login.apassword.generic.m_Type = EMenuItemType::Field;
		s_login.apassword.field.widthInChars = 30;
		s_login.apassword.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O */
		s_login.apassword.generic.x = 200 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_MESSAGE][0], UI_SMALLFONT); /* 159; */
		s_login.apassword.generic.y = 290;
		s_login.apassword.field.style = UI_SMALLFONT;
		s_login.apassword.field.titleEnum = MBT_ADMIN_MESSAGE;
		s_login.apassword.field.titlecolor = CT_WHITE;
		s_login.apassword.field.textcolor = CT_DKGOLD1;
		s_login.apassword.field.textcolor2 = CT_LTGOLD1;

		s_login.admin.generic.m_Type = EMenuItemType::Action;
		s_login.admin.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_login.admin.generic.callback = M_Login_Event;
		s_login.admin.generic.ownerdraw = AdminMenu_DrawButton;
		s_login.admin.generic.id = ID_ADMIN;
		s_login.admin.generic.x = 200;
		s_login.admin.generic.y = 312;
		s_login.admin.textX = 9;
		s_login.admin.textY = 1;
		s_login.admin.textEnum = MBT_ADMIN_EXECUTE;
		s_login.admin.color = CT_DKPURPLE1;
		s_login.admin.color2 = CT_LTPURPLE1;
		s_login.admin.textcolor = CT_BLACK;
		s_login.admin.textcolor2 = CT_WHITE;
		s_login.admin.width = 100;
		s_login.admin.height = 18;

		s_login.username.generic.m_Type = EMenuItemType::Field;
		s_login.username.field.widthInChars = 30;
		s_login.username.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O */
		s_login.username.generic.x = 200 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_MESSAGE][0], UI_SMALLFONT); /* 159; */
		s_login.username.generic.y = 364;
		s_login.username.field.style = UI_SMALLFONT;
		s_login.username.field.titleEnum = MBT_ADMIN_MESSAGE;
		s_login.username.field.titlecolor = CT_WHITE;
		s_login.username.field.textcolor = CT_DKGOLD1;
		s_login.username.field.textcolor2 = CT_LTGOLD1;

		s_login.password.generic.m_Type = EMenuItemType::Field;
		s_login.password.field.widthInChars = 30;
		s_login.password.field.maxchars = MAX_TOKEN_CHARS; /* Freeking hell this is long... who coded this admin thing? O_O */
		s_login.password.generic.x = 200 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_ADMIN_MESSAGE][0], UI_SMALLFONT); /* 159; */
		s_login.password.generic.y = 386;
		s_login.password.field.style = UI_SMALLFONT;
		s_login.password.field.titleEnum = MBT_ADMIN_MESSAGE;
		s_login.password.field.titlecolor = CT_WHITE;
		s_login.password.field.textcolor = CT_DKGOLD1;
		s_login.password.field.textcolor2 = CT_LTGOLD1;

		s_login.sql.generic.m_Type = EMenuItemType::Action;
		s_login.sql.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_login.sql.generic.callback = M_Login_Event;
		s_login.sql.generic.ownerdraw = AdminMenu_DrawButton;
		s_login.sql.generic.id = ID_SQL;
		s_login.sql.generic.x = 200;
		s_login.sql.generic.y = 408;
		s_login.sql.textX = 9;
		s_login.sql.textY = 1;
		s_login.sql.textEnum = MBT_ADMIN_EXECUTE;
		s_login.sql.color = CT_DKPURPLE1;
		s_login.sql.color2 = CT_LTPURPLE1;
		s_login.sql.textcolor = CT_BLACK;
		s_login.sql.textcolor2 = CT_WHITE;
		s_login.sql.width = 100;
		s_login.sql.height = 18;


		Menu_AddItem(&s_login.menu, &s_login.apassword);
		Menu_AddItem(&s_login.menu, &s_login.admin);

		Menu_AddItem(&s_login.menu, &s_login.username);
		Menu_AddItem(&s_login.menu, &s_login.password);
		Menu_AddItem(&s_login.menu, &s_login.sql);

		AdminMenu_InitButtons(&s_login.menu);
	}
	UI_LogFuncEnd();

}

/*
===============
UI_LoginMenu
===============
*/
void UI_LoginMenu(qboolean fromConsole) {
	UI_LogFuncBegin();
	memset(&s_login, 0, sizeof(s_login));

	uis.menusp = 0;

	ingameFlag = qtrue;

	trap_GetClientState(&cs);
	trap_GetConfigString(CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING);
	s_login.isAdmin = static_cast<qboolean>(atoi(Info_ValueForKey(info, "admin")));
	s_login.isSQL = atoi(Info_ValueForKey(info, "uid"));
	s_login.fromConsole = fromConsole;

	Mouse_Show();

	LoginMenu_Init();

	UI_PushMenu(&s_login.menu);

	Menu_AdjustCursor(&s_login.menu, 1);
	UI_LogFuncEnd();
}