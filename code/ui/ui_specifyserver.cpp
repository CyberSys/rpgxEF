// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"
#include "ui_logger.h"

/*********************************************************************************
	SPECIFY SERVER
	*********************************************************************************/

enum ui_specifyserverIDs_e {
	ID_MAINMENU = 100,
	ID_BACK = 102,
	ID_SPECIFYSERVERGO
};

typedef struct
{
	menuframework_s	menu;
	menubitmap_s	mainmenu;
	menubitmap_s	back;
	menufield_s		domain;
	menufield_s		port;
	menubitmap_s	go;

	qhandle_t		corner_ul;
	qhandle_t		corner_ur;
	qhandle_t		corner_ll;
	qhandle_t		corner_lr;

} specifyserver_t;


static specifyserver_t	s_specifyserver;

/*
=================
SpecifyServer_Event
=================
*/
static void SpecifyServer_Event(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	char	buff[256];

	if (event != QM_ACTIVATED)
	{
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
	case ID_MAINMENU:
		UI_PopMenu();
		UI_MainMenu();
		break;

	case ID_SPECIFYSERVERGO:
		if (s_specifyserver.domain.field.buffer[0])
		{
			strcpy(buff, s_specifyserver.domain.field.buffer);
			if (s_specifyserver.port.field.buffer[0])
				Com_sprintf(buff + strlen(buff), 128, ":%s", s_specifyserver.port.field.buffer);

			trap_Cmd_ExecuteText(EXEC_APPEND, va("connect %s\n", buff));
		}
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
SpecifyServer_Graphics
=================
*/
void SpecifyServer_Graphics(void)
{
	UI_LogFuncBegin();
	UI_MenuFrame2(&s_specifyserver.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Left hand column of screen frame

	trap_R_SetColor(colorTable[CT_LTPURPLE3]);
	UI_DrawHandlePic(135, 100, 32, 32, s_specifyserver.corner_ul);	// Upper left corner
	UI_DrawHandlePic(574, 100, 32, 32, s_specifyserver.corner_ur);	// Upper Right corner
	UI_DrawHandlePic(135, 282, 32, 32, s_specifyserver.corner_ll);	// corner
	UI_DrawHandlePic(574, 282, 32, 32, s_specifyserver.corner_lr);	// corner

	// Address label
	trap_R_SetColor(colorTable[CT_LTPURPLE3]);
	UI_DrawHandlePic(189, 166, 385, 20, uis.whiteShader);	// Background for Address label
	trap_R_SetColor(colorTable[CT_BLACK]);
	UI_DrawHandlePic(253, 168, 315, 16, uis.whiteShader);	// Black square for data

	// Port label
	trap_R_SetColor(colorTable[CT_LTPURPLE3]);
	UI_DrawHandlePic(191, 214, 135, 20, uis.whiteShader);	// Background for Port label
	trap_R_SetColor(colorTable[CT_BLACK]);
	UI_DrawHandlePic(254, 216, 58, 16, uis.whiteShader);	// Black square for data

	// Frame around data
	trap_R_SetColor(colorTable[CT_LTPURPLE3]);
	UI_DrawHandlePic(152, 100, 426, 18, uis.whiteShader);	// Top line
	UI_DrawHandlePic(135, 120, 18, 171, uis.whiteShader);	// Left side
	UI_DrawHandlePic(578, 120, 18, 171, uis.whiteShader);	// Right side
	UI_DrawHandlePic(160, 286, 411, 18, uis.whiteShader);	// Bottom line to left of Engage button

	UI_DrawProportionalString(169, 102, menu_normal_text[MNT_SERVERINFORMATION], UI_SMALLFONT, colorTable[CT_BLACK]);


	UI_DrawProportionalString(74, 27, "216", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 74, "1009-3", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 188, "9-3001", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "10963", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "10961", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	// Left rounded ends for buttons
	trap_R_SetColor(colorTable[s_specifyserver.mainmenu.color]);
	UI_DrawHandlePic(s_specifyserver.mainmenu.generic.x - 14, s_specifyserver.mainmenu.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_specifyserver.back.color]);
	UI_DrawHandlePic(s_specifyserver.back.generic.x - 14, s_specifyserver.back.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_LogFuncEnd();
}

/*
=================
SpecifyServer_MenuDraw
=================
*/
static void SpecifyServer_MenuDraw(void)
{
	UI_LogFuncBegin();
	SpecifyServer_Graphics();

	Menu_Draw(&s_specifyserver.menu);

	UI_LogFuncEnd();
}

/*
=================
SpecifyServer_MenuInit
=================
*/
void SpecifyServer_MenuInit(void)
{
	UI_LogFuncBegin();
	// zero set all our globals
	memset(&s_specifyserver, 0, sizeof(specifyserver_t));

	SpecifyServer_Cache();

	s_specifyserver.menu.m_WrapAround = true;
	s_specifyserver.menu.m_Fullscreen = qtrue;
	s_specifyserver.menu.OnDraw = SpecifyServer_MenuDraw;
	s_specifyserver.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_specifyserver.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_MULTIPLAYER_TITLE };
	s_specifyserver.menu.m_FootNote = MNT_SPECIFYSERVER;

	s_specifyserver.mainmenu.generic.m_Type = EMenuItemType::Bitmap;
	s_specifyserver.mainmenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_specifyserver.mainmenu.generic.x = 110;
	s_specifyserver.mainmenu.generic.y = 391;
	s_specifyserver.mainmenu.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_specifyserver.mainmenu.generic.id = ID_MAINMENU;
	s_specifyserver.mainmenu.generic.callback = SpecifyServer_Event;
	s_specifyserver.mainmenu.width = MENU_BUTTON_MED_WIDTH;
	s_specifyserver.mainmenu.height = MENU_BUTTON_MED_HEIGHT;
	s_specifyserver.mainmenu.color = CT_DKPURPLE1;
	s_specifyserver.mainmenu.color2 = CT_LTPURPLE1;
	s_specifyserver.mainmenu.textX = 5;
	s_specifyserver.mainmenu.textY = 2;
	s_specifyserver.mainmenu.textEnum = MBT_MAINMENU;
	s_specifyserver.mainmenu.textcolor = CT_BLACK;
	s_specifyserver.mainmenu.textcolor2 = CT_WHITE;

	s_specifyserver.back.generic.m_Type = EMenuItemType::Bitmap;
	s_specifyserver.back.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_specifyserver.back.generic.x = 110;
	s_specifyserver.back.generic.y = 415;
	s_specifyserver.back.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_specifyserver.back.generic.id = ID_BACK;
	s_specifyserver.back.generic.callback = SpecifyServer_Event;
	s_specifyserver.back.width = MENU_BUTTON_MED_WIDTH;
	s_specifyserver.back.height = MENU_BUTTON_MED_HEIGHT;
	s_specifyserver.back.color = CT_DKPURPLE1;
	s_specifyserver.back.color2 = CT_LTPURPLE1;
	s_specifyserver.back.textX = 5;
	s_specifyserver.back.textY = 2;
	s_specifyserver.back.textEnum = MBT_BACK;
	s_specifyserver.back.textcolor = CT_BLACK;
	s_specifyserver.back.textcolor2 = CT_WHITE;

	s_specifyserver.domain.generic.m_Type = EMenuItemType::Field;
	s_specifyserver.domain.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_specifyserver.domain.generic.x = 255;
	s_specifyserver.domain.generic.y = 168;
	s_specifyserver.domain.field.widthInChars = 38;
	s_specifyserver.domain.field.maxchars = 80;
	s_specifyserver.domain.field.style = UI_SMALLFONT;
	s_specifyserver.domain.field.titleEnum = MBT_ADDRESS;
	s_specifyserver.domain.field.textcolor = CT_LTGOLD1;
	s_specifyserver.domain.field.textcolor2 = CT_LTGOLD1;
	s_specifyserver.domain.field.titlecolor = CT_LTGOLD1;

	s_specifyserver.port.generic.m_Type = EMenuItemType::Field;
	s_specifyserver.port.generic.flags = QMF_PULSEIFFOCUS | QMF_SMALLFONT | QMF_NUMBERSONLY;
	s_specifyserver.port.generic.x = 255;
	s_specifyserver.port.generic.y = 216;
	s_specifyserver.port.field.widthInChars = 6;
	s_specifyserver.port.field.maxchars = 5;
	s_specifyserver.port.field.style = UI_SMALLFONT;
	s_specifyserver.port.field.titleEnum = MBT_PORT;
	s_specifyserver.port.field.textcolor = CT_LTGOLD1;
	s_specifyserver.port.field.textcolor2 = CT_LTGOLD1;
	s_specifyserver.port.field.titlecolor = CT_LTGOLD1;

	s_specifyserver.go.generic.m_Type = EMenuItemType::Bitmap;
	s_specifyserver.go.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_specifyserver.go.generic.x = 462;
	s_specifyserver.go.generic.y = 391;
	s_specifyserver.go.generic.name = GRAPHIC_SQUARE;
	s_specifyserver.go.generic.id = ID_SPECIFYSERVERGO;
	s_specifyserver.go.generic.callback = SpecifyServer_Event;
	s_specifyserver.go.width = 150;
	s_specifyserver.go.height = 42;
	s_specifyserver.go.color = CT_DKPURPLE1;
	s_specifyserver.go.color2 = CT_LTPURPLE1;
	s_specifyserver.go.textX = 5;
	s_specifyserver.go.textY = 2;
	s_specifyserver.go.textEnum = MBT_ENGAGEMULTIPLAYER;
	s_specifyserver.go.textcolor = CT_BLACK;
	s_specifyserver.go.textcolor2 = CT_WHITE;

	Menu_AddItem(&s_specifyserver.menu, &s_specifyserver.mainmenu);

	Menu_AddItem(&s_specifyserver.menu, &s_specifyserver.domain);
	Menu_AddItem(&s_specifyserver.menu, &s_specifyserver.port);
	Menu_AddItem(&s_specifyserver.menu, &s_specifyserver.go);
	Menu_AddItem(&s_specifyserver.menu, &s_specifyserver.back);

	Com_sprintf(s_specifyserver.port.field.buffer, 6, "%i", 27960);
	UI_LogFuncEnd();
}

/*
=================
SpecifyServer_Cache
=================
*/
void SpecifyServer_Cache(void)
{
	UI_LogFuncBegin();
	s_specifyserver.corner_ul = trap_R_RegisterShaderNoMip("menu/common/corner_ul_18_18.tga");
	s_specifyserver.corner_ur = trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18.tga");
	s_specifyserver.corner_ll = trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_18.tga");
	s_specifyserver.corner_lr = trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_18.tga");
	UI_LogFuncEnd();
}

/*
=================
UI_SpecifyServerMenu
=================
*/
void UI_SpecifyServerMenu(void)
{
	UI_LogFuncBegin();
	SpecifyServer_MenuInit();
	UI_PushMenu(&s_specifyserver.menu);
	UI_LogFuncEnd();
}
