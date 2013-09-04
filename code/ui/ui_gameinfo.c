// Copyright (C) 1999-2000 Id Software, Inc.
//
//
// gameinfo.c
//

#include "ui_local.h"
#include "ui_logger.h"


//
// arena and bot info
//

#define DIRLIST_SIZE 	16384
#define POOLSIZE	48 * DIRLIST_SIZE

//#define POOLSIZE	128 * 1024

int32_t				ui_numBots;
static char		*ui_botInfos[MAX_BOTS];

static int32_t		ui_numArenas;
static char		*ui_arenaInfos[MAX_ARENAS];

static int32_t		ui_numSinglePlayerArenas;
static int32_t		ui_numSpecialSinglePlayerArenas;

static char		dirlist[DIRLIST_SIZE];

static char		memoryPool[POOLSIZE];
static int32_t		allocPoint, outOfMemory;


/*
===============
UI_Alloc
===============
*/
void *UI_Alloc( int32_t size ) {
	UI_LogFuncBegin();
	char	*p;

	if ( allocPoint + size > POOLSIZE ) {
		outOfMemory = qtrue;
		UI_LogFuncEnd();
		return NULL;
	}

	p = &memoryPool[allocPoint];

	allocPoint += ( size + 31 ) & ~31;

	UI_LogFuncEnd();
	return p;
}

/*
===============
UI_InitMemory
===============
*/
void UI_InitMemory( void ) {
	UI_LogFuncBegin();
	allocPoint = 0;
	outOfMemory = qfalse;
	UI_LogFuncEnd();
}

/*
===============
UI_ParseInfos
===============
*/
int32_t UI_ParseInfos( char *buf, int32_t max, char *infos[] ) {
	UI_LogFuncBegin();
	char	*token;
	int32_t		count;
	char	key[MAX_TOKEN_CHARS];
	char	info[MAX_INFO_STRING];

	count = 0;

	while ( 1 ) {
		token = COM_Parse( &buf );
		if ( !token[0] ) {
			break;
		}
		if ( strcmp( token, "{" ) ) {
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == max ) {
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		info[0] = '\0';
		while ( 1 ) {
			token = COM_ParseExt( &buf, qtrue );
			if ( !token[0] ) {
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) {
				break;
			}
			Q_strncpyz( key, token, sizeof( key ) );

			token = COM_ParseExt( &buf, qfalse );
			if ( !token[0] ) {
				strcpy( token, "<NULL>" );
			}
			Info_SetValueForKey( info, key, token );
		}
		//NOTE: extra space for arena number
		infos[count] = UI_Alloc(strlen(info) + strlen("\\num\\") + strlen(va("%d", MAX_ARENAS)) + 1);
		if (infos[count]) {
			strcpy(infos[count], info);
			count++;
		}
	}
	UI_LogFuncEnd();
	return count;
}

/*
===============
UI_LoadArenasFromFile
===============
*/
static void UI_LoadArenasFromFile( char *filename ) {
	UI_LogFuncBegin();
	int32_t				len;
	fileHandle_t	f;
	char			buf[MAX_ARENAS_TEXT];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "file not found: %s\n", filename ) );
		UI_LogFuncEnd();
		return;
	}
	if ( len >= MAX_ARENAS_TEXT ) {
		trap_Print( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_ARENAS_TEXT ) );
		trap_FS_FCloseFile( f );
		UI_LogFuncEnd();
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ui_numArenas += UI_ParseInfos( buf, MAX_ARENAS - ui_numArenas, &ui_arenaInfos[ui_numArenas] );
}


