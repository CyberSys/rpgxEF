// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

CD KEY MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_logger.h"

enum ui_cdleyIDs_e {
	ID_CDKEY = 10,
	ID_ACCEPT,
	ID_ACCEPTLATER
};


typedef struct {
	menuframework_s	menu;

	menufield_s		cdkey;

	menubitmap_s	accept;
	menubitmap_s	acceptlater;
	int32_t				keyResult;

	qhandle_t		halfround;
	int32_t				fromMenu;
} cdkeyMenuInfo_t;

static cdkeyMenuInfo_t	cdkeyMenuInfo;

/*
=================
UI_CDKeyMenu_PreValidateKey
=================
*/
static int32_t UI_CDKeyMenu_PreValidateKey(const char *key)
{
	UI_LogFuncBegin();
	int32_t		cnt, i;

	// With dashes can't be more than 21 long
	if (strlen(key) > 22)
	{
		UI_LogFuncEnd();
		return -1;
	}

	cnt = 0;
	for (i = 0; i < strlen(key) && key[i]; i++)
	{
		if ((key[i] >= '0' && key[i] <= '9')
			|| (key[i] >= 'a' && key[i] <= 'z')
			|| (key[i] >= 'A' && key[i] <= 'Z')
			)
		{
			cnt++;
		}
	}

	// Count without dashes (Should be 18 but 0's at the end weren't printed on the documentation so we're allowing 16 and adding the 0's in the keycheck)
	if ((cnt < 16) || (cnt > 18))
	{
		UI_LogFuncEnd();
		return -1;
	}
	UI_LogFuncEnd();
	return 0;
}


/*
===============
UI_CDKeyMenu_Event
===============
*/
static void UI_CDKeyMenu_Event(void *ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event != QM_ACTIVATED)
	{
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
	case ID_ACCEPT:

		cdkeyMenuInfo.keyResult = UI_CDKeyMenu_PreValidateKey(cdkeyMenuInfo.cdkey.field.buffer);

		if (cdkeyMenuInfo.keyResult == 0)
		{//passed quick check, now check for real
			if (trap_SetCDKey(cdkeyMenuInfo.cdkey.field.buffer))
			{//success
				trap_Cvar_Set("ui_cdkeychecked", "1");
				if (cdkeyMenuInfo.fromMenu)
				{
					UI_PopMenu();
				}
			}
			else
			{//failed real check
				cdkeyMenuInfo.keyResult = -1;
				trap_S_StartLocalSound(menu_buzz_sound, CHAN_MENU1);	// WRONG!!!! sound
			}
		}
		else if (cdkeyMenuInfo.keyResult < 0)
		{//failed ui quick check
			trap_S_StartLocalSound(menu_buzz_sound, CHAN_MENU1);	// WRONG!!!! sound
		}
		break;

	case ID_ACCEPTLATER:
		if (cdkeyMenuInfo.keyResult != 0) {	//not valid or not entered
			trap_Cvar_Set("ui_cdkeychecked", "-1");
		}
		UI_PopMenu();
		break;

	}
	UI_LogFuncEnd();
}


