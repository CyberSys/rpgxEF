// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"
#include "cg_text.h"


#define	SCOREBOARD_X		(37) //Before RPG-X: (13) //(53)

//Header info of scoreboard
#define SB_HEADER			40 //Before RPG-X: 105 //40
#define SB_TOP				(SB_HEADER+32)
#define SB_HEADERTEXT		(SB_HEADER + 8)
#define SB_TOPLINE_LENGTH	550 //Before RPG-X: 590 //530

// Where the status bar starts, so we don't overwrite it
#define SB_STATUSBAR		420

//Scoreboard height
#define SB_NORMAL_HEIGHT_BIG	14 //RPG-X: RedTechie - Spacing between names
#define SB_NORMAL_HEIGHT		22
#define SB_INTER_HEIGHT			16 //interleaved height
#define SB_RPG_X_FIXHEIGHT		350 //RPG-X: RedTechie - Fixed background height 350 before new health bar

#define SB_MAXCLIENTS_BIG	  ((SB_STATUSBAR - SB_TOP) / SB_NORMAL_HEIGHT_BIG)
#define SB_MAXCLIENTS_NORMAL  ((SB_STATUSBAR - SB_TOP) / SB_NORMAL_HEIGHT)
#define SB_MAXCLIENTS_INTER   ((SB_STATUSBAR - SB_TOP) / SB_INTER_HEIGHT - 1)

// Used when interleaved
#define SB_LEFT_BOTICON_X	(SCOREBOARD_X+0)
#define SB_LEFT_HEAD_X		(SCOREBOARD_X+12)
#define SB_RIGHT_BOTICON_X	(SCOREBOARD_X+64)
#define SB_RIGHT_HEAD_X		(SCOREBOARD_X+96)

// Normal
#define SB_BOTICON_X		(SCOREBOARD_X+10) //Before RPG-X - This is for positioning of class icon: (SCOREBOARD_X+20)
#define SB_HEAD_X			(SCOREBOARD_X+10) //Before RPG-X - This is for positioning of head icon: (SCOREBOARD_X+50)

//Intermission text alignment
#define SB_SCORELINE_X		(SCOREBOARD_X + 76)
#define	SB_CHAR_WIDTH		8
#define SB_NAME_X			(SB_SCORELINE_X + 12) 
#define SB_SCORE_X			(SB_SCORELINE_X + 112) 
#define SB_KILLEDCNT_X		(SB_SCORELINE_X + 155) 
#define SB_TIME_X			(SB_SCORELINE_X + 212) 
#define SB_PING_X			(SB_SCORELINE_X + 251) 
#define SB_WORSTENEMY_X		(SB_SCORELINE_X + 294) 
#define SB_FAVEWEAPON_X		(SB_SCORELINE_X + 398) 

#define SB_FAVETARGET_X		(SB_SCORELINE_X + 182) 

//Scoreboard text alignment
#define SB_SCORELINE_X_BIG	( SCOREBOARD_X /*+ 13*/		) //Before RPG-X: - This is the first leading space in the score line (SCOREBOARD_X		+ 125)
#define SB_RANK_X_BIG		( SB_SCORELINE_X_BIG + 6	) //Before RPG-X: (SB_SCORELINE_X_BIG + 6) = 43
#define SB_RPGCLASS_X_BIG	( SB_RANK_X_BIG + 78		) //70	= 121
#define SB_NAME_X_BIG		( SB_RPGCLASS_X_BIG + 46	) //132 = 167
#define SB_LOC_X_BIG		( SB_NAME_X_BIG + 188		)
#define SB_SCORE_X_BIG		( SB_LOC_X_BIG + 148		) //405 = 464 //335 //332
#define SB_TIME_X_BIG		( SB_SCORE_X_BIG + 44		) //	= 493
#define SB_PING_X_BIG		( SB_TIME_X_BIG + 35		) //491 = 528

#define AWARD_Y						50			// height of awards medals on screen

// The new and improved score board
//
// In cases where the number of clients is high, the score board heads are interleaved
// here's the layout

//
//	0   32   80  112  144   240  320  400   <-- pixel position
//  bot head bot head score ping time name
//  
//  wins/losses are drawn on bot icon now

static qboolean localClient; // true if local client has been displayed

static void CG_ClipString(char *buffer,char *name,int pixelCnt,int font)
{
	char str[2];
	int	length;

	str[1] = '\0';

	length = 0;
	while (*name)
	{
		str[0] = *name;
		length += UI_ProportionalStringWidth(str,font );

		if (font == UI_TINYFONT)
		{
			length += PROP_GAP_TINY_WIDTH;
		}
		else
		{
			length += PROP_GAP_WIDTH;
		}

		if (length > pixelCnt)
		{
			*buffer = '\0';	
			return;
		}
		else
		{
			*buffer = *name;	
		}
		name++;
		buffer++;
	}
	*buffer = '\0';	
}

/*
#define 	crewman	1		
#define 	cadet1		2		
#define 	cadet2		4		
#define 	cadet3		8		
#define 	cadet4		16		
#define 	ensign		32		
#define 	ltjg			64		
#define 	lt				128	
#define 	ltcmdr		256	
#define 	cmdr			512	
#define 	cpt			1024	
#define 	cmmdr		2048	
#define 	adm2		4096	
#define 	adm3		8192	
#define  adm4		16384
#define 	adm5		32768
*/

//Modified by TiM for the new rank system
static void CG_DrawRank(int x, int y, pclass_t pClass, int score) {
	int classno;
	int score_log;
	rankMenuData_t*	ScoreMenuData;

	//Q_log2
	if ( ( score_log = score ) < 0 ) { //The log function takes the score and turns it into a useful
		score_log = 0;							 //array value: ie 1024 -> 10. :) Hoorah for logarithms!
	}
	
	classno = cgs.classData[pClass].iconColor-1;

	/*if ( !Q_stricmp( cgs.classData[pClass].iconColor, "r" ) ) {
		classno = CLR_RED;
	}
	else if ( !Q_stricmp( cgs.classData[pClass].iconColor, "y" ) ) {
		classno = CLR_GOLD; 
	}
	else if ( !Q_stricmp( cgs.classData[pClass].iconColor, "t" ) ) {
		classno = CLR_TEAL; 
	}
	else if ( !Q_stricmp( cgs.classData[pClass].iconColor, "g" ) ) {
		classno = CLR_GREEN;
	}
	else
		classno = -1;*/

	//based on class, choose our right color.
	/*switch(pClass)
	{
		//Redshirt... heh heh
		case PC_COMMAND:
		case PC_ADMIN: 
			classno = CLR_RED; 
			break;

		//Yellowshirt
		case PC_SECURITY:
		case PC_ENGINEER: 
			classno = CLR_GOLD; 
			break;
				
		//Blueshirt
		case PC_MEDICAL:
		case PC_SCIENCE: 
			classno = CLR_TEAL; 
			break;

		//Marine
		case PC_ALPHAOMEGA22:  
			classno = CLR_GREEN; 
			break;

		//N/A
		case PC_NOCLASS:
		case PC_N00B:
		case PC_ALIEN:
		default:
		{
			classno = -1;
			//CG_DrawPic(x, y, 65, 14, cgs.media.ri_Civ ); 
			//return;
		}
	} */

	//CG_Printf( "%i\n", classno );

	//if we have a valid class and/or color
	if ( classno >= 0 && cgs.ranksData[score_log].rankMenuData[classno].graphic ) {
		ScoreMenuData = &cgs.ranksData[score_log].rankMenuData[classno];
	}
	else { //else use our default data
		ScoreMenuData = &cgs.defaultRankData.rankMenuData;
	}

	//Finally, render our rank using the carefully crafted trap_R_DrawStretchPic API function :)
	CG_DrawStretchPic( x, y, ScoreMenuData->w, ScoreMenuData->h, 
								(float)ScoreMenuData->s1/(float)ScoreMenuData->gWidth, 
								(float)ScoreMenuData->t1/(float)ScoreMenuData->gHeight, 
								(float)ScoreMenuData->s2/(float)ScoreMenuData->gWidth, 
								(float)ScoreMenuData->t2/(float)ScoreMenuData->gHeight, 
								ScoreMenuData->graphic );


			/*switch(score)
			{
			default:
			case		crewman:	CG_DrawPic(x, y, 65, 14, cgs.media.ri_Crewman[classno] ); break;	
			case		cadet1:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cadet1[classno] ); break;		
			case		cadet2:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cadet2[classno] ); break;		
			case		cadet3:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cadet3[classno] ); break;		
			case		cadet4:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cadet4[classno] ); break;		
			case		ensign:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Ensign[classno]); break;		
			case		ltjg:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Ltjg[classno]); break;				
			case		lt:			CG_DrawPic(x, y, 65, 14, cgs.media.ri_Lt[classno]); break;				
			case		ltcmdr:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Ltcmdr[classno]); break;		
			case		cmdr:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cmdr[classno]); break;			
			case		cpt:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Capt[classno]); break;		
			case		cmmdr:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Cmmdr[classno]); break;		
			case		adm2:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Admr2[classno]); break;		
			case		adm3:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Admr3[classno]); break;		
			case		 adm4:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Admr4[classno]); break;		
			case		adm5:		CG_DrawPic(x, y, 65, 14, cgs.media.ri_Admr5[classno]); break;		
			}*/
}