/*
===============
UI_LoadArenas
===============
*/
static void UI_LoadArenas( void ) {
	UI_LogFuncBegin();
	int32_t			numdirs;
	vmCvar_t	arenasFile;
	char		filename[128];
	char*		dirptr;
	int32_t			i, n;
	int32_t			dirlen;
	char		*type;
	char		*tag;
	int32_t			singlePlayerNum, specialNum, otherNum;

	ui_numArenas = 0;

	trap_Cvar_Register( &arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM );
	if( *arenasFile.string )
	{
		UI_LoadArenasFromFile(arenasFile.string);
	}
	else
	{
		UI_LoadArenasFromFile("scripts/arenas.txt");
	}

	// get all arenas from .arena files
	numdirs = trap_FS_GetFileList("scripts", ".arena", dirlist, DIRLIST_SIZE );
	dirptr  = dirlist;
	for (i = 0; i < numdirs && ui_numArenas < 1024; i++, dirptr += dirlen+1)
	{
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		UI_LoadArenasFromFile(filename);
	}
	trap_Print( va( "%i arenas parsed\n", ui_numArenas ) );
	if (outOfMemory) trap_Print(S_COLOR_YELLOW"WARNING: not enough memory in pool to load all arenas\n");

	// set initial numbers
	for( n = 0; n < ui_numArenas; n++ ) {
		Info_SetValueForKey( ui_arenaInfos[n], "num", va( "%i", n ) );
	}

	// go through and count single players levels
	ui_numSinglePlayerArenas = 0;
	ui_numSpecialSinglePlayerArenas = 0;
	for( n = 0; n < ui_numArenas; n++ ) {
		// determine type
		type = Info_ValueForKey( ui_arenaInfos[n], "type" );

		// if no type specified, it will be treated as "ffa"
		if( !*type ) {
			continue;
		}

		if( strstr( type, "single" ) ) {
			// check for special single player arenas (training, final)
			tag = Info_ValueForKey( ui_arenaInfos[n], "special" );
			if( *tag ) {
				ui_numSpecialSinglePlayerArenas++;
				continue;
			}

			ui_numSinglePlayerArenas++;
		}
	}

	// go through once more and assign number to the levels
	singlePlayerNum = 0;
	specialNum = singlePlayerNum + ui_numSinglePlayerArenas;
	otherNum = specialNum + ui_numSpecialSinglePlayerArenas;
	for( n = 0; n < ui_numArenas; n++ ) {
		// determine type
		type = Info_ValueForKey( ui_arenaInfos[n], "type" );

		// if no type specified, it will be treated as "ffa"
		if( *type ) {
			if( strstr( type, "single" ) ) {
				// check for special single player arenas (training, final)
				tag = Info_ValueForKey( ui_arenaInfos[n], "special" );
				if( *tag ) {
					Info_SetValueForKey( ui_arenaInfos[n], "num", va( "%i", specialNum++ ) );
					continue;
				}

				Info_SetValueForKey( ui_arenaInfos[n], "num", va( "%i", singlePlayerNum++ ) );
				continue;
			}
		}

		Info_SetValueForKey( ui_arenaInfos[n], "num", va( "%i", otherNum++ ) );
	}
}

/*
===============
UI_GetArenaInfoByNumber
===============
*/
const char *UI_GetArenaInfoByNumber( int32_t num ) {
	UI_LogFuncBegin();
	int32_t		n;
	char	*value;

	if( num < 0 || num >= ui_numArenas ) {
		trap_Print( va( S_COLOR_RED "Invalid arena number: %i\n", num ) );
		UI_LogFuncEnd();
		return NULL;
	}

	for( n = 0; n < ui_numArenas; n++ ) {
		value = Info_ValueForKey( ui_arenaInfos[n], "num" );
		if( *value && atoi(value) == num ) {
			UI_LogFuncEnd();
			return ui_arenaInfos[n];
		}
	}
	UI_LogFuncEnd();
	return NULL;
}


/*
===============
UI_GetArenaInfoByMap
===============
*/
const char *UI_GetArenaInfoByMap( const char *map ) {
	UI_LogFuncBegin();
	int32_t			n;

	for( n = 0; n < ui_numArenas; n++ ) {
		if( Q_stricmp( Info_ValueForKey( ui_arenaInfos[n], "map" ), map ) == 0 ) {
			UI_LogFuncEnd();
			return ui_arenaInfos[n];
		}
	}
	UI_LogFuncEnd();
	return NULL;
}


/*
===============
UI_GetSpecialArenaInfo
===============
*/
const char *UI_GetSpecialArenaInfo( const char *tag ) {
	UI_LogFuncBegin();
	int32_t			n;

	for( n = 0; n < ui_numArenas; n++ ) {
		if( Q_stricmp( Info_ValueForKey( ui_arenaInfos[n], "special" ), tag ) == 0 ) {
			UI_LogFuncEnd();
			return ui_arenaInfos[n];
		}
	}
	UI_LogFuncEnd();
	return NULL;
}