/*
=================
UI_CDKeyMenu_DrawKey
=================
*/
static void UI_CDKeyMenu_DrawKey(void *self)
{
	UI_LogFuncBegin();
	menufield_s		*f;
	qboolean		focus;
	int32_t				style;
	float			*color;
	int32_t				x, y;

	f = (menufield_s *)self;

	focus = static_cast<qboolean>(f->generic.parent->m_Cursor == f->generic.menuPosition);

	style = UI_LEFT;
	if (focus)
	{
		color = colorTable[CT_LTGOLD1];
	}
	else
	{
		color = colorTable[CT_DKGOLD1];
	}

	x = cdkeyMenuInfo.cdkey.generic.x;
	y = cdkeyMenuInfo.cdkey.generic.y;

	UI_FillRect(x, y, cdkeyMenuInfo.cdkey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, colorTable[CT_BLACK]);
	UI_FillRect(x, y, cdkeyMenuInfo.cdkey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, listbar_color);
	UI_DrawString(x, y, f->field.buffer, style, color, qtrue);

	// draw cursor if we have focus
	if (focus)
	{
		char			c;
		if (trap_Key_GetOverstrikeMode())
		{
			c = 11;
		}
		else
		{
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar(x + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white);
	}
	UI_LogFuncEnd();
}

/*
=================
CDKeyMenu_Graphics
=================
*/
void CDKeyMenu_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y;

	// Draw the basic screen layout
	UI_MenuFrame(&cdkeyMenuInfo.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Middle left line of frame

	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(246, 238, 197, 24, uis.whiteShader);
	UI_DrawHandlePic(227, 238, -16, 32, cdkeyMenuInfo.halfround);	// Left round
	UI_DrawHandlePic(446, 238, 16, 32, cdkeyMenuInfo.halfround);	// Right round

	UI_DrawProportionalString(74, 66, "557", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 84, "2344", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 188, "89-35", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "32906", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "30-1789", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	x = 344;
	y = 279;
	if (cdkeyMenuInfo.keyResult == 0)
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_VALID_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_WHITE]);
	}
	else if (cdkeyMenuInfo.keyResult == 1)
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_ENTER_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}
	else
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_CDKEY_INVALID], UI_CENTER | UI_SMALLFONT, colorTable[CT_RED]);
	}

	UI_Setup_MenuButtons();
	UI_LogFuncEnd();
}

/*
=================
CDKey_MenuDraw
=================
*/
static void CDKey_MenuDraw(void)
{
	UI_LogFuncBegin();
	CDKeyMenu_Graphics();
	Menu_Draw(&cdkeyMenuInfo.menu);
	UI_LogFuncEnd();
}

/*
===============
UI_CDKeyMenu_Init
===============
*/
static void UI_CDKeyMenu_Init(void)
{
	UI_LogFuncBegin();
	memset(&cdkeyMenuInfo, 0, sizeof(cdkeyMenuInfo));

	UI_CDKeyMenu_Cache();

	cdkeyMenuInfo.menu.m_WrapAround = true;
	cdkeyMenuInfo.menu.m_Fullscreen = qtrue;
	cdkeyMenuInfo.menu.OnDraw = CDKey_MenuDraw;
	cdkeyMenuInfo.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	cdkeyMenuInfo.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CDKEYMENU_TITLE };
	cdkeyMenuInfo.menu.m_FootNote = MNT_CDKEY;

	cdkeyMenuInfo.cdkey.generic.m_Type = EMenuItemType::Field;
	cdkeyMenuInfo.cdkey.generic.flags = QMF_UPPERCASE;
	cdkeyMenuInfo.cdkey.generic.x = 256;
	cdkeyMenuInfo.cdkey.generic.y = 242;
	cdkeyMenuInfo.cdkey.field.style = UI_SMALLFONT;
	cdkeyMenuInfo.cdkey.field.widthInChars = 22;
	cdkeyMenuInfo.cdkey.field.maxchars = 22;
	cdkeyMenuInfo.cdkey.generic.ownerdraw = UI_CDKeyMenu_DrawKey;
	cdkeyMenuInfo.cdkey.field.titleEnum = MNT_CDKEY;
	cdkeyMenuInfo.cdkey.field.titlecolor = CT_LTGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor = CT_DKGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor2 = CT_LTGOLD1;


	cdkeyMenuInfo.accept.generic.m_Type = EMenuItemType::Bitmap;
	cdkeyMenuInfo.accept.generic.name = GRAPHIC_SQUARE;
	cdkeyMenuInfo.accept.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.accept.generic.id = ID_ACCEPT;
	cdkeyMenuInfo.accept.generic.callback = UI_CDKeyMenu_Event;
	cdkeyMenuInfo.accept.generic.x = 279;
	cdkeyMenuInfo.accept.generic.y = 391;
	cdkeyMenuInfo.accept.width = MENU_BUTTON_MED_WIDTH;
	cdkeyMenuInfo.accept.height = 42;
	cdkeyMenuInfo.accept.color = CT_DKPURPLE1;
	cdkeyMenuInfo.accept.color2 = CT_LTPURPLE1;
	cdkeyMenuInfo.accept.textX = MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.accept.textY = MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.accept.textEnum = MBT_ACCEPT;
	cdkeyMenuInfo.accept.textcolor = CT_BLACK;
	cdkeyMenuInfo.accept.textcolor2 = CT_WHITE;


	SetupMenu_TopButtons(&cdkeyMenuInfo.menu, MENU_CDKEY, NULL);

	Menu_AddItem(&cdkeyMenuInfo.menu, &cdkeyMenuInfo.cdkey);
	Menu_AddItem(&cdkeyMenuInfo.menu, &cdkeyMenuInfo.accept);

	if (ui_cdkeychecked.integer == 1) {
		cdkeyMenuInfo.keyResult = 0; //already got a good one once
	}
	else {
		cdkeyMenuInfo.keyResult = 1;
	}
	UI_LogFuncEnd();
}