static qboolean AW_Draw( void );
/*
=======================
CG_DrawClientScoreboard_Big
=======================
*/
static void CG_DrawClientScore_Big( int y, score_t *score, float *color, float fade, qboolean largeFormat ) {
	char			string[1024];
	char			string2[1024];
	//RPG-X: Not Needed without a head
	//vec3_t			headAngles;
	clientInfo_t	*ci;
	char	        *rpg_class;
	vec4_t	        rpg_color;
	vec_t			*ping_txtcolor = NULL;
	int				intClamp;

	ci = &cgs.clientinfo[score->client];

	//RPG-X: RedTechie - Side Strips
	CG_FillRect( SCOREBOARD_X-15, SB_HEADER+20, 15, SB_RPG_X_FIXHEIGHT-4, colorTable[CT_DKBLUE1] ); //RPG-X: RedTechie - Left Strip
	CG_FillRect( SB_TOPLINE_LENGTH+53, SB_HEADER+20, 15, SB_RPG_X_FIXHEIGHT-4, colorTable[CT_DKBLUE1] ); //RPG-X: RedTechie - Right Strip

	rpg_class = cgs.classData[ci->pClass].formalName;
	rpg_color[0] = (float)cgs.classData[ci->pClass].radarColor[0] / 255.0f;
	rpg_color[1] = (float)cgs.classData[ci->pClass].radarColor[1] / 255.0f;
	rpg_color[2] = (float)cgs.classData[ci->pClass].radarColor[2] / 255.0f;
	rpg_color[3] = 1.0f;
	
	// highlight your position
	if ( score->client == cg.snap->ps.clientNum ) 
	{
		localClient = qtrue;
	}


	// draw the score line
	//RPG-X: Redtechie - Ping color
	if(score->ping >= 501){
		ping_txtcolor = colorTable[CT_RED];
	}else if(score->ping >= 230){
		ping_txtcolor = colorTable[CT_DKRED1];
	}else if(score->ping >= 118){
		ping_txtcolor = colorTable[CT_YELLOW];
	}else if(score->ping <= 117){
		ping_txtcolor = colorTable[CT_WHITE];
	}

	//TiM : Re-structured slightly for lesser redundancy.
	//Player is connecting
	if ( score->ping == -1 ) {
		Com_sprintf(string,sizeof(string),"(%s)%s",ingame_text[IGT_CONNECTING],ci->name);
		UI_DrawProportionalString( SB_NAME_X_BIG, y , string, UI_TINYFONT, colorTable[CT_WHITE]);
	}
	else {
		int clipLength;

		//draw rank image
		CG_DrawRank( SB_RANK_X_BIG, y-2,ci->pClass, score->score);

		//draw player class name
		if(rpg_class){
			UI_DrawProportionalString( SB_RPGCLASS_X_BIG, y , rpg_class, UI_TINYFONT | UI_LEFT, rpg_color); //CT_VLTPURPLE1 CT_WHITE
		}

		//if we have locations, change the cliplength of the name field
		if ( cgs.locations ) {
			clipLength = 185;
		}
		else {
			clipLength = 327;
		}

		//draw player name with varying instances
		//If spect, concat a spect msg next to it
		if ( ci->team == TEAM_SPECTATOR ) {
			Com_sprintf(string,sizeof(string),"(%s)%s",ingame_text[IGT_SPECTABBREV],ci->name);
			CG_ClipString(string2,string,clipLength,UI_TINYFONT); //RPG-X ADDED: RedTechie - 200 pixels in the name column use to be 184
			UI_DrawProportionalString( SB_NAME_X_BIG, y , string2, UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);
		}
		//if lagged out, add a lagged out msg
		else if ( score->ping >= 827 )
		{
			Com_sprintf(string,sizeof(string),"(%s)%s",ingame_text[IGT_PINGEDOUT],ci->name);
			CG_ClipString(string2,string,clipLength,UI_TINYFONT); //RPG-X ADDED: RedTechie - 200 pixels in the name column use to be 184
			UI_DrawProportionalString( SB_NAME_X_BIG, y , string2, UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);			
		}
		//else, draw the name normally
		else 
		{
			CG_ClipString(string,ci->name,clipLength,UI_TINYFONT); //RPG-X ADDED: RedTechie - 200 pixels in the name column use to be 184
			UI_DrawProportionalString( SB_NAME_X_BIG, y , string, UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);			
		}

		//Player locations
		if ( cgs.locations ) {
			CG_ClipString(string, va("%s", CG_ConfigString( CS_LOCATIONS + ci->location ) ), 495, UI_TINYFONT); //RPG-X ADDED: RedTechie - 200 pixels in the name column use to be 184
			UI_DrawProportionalString( SB_LOC_X_BIG, y , string, UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);
		}

		//player client Num
		//Com_sprintf(string,sizeof(string), "%i", intClamp);				//RPG-X: J2J Switched Scoore to Client No.
		intClamp = Com_Clamp( 0, 128, cg_entities[score->client].currentState.clientNum );
		UI_DrawProportionalString( SB_SCORE_X_BIG, y , va("%i", intClamp), UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);

		//player time
		//If someone actually hits this limit, they officially have no life.
		//Com_sprintf(string,sizeof(string),"%i", intClamp);
		intClamp = Com_Clamp( 0, 99999, score->time );
		UI_DrawProportionalString( SB_TIME_X_BIG, y ,  va("%i", intClamp), UI_TINYFONT | UI_LEFT, colorTable[CT_WHITE]);

		//player ping
		//Com_sprintf(string,sizeof(string),"%i",score->ping);
		intClamp = Com_Clamp( 0, 999, score->ping );
		UI_DrawProportionalString( SB_PING_X_BIG, y, va("%i", intClamp), UI_TINYFONT | UI_LEFT, ping_txtcolor);	

	}

	//RPG-X: RedTechie - Draw live divider after every client
	CG_FillRect( SB_NAME_X_BIG, y+11, SB_TOPLINE_LENGTH-119, 1, colorTable[CT_VDKBLUE2] ); // y off - 10, thickness - 2 //150

}

/*
=======================
CG_DrawClientScoreboard
=======================
*/
static void CG_DrawClientScore( int y, score_t *score, float *color, float fade, qboolean largeFormat ) {
	//char			string[1024];
	//char			string2[1024];
	//vec3_t			headAngles;
	clientInfo_t	*ci;
	//int				picSize;
	//char			/**worstEnemy,*/*faveWeapon=0;
	//float			hcolor[4];
	//gitem_t *		item;
	int		inIntermission/*,length*/;

	inIntermission = (
		   (cg.snap->ps.pm_type==PM_INTERMISSION) 
		|| (cg.intermissionStarted)
		|| (cg.predictedPlayerState.pm_type==PM_INTERMISSION)   );


	if ( score->client < 0 || score->client >= cgs.maxclients ) 
	{
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}
	

	ci = &cgs.clientinfo[score->client];
	if (!ci->infoValid)
	{
		Com_Printf( "Bad clientInfo: %i\n", score->client );
		return;
	}
	
	/*trap_R_SetColor( colorTable[CT_DKBLUE1] );
	CG_DrawPic( SCOREBOARD_X-15, y-28, 25, 28, cgs.media.scoreboardtopleft ); //RPG-X: - RedTechie Top Left
	CG_DrawPic( SB_TOPLINE_LENGTH+43, y-28, 25, 28, cgs.media.scoreboardtopright ); //RPG-X: - RedTechie Top Right
	CG_DrawPic( SCOREBOARD_X-15, y+325, 36, 28, cgs.media.scoreboardbotleft ); //RPG-X: - RedTechie Bottom Left
	CG_DrawPic( SB_TOPLINE_LENGTH+32, y+321, 36, 32, cgs.media.scoreboardbotright ); //RPG-X: - RedTechie Bottom Right
	*/
	//RPG-X: - RedTechie Fixed intermissions scoreboard
	if ( !inIntermission ) 
	{
		//RPG-X BOOKMARK
		CG_DrawClientScore_Big(y, score, color, fade, largeFormat );
		return;
	}else{
		CG_DrawClientScore_Big(y, score, color, fade, largeFormat );
		return;
	}
}

