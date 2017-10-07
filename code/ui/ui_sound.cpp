// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"
#include "ui_logger.h"


extern int32_t rate_items[];

// action identifiers
enum ui_soundIDs_e {
	ID_EFFECTSVOLUME = 14,
	ID_MUSICVOLUME,
	ID_QUALITY,
	ID_A3D,
	ID_RATE
};

static int32_t s_sndquality_Names[] =
{
	MNT_LOW,
	MNT_HIGH,
	MNT_NONE
};


extern int32_t s_OffOnNone_Names[];

/*
=======================================================================

SOUND OPTIONS MENU

=======================================================================
*/

typedef struct
{
	menuframework_s menu;

	qhandle_t corner;
	qhandle_t grid;
	qhandle_t wave1;
	qhandle_t wave2;

	menulist_s rate;
	menuslider_s sfxvolume;
	menuslider_s musicvolume;
	menulist_s quality;
	menulist_s a3d;

	menubitmap_s back;
	int32_t holdSoundQuality;
	int32_t a3dtimer;
} soundOptionsInfo_t;

static soundOptionsInfo_t	soundOptionsInfo;

static void SoundQualityAction(qboolean result)
{
	UI_LogFuncBegin();
	if (result)
	{
		soundOptionsInfo.holdSoundQuality = soundOptionsInfo.quality.curvalue;
		if (soundOptionsInfo.quality.curvalue)
		{
			trap_Cvar_SetValue("s_khz", 22);
			//			trap_Cvar_SetValue( "s_compression", 0 );
		}
		else
		{
			trap_Cvar_SetValue("s_khz", 11);
			//			trap_Cvar_SetValue( "s_compression", 1 );
		}
		//UI_ForceMenuOff();
		trap_Cmd_ExecuteText(EXEC_APPEND, "snd_restart\n");
	}
	else
	{
		soundOptionsInfo.quality.curvalue = soundOptionsInfo.holdSoundQuality;
	}
	UI_LogFuncEnd();
}

/*
=================
UI_SoundOptionsMenu_Event
=================
*/

static void UI_SoundOptionsMenu_Event(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	int32_t holdCurvalue;

	if (event != QM_ACTIVATED)
	{
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
	case ID_RATE:
		if (soundOptionsInfo.rate.curvalue == 0)
		{
			trap_Cvar_SetValue("rate", 2500);
		}
		else if (soundOptionsInfo.rate.curvalue == 1)
		{
			trap_Cvar_SetValue("rate", 3000);
		}
		else if (soundOptionsInfo.rate.curvalue == 2)
		{
			trap_Cvar_SetValue("rate", 4000);
		}
		else if (soundOptionsInfo.rate.curvalue == 3)
		{
			trap_Cvar_SetValue("rate", 5000);
		}
		else if (soundOptionsInfo.rate.curvalue == 4)
		{
			trap_Cvar_SetValue("rate", 25000);
		}
		break;

	case ID_EFFECTSVOLUME:
		trap_Cvar_SetValue("s_volume", soundOptionsInfo.sfxvolume.curvalue / 10);
		break;

	case ID_MUSICVOLUME:
		trap_Cvar_SetValue("s_musicvolume", soundOptionsInfo.musicvolume.curvalue / 10);
		break;

	case ID_QUALITY:
		//UI_ConfirmMenu(menu_normal_text[MNT_THISWILLRESETMENU],0,SoundQualityAction);
		SoundQualityAction(qtrue);
		break;

	case ID_A3D:
		if (soundOptionsInfo.a3d.curvalue) {
			trap_Cmd_ExecuteText(EXEC_NOW, "s_enable_a3d\n");
		}
		else {
			trap_Cmd_ExecuteText(EXEC_NOW, "s_disable_a3d\n");
		}

		holdCurvalue = (int32_t)trap_Cvar_VariableValue("s_usingA3D");
		if ((soundOptionsInfo.a3d.curvalue == 1) && (holdCurvalue != 1))
		{
			soundOptionsInfo.a3dtimer = uis.realtime + 10000;
		}
		soundOptionsInfo.a3d.curvalue = holdCurvalue;

		break;
	}
	UI_LogFuncEnd();
}