/*
===============
UI_LoadBotsFromFile
===============
*/
static void UI_LoadBotsFromFile( char *filename ) {
	UI_LogFuncBegin();
	int32_t				len;
	fileHandle_t	f;
	char			buf[MAX_BOTS_TEXT];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "file not found: %s\n", filename ) );
		UI_LogFuncEnd();
		return;
	}
	if ( len >= MAX_BOTS_TEXT ) {
		trap_Print( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_BOTS_TEXT ) );
		trap_FS_FCloseFile( f );
		UI_LogFuncEnd();
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ui_numBots += UI_ParseInfos( buf, MAX_BOTS - ui_numBots, &ui_botInfos[ui_numBots] );
	if (outOfMemory) trap_Print(S_COLOR_YELLOW"WARNING: not enough memory in pool to load all bots\n");
	UI_LogFuncEnd();
}


/*
===============
UI_LoadBots
===============
*/
static void UI_LoadBots( void ) {
	UI_LogFuncBegin();
	vmCvar_t	botsFile;
	int32_t			numdirs;
	char		filename[128];
	char*		dirptr;
	int32_t			i;
	int32_t			dirlen;

	ui_numBots = 0;

	trap_Cvar_Register( &botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM );
	if( *botsFile.string ) {
		UI_LoadBotsFromFile(botsFile.string);
	}
	else {
		UI_LoadBotsFromFile("scripts/bots.txt");
	}

	// get all bots from .bot files
	numdirs = trap_FS_GetFileList("scripts", ".bot", dirlist, DIRLIST_SIZE );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1)
	{
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		UI_LoadBotsFromFile(filename);
	}
	trap_Print( va( "%i bots parsed\n", ui_numBots ) );
	UI_LogFuncEnd();
}


/*
===============
UI_GetBotInfoByNumber
===============
*/
char *UI_GetBotInfoByNumber( int32_t num ) {
	UI_LogFuncBegin();
	if( num < 0 || num >= ui_numBots ) {
		trap_Print( va( S_COLOR_RED "Invalid bot number: %i\n", num ) );
		UI_LogFuncEnd();
		return NULL;
	}
	UI_LogFuncEnd();
	return ui_botInfos[num];
}


/*
===============
UI_GetBotInfoByName
===============
*/
char *UI_GetBotInfoByName( const char *name ) {
	UI_LogFuncBegin();
	int32_t		n;
	char	*value;

	for ( n = 0; n < ui_numBots ; n++ ) {
		value = Info_ValueForKey( ui_botInfos[n], "name" );
		if ( !Q_stricmp( value, name ) ) {
			UI_LogFuncEnd();
			return ui_botInfos[n];
		}
	}
	UI_LogFuncEnd();
	return NULL;
}


//
// single player game info
//

/*
===============
UI_GetBestScore

Returns the player's best finish on a given level, 0 if the have not played the level
===============
*/
void UI_GetBestScore( int32_t level, int32_t *score, int32_t *skill ) {
	UI_LogFuncBegin();
	int32_t		n;
	int32_t		skillScore;
	int32_t		bestScore;
	int32_t		bestScoreSkill;
	char	arenaKey[16];
	char	scores[MAX_INFO_VALUE];

	if( !score || !skill ) {
		UI_LogFuncEnd();
		return;
	}

	if( level < 0 || level > ui_numArenas ) {
		UI_LogFuncEnd();
		return;
	}

	bestScore = 0;
	bestScoreSkill = 0;

	for( n = 1; n <= 5; n++ ) {
		trap_Cvar_VariableStringBuffer( va( "g_spScores%i", n ), scores, MAX_INFO_VALUE );

		Com_sprintf( arenaKey, sizeof( arenaKey ), "l%i", level );
		skillScore = atoi( Info_ValueForKey( scores, arenaKey ) );

		if( skillScore < 1 || skillScore > 8 ) {
			continue;
		}

		if( !bestScore || skillScore <= bestScore ) {
			bestScore = skillScore;
			bestScoreSkill = n;
		}
	}

	*score = bestScore;
	*skill = bestScoreSkill;
	UI_LogFuncEnd();
}