/*
=================
CG_TeamScoreboard
=================
*/
static int CG_TeamScoreboard( int y, team_t team, float fade, int maxClients, int lineHeight ) {
	int		i;
	score_t	*score;
	float	color[4];
	int		count;
	clientInfo_t	*ci;

	color[0] = color[1] = color[2] = 1.0;
	color[3] = fade;

	count = 0;

	for ( i = 0 ; i < cg.numScores && count < maxClients ; i++ ) {
		score = &cg.scores[i];
		ci = &cgs.clientinfo[ score->client ];

		if ( team != ci->team ) {
			continue;
		}

		if (ci->infoValid)
		{
			//RPG-X BOOKMARK
			//UI_DrawProportionalString(60,y + lineHeight * count,va("%i",cg_entities[i].currentState.clientNum), UI_TINYFONT, colorTable[CT_LTGOLD1]);
			CG_DrawClientScore( y + lineHeight * count, score, color, fade, lineHeight == SB_NORMAL_HEIGHT );
			count++;
		}
	}

	return count;
}

void CG_AddGameModNameToGameName( char *gamename )
{
	//Primary Mod
	if ( cgs.pModElimination )
	{
		strcat( gamename, ": " );
		strcat( gamename, ingame_text[IGT_GAME_ELIMINATION] );
	}
	else if ( cgs.pModAssimilation )
	{
		strcat( gamename, ": " );
		strcat( gamename, ingame_text[IGT_GAME_ASSIMILATION] );
	}
	else if ( cgs.pModActionHero )
	{
		strcat( gamename, ": " );
		strcat( gamename, ingame_text[IGT_GAME_ACTIONHERO] );
	}

	//Secondary Mod
	if ( cgs.pModSpecialties )
	{
		strcat( gamename, " (" );
		strcat( gamename, ingame_text[IGT_GAME_SPECIALTIES] );
		strcat( gamename, ")" );
	}
	else if ( cgs.pModDisintegration )
	{
		strcat( gamename, " (" );
		strcat( gamename, ingame_text[IGT_GAME_DISINTEGRATION] );
		strcat( gamename, ")" );
	}
}

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) 
{
	int		y, i, n1 = 0, n2 = 0; // n3 = 0;
	float	fade;
	float	*fadeColor;
	char	*s;
	//RPG-X: RedTechie dont need buf with all the print team code commented out
	//char	buf[64];
	int		maxClients;
	int		lineHeight;
	float	hcolor[4];
	int		inIntermission;
	char	gamename[1024];
	//int		gOffset = 20;

	inIntermission = (
		   (cg.snap->ps.pm_type==PM_INTERMISSION) 
		|| (cg.intermissionStarted)
		|| (cg.predictedPlayerState.pm_type==PM_INTERMISSION)   );

	// DO NOT SHOW THE SCOREBOARD IF:
	////////////////////////////////////////////////////////////////
	// 1) Menu or Console is Up
	if ( cg_paused.integer ) 
	{	cg.deferredPlayerLoading = 0;
		return qfalse;	}
	//
	// 2) Awards Ceremony is not finished during Intermisison
	if (inIntermission && !AW_Draw()) 
	{	return qfalse;	}
	//
	// 3) If we are doing a warmup
	if (cg.warmup && !cg.showScores)
		return qfalse;
	//RPG-X: RedTechie - Hacked so scoreboard dosnt show when players die
	if(cg.predictedPlayerState.pm_type==PM_DEAD){
		return qfalse;
	}

	// FADE SETUP... ??  
	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 inIntermission ) 
	{
		fade = 1.0;
		fadeColor = colorWhite;
	} else 
	{
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		
		if ( !fadeColor ) 
		{
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}


	// IN GAME SCOREBOARD HEADER STUFF
	//---------------------------------

	if ( !inIntermission && cg.snap->ps.persistant[PERS_TEAM]!=TEAM_SPECTATOR) 
	{
		//  Display Who Killed You
		//-----------------------------------------------
		if ( cg.killerName[0] ) 
		{
			if ( cg.mod == MOD_ASSIMILATE )
			{
				s = va("%s %s",ingame_text[IGT_ASSIMILATEDBY], cg.killerName );
			}
			else
			{
				s = va("%s %s",ingame_text[IGT_FRAGGEDBY], cg.killerName );
			}
			UI_DrawProportionalString((SCREEN_WIDTH/2), 438, s, UI_BIGFONT|UI_CENTER, colorTable[CT_CYAN]);//RPG-X: - RedTechie Y axis use to be 75
		}
	}
	// scoreboard
	y = SB_HEADER;

	hcolor[0] = 0;
	hcolor[1] = 0;
	hcolor[2] = 0;
	hcolor[3] = fade * 0.7;
	CG_FillRect( SCOREBOARD_X, y + 5,SB_TOPLINE_LENGTH + 26, SB_RPG_X_FIXHEIGHT+15, hcolor );//RPG-X: RedTechie - Before CG_FillRect( SCOREBOARD_X, y + 20,SB_TOPLINE_LENGTH, SB_RPG_X_FIXHEIGHT, hcolor );

	trap_R_SetColor( colorTable[CT_DKBLUE1] );
	CG_DrawPic( SCOREBOARD_X-15,		SB_TOP-28,	25, 28, cgs.media.scoreboardtopleft ); //RPG-X: - RedTechie Top Left
	CG_DrawPic( SB_TOPLINE_LENGTH+43,	SB_TOP-28,	25, 28, cgs.media.scoreboardtopright ); //RPG-X: - RedTechie Top Right
	CG_DrawPic( SCOREBOARD_X-15,		SB_TOP+325, 36, 28, cgs.media.scoreboardbotleft ); //RPG-X: - RedTechie Bottom Left
	CG_DrawPic( SB_TOPLINE_LENGTH+32,	SB_TOP+321, 36, 32, cgs.media.scoreboardbotright ); //RPG-X: - RedTechie Bottom Right

	CG_FillRect( SCOREBOARD_X,			y+5,	( SB_RPGCLASS_X_BIG - 5 ) - SCOREBOARD_X,			15, colorTable[CT_DKBLUE1]); //RPG-X: Rank Bar //75 //79
	CG_FillRect( SB_RPGCLASS_X_BIG - 3, y+5,	( SB_NAME_X_BIG - 5 ) - ( SB_RPGCLASS_X_BIG - 3 ), 15, colorTable[CT_DKBLUE1]); //RPG-X: Class Bar //46 - 60
	if ( cgs.locations ) {
		CG_FillRect( SB_NAME_X_BIG - 3,		y+5, ( SB_LOC_X_BIG - 5 ) - ( SB_NAME_X_BIG - 3 ),	15, colorTable[CT_DKBLUE1]); //RPG-X: Name Bar //269
		CG_FillRect( SB_LOC_X_BIG - 3,		y+5, ( SB_SCORE_X_BIG - 5 ) - ( SB_LOC_X_BIG - 3 ),	15, colorTable[CT_DKBLUE1]); //RPG-X: Locations Bar //269
	}
	else {
		CG_FillRect( SB_NAME_X_BIG - 3,		y+5, ( SB_SCORE_X_BIG - 5 ) - ( SB_NAME_X_BIG - 3 ),	15, colorTable[CT_DKBLUE1]); //RPG-X: Name Bar //269
	}

	CG_FillRect( SB_SCORE_X_BIG - 3,	y+5,	( SB_TIME_X_BIG - 5 ) -  ( SB_SCORE_X_BIG - 3 ),	15, colorTable[CT_DKBLUE1]); //RPG-X: Client ID Bar //47
	CG_FillRect( SB_TIME_X_BIG - 3,		y+5,	( SB_PING_X_BIG - 5 ) - ( SB_TIME_X_BIG - 3 ),		15, colorTable[CT_DKBLUE1]); //RPG-X: Time Bar //34
	CG_FillRect( SB_PING_X_BIG - 3,		y+5,	602 - ( SB_PING_X_BIG - 3 ),						15, colorTable[CT_DKBLUE1]); //RPG-X: Ping Bar //35

	//RPG-X: RedTechie (Rank BookMark)
	UI_DrawProportionalString( SB_RANK_X_BIG,		SB_HEADERTEXT, ingame_text[IGT_SB_RANK],	UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	UI_DrawProportionalString( SB_RPGCLASS_X_BIG,	SB_HEADERTEXT, ingame_text[IGT_SB_RPGCLASS],UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	UI_DrawProportionalString( SB_NAME_X_BIG,		SB_HEADERTEXT, ingame_text[IGT_SB_NAME],	UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	if ( cgs.locations ) {
		UI_DrawProportionalString( SB_LOC_X_BIG,	SB_HEADERTEXT, ingame_text[IGT_SB_LOC],		UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	}
	UI_DrawProportionalString( SB_SCORE_X_BIG,		SB_HEADERTEXT, ingame_text[IGT_SB_SCORE],	UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	UI_DrawProportionalString( SB_TIME_X_BIG,		SB_HEADERTEXT, ingame_text[IGT_SB_TIME],	UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );
	UI_DrawProportionalString( SB_PING_X_BIG,		SB_HEADERTEXT, ingame_text[IGT_SB_PING],	UI_TINYFONT | UI_LEFT, colorTable[CT_BLACK] );

	y = SB_TOP;

	// SET UP SIZES
	//--------------
	if ( (inIntermission) ||
		 (cg.numScores>SB_MAXCLIENTS_BIG)  )
	{// POSTGAME
		maxClients = SB_MAXCLIENTS_NORMAL;
		lineHeight = SB_NORMAL_HEIGHT;
	}
	else
	{// INGAME
		maxClients = SB_MAXCLIENTS_BIG;
		lineHeight = SB_NORMAL_HEIGHT_BIG;
	}

	localClient = qfalse;
	
	n1 = CG_TeamScoreboard( y, TEAM_FREE, fade, maxClients, lineHeight );
	y += (n1 * lineHeight);
	maxClients -= n1;

	n2 = CG_TeamScoreboard( y, TEAM_SPECTATOR, fade, maxClients, lineHeight );
	y += (n2 * lineHeight);
	maxClients -= n2;

	localClient = qtrue;

	i=0;
	while (!localClient && i<cg.numScores)
	{
		if ( cg.scores[i].client==cg.snap->ps.clientNum )		// it's me!!
		{
			CG_DrawClientScore( y, &cg.scores[i], colorYellow, 0.0, (qboolean)(lineHeight==SB_NORMAL_HEIGHT) );
			y += lineHeight;									
			localClient = qtrue;
		}
		i++;
	}

	// Bottom of scoreboard
	//RPG-X BookMark Bottum bar
	CG_FillRect( SCOREBOARD_X, SB_RPG_X_FIXHEIGHT+60, SB_TOPLINE_LENGTH, 15, colorTable[CT_DKBLUE1]);

	s = va("%s: %i", ingame_text[IGT_PLAYERS], cg.numScores); // Number of Players

	Q_strncpyz( gamename, s, sizeof(gamename) );

	strcat( gamename, "      "S_COLOR_YELLOW );

	//CG_AddGameModNameToGameName( gamename );
	//RPG-X: RedTechie - Number of Characters in server
	UI_DrawProportionalString( SCOREBOARD_X+10, SB_RPG_X_FIXHEIGHT+63, gamename, UI_TINYFONT, colorTable[CT_BLACK]);

	// load any models that have been deferred
	if ( inIntermission || (++cg.deferredPlayerLoading > 10) ) 
		CG_LoadDeferredPlayers();
	
	return qtrue;
}

//================================================================================

/*
================
CG_CenterGiantLine
================
*/
static void CG_CenterGiantLine( float y, const char *string ) {
	float		x;
	vec4_t		color;

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

//	x = 0.5 * ( 640 - GIANT_WIDTH * CG_DrawStrlen( string ) );
//	CG_DrawStringExt( x, y, string, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );

	x = 0.5 * ( 640 - UI_ProportionalStringWidth(string,UI_BIGFONT) );
	UI_DrawProportionalString( x, y, string, UI_BIGFONT|UI_DROPSHADOW, color);

}

/*
=================
CG_DrawTourneyScoreboard

Draw the oversize scoreboard for tournements
=================
*/
void CG_DrawTourneyScoreboard( void ) {
	const char		*s;
	vec4_t			color;
	int				min, tens, ones;
	clientInfo_t	*ci;
	int				y;
	int				i,w;

	// request more scores regularly
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );
	}

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	// draw the dialog background
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;
	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color );

	// print the mesage of the day
	s = CG_ConfigString( CS_MOTD );
	if ( !s[0] ) {
		s = "Scoreboard";
	}

	// print optional title
	CG_CenterGiantLine( 8, s );

	// print server time
	ones = cg.time / 1000;
	min = ones / 60;
	ones %= 60;
	tens = ones / 10;
	ones %= 10;
	s = va("%i:%i%i", min, tens, ones );

	CG_CenterGiantLine( 64, s );


	// print the two scores

	y = 160;
	for ( i = 0 ; i < MAX_CLIENTS ; i++ ) {
		ci = &cgs.clientinfo[i];
		if ( !ci->infoValid ) {
			continue;
		}
		if ( ci->team != TEAM_FREE ) {
			continue;
		}

//			CG_DrawStringExt( 8, y, ci->name, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );
		UI_DrawProportionalString( 8, y, ci->name, UI_BIGFONT|UI_DROPSHADOW, color);
		s = va("%i", ci->score );
//			CG_DrawStringExt( 632 - GIANT_WIDTH * strlen(s), y, s, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );
		w = UI_ProportionalStringWidth(s,UI_BIGFONT);
		UI_DrawProportionalString(632 - w, y, s, UI_BIGFONT|UI_DROPSHADOW, color);

		y += 64;
	}
}








/*
=============================================================================

AWARDS PRESENTATION

  The following code was ripped from ui_sppostgame.c and has been modified
  to show the awards presentation for all game types

FROM ????Server????
  AW_SPPostgameMenu_f()
      -Parse award information and store in struct


FROM DrawScoreboard()
  AW_Draw()
	  -PHASE I
		Play Sounds

	  -PHASE II
		AW_DrawAwardsPresentation(timer)
			AW_DrawAwards(max)

	  -PHASE III
		AW_DrawAwards(ALL)

	  -PHASE IV 
	    Draw the New Scoreboard


=============================================================================
*/
#define AWARD_PRESENTATION_TIME		2000		// time each medal is shown

typedef struct {

	int				phase;
	int				ignoreKeysTime;
	int				starttime;
	int				scoreboardtime;
	int				serverId;

	int				playerGameRank;
	qboolean		playersTeamWon;
	int				playerTied;

	char			placeNames[3][64];
	char			winTeamText[64];
	char			losTeamText[64];
	char			winTeamMVPText[64];
	char			nameOfMVP[64];
	int				scoreOfMVP;
	int				totalCaptures;
	int				totalPoints;
	int				losCaptures;
	int				losPoints;
	int				secondPlaceTied;

	int				level;
	int				numClients;
	int				won;
	int				numAwards;
	int				awardsEarned[9];
	int				awardsLevels[9];
	qboolean		playedSound[9];
	int				lastTier;
	sfxHandle_t		winnerSound;
	int				winnerDelay;
	sfxHandle_t		youPlacedSound;
	sfxHandle_t		commendationsSound;

	vec3_t			intermission_origin;
	vec3_t			intermission_angle;
} postgameMenuInfo_t;

static postgameMenuInfo_t	postgameMenuInfo;
static qboolean				postGameMenuStructInited = qfalse;

void InitPostGameMenuStruct(void)
{
	if (qfalse == postGameMenuStructInited)
	{
		memset( &postgameMenuInfo, 0, sizeof(postgameMenuInfo) );
		postGameMenuStructInited = qtrue;
	}
}

/*
char	*cg_medalNames[AWARD_MAX] = 
{
	"Efficiency",		// AWARD_EFFICIENCY,		Accuracy	
	"Sharpshooter",		// AWARD_SHARPSHOOTER,		Most compression rifle frags
	"Untouchable",		// AWARD_UNTOUCHABLE,		Perfect (no deaths)
	"Logistics",		// AWARD_LOGISTICS,			Most pickups
	"Tactician",		// AWARD_TACTICIAN,			Kills with all weapons
	"Demolitionist",	// AWARD_DEMOLITIONIST,		Most explosive damage kills
	"Streak",			// AWARD_STREAK,			Ace/Expert/Master/Champion
	"Role",				// AWARD_TEAM,				MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	"Section 31"		// AWARD_SECTION31			All-around god
};
*/

int	cg_medalNames[AWARD_MAX] = 
{
	IGT_EFFICIENCY,	// AWARD_EFFICIENCY,		Accuracy	
	IGT_SHARPSHOOTER,	// AWARD_SHARPSHOOTER,		Most compression rifle frags
	IGT_UNTOUCHABLE,	// AWARD_UNTOUCHABLE,		Perfect (no deaths)
	IGT_LOGISTICS,		// AWARD_LOGISTICS,			Most pickups
	IGT_TACTICIAN,		// AWARD_TACTICIAN,			Kills with all weapons
	IGT_DEMOLITIONIST,	// AWARD_DEMOLITIONIST,		Most explosive damage kills
	IGT_ROLE,			// AWARD_STREAK,			Ace/Expert/Master/Champion
	IGT_STREAK,			// AWARD_TEAM,				MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	IGT_SECTION31		// AWARD_SECTION31			All-around god
};


char	*cg_medalPicNames[AWARD_MAX] = {
	"menu/medals/medal_efficiency",		// AWARD_EFFICIENCY,		
	"menu/medals/medal_sharpshooter",	// AWARD_SHARPSHOOTER,		
	"menu/medals/medal_untouchable",	// AWARD_UNTOUCHABLE,		
	"menu/medals/medal_logistics",		// AWARD_LOGISTICS,			
	"menu/medals/medal_tactician",		// AWARD_TACTICIAN,			
	"menu/medals/medal_demolitionist",	// AWARD_DEMOLITIONIST,		
	"menu/medals/medal_ace",			// AWARD_STREAK,			
	"menu/medals/medal_teammvp",		// AWARD_TEAM,				
	"menu/medals/medal_section31"		// AWARD_SECTION31			
};

char	*cg_medalSounds[AWARD_MAX] = {
	"sound/voice/computer/misc/effic.wav",		// AWARD_EFFICIENCY,		
	"sound/voice/computer/misc/sharp.wav",		// AWARD_SHARPSHOOTER,		
	"sound/voice/computer/misc/untouch.wav",	// AWARD_UNTOUCHABLE,		
	"sound/voice/computer/misc/log.wav",		// AWARD_LOGISTICS,			
	"sound/voice/computer/misc/tact.wav",		// AWARD_TACTICIAN,			
	"sound/voice/computer/misc/demo.wav",		// AWARD_DEMOLITIONIST,		
	"sound/voice/computer/misc/ace.wav",		// AWARD_STREAK,			
	"",/*shouldn't use. use cg_medalTeamSounds instead*/			// AWARD_TEAM,				
	"sound/voice/computer/misc/sec31.wav"		// AWARD_SECTION31			
};

char	*cg_medalTeamPicNames[TEAM_MAX] = {
	"",									// TEAM_NONE
	"menu/medals/medal_teammvp",		// TEAM_MVP,		
	"menu/medals/medal_teamdefender",	// TEAM_DEFENDER,		
	"menu/medals/medal_teamwarrior",	// TEAM_WARRIOR,		
	"menu/medals/medal_teamcarrier",	// TEAM_CARRIER,			
	"menu/medals/medal_teaminterceptor",// TEAM_INTERCEPTOR,			
	"menu/medals/medal_teambravery"		// TEAM_BRAVERY,			
};

char	*cg_medalTeamSounds[TEAM_MAX] = {
	"",									// TEAM_NONE
	"sound/voice/computer/misc/mvp.wav",			// TEAM_MVP,		
	"sound/voice/computer/misc/defender.wav",		// TEAM_DEFENDER,		
	"sound/voice/computer/misc/warrior.wav",		// TEAM_WARRIOR,		
	"sound/voice/computer/misc/carrier.wav",		// TEAM_CARRIER,			
	"sound/voice/computer/misc/intercept.wav",		// TEAM_INTERCEPTOR,			
	"sound/voice/computer/misc/bravery.wav"		// TEAM_BRAVERY,			
};


char	*cg_medalStreakPicNames[AWARD_STREAK_MAX] = {
	"",									// AWARD_NONE
	"menu/medals/medal_ace",			// AWARD_STREAK_ACE,		
	"menu/medals/medal_expert",			// AWARD_STREAK_EXPERT,		
	"menu/medals/medal_master",			// AWARD_STREAK_MASTER,		
	"menu/medals/medal_champion"		// AWARD_STREAK_CHAMPION,			
};

char	*cg_medalStreakSounds[AWARD_STREAK_MAX] = {
	"",									// AWARD_NONE
	"sound/voice/computer/misc/ace.wav",			// AWARD_STREAK_ACE,		
	"sound/voice/computer/misc/expert.wav",		// AWARD_STREAK_EXPERT,		
	"sound/voice/computer/misc/master.wav",		// AWARD_STREAK_MASTER,		
	"sound/voice/computer/misc/champion.wav"		// AWARD_STREAK_CHAMPION,			
};
/*
char	*cg_medalStreakNames[AWARD_STREAK_MAX] = {
	"",									// AWARD_NONE
	"Ace",			// AWARD_STREAK_ACE,		
	"Expert",		// AWARD_STREAK_EXPERT,		
	"Master",		// AWARD_STREAK_MASTER,		
	"Champion"		// AWARD_STREAK_CHAMPION,			
};
*/

int cg_medalStreakNames[AWARD_STREAK_MAX] = {
	IGT_NONE,		// AWARD_NONE
	IGT_ACE,		// AWARD_STREAK_ACE,		
	IGT_EXPERT,		// AWARD_STREAK_EXPERT,		
	IGT_MASTER,		// AWARD_STREAK_MASTER,		
	IGT_CHAMPION	// AWARD_STREAK_CHAMPION,			
};

int cg_medalTeamNames[TEAM_MAX] = {
	IGT_NONE,			// AWARD_NONE
	IGT_MVP,			// TEAM_MVP,		
	IGT_DEFENDER,		// TEAM_DEFENDER,		
	IGT_WARRIOR,		// TEAM_WARRIOR,		
	IGT_CARRIER,		// TEAM_CARRIER,			
	IGT_INTERCEPTOR,	// TEAM_INTERCEPTOR
	IGT_BRAVERY			// TEAM_BRAVERY
};


// spaced by 70 pixels apart
//static int medalLocations[6] = {570, 500, 430, 360, 290};

/*static void AW_DrawMedal( int medal, int amount, int x, int y )
{
	char		buf[20];
	qhandle_t	hShader;
	vec4_t		yellow;

	//         RED                GREEN             BLUE               ALPHA
	//-----------------------------------------------------------------------
	yellow[0]=1.0f;		yellow[1]=1.0f;		yellow[2]=0.5f;		yellow[3]=1.0f;

	if (medal == AWARD_STREAK)		{
		hShader = trap_R_RegisterShaderNoMip( cg_medalStreakPicNames[amount/5] );
	}
	else if (medal == AWARD_TEAM)	{
		hShader = trap_R_RegisterShaderNoMip( cg_medalTeamPicNames[amount] );
	}
	else							{
		hShader = trap_R_RegisterShaderNoMip( cg_medalPicNames[medal] );
	}
	CG_DrawPic( x, y, 48, 48, hShader);

	if (postgameMenuInfo.phase<4)
	{
		//--------------------------------------------------------------- NAME OF THE AWARD
		Com_sprintf( buf, sizeof(buf), "%s", ingame_text[cg_medalNames[medal]]);
		if (medal== AWARD_STREAK)
			Com_sprintf( buf, sizeof(buf), "%s", ingame_text[cg_medalStreakNames[amount/5]]);
		if (medal== AWARD_TEAM)
			Com_sprintf( buf, sizeof(buf), "%s", ingame_text[cg_medalTeamNames[amount]]);

//			Q_CleanStr( buf );

		UI_DrawProportionalString( x+24, y+50, buf, UI_TINYFONT|UI_CENTER, yellow );
		//--------------------------------------------------------------- NAME OF THE AWARD
	}
}*/

//========================
//	DrawTheMedals(int max)
//
//	This function cycles from 0 to max and calls some functions to Draw the Picture and
// the text below for efficency.
//
//========================
//RPG-X: RedTechie - No awards
/*static int presentedTeamAward[TEAM_MAX];
static void AW_DrawTheMedals( int max ) {
	int			n, i;
	int			medal;
	int			amount;
	int			x, y;
	int			extraAwardOffset = 0;

	for( n = 0; n < max; n++ ) {
		x = medalLocations[n] - extraAwardOffset;
		y = AWARD_Y;
		medal = postgameMenuInfo.awardsEarned[n];
		amount = postgameMenuInfo.awardsLevels[n];

		if (medal == AWARD_TEAM)	
		{//amount is going to be a bitflag field...
			for( i = TEAM_MVP; i < TEAM_MAX; i++ )
			{
				if ( amount & (1<<i) )
				{
					if ( presentedTeamAward[i] )
					{
						AW_DrawMedal( medal, i, x, y );
						extraAwardOffset += 70;
						//recalc x
						x = medalLocations[n] - extraAwardOffset;
					}
				}
			}
		}
		else
		{
			AW_DrawMedal( medal, amount, x, y );
		}
	}
}

static int awardNextDebounceTime = 0;
static void AW_PresentMedal( int medal, int amount, int timer, int awardNum )
{
	vec4_t	color;

	// Draw the name of the medal being presented:
	color[0] = color[1] = color[2] = 1.0f;

	if ( !awardNextDebounceTime )
	{
		color[3] = 1.0f;
	}
	else
	{
		color[3] = (float)(awardNextDebounceTime-cg.time)/(float)AWARD_PRESENTATION_TIME;
	}

	if (AWARD_STREAK == medal)
	{
		UI_DrawProportionalString( 320, 64,  ingame_text[cg_medalStreakNames[amount/5]], UI_CENTER, color );
	}
	else if (medal == AWARD_TEAM)
	{
		UI_DrawProportionalString( 320, 64,  ingame_text[cg_medalTeamNames[amount]], UI_CENTER, color );
	}
	else
	{
		UI_DrawProportionalString( 320, 64, ingame_text[cg_medalNames[medal]], UI_CENTER, color );
	}
	// If we have not played the SF for the current award, play it.
	if( !postgameMenuInfo.playedSound[awardNum] )
	{
		if (medal == AWARD_STREAK)			{
			postgameMenuInfo.playedSound[awardNum] = qtrue;
			trap_S_StartLocalSound( trap_S_RegisterSound( cg_medalStreakSounds[amount/5] ), CHAN_ANNOUNCER );
		}
		else if (medal == AWARD_TEAM)		{
			if ( !presentedTeamAward[amount] )
			{
				trap_S_StartLocalSound( trap_S_RegisterSound( cg_medalTeamSounds[amount] ), CHAN_ANNOUNCER );
			}
		}
		else								{
			postgameMenuInfo.playedSound[awardNum] = qtrue;
			trap_S_StartLocalSound( trap_S_RegisterSound( cg_medalSounds[medal] ), CHAN_ANNOUNCER );
		}
	}
}
*/
//========================
//	DrawAwardsPresentation(int timer)
//
//	This function incriments the timer and then calles the above function to actually draw the
//  award icons.  This function also triggers the sounds for winning the medals.
//
//========================
//RPG-X: RedTechie - No awards
/*static int extraAwardTime = 0;
static qboolean allTeamAwardsAnnounced = qfalse;
static int awardNum = 0;
static int nextTeamAward = 0;
static void AW_DrawAwardsPresentation( int timer ) {
	int		medal;
	int		amount;
	int		i=0;
	int		teamAward = 0;

	if ( !postgameMenuInfo.numAwards )
	{
		return;
	}

	if ( awardNum >= postgameMenuInfo.numAwards )
	{
		awardNum = (postgameMenuInfo.numAwards-1);
	}

	medal = postgameMenuInfo.awardsEarned[awardNum];
	amount = postgameMenuInfo.awardsLevels[awardNum];

	if ( medal == AWARD_TEAM )	
	{//amount is going to be a bitflag field...
		if ( !allTeamAwardsAnnounced )
		{
			for( i = TEAM_MVP; i < TEAM_MAX; i++ )
			{
				if ( amount & (1<<i) )
				{
					if ( !presentedTeamAward[i] )
					{
						if ( teamAward <= nextTeamAward )
						{//okay to announce the next award
							AW_PresentMedal( AWARD_TEAM, i, timer, awardNum );
							extraAwardTime += AWARD_PRESENTATION_TIME;
							presentedTeamAward[i] = qtrue;
						}
						break;
					}
					else if ( teamAward == nextTeamAward && awardNextDebounceTime > cg.time )
					{//already announced this award
						AW_PresentMedal( AWARD_TEAM, i, timer, awardNum );
					}
					teamAward++;
				}
			}
		}
	}
	else
	{
		AW_PresentMedal( medal, amount, timer, awardNum );
	}

	AW_DrawTheMedals( awardNum + 1 );	// Draw the awards

	if ( !awardNextDebounceTime )
	{
		awardNextDebounceTime = cg.time + AWARD_PRESENTATION_TIME;
	}
	else if ( awardNextDebounceTime <= cg.time )
	{
		awardNextDebounceTime = cg.time + AWARD_PRESENTATION_TIME;
		if ( i == TEAM_MAX )
		{
			//finished loop?  Then we announced all these
			allTeamAwardsAnnounced = qtrue;
		}
		if ( !extraAwardTime || allTeamAwardsAnnounced )
		{
			awardNum++;
		}
		else
		{
			nextTeamAward++;
		}
	}
}
*/

/*
=================
AW_Draw
=================
*/
static qboolean AW_Draw( void )
{
	int		timer;
	vec4_t	white, yellow;	// new colors

	//         RED                GREEN             BLUE               ALPHA
	//-----------------------------------------------------------------------
	white[0] =         white[1] =        white[2] = 1.0f;	white[3] = 1.0f;
	yellow[0]= 1.0f;   yellow[1]= 1.0f;  yellow[2]= 0.5f;	yellow[3]= 1.0f;

	// REASONS NOT TO SHOW THE AWARDS CEREMONY:
	//--------------------------------------------
	// if scoreboardtime is not set, we set it to current time plus five seconds slop time
	if (postgameMenuInfo.scoreboardtime == 0)
	{
		postgameMenuInfo.scoreboardtime = cg.time+5000;
		return qfalse;
	}

	// if scoreboardtime is greater than current time, we don't draw anything (including no scoreboard)
	//  This should only happen if the scoreboard time had not been set properly to current time by
	//  the setup function below
	if (postgameMenuInfo.scoreboardtime > cg.time+10)
		return qfalse;

	// OK, so we've given the slop time and a chance for the information to be set properly in
	//  case of network lag.  if .starttime is still not set properly, we have no right to show
	//  anything in the awards ceremony and go straight to the spiffy scoreboard...
	if (postgameMenuInfo.starttime==0)
		return qtrue;

	// PHASES I & II & III
	//  Draw Character / Team names by podiums
	//-----------------------------------------------
	if (postgameMenuInfo.phase < 4)
	{
		// NON TEAM GAMES
		//   We want the top three player's names and their ranks below the podium 
		if ( postgameMenuInfo.numClients > 2 )
		{
		UI_DrawProportionalString( 510, 480 - 64 - PROP_HEIGHT, postgameMenuInfo.placeNames[2], UI_CENTER, white );
		if (postgameMenuInfo.secondPlaceTied)
			UI_DrawProportionalString( 510, 480 - 38 - PROP_HEIGHT, ingame_text[IGT_2ND], UI_CENTER, yellow );
		else
			UI_DrawProportionalString( 510, 480 - 38 - PROP_HEIGHT, ingame_text[IGT_3RD], UI_CENTER, yellow );
		}
		if ( postgameMenuInfo.numClients > 1)
		{
		UI_DrawProportionalString( 130, 480 - 64 - PROP_HEIGHT, postgameMenuInfo.placeNames[1], UI_CENTER, white );
		UI_DrawProportionalString( 130, 480 - 38 - PROP_HEIGHT, ingame_text[IGT_2ND], UI_CENTER, yellow );
		}

		UI_DrawProportionalString( 320, 480 - 64 - 2 * PROP_HEIGHT, postgameMenuInfo.placeNames[0], UI_CENTER, white );
		UI_DrawProportionalString( 320, 480 - 38 - 2 * PROP_HEIGHT, ingame_text[IGT_1ST], UI_CENTER, yellow );
	}


	// if we are arriving as a spectator, there are no awards for you (not even sure that spectators
	// will get the awards string that sets the whole thing up...)
//	if (cg.snap->ps.persistant[PERS_TEAM]==TEAM_SPECTATOR)
//		return qtrue;
	

	// PHASE I
	//  Play Winner sound
	//-----------------------------------------------
	if( postgameMenuInfo.phase == 1 )
	{
		timer = cg.time - postgameMenuInfo.starttime;							// set up the timer
		
		//RPG-X: RedTechie - No winner
		/*if( postgameMenuInfo.winnerSound )											// if havn't played yet
		{
			trap_S_StartLocalSound( postgameMenuInfo.winnerSound, CHAN_ANNOUNCER );	// play the sound
			postgameMenuInfo.winnerSound = 0;										// don't play again
			// force another wait since we played a sound here
			postgameMenuInfo.starttime = cg.time + postgameMenuInfo.winnerDelay;
			timer = 0;
		}*/

		if( timer < 2000 )
		{
			return qfalse;
		}
		 
		//RPG-X: RedTechie - No ranking sound
		/*if (cg.snap->ps.persistant[PERS_TEAM]!=TEAM_SPECTATOR)
		{
			if (postgameMenuInfo.youPlacedSound)
			{
				trap_S_StartLocalSound( postgameMenuInfo.youPlacedSound, CHAN_ANNOUNCER );
				postgameMenuInfo.youPlacedSound = 0;
				// force another wait since we played a sound here
				postgameMenuInfo.starttime = cg.time;
				timer = 0;
			}
		}*/
		
		// if we didn't play a youPlaced sound, this won't slow us up at all
		if( timer < 2000 )
		{
			return qfalse;
		}
												
		// play "commendations" sound
		//RPG-X: RedTechie - No commendations
		/*if (postgameMenuInfo.commendationsSound)
		{
			trap_S_StartLocalSound( postgameMenuInfo.commendationsSound, CHAN_ANNOUNCER );
			postgameMenuInfo.commendationsSound = 0;
			// force another wait since we played a sound here
			postgameMenuInfo.starttime = cg.time;
		}*/

		if( timer < 2500 )
		{
			return qfalse;
		}
												// After 7 seconds, go on to phase II
		postgameMenuInfo.phase = 2;				// Setup PHASE II
		postgameMenuInfo.starttime = cg.time;	//
//		memset( presentedTeamAward, 0, sizeof( presentedTeamAward ) );
//		extraAwardTime = 0;
//		allTeamAwardsAnnounced = qfalse;
//		awardNextDebounceTime = 0;
//		awardNum = 0;
//		nextTeamAward = 0;
		
		return qfalse;
	}

	// PHASE II
	//  Introduce each medal on a timer
	//-----------------------------------------------
	//RPG-X: RedTechie - No awards
	if( postgameMenuInfo.phase == 2 )
	{

		timer = cg.time - postgameMenuInfo.starttime;// - extraAwardTime;	// Setup Timer
		if( timer >= ( postgameMenuInfo.numAwards * AWARD_PRESENTATION_TIME ) )
		{
			postgameMenuInfo.phase = 3;	
			//AW_DrawTheMedals( postgameMenuInfo.numAwards );	// Draw All Medals for this frame
		}
		else
		{
			//AW_DrawAwardsPresentation( timer );				// Draw Some Medals
		}
				
		return qfalse;
	}

	// PHASE III
	//  Just draw all the awards medals
	//-----------------------------------------------
	//RPG-X: RedTechie - No awards
	if( postgameMenuInfo.phase == 3 ) 
	{
		//AW_DrawTheMedals( postgameMenuInfo.numAwards );
		timer = cg.time - postgameMenuInfo.starttime;	// Setup Timer
		if (timer >= 5000)
			postgameMenuInfo.phase = 4;					// Got To Phase IV after 5 seconds

		return qfalse;
	}


	// PHASE IV
	//  Draw Medals & Draw the scoreboard
	//-----------------------------------------------
	//RPG-X: RedTechie - No awards
	
	postgameMenuInfo.phase = 4;
	//AW_DrawTheMedals( postgameMenuInfo.numAwards );
	return qtrue;
	
}


/*
=================
AW_SPPostgameMenu_Cache
=================

void AW_SPPostgameMenu_Cache( void ) {
	int			n;
	qboolean	buildscript;

	buildscript = trap_Cvar_VariableValue("com_buildscript");

	for( n = 0; n < AWARD_MAX; n++ )
	{
		trap_R_RegisterShaderNoMip( ui_medalPicNames[n] );
		trap_S_RegisterSound( ui_medalSounds[n] );
	}

	if( buildscript ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/win\n" );
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/loss\n" );
		trap_S_RegisterSound( "sound/player/announce/youwin.wav" );
	}
}
*/


/*
=================
AW_SPPostgameMenu_Cache

 This function writes the winning the winning team name if
 we are in a team game.
=================
*/
static void AW_Prepname( int index)
{
//	int		len;
	char	name[64];				// The winning team's name..  Red/Blue/ Klingon...
	char	MVPname[64];
	char	otherName[64];
	const char	*redPtr = NULL;
	const char	*bluePtr = NULL;
	char	*red_team;
	char	*blue_team;

	red_team = ingame_text[IGT_REDTEAM];
	blue_team = ingame_text[IGT_BLUETEAM];


	redPtr = CG_ConfigString( CS_RED_GROUP );
	bluePtr= CG_ConfigString( CS_BLUE_GROUP);
	if (!Q_stricmp(redPtr, ""))
		redPtr = red_team;

	if (!Q_stricmp(bluePtr, ""))
		bluePtr = blue_team;

	if (postgameMenuInfo.playerGameRank == 1)		// Blue Team Won
	{
		Com_sprintf(name, sizeof(name), "%s", bluePtr);				//Winners
		Com_sprintf(otherName, sizeof(otherName), "%s", redPtr);	//Losers
		Com_sprintf(MVPname, sizeof(name), "%s %s", bluePtr, ingame_text[IGT_MVP]);
	}
	else if (postgameMenuInfo.playerGameRank == 0)	// Red Team Won
	{
		Com_sprintf(name, sizeof(name), "%s", redPtr);				//Winners
		Com_sprintf(otherName, sizeof(otherName), "%s", bluePtr);	//Losers
		Com_sprintf(MVPname, sizeof(name), "%s %s", redPtr, ingame_text[IGT_MVP]);
	}
	else											// Teams Tied
	{
		Com_sprintf(name, sizeof(name), "%s", ingame_text[IGT_TEAMS_TIED]);
		Com_sprintf(otherName, sizeof(otherName), "%s", ingame_text[IGT_TEAMS_TIED]);
		Com_sprintf(MVPname, sizeof(name), "%s %s", ingame_text[IGT_OVERALL], ingame_text[IGT_MVP]);
	}

	Q_strncpyz( postgameMenuInfo.winTeamText,    name,    sizeof(postgameMenuInfo.winTeamText) );
	Q_strncpyz( postgameMenuInfo.losTeamText, otherName,  sizeof(postgameMenuInfo.losTeamText) );
	Q_strncpyz( postgameMenuInfo.winTeamMVPText, MVPname, sizeof(postgameMenuInfo.winTeamMVPText) );
}


/*
=================
AW_SPPostgameMenu_f

  This function essentially initializes the SPPostgameMenu_t structure
  
  Is called by the server command "awards ..."
=================
*/

void AW_SPPostgameMenu_f( void ) {
	int			playerGameRank;
	int			playerClientNum;
	int			playerTeam;
	int			n, clNum[3] = {0,0,0};
	int			awardFlags;
	int			numNames;
	char		temp_string[200];

	memset( &postgameMenuInfo, 0, sizeof(postgameMenuInfo) );


	// IMPORT AWARDS INFORMATION FROM SERVER AND CMD STRING
	//___________________________________________________________________________________________
	//
	playerClientNum = (cg.snap->ps.clientNum);
	playerGameRank  = (cg.snap->ps.persistant[PERS_RANK]&~RANK_TIED_FLAG);
	//playerTeam      = (cg.snap->ps.persistant[PERS_TEAM]);
	playerTeam      = (cgs.clientinfo[playerClientNum].team);//this should be more accurate

	postgameMenuInfo.starttime      = cg.time;					// Initialize Timers
	postgameMenuInfo.scoreboardtime = cg.time;
	postgameMenuInfo.numAwards      = 0;						// Initialize Awards Count
	postgameMenuInfo.playerGameRank = playerGameRank;			// Store Player Rank
	postgameMenuInfo.numClients     = atoi( CG_Argv( 1 ) );		// Store Number Of Clients
	postgameMenuInfo.playerTied		= (cg.snap->ps.persistant[PERS_RANK] & RANK_TIED_FLAG);

	if (playerTeam == playerGameRank+1 || playerGameRank==2)
		postgameMenuInfo.playersTeamWon = qtrue;
	else
		postgameMenuInfo.playersTeamWon = qfalse;


	for (n=0; (n<postgameMenuInfo.numClients && n<3); n++)
	{
		// Top Three Character Names
		clNum[n] = atoi(CG_Argv(2+n));
		Q_strncpyz( postgameMenuInfo.placeNames[n], cgs.clientinfo[clNum[n]].name, sizeof(postgameMenuInfo.placeNames[n]) );
	}

	numNames = n;
	awardFlags = atoi( CG_Argv(2+n) );							// Awards Bit Flag

	for (n=AWARD_EFFICIENCY; n<AWARD_MAX; n++)
	{
		if (awardFlags & 1)
		{
			//Log the type of Award
			postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = n;				
			//Log the amount
			postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 3));
			//Increment Away
			postgameMenuInfo.numAwards++;
			// kef -- Pat sez, "no more than 4 awards!"
			if (4 == postgameMenuInfo.numAwards)
			{
				break;
			}
		}
		awardFlags = awardFlags>>1;
	}

	Q_strncpyz( postgameMenuInfo.nameOfMVP, (CG_Argv(numNames + postgameMenuInfo.numAwards + 3)), sizeof(postgameMenuInfo.nameOfMVP) );
	postgameMenuInfo.scoreOfMVP    = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 4));
	postgameMenuInfo.totalCaptures = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 5));
	postgameMenuInfo.totalPoints   = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 6));
	postgameMenuInfo.playerGameRank = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 7));
	playerGameRank = postgameMenuInfo.playerGameRank;
	postgameMenuInfo.playerTied	   = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 8));
	postgameMenuInfo.losCaptures = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 9));
	postgameMenuInfo.losPoints   = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 10));
	postgameMenuInfo.secondPlaceTied = atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 11));
	VectorSet(postgameMenuInfo.intermission_origin, 
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 12)),
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 13)),
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 14)));
	VectorSet(postgameMenuInfo.intermission_angle, 
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 15)),
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 16)),
		atoi( CG_Argv(numNames + postgameMenuInfo.numAwards + 17)));
	
	cg.predictedPlayerState.pm_type = PM_INTERMISSION;
	VectorCopy(postgameMenuInfo.intermission_origin, cg.predictedPlayerState.origin);
	VectorCopy(postgameMenuInfo.intermission_angle, cg.predictedPlayerState.viewangles);


	// SOUNDS
	//___________________________________________________________________________________________
	//
	if (postgameMenuInfo.numAwards)
	{
		postgameMenuInfo.commendationsSound = trap_S_RegisterSound("sound/voice/computer/misc/commendations.wav");
	}
	else
	{
		postgameMenuInfo.commendationsSound = 0;
	}
	if ( playerGameRank != 0 )
	{	// Someone else wins
		char	*skin;

		skin = cgs.clientinfo[clNum[0]].skinName;

		if( Q_stricmp( skin, "default" ) == 0 ) {
			skin = cgs.clientinfo[clNum[0]].modelName;
		}
		if( Q_stricmp( skin, "red" ) == 0 ) {
			skin = cgs.clientinfo[clNum[0]].modelName;
		}
		if( Q_stricmp( skin, "blue" ) == 0 ) {
			skin = cgs.clientinfo[clNum[0]].modelName;
		}

		postgameMenuInfo.winnerSound = trap_S_RegisterSound( va( "sound/voice/computer/misc/%s_wins.wav", skin ) );
		if (0 == postgameMenuInfo.winnerSound)
		{
			postgameMenuInfo.winnerSound = trap_S_RegisterSound( va( "%s", "sound/voice/computer/misc/progcomp.wav" ) );
		}
		postgameMenuInfo.winnerDelay = 2500;
		if (1 == playerGameRank || postgameMenuInfo.playerTied==2)
		{
			postgameMenuInfo.youPlacedSound = trap_S_RegisterSound("sound/voice/computer/misc/2nd.wav");
		}
		else if (2 == playerGameRank || postgameMenuInfo.playerTied==3)
		{
			postgameMenuInfo.youPlacedSound = trap_S_RegisterSound("sound/voice/computer/misc/3rd.wav");
		}
		else
		{
			postgameMenuInfo.youPlacedSound = trap_S_RegisterSound("sound/voice/computer/misc/notPlace.wav");
		}

		// You lost, you get to listen to loser music.
		// This might be a bit of a downer in FFA, since far more often than not you are not the winner...
		// However, in this case the track is NOT a funeral march with an opera singer bellowing "LOSER, LOSER, LOSER, HA HA".
		// SOOOOOO for consistency's sake, you will always hear the "loss" track when you don't win.  --Pat
		trap_S_StartBackgroundTrack( "music/loss", "music/loss" );
	}
	else 									// for TEAM, and CAPTURE THE FLAG:
	{
		qboolean	bRaceSound = qfalse;

		AW_Prepname( 0);	// Set Up "xxxx Team Wins" text

		// THE VOICE
		//__________________________________________________
		if (playerGameRank == 1)
		{
			Com_sprintf(temp_string, sizeof(temp_string), "sound/voice/computer/misc/%s_wins.wav" , CG_ConfigString( CS_BLUE_GROUP ));

			postgameMenuInfo.winnerSound = trap_S_RegisterSound(temp_string);
			if (!postgameMenuInfo.winnerSound)
			{
				postgameMenuInfo.winnerSound = trap_S_RegisterSound( "sound/voice/computer/misc/blueteam_wins.wav" );
			}
			else
			{
				bRaceSound = qtrue;
			}
		}
		else if (playerGameRank == 0)
		{
			Com_sprintf(temp_string, sizeof(temp_string), "sound/voice/computer/misc/%s_wins.wav" , CG_ConfigString( CS_RED_GROUP ));

			postgameMenuInfo.winnerSound = trap_S_RegisterSound(temp_string);
			if (!postgameMenuInfo.winnerSound)
			{
				postgameMenuInfo.winnerSound = trap_S_RegisterSound( "sound/voice/computer/misc/redteam_wins.wav" );
			}
			else
			{
				bRaceSound = qtrue;
			}
		}
		else if (playerGameRank == 2)
		{
			postgameMenuInfo.winnerSound = trap_S_RegisterSound( "sound/voice/computer/misc/teamstied.wav" );
		}
		if (!bRaceSound && (playerTeam == playerGameRank+1)	)
		{
			postgameMenuInfo.winnerSound = trap_S_RegisterSound( "sound/voice/computer/misc/yourteam_wins.wav" );
		}

		if (bRaceSound)
		{
			postgameMenuInfo.winnerDelay = 4000;
		}
		else
		{
			postgameMenuInfo.winnerDelay = 1000;
		}

		// THE MUSIC   
		//__________________________________________________
		if (postgameMenuInfo.playersTeamWon) {
			trap_S_StartBackgroundTrack( "music/win", "music/win" );
		} else {
			trap_S_StartBackgroundTrack( "music/loss", "music/loss" );
		}
	}

	//Set To Phase I
	postgameMenuInfo.phase = 1;

}