/*
=================
M_Sound_Graphics
=================
*/
void M_Sound_Graphics(void)
{
	UI_LogFuncBegin();
	UI_MenuFrame(&soundOptionsInfo.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);		// Long left column square on bottom 3rd

	// Network Frame
	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(88, 164, 524, 18, uis.whiteShader);		// Top line
	UI_DrawHandlePic(88, 182, 36, 39, uis.whiteShader);		// Left column
	UI_DrawProportionalString(130, 165, menu_normal_text[MNT_NETWORK], UI_SMALLFONT, colorTable[CT_BLACK]);	// Top

	if (soundOptionsInfo.a3dtimer > uis.realtime)
	{
		UI_DrawProportionalString(490, 305, menu_normal_text[MNT_A3DNOTAVAILABLE], UI_CENTER | UI_SMALLFONT, colorTable[CT_WHITE]);
	}

	// Sound Frame
	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(88, 228, 524, 18, uis.whiteShader);		// Top line
	UI_DrawHandlePic(88, 246, 36, 27, uis.whiteShader);		//	Left column
	UI_DrawHandlePic(88, 277, 36, 25, uis.whiteShader);		// Left column
	UI_DrawProportionalString(130, 229, menu_normal_text[MNT_SOUND], UI_SMALLFONT, colorTable[CT_BLACK]);	// Top

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(112, 303, 500, 4, uis.whiteShader);		// Middle long line
	UI_DrawHandlePic(88, 300, 64, 8, soundOptionsInfo.corner);	// Corner graphic

	trap_R_SetColor(colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(356, 342, 8, 93, uis.whiteShader);		// Middle Bottom Left column
	trap_R_SetColor(colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(98, 359, 248, 64, soundOptionsInfo.grid);
	trap_R_SetColor(colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(98, 359, 248, 64, soundOptionsInfo.wave1);
	trap_R_SetColor(colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(96, 359, 4, 64, uis.whiteShader);		// Left side of frame
	UI_DrawHandlePic(342, 359, 4, 64, uis.whiteShader);		// Right side of frame
	UI_DrawHandlePic(96, 355, 250, 4, uis.whiteShader);		// Top of frame
	UI_DrawHandlePic(96, 423, 250, 4, uis.whiteShader);		// Bottom of frame


	UI_DrawHandlePic(367, 342, 8, 93, uis.whiteShader);		// Middle Bottom Right column
	trap_R_SetColor(colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(392, 359, 220, 64, soundOptionsInfo.grid);
	trap_R_SetColor(colorTable[CT_LTRED1]);
	UI_DrawHandlePic(392, 359, 220, 64, soundOptionsInfo.wave2);
	trap_R_SetColor(colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(392, 359, 4, 64, uis.whiteShader);		// Left side of frame
	UI_DrawHandlePic(608, 359, 4, 64, uis.whiteShader);		// Right side of frame
	UI_DrawHandlePic(392, 355, 220, 4, uis.whiteShader);		// Top of frame
	UI_DrawHandlePic(392, 423, 220, 4, uis.whiteShader);		// Bottom of frame

	UI_Setup_MenuButtons();

	UI_DrawProportionalString(74, 66, "29876", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 84, "568", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 188, "45-009", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "18856", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "18857", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_LogFuncEnd();
}

/*
=================
M_Sound_MenuDraw
=================
*/
void M_Sound_MenuDraw(void)
{
	UI_LogFuncBegin();
	M_Sound_Graphics();

	Menu_Draw(&soundOptionsInfo.menu);
	UI_LogFuncEnd();
}


/*
=================
M_Sound_MenuKey
=================
*/
static sfxHandle_t M_Sound_MenuKey(int32_t key)
{
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return Menu_DefaultKey(&soundOptionsInfo.menu, key);
}


/*
===============
SoundMenu_Cache
===============
*/
void UI_SoundMenu_Cache(void)
{
	UI_LogFuncBegin();
	soundOptionsInfo.corner = trap_R_RegisterShaderNoMip("menu/common/con_ssetup.tga");
	soundOptionsInfo.grid = trap_R_RegisterShaderNoMip("menu/special/grid.tga");
	soundOptionsInfo.wave1 = trap_R_RegisterShaderNoMip("menu/special/sinwave_1.tga");
	soundOptionsInfo.wave2 = trap_R_RegisterShaderNoMip("menu/special/sinwave_2.tga");
	UI_LogFuncEnd();
}


/*
===============
SoundMenu_Init
===============
*/
void SoundMenu_Init(void)
{
	UI_LogFuncBegin();
	int32_t x, y;
	int32_t	rate;

	UI_SoundMenu_Cache();

	soundOptionsInfo.menu.m_ItemCount = 0;
	soundOptionsInfo.menu.m_WrapAround = true;
	soundOptionsInfo.menu.OnDraw = M_Sound_MenuDraw;
	soundOptionsInfo.menu.OnKey = M_Sound_MenuKey;
	soundOptionsInfo.menu.m_Fullscreen = qtrue;
	soundOptionsInfo.menu.m_WrapAround = false;
	soundOptionsInfo.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	soundOptionsInfo.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CONTROLSMENU_TITLE };
	soundOptionsInfo.menu.m_FootNote = MNT_SOUND_SETUP;

	x = 212;
	soundOptionsInfo.rate.generic.m_Type = EMenuItemType::SpinControl;
	soundOptionsInfo.rate.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	soundOptionsInfo.rate.generic.x = x;
	soundOptionsInfo.rate.generic.y = 192;
	soundOptionsInfo.rate.textEnum = MBT_DATARATE;
	soundOptionsInfo.rate.generic.callback = UI_SoundOptionsMenu_Event;
	soundOptionsInfo.rate.generic.id = ID_RATE;
	soundOptionsInfo.rate.textcolor = CT_BLACK;
	soundOptionsInfo.rate.textcolor2 = CT_WHITE;
	soundOptionsInfo.rate.color = CT_DKPURPLE1;
	soundOptionsInfo.rate.color2 = CT_LTPURPLE1;
	soundOptionsInfo.rate.textX = 5;
	soundOptionsInfo.rate.textY = 2;
	soundOptionsInfo.rate.listnames = rate_items;

	y = 256;
	soundOptionsInfo.sfxvolume.generic.m_Type = EMenuItemType::Slider;
	soundOptionsInfo.sfxvolume.generic.x = x + MENU_BUTTON_MED_WIDTH + 35;
	soundOptionsInfo.sfxvolume.generic.y = y;
	soundOptionsInfo.sfxvolume.generic.flags = QMF_SMALLFONT;
	soundOptionsInfo.sfxvolume.generic.callback = UI_SoundOptionsMenu_Event;
	soundOptionsInfo.sfxvolume.generic.id = ID_EFFECTSVOLUME;
	soundOptionsInfo.sfxvolume.minvalue = 0;
	soundOptionsInfo.sfxvolume.maxvalue = 10;
	soundOptionsInfo.sfxvolume.color = CT_DKPURPLE1;
	soundOptionsInfo.sfxvolume.color2 = CT_LTPURPLE1;
	soundOptionsInfo.sfxvolume.generic.name = "menu/common/monbar_2.tga";
	soundOptionsInfo.sfxvolume.width = 256;
	soundOptionsInfo.sfxvolume.height = 32;
	soundOptionsInfo.sfxvolume.focusWidth = 145;
	soundOptionsInfo.sfxvolume.focusHeight = 18;
	soundOptionsInfo.sfxvolume.picName = "menu/common/square.tga";
	soundOptionsInfo.sfxvolume.picX = x;
	soundOptionsInfo.sfxvolume.picY = y;
	soundOptionsInfo.sfxvolume.picWidth = MENU_BUTTON_MED_WIDTH;
	soundOptionsInfo.sfxvolume.picHeight = MENU_BUTTON_MED_HEIGHT;
	soundOptionsInfo.sfxvolume.textEnum = MBT_EFFECTSVOLUME;
	soundOptionsInfo.sfxvolume.textX = 5;
	soundOptionsInfo.sfxvolume.textY = 1;
	soundOptionsInfo.sfxvolume.textcolor = CT_BLACK;
	soundOptionsInfo.sfxvolume.textcolor2 = CT_WHITE;
	soundOptionsInfo.sfxvolume.thumbName = "menu/common/slider.tga";
	soundOptionsInfo.sfxvolume.thumbHeight = 32;
	soundOptionsInfo.sfxvolume.thumbWidth = 16;
	soundOptionsInfo.sfxvolume.thumbGraphicWidth = 9;
	soundOptionsInfo.sfxvolume.thumbColor = CT_DKBLUE1;
	soundOptionsInfo.sfxvolume.thumbColor2 = CT_LTBLUE1;

	y = 278;
	soundOptionsInfo.musicvolume.generic.m_Type = EMenuItemType::Slider;
	soundOptionsInfo.musicvolume.generic.x = x + MENU_BUTTON_MED_WIDTH + 35;
	soundOptionsInfo.musicvolume.generic.y = y;
	soundOptionsInfo.musicvolume.generic.flags = QMF_SMALLFONT;
	soundOptionsInfo.musicvolume.generic.callback = UI_SoundOptionsMenu_Event;
	soundOptionsInfo.musicvolume.generic.id = ID_MUSICVOLUME;
	soundOptionsInfo.musicvolume.minvalue = 0;
	soundOptionsInfo.musicvolume.maxvalue = 10;
	soundOptionsInfo.musicvolume.color = CT_DKPURPLE1;
	soundOptionsInfo.musicvolume.color2 = CT_LTPURPLE1;
	soundOptionsInfo.musicvolume.generic.name = "menu/common/monbar_2.tga";
	soundOptionsInfo.musicvolume.width = 256;
	soundOptionsInfo.musicvolume.height = 32;
	soundOptionsInfo.musicvolume.focusWidth = 145;
	soundOptionsInfo.musicvolume.focusHeight = 18;
	soundOptionsInfo.musicvolume.picName = "menu/common/square.tga";
	soundOptionsInfo.musicvolume.picX = x;
	soundOptionsInfo.musicvolume.picY = y;
	soundOptionsInfo.musicvolume.picWidth = MENU_BUTTON_MED_WIDTH;
	soundOptionsInfo.musicvolume.picHeight = MENU_BUTTON_MED_HEIGHT;
	soundOptionsInfo.musicvolume.textEnum = MBT_MUSICVOLUME;
	soundOptionsInfo.musicvolume.textX = 5;
	soundOptionsInfo.musicvolume.textY = 1;
	soundOptionsInfo.musicvolume.textcolor = CT_BLACK;
	soundOptionsInfo.musicvolume.textcolor2 = CT_WHITE;
	soundOptionsInfo.musicvolume.thumbName = "menu/common/slider.tga";
	soundOptionsInfo.musicvolume.thumbHeight = 32;
	soundOptionsInfo.musicvolume.thumbWidth = 16;
	soundOptionsInfo.musicvolume.thumbGraphicWidth = 9;
	soundOptionsInfo.musicvolume.thumbColor = CT_DKBLUE1;
	soundOptionsInfo.musicvolume.thumbColor2 = CT_LTBLUE1;

	soundOptionsInfo.quality.generic.m_Type = EMenuItemType::SpinControl;
	soundOptionsInfo.quality.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	soundOptionsInfo.quality.generic.x = 120;
	soundOptionsInfo.quality.generic.y = 322;
	soundOptionsInfo.quality.generic.callback = UI_SoundOptionsMenu_Event;
	soundOptionsInfo.quality.generic.id = ID_QUALITY;
	soundOptionsInfo.quality.textEnum = MBT_SOUNDQUALITY;
	soundOptionsInfo.quality.textcolor = CT_BLACK;
	soundOptionsInfo.quality.textcolor2 = CT_WHITE;
	soundOptionsInfo.quality.color = CT_DKPURPLE1;
	soundOptionsInfo.quality.color2 = CT_LTPURPLE1;
	soundOptionsInfo.quality.textX = 5;
	soundOptionsInfo.quality.textY = 2;
	soundOptionsInfo.quality.listnames = s_sndquality_Names;

	soundOptionsInfo.a3d.generic.m_Type = EMenuItemType::SpinControl;
	soundOptionsInfo.a3d.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	soundOptionsInfo.a3d.generic.x = 416;
	soundOptionsInfo.a3d.generic.y = 322;
	soundOptionsInfo.a3d.generic.callback = UI_SoundOptionsMenu_Event;
	soundOptionsInfo.a3d.generic.id = ID_A3D;
	soundOptionsInfo.a3d.textEnum = MBT_A3D;
	soundOptionsInfo.a3d.textcolor = CT_BLACK;
	soundOptionsInfo.a3d.textcolor2 = CT_WHITE;
	soundOptionsInfo.a3d.color = CT_DKPURPLE1;
	soundOptionsInfo.a3d.color2 = CT_LTPURPLE1;
	soundOptionsInfo.a3d.textX = 5;
	soundOptionsInfo.a3d.textY = 2;
	soundOptionsInfo.a3d.listnames = s_OffOnNone_Names;

	SetupMenu_TopButtons(&soundOptionsInfo.menu, MENU_SOUND, NULL);

	Menu_AddItem(&soundOptionsInfo.menu, (void *)&soundOptionsInfo.rate);
	Menu_AddItem(&soundOptionsInfo.menu, (void *)&soundOptionsInfo.sfxvolume);
	Menu_AddItem(&soundOptionsInfo.menu, (void *)&soundOptionsInfo.musicvolume);
	Menu_AddItem(&soundOptionsInfo.menu, (void *)&soundOptionsInfo.quality);
	Menu_AddItem(&soundOptionsInfo.menu, (void *)&soundOptionsInfo.a3d);

	soundOptionsInfo.sfxvolume.curvalue = trap_Cvar_VariableValue("s_volume") * 10;
	soundOptionsInfo.musicvolume.curvalue = trap_Cvar_VariableValue("s_musicvolume") * 10;
	soundOptionsInfo.quality.curvalue = trap_Cvar_VariableValue("s_khz") != 11;
	soundOptionsInfo.holdSoundQuality = soundOptionsInfo.quality.curvalue;
	soundOptionsInfo.a3d.curvalue = (int32_t)trap_Cvar_VariableValue("s_usingA3D");

	soundOptionsInfo.menu.m_Initialized = qtrue;


	rate = trap_Cvar_VariableValue("rate");
	if (rate <= 2500)
	{
		soundOptionsInfo.rate.curvalue = 0;
	}
	else if (rate <= 3000)
	{
		soundOptionsInfo.rate.curvalue = 1;
	}
	else if (rate <= 4000)
	{
		soundOptionsInfo.rate.curvalue = 2;
	}
	else if (rate <= 5000)
	{
		soundOptionsInfo.rate.curvalue = 3;
	}
	else
	{
		soundOptionsInfo.rate.curvalue = 4;
	}
	UI_LogFuncEnd();
}

/*
===============
UI_SoundMenu
===============
*/
void UI_SoundMenu(void)
{
	UI_LogFuncBegin();
	SoundMenu_Init();

	UI_PushMenu(&soundOptionsInfo.menu);
	UI_LogFuncEnd();
}