/*
===============
UI_SetBestScore

Set the player's best finish for a level
===============
*/
void UI_SetBestScore( int32_t level, int32_t score ) {
	UI_LogFuncBegin();
	int32_t		skill;
	int32_t		oldScore;
	char	arenaKey[16];
	char	scores[MAX_INFO_VALUE];

	// validate score
	if( score < 1 || score > 8 ) {
		UI_LogFuncEnd();
		return;
	}

	// validate skill
	skill = (int32_t)trap_Cvar_VariableValue( "g_spSkill" );
	if( skill < 1 || skill > 5 ) {
		UI_LogFuncEnd();
		return;
	}

	// get scores
	trap_Cvar_VariableStringBuffer( va( "g_spScores%i", skill ), scores, MAX_INFO_VALUE );

	// see if this is better
	Com_sprintf( arenaKey, sizeof( arenaKey ), "l%i", level );
	oldScore = atoi( Info_ValueForKey( scores, arenaKey ) );
	if( oldScore && oldScore <= score ) {
		UI_LogFuncEnd();
		return;
	}

	// update scores
	Info_SetValueForKey( scores, arenaKey, va( "%i", score ) );
	trap_Cvar_Set( va( "g_spScores%i", skill ), scores );
	UI_LogFuncEnd();
}


