/**********************************************************************
	UI_sql.C

	User interface trigger from within game
	**********************************************************************/

#include "ui_local.h"
#include "ui_logger.h"

typedef struct {
	menuframework_s menu;
	menubitmap_s quitmenu;
	menubitmap_s login;
	menufield_s user;
	menufield_s password;

	char* sUsername;
	char* sPassword;
} sql_t;

sql_t s_sql;

enum ui_sqlIDs_e {
	ID_QUIT = 10,
	ID_LOGIN
};

/*
=================
M_sql_Event
=================
*/
static void M_sql_Event(void *ptr, int32_t notification) {
	int32_t id;

	UI_LogFuncBegin();

	id = ((menucommon_s*)ptr)->id;

	switch (id)
	{
	case ID_QUIT:
		if (notification == QM_ACTIVATED) {
			UI_PopMenu();
		}
		break;
	case ID_LOGIN:
		if (notification == QM_ACTIVATED) {
			// do login
		}
		break;
	default:
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
sqlMenu_Key
=================
*/
sfxHandle_t sqlMenu_Key(int32_t key)
{
	return (Menu_DefaultKey(&s_sql.menu, key));
}

qhandle_t box_rounded;
qhandle_t corner_lu;


/*
=================
M_sqlMenu_Graphics
=================
*/
static void M_sqlMenu_Graphics(void)
{
	UI_LogFuncBegin();
	float bg[] = { 0, 0, 0, 0.7 };

	trap_R_SetColor(bg);
	UI_DrawHandlePic(160, 120, 320, 240, box_rounded);

	trap_R_SetColor(colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(164, 124, 64, 64, corner_lu);
	UI_DrawHandlePic(412, 124, -64, 64, corner_lu);
	UI_DrawHandlePic(164, 292, 64, -64, corner_lu);
	UI_DrawHandlePic(412, 292, -64, -64, corner_lu);
	UI_DrawHandlePic(232, 124, 176, 31.75, uis.whiteShader);
	UI_DrawHandlePic(232, 324, 176, 31.75, uis.whiteShader);
	UI_DrawHandlePic(164, 192, 31.75, 96, uis.whiteShader);
	UI_DrawHandlePic(444, 192, 31.75, 96, uis.whiteShader);
	UI_DrawString(234, 132, "LCARS AUTH", UI_LEFT | UI_SMALLFONT, colorTable[CT_BLACK], qtrue); // TODO multi lang text

	// TODO
	UI_LogFuncEnd();
}

/*
===============
UI_sqlMenu_Cache
===============
*/
void UI_sqlMenu_Cache(void)
{
	UI_LogFuncBegin();
	box_rounded = trap_R_RegisterShaderNoMip("menu/common/box_rounded.tga");
	corner_lu = trap_R_RegisterShaderNoMip("menu/common/corner_lu.tga");
	UI_LogFuncEnd();
}

/*
===============
sqlMenu_Draw
===============
*/
static void sqlMenu_Draw(void)
{
	UI_LogFuncBegin();
	// Draw graphics particular to Main Menu
	M_sqlMenu_Graphics();

	Menu_Draw(&s_sql.menu);
	UI_LogFuncEnd();
}

/*
===============
msdMenu_Init
===============
*/
void sqlMenu_Init(void)
{
	UI_LogFuncBegin();
	s_sql.menu.m_ItemCount = 0;
	s_sql.menu.OnDraw = sqlMenu_Draw;
	s_sql.menu.OnKey = sqlMenu_Key;
	s_sql.menu.m_WrapAround = true;
	s_sql.menu.m_DescriptionPosition = { -400, -400 };
	s_sql.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y } };
	s_sql.menu.m_Fullscreen = qfalse;
	s_sql.menu.m_NoBackground = qtrue;

	s_sql.quitmenu.generic.m_Type = EMenuItemType::Bitmap;
	s_sql.quitmenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_sql.quitmenu.generic.x = 490;
	s_sql.quitmenu.generic.y = 380;
	s_sql.quitmenu.generic.name = GRAPHIC_BUTTONLEFT;
	s_sql.quitmenu.generic.id = ID_QUIT;
	s_sql.quitmenu.generic.callback = M_sql_Event;
	s_sql.quitmenu.width = MENU_BUTTON_MED_WIDTH - 20;
	s_sql.quitmenu.height = PROP_BIG_HEIGHT;
	s_sql.quitmenu.color = CT_DKPURPLE1;
	s_sql.quitmenu.color2 = CT_LTPURPLE1;
	s_sql.quitmenu.textX = MENU_BUTTON_TEXT_X;
	s_sql.quitmenu.textY = 12;
	s_sql.quitmenu.textEnum = MBT_QUIT; // TODO needs own text
	s_sql.quitmenu.textcolor = CT_BLACK;
	s_sql.quitmenu.textcolor2 = CT_WHITE;
	s_sql.quitmenu.textStyle = UI_TINYFONT;

	s_sql.login.generic.m_Type = EMenuItemType::Bitmap;
	s_sql.login.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_sql.login.generic.x = 350;
	s_sql.login.generic.y = 380;
	s_sql.login.generic.name = GRAPHIC_BUTTONRIGHT;
	s_sql.login.generic.id = ID_LOGIN;
	s_sql.login.generic.callback = M_sql_Event;
	s_sql.login.width = MENU_BUTTON_MED_WIDTH - 20;
	s_sql.login.height = PROP_BIG_HEIGHT;
	s_sql.login.color = CT_DKPURPLE1;
	s_sql.login.color2 = CT_LTPURPLE1;
	s_sql.login.textX = MENU_BUTTON_TEXT_X;
	s_sql.login.textY = 12;
	s_sql.login.textEnum = MBT_ACCEPT; // TODO needs own text
	s_sql.login.textcolor = CT_BLACK;
	s_sql.login.textcolor2 = CT_WHITE;
	s_sql.login.textStyle = UI_TINYFONT;

	s_sql.user.generic.m_Type = EMenuItemType::Field;
	s_sql.user.field.widthInChars = 16;
	s_sql.user.field.maxchars = 64;
	s_sql.user.generic.x = 260;
	s_sql.user.generic.y = 217;
	s_sql.user.field.style = UI_SMALLFONT;
	s_sql.user.field.titleEnum = MBT_ADMIN_ENTITY; // TODO ... needs own text
	s_sql.user.field.titlecolor = CT_WHITE;
	s_sql.user.field.textcolor = CT_WHITE;
	s_sql.user.field.textcolor2 = CT_LTGREY;

	Menu_AddItem(&s_sql.menu, &s_sql.quitmenu);
	Menu_AddItem(&s_sql.menu, &s_sql.login);
	Menu_AddItem(&s_sql.menu, &s_sql.user);
	UI_LogFuncEnd();
}

/*
===============
UI_sqlMenu
===============
*/
void UI_sqlMenu(void) {
	UI_LogFuncBegin();
	memset(&s_sql, 0, sizeof(s_sql));

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_sqlMenu_Cache();

	sqlMenu_Init();

	UI_PushMenu(&s_sql.menu);

	Menu_AdjustCursor(&s_sql.menu, 1);
	UI_LogFuncEnd();
}