/*
=================
UI_CDKeyMenu_Cache
=================
*/
void UI_CDKeyMenu_Cache(void)
{
	UI_LogFuncBegin();
	cdkeyMenuInfo.halfround = trap_R_RegisterShaderNoMip("menu/common/halfround_r_24.tga");
	UI_LogFuncEnd();
}


/*
===============
UI_CDKeyMenu
===============
*/
void UI_CDKeyMenu(void)
{
	UI_LogFuncBegin();
	UI_CDKeyMenu_Init();
	UI_PushMenu(&cdkeyMenuInfo.menu);
	UI_LogFuncEnd();
}


/*
=================
M_CDKey2Menu_Key
=================
*/
sfxHandle_t M_CDKey2Menu_Key(int32_t key)
{
	UI_LogFuncBegin();
	if (key == K_ESCAPE)
	{
		UI_LogFuncEnd();
		return(0);
	}

	UI_LogFuncEnd();
	return (Menu_DefaultKey(&cdkeyMenuInfo.menu, key));
}

/*
=================
CDKeyMenu2_Graphics
=================
*/
void CDKeyMenu2_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y;

	// Draw the basic screen layout
	UI_MenuFrame2(&cdkeyMenuInfo.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Middle left line of frame

	// Background for CD Key data
	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(246, 168, 197, 24, uis.whiteShader);
	UI_DrawHandlePic(227, 168, -16, 32, cdkeyMenuInfo.halfround);	// Left round
	UI_DrawHandlePic(446, 168, 16, 32, cdkeyMenuInfo.halfround);	// Right round


	UI_DrawProportionalString(74, 36, "755", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 132, "4423", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 188, "35-89", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "60932", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "1789-30", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	x = 344;
	y = 228;
	if (cdkeyMenuInfo.keyResult == 0)
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_VALID_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_WHITE]);
	}
	else if (cdkeyMenuInfo.keyResult == 1)
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_ENTER_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}
	else
	{
		UI_DrawProportionalString(x, y, menu_normal_text[MNT_CDKEY_INVALID], UI_CENTER | UI_SMALLFONT, colorTable[CT_RED]);
	}
	UI_LogFuncEnd();
}

/*
=================
CDKey2_MenuDraw
=================
*/
static void CDKey2_MenuDraw(void)
{
	UI_LogFuncBegin();
	CDKeyMenu2_Graphics();
	Menu_Draw(&cdkeyMenuInfo.menu);
	UI_LogFuncEnd();
}


