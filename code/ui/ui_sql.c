/**********************************************************************
	UI_sql.C

	User interface trigger from within game
**********************************************************************/

#include "ui_local.h"

typedef struct {
	menuframework_s menu;
	menubitmap_s quitmenu;
	menubitmap_s login;

	char *username;
	char *password;
} sql_t;

sql_t s_sql;

#define ID_QUIT 10
#define ID_LOGIN 11

/*
=================
M_sql_Event
=================
*/
static void M_sql_Event(void *ptr, int notification) {
	int id;

	id = ((menucommon_s*)ptr)->id;

	switch (id)
	{
	case ID_QUIT:
		if(notification == QM_ACTIVATED) {
			UI_PopMenu();
		}
		break;
	case ID_LOGIN:
		if(notification == QM_ACTIVATED) {
			// do login
		}
		break;
	default:
		break;
	}
}

/*
=================
sqlMenu_Key
=================
*/
sfxHandle_t sqlMenu_Key (int key)
{
	return ( Menu_DefaultKey( &s_sql.menu, key ) );
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;


/*
=================
M_sqlMenu_Graphics
=================
*/
static void M_sqlMenu_Graphics (void)
{
	// TODO
}

/*
===============
UI_sqlMenu_Cache
===============
*/
void UI_sqlMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
}

/*
===============
sqlMenu_Draw
===============
*/
static void sqlMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_sqlMenu_Graphics();
	
	Menu_Draw( &s_sql.menu );
}

/*
===============
msdMenu_Init
===============
*/
void sqlMenu_Init(void)
{
	s_sql.menu.nitems				= 0;
	s_sql.menu.draw					= sqlMenu_Draw;
	s_sql.menu.key					= sqlMenu_Key;
	s_sql.menu.wrapAround			= qtrue;
	s_sql.menu.descX				= MENU_DESC_X;
	s_sql.menu.descY				= MENU_DESC_Y;
	s_sql.menu.titleX				= MENU_TITLE_X;
	s_sql.menu.titleY				= MENU_TITLE_Y;
	s_sql.menu.fullscreen			= qfalse;
	s_sql.menu.nobackground			= qtrue;

	s_sql.quitmenu.generic.type		= MTYPE_BITMAP;
	s_sql.quitmenu.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_sql.quitmenu.generic.x		= 490;
	s_sql.quitmenu.generic.y		= 380;
	s_sql.quitmenu.generic.name		= GRAPHIC_BUTTONLEFT;
	s_sql.quitmenu.generic.id		= ID_QUIT;
	s_sql.quitmenu.generic.callback	= M_sql_Event;
	s_sql.quitmenu.width			= MENU_BUTTON_MED_WIDTH-20;
	s_sql.quitmenu.height			= PROP_BIG_HEIGHT;
	s_sql.quitmenu.color			= CT_DKPURPLE1;
	s_sql.quitmenu.color2			= CT_LTPURPLE1;
	s_sql.quitmenu.textX			= MENU_BUTTON_TEXT_X;
	s_sql.quitmenu.textY			= 12;
	s_sql.quitmenu.textEnum			= MBT_QUIT;
	s_sql.quitmenu.textcolor		= CT_BLACK;
	s_sql.quitmenu.textcolor2		= CT_WHITE;
	s_sql.quitmenu.textStyle		= UI_TINYFONT;

	s_sql.login.generic.type		= MTYPE_BITMAP;
	s_sql.login.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_sql.login.generic.x			= 350;
	s_sql.login.generic.y			= 380;
	s_sql.login.generic.name		= GRAPHIC_BUTTONRIGHT;
	s_sql.login.generic.id			= ID_LOGIN;
	s_sql.login.generic.callback	= M_sql_Event;
	s_sql.login.width				= MENU_BUTTON_MED_WIDTH-20;
	s_sql.login.height				= PROP_BIG_HEIGHT;
	s_sql.login.color				= CT_DKPURPLE1;
	s_sql.login.color2				= CT_LTPURPLE1;
	s_sql.login.textX				= MENU_BUTTON_TEXT_X;
	s_sql.login.textY				= 12;
	s_sql.login.textEnum			= MBT_ACCEPT;
	s_sql.login.textcolor			= CT_BLACK;
	s_sql.login.textcolor2			= CT_WHITE;
	s_sql.login.textStyle			= UI_TINYFONT;

	Menu_AddItem( &s_sql.menu, &s_sql.quitmenu );
	Menu_AddItem( &s_sql.menu, &s_sql.login );
}

/*
===============
UI_sqlMenu
===============
*/
void UI_sqlMenu(void) {
	memset( &s_sql, 0, sizeof( s_sql ) );

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_sqlMenu_Cache();

	sqlMenu_Init(); 

	UI_PushMenu( &s_sql.menu );

	Menu_AdjustCursor( &s_sql.menu, 1 );	
}