#include "ui_local.h"
#include "ui_logger.h"

//===================================================================
//
// CDKey Menu
//
//===================================================================

static int32_t currentFont;

// menu action identifiers
enum ui_fontsIDs {
	ID_MAINMENU = 100,
	ID_CONTROLS,
	ID_VIDEO,
	ID_SOUND,
	ID_CDKEY,

	ID_FONT_SMALL = 301,
	ID_FONT_MEDIUM,
	ID_FONT_LARGE,
	ID_FONT_GRID
};

static menuframework_s	s_fonts_menu;
static menubitmap_s			s_fonts_small;
static menubitmap_s			s_fonts_medium;
static menubitmap_s			s_fonts_large;
static menubitmap_s			s_fonts_grid;

/*
=================
M_GridFont_Graphics
=================
*/
void M_GridFont_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y;
	int32_t i, i2, xInc;
	char	character[2];

	character[1] = 0;
	y = 190;
	for (i = 32; i < 256;)
	{
		x = 100;
		for (i2 = 0; i2<30; ++i2, ++i)
		{
			character[0] = i;
			UI_DrawString(x, y, character, UI_SMALLFONT, colorTable[CT_WHITE], qtrue);

			xInc = SMALLCHAR_WIDTH + 1;

			if (xInc > 0)
			{
				x += xInc;
			}

			if (i >= 256)
			{
				break;
			}
		}
		y += (SMALLCHAR_HEIGHT * 1.25);
	}
	UI_LogFuncEnd();
}

/*
=================
M_LargeFont_Graphics
=================
*/
void M_LargeFont_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y, color;
	int32_t i, i2, xInc;
	char	character[2];

	character[1] = 0;
	y = 190;
	color = CT_LTORANGE;
	for (i = 32; i < 256;)
	{
		x = 100;
		for (i2 = 0; i2<30; ++i2, ++i)
		{
			character[0] = i;
			UI_DrawProportionalString(x, y, character, UI_BIGFONT, colorTable[color]);
			xInc = UI_ProportionalStringWidth(character, UI_BIGFONT) + 1;

			if (xInc > 0)
			{
				x += xInc;
			}

			if (i >= 256)
			{
				break;
			}
		}
		y += 30;
	}
	UI_LogFuncEnd();
}

/*
=================
M_MediumFont_Graphics
=================
*/
void M_MediumFont_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y, color;
	int32_t i, i2, xInc;
	char	character[2];

	character[1] = 0;
	y = 190;
	color = CT_LTORANGE;
	for (i = 32; i < 256;)
	{
		x = 100;
		for (i2 = 0; i2<30; ++i2, ++i)
		{
			character[0] = i;
			UI_DrawProportionalString(x, y, character, UI_SMALLFONT, colorTable[color]);
			xInc = UI_ProportionalStringWidth(character, UI_SMALLFONT) + 1;

			if (xInc > 0)
			{
				x += xInc;
			}

			if (i >= 256)
			{
				break;
			}
		}
		y += 24;
	}
	UI_LogFuncEnd();
}

/*
=================
M_SmallFont_Graphics
=================
*/
void M_SmallFont_Graphics(void)
{
	UI_LogFuncBegin();
	int32_t x, y, color;
	int32_t i, i2, xInc;
	char	character[2];

	character[1] = 0;
	y = 190;
	color = CT_LTORANGE;
	for (i = 32; i < 256;)
	{
		x = 100;
		for (i2 = 0; i2<30; ++i2, ++i)
		{
			character[0] = i;
			UI_DrawProportionalString(x, y, character, UI_TINYFONT, colorTable[color]);
			xInc = UI_ProportionalStringWidth(character, UI_TINYFONT) + 1;

			if (xInc > 0)
			{
				x += xInc;
			}

			if (i >= 256)
			{
				break;
			}
		}
		y += 16;
	}
	UI_LogFuncEnd();
}