/*
===============
UI_CDKeyMenu2_Init
===============
*/
static void UI_CDKeyMenu2_Init(void)
{//initial first time menu
	UI_LogFuncBegin();
	memset(&cdkeyMenuInfo, 0, sizeof(cdkeyMenuInfo));

	cdkeyMenuInfo.fromMenu = 1;

	UI_CDKeyMenu_Cache();

	cdkeyMenuInfo.menu.m_WrapAround = true;
	cdkeyMenuInfo.menu.m_Fullscreen = qtrue;
	cdkeyMenuInfo.menu.OnDraw = CDKey2_MenuDraw;
	cdkeyMenuInfo.menu.OnKey = M_CDKey2Menu_Key;
	cdkeyMenuInfo.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	cdkeyMenuInfo.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CDKEYMENU_TITLE };
	cdkeyMenuInfo.menu.m_FootNote = MNT_CDKEY;

	cdkeyMenuInfo.cdkey.generic.m_Type = EMenuItemType::Field;
	cdkeyMenuInfo.cdkey.generic.flags = QMF_UPPERCASE;
	cdkeyMenuInfo.cdkey.generic.x = 260;
	cdkeyMenuInfo.cdkey.generic.y = 172;
	cdkeyMenuInfo.cdkey.field.style = UI_SMALLFONT;
	cdkeyMenuInfo.cdkey.field.widthInChars = 22;
	cdkeyMenuInfo.cdkey.field.maxchars = 22;
	cdkeyMenuInfo.cdkey.generic.ownerdraw = UI_CDKeyMenu_DrawKey;
	cdkeyMenuInfo.cdkey.field.titleEnum = MNT_CDKEY;
	cdkeyMenuInfo.cdkey.field.titlecolor = CT_LTGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor = CT_DKGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor2 = CT_LTGOLD1;

	cdkeyMenuInfo.accept.generic.m_Type = EMenuItemType::Bitmap;
	cdkeyMenuInfo.accept.generic.name = GRAPHIC_SQUARE;
	cdkeyMenuInfo.accept.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.accept.generic.id = ID_ACCEPT;
	cdkeyMenuInfo.accept.generic.callback = UI_CDKeyMenu_Event;
	cdkeyMenuInfo.accept.generic.x = 96;
	cdkeyMenuInfo.accept.generic.y = 347;
	cdkeyMenuInfo.accept.width = MENU_BUTTON_MED_WIDTH;
	cdkeyMenuInfo.accept.height = 42;
	cdkeyMenuInfo.accept.color = CT_DKPURPLE1;
	cdkeyMenuInfo.accept.color2 = CT_LTPURPLE1;
	cdkeyMenuInfo.accept.textX = MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.accept.textY = MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.accept.textEnum = MBT_ACCEPT;
	cdkeyMenuInfo.accept.textcolor = CT_BLACK;
	cdkeyMenuInfo.accept.textcolor2 = CT_WHITE;

	cdkeyMenuInfo.acceptlater.generic.m_Type = EMenuItemType::Bitmap;
	cdkeyMenuInfo.acceptlater.generic.name = GRAPHIC_SQUARE;
	cdkeyMenuInfo.acceptlater.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.acceptlater.generic.id = ID_ACCEPTLATER;
	cdkeyMenuInfo.acceptlater.generic.callback = UI_CDKeyMenu_Event;
	cdkeyMenuInfo.acceptlater.generic.x = 482;
	cdkeyMenuInfo.acceptlater.generic.y = 347;
	cdkeyMenuInfo.acceptlater.width = MENU_BUTTON_MED_WIDTH + 20;
	cdkeyMenuInfo.acceptlater.height = 42;
	cdkeyMenuInfo.acceptlater.color = CT_DKPURPLE1;
	cdkeyMenuInfo.acceptlater.color2 = CT_LTPURPLE1;
	cdkeyMenuInfo.acceptlater.textX = MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.acceptlater.textY = MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.acceptlater.textEnum = MBT_CHECKKEYLATER;
	cdkeyMenuInfo.acceptlater.textcolor = CT_BLACK;
	cdkeyMenuInfo.acceptlater.textcolor2 = CT_WHITE;

	Menu_AddItem(&cdkeyMenuInfo.menu, &cdkeyMenuInfo.cdkey);
	Menu_AddItem(&cdkeyMenuInfo.menu, &cdkeyMenuInfo.accept);
	Menu_AddItem(&cdkeyMenuInfo.menu, &cdkeyMenuInfo.acceptlater);

	if (ui_cdkeychecked.integer == 1) {
		cdkeyMenuInfo.keyResult = 0; //already got a good one once
	}
	else {
		cdkeyMenuInfo.keyResult = 1;
	}
	UI_LogFuncEnd();
}

/*
===============
UI_CDKeyMenu
===============
*/
void UI_CDKeyMenu2(void)
{
	UI_LogFuncBegin();
	UI_CDKeyMenu2_Init();
	UI_PushMenu(&cdkeyMenuInfo.menu);
	UI_LogFuncEnd();
}

/*
===============
UI_CDKeyMenu_f
===============
*/
void UI_CDKeyMenu_f(void)
{
	UI_LogFuncBegin();
	UI_CDKeyMenu2();
	UI_LogFuncEnd();
}