/*
===============
UI_LogAwardData
===============
*/
void UI_LogAwardData( int32_t award, int32_t data ) {
	UI_LogFuncBegin();
	char	key[16];
	char	awardData[MAX_INFO_VALUE];
	int32_t		oldValue;

	if( data == 0 ) {
		UI_LogFuncEnd();
		return;
	}

	if( award >= AWARD_MAX ) {
		trap_Print( va( S_COLOR_RED "Bad award %i in UI_LogAwardData\n", award ) );
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_VariableStringBuffer( "g_spAwards", awardData, sizeof(awardData) );

	Com_sprintf( key, sizeof(key), "a%i", award );
	oldValue = atoi( Info_ValueForKey( awardData, key ) );

	Info_SetValueForKey( awardData, key, va( "%i", oldValue + data ) );
	trap_Cvar_Set( "g_spAwards", awardData );
}


/*
===============
UI_GetAwardLevel
===============
*/
int32_t UI_GetAwardLevel( int32_t award ) {
	UI_LogFuncBegin();
	char	key[16];
	char	awardData[MAX_INFO_VALUE];

	trap_Cvar_VariableStringBuffer( "g_spAwards", awardData, sizeof(awardData) );

	Com_sprintf( key, sizeof(key), "a%i", award );
	UI_LogFuncEnd();
	return atoi( Info_ValueForKey( awardData, key ) );
}


/*
===============
UI_TierCompleted
===============
*/
int32_t UI_TierCompleted( int32_t levelWon ) {
	UI_LogFuncBegin();
	int32_t			level;
	int32_t			n;
	int32_t			tier;
	int32_t			score;
	int32_t			skill;
	int32_t			maxarenas;
	const char	*info;

	tier = levelWon / ARENAS_PER_TIER;
	level = tier * ARENAS_PER_TIER;

	if( tier == UI_GetNumSPTiers() ) {
		info = UI_GetSpecialArenaInfo( "training" );
		if( levelWon == atoi( Info_ValueForKey( info, "num" ) ) ) {
			UI_LogFuncEnd();
			return 0;
		}
		info = UI_GetSpecialArenaInfo( "final" );
		if( !info || levelWon == atoi( Info_ValueForKey( info, "num" ) ) ) {
			UI_LogFuncEnd();
			return tier + 1;
		}
		UI_LogFuncEnd();
		return -1;
	}

	if (uis.demoversion) {
		maxarenas = 2;
	} else {
		maxarenas = ARENAS_PER_TIER;
	}
	for( n = 0; n < maxarenas; n++, level++ ) {
		UI_GetBestScore( level, &score, &skill );
		if ( score != 1 ) {
			UI_LogFuncEnd();
			return -1;
		}
	}
	UI_LogFuncEnd();
	return tier + 1;
}



/*
===============
UI_GetCurrentGame

Returns the next level the player has not won
===============
*/
int32_t UI_GetCurrentGame( int32_t curLevel ) {
	UI_LogFuncBegin();
	int32_t		level;
	int32_t		rank;
	int32_t		skill;
	const char *info;

	info = UI_GetSpecialArenaInfo( "training" );
	if( info ) {
		level = atoi( Info_ValueForKey( info, "num" ) );
		UI_GetBestScore( level, &rank, &skill );
		if ( !rank || rank > 1 ) {
			UI_LogFuncEnd();
			return level;
		}
	}

	// kef 7/31/00 -- we think we'd like to just send you to the next map, not the first map you haven't won
	
	if (curLevel == -1)
	{
		// -1 is a special value, the meaning of which is not currently clear to me
		for( level = 0; level < ui_numSinglePlayerArenas; level++ ) {
			UI_GetBestScore( level, &rank, &skill );
			if ( !rank || rank > 1 )
			{
				UI_LogFuncEnd();
				return level;
			}
		}
		info = UI_GetSpecialArenaInfo( "final" );
		if( !info ) {
			UI_LogFuncEnd();
			return -1;
		}
		UI_LogFuncEnd();
		return atoi( Info_ValueForKey( info, "num" ) );
	}
	else if ((curLevel >= 0) && (curLevel < (ui_numSinglePlayerArenas-1)))
	{
		// go to next map
		UI_LogFuncEnd();
		return curLevel+1;
	}
	else if (curLevel == (ui_numSinglePlayerArenas-1))
	{
		// finished final map...back to the beginning
		UI_LogFuncEnd();
		return 0;
	}
	else
	{
		// bogus value for curLevel
		info = UI_GetSpecialArenaInfo( "final" );
		if( !info ) {
			UI_LogFuncEnd();
			return -1;
		}
		UI_LogFuncEnd();
		return atoi( Info_ValueForKey( info, "num" ) );
	}
	UI_LogFuncEnd();
}


/*
===============
UI_NewGame

Clears the scores and sets the difficutly level
===============
*/
void UI_NewGame( void ) {
	UI_LogFuncBegin();
	trap_Cvar_Set( "g_spScores1", "" );
	trap_Cvar_Set( "g_spScores2", "" );
	trap_Cvar_Set( "g_spScores3", "" );
	trap_Cvar_Set( "g_spScores4", "" );
	trap_Cvar_Set( "g_spScores5", "" );
	trap_Cvar_Set( "g_spAwards", "" );
	trap_Cvar_Set( "g_spVideos", "" );
	UI_LogFuncEnd();
}


/*
===============
UI_GetNumArenas
===============
*/
int32_t UI_GetNumArenas( void ) {
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return ui_numArenas;
}


/*
===============
UI_GetNumSPArenas
===============
*/
int32_t UI_GetNumSPArenas( void ) {
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return ui_numSinglePlayerArenas;
}


/*
===============
UI_GetNumSPTiers
===============
*/
int32_t UI_GetNumSPTiers( void ) 
{
	UI_LogFuncBegin();
	int32_t remainder;

	remainder = ui_numSinglePlayerArenas % ARENAS_PER_TIER;

	if (remainder)
	{
		remainder = 1;
	}

	UI_LogFuncEnd();
	return ((ui_numSinglePlayerArenas / ARENAS_PER_TIER) + remainder);
}


/*
===============
UI_GetNumBots
===============
*/
int32_t UI_GetNumBots( void ) {
	UI_LogFuncBegin();
	UI_LogFuncEnd();
	return ui_numBots;
}


/*
===============
UI_InitGameinfo
===============
*/
void UI_InitGameinfo( void ) {
	UI_LogFuncBegin();
	UI_InitMemory();
	UI_LoadArenas();
	UI_LoadBots();

	if( (trap_Cvar_VariableValue( "fs_restrict" )) || (ui_numSpecialSinglePlayerArenas == 0 && ui_numSinglePlayerArenas == 2) ) {
		uis.demoversion = qtrue;
	}
	else {
		uis.demoversion = qfalse;
	}
	UI_LogFuncEnd();
}