/*
=================
M_Fonts_Graphics
=================
*/
void M_Fonts_Graphics(void)
{
	UI_LogFuncBegin();
	UI_MenuFrame(&s_fonts_menu);

	UI_Setup_MenuButtons();

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Long left column square on bottom 3rd

	switch (currentFont)
	{
	case ID_FONT_SMALL:
		M_SmallFont_Graphics();
		break;
	case ID_FONT_MEDIUM:
		M_MediumFont_Graphics();
		break;
	case ID_FONT_LARGE:
		M_LargeFont_Graphics();
		break;

	case ID_FONT_GRID:
		M_GridFont_Graphics();
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
M_Fonts_MenuDraw
=================
*/
void M_Fonts_MenuDraw(void)
{
	UI_LogFuncBegin();
	M_Fonts_Graphics();

	Menu_Draw(&s_fonts_menu);
	UI_LogFuncEnd();
}

/*
=================
Fonts_MenuEvent
=================
*/
static void Fonts_MenuEvent(void* ptr, int32_t notification)
{
	UI_LogFuncBegin();
	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
	case ID_FONT_SMALL:
		currentFont = ID_FONT_SMALL;
		break;
	case ID_FONT_MEDIUM:
		currentFont = ID_FONT_MEDIUM;
		break;
	case ID_FONT_LARGE:
		currentFont = ID_FONT_LARGE;
		break;
	case ID_FONT_GRID:
		currentFont = ID_FONT_GRID;
		break;
	case ID_VIDEO:
		UI_PopMenu();
		UI_VideoDataMenu();
		return;
	case ID_CONTROLS:
		UI_PopMenu();
		//			UI_SetupWeaponsMenu();
		break;
	case ID_SOUND:
		UI_PopMenu();
		UI_SoundMenu();
		break;
	case ID_CDKEY:
		UI_PopMenu();
		UI_CDKeyMenu();
		break;
	case ID_MAINMENU:
		UI_PopMenu();
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
M_Fonts_MenuKey
=================
*/
static sfxHandle_t M_Fonts_MenuKey(int32_t key)
{
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return Menu_DefaultKey(&s_fonts_menu, key);
}


/*
===============
UI_FontsMenu_Cache
===============
*/
void UI_FontsMenu_Cache(void)
{
	UI_LogFuncBegin();
	UI_LogFuncEnd();
}


/*
===============
FontsMenu_Init
===============
*/
void FontsMenu_Init(void)
{
	UI_LogFuncBegin();
	UI_FontsMenu_Cache();

	s_fonts_menu.m_ItemCount = 0;
	s_fonts_menu.m_WrapAround = true;
	s_fonts_menu.OnDraw = M_Fonts_MenuDraw;
	s_fonts_menu.OnKey = M_Fonts_MenuKey;
	s_fonts_menu.m_Fullscreen = qtrue;
	s_fonts_menu.m_WrapAround = false;
	s_fonts_menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_fonts_menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CONTROLSMENU_TITLE };
	s_fonts_menu.m_FootNote = MNT_FONTS_DEVELOPER;

	SetupMenu_TopButtons(&s_fonts_menu, MENU_FONTS, NULL);

	s_fonts_small.generic.m_Type = EMenuItemType::Bitmap;
	s_fonts_small.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_fonts_small.generic.x = 100;
	s_fonts_small.generic.y = 170;
	s_fonts_small.generic.name = GRAPHIC_BUTTONRIGHT;
	s_fonts_small.generic.id = ID_FONT_SMALL;
	s_fonts_small.generic.callback = Fonts_MenuEvent;
	s_fonts_small.width = 100;
	s_fonts_small.height = MENU_BUTTON_MED_HEIGHT;
	s_fonts_small.color = CT_DKPURPLE1;
	s_fonts_small.color2 = CT_LTPURPLE1;
	s_fonts_small.textX = 5;
	s_fonts_small.textY = 2;
	s_fonts_small.textEnum = MBT_FONTSSMALL;
	s_fonts_small.textcolor = CT_BLACK;
	s_fonts_small.textcolor2 = CT_WHITE;

	s_fonts_medium.generic.m_Type = EMenuItemType::Bitmap;
	s_fonts_medium.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_fonts_medium.generic.x = 250;
	s_fonts_medium.generic.y = 170;
	s_fonts_medium.generic.name = GRAPHIC_BUTTONRIGHT;
	s_fonts_medium.generic.id = ID_FONT_MEDIUM;
	s_fonts_medium.generic.callback = Fonts_MenuEvent;
	s_fonts_medium.width = 100;
	s_fonts_medium.height = MENU_BUTTON_MED_HEIGHT;
	s_fonts_medium.color = CT_DKPURPLE1;
	s_fonts_medium.color2 = CT_LTPURPLE1;
	s_fonts_medium.textX = 5;
	s_fonts_medium.textY = 2;
	s_fonts_medium.textEnum = MBT_FONTSMEDIUM;
	s_fonts_medium.textcolor = CT_BLACK;
	s_fonts_medium.textcolor2 = CT_WHITE;

	s_fonts_large.generic.m_Type = EMenuItemType::Bitmap;
	s_fonts_large.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_fonts_large.generic.x = 400;
	s_fonts_large.generic.y = 170;
	s_fonts_large.generic.name = GRAPHIC_BUTTONRIGHT;
	s_fonts_large.generic.id = ID_FONT_LARGE;
	s_fonts_large.generic.callback = Fonts_MenuEvent;
	s_fonts_large.width = 100;
	s_fonts_large.height = MENU_BUTTON_MED_HEIGHT;
	s_fonts_large.color = CT_DKPURPLE1;
	s_fonts_large.color2 = CT_LTPURPLE1;
	s_fonts_large.textX = 5;
	s_fonts_large.textY = 2;
	s_fonts_large.textEnum = MBT_FONTSLARGE;
	s_fonts_large.textcolor = CT_BLACK;
	s_fonts_large.textcolor2 = CT_WHITE;

	s_fonts_grid.generic.m_Type = EMenuItemType::Bitmap;
	s_fonts_grid.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_fonts_grid.generic.x = 550;
	s_fonts_grid.generic.y = 170;
	s_fonts_grid.generic.name = GRAPHIC_BUTTONRIGHT;
	s_fonts_grid.generic.id = ID_FONT_GRID;
	s_fonts_grid.generic.callback = Fonts_MenuEvent;
	s_fonts_grid.width = 100;
	s_fonts_grid.height = MENU_BUTTON_MED_HEIGHT;
	s_fonts_grid.color = CT_DKPURPLE1;
	s_fonts_grid.color2 = CT_LTPURPLE1;
	s_fonts_grid.textX = 5;
	s_fonts_grid.textY = 2;
	s_fonts_grid.textEnum = MBT_FONTSGRID;
	s_fonts_grid.textcolor = CT_BLACK;
	s_fonts_grid.textcolor2 = CT_WHITE;

	Menu_AddItem(&s_fonts_menu, (void *)&s_fonts_small);
	Menu_AddItem(&s_fonts_menu, (void *)&s_fonts_medium);
	Menu_AddItem(&s_fonts_menu, (void *)&s_fonts_large);
	Menu_AddItem(&s_fonts_menu, (void *)&s_fonts_grid);

	currentFont = ID_FONT_SMALL;
	UI_LogFuncEnd();
}

/*
===============
UI_FontsMenu
===============
*/
void UI_FontsMenu(void)
{
	UI_LogFuncBegin();
	FontsMenu_Init();

	UI_PushMenu(&s_fonts_menu);
	UI_LogFuncEnd();
}

