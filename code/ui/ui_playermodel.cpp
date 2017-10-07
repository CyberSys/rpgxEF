// Copyright (C) 1999-2000 Id Software, Inc.
//
//=================================================
//
// TiM: Just a reference for my whacky jargon in here
// Character	= the player model as a whole group (ie kulhane )
// Model		= the .model file used to build the character ( ie admiral, cadet etc)
// Skin			= the skinset field in the .model file, dictating which .skins to use (ie red/teal/gold... )
// ADDENDUM:	Skin = the .skinset file linked to the .model file
//
//=================================================
#include "ui_local.h"
#include "ui_logger.h"

static const char PIC_ARROW_UP[] = "menu/common/arrow_up_16.tga";
static const char PIC_ARROW_DOWN[] = "menu/common/arrow_dn_16.tga";

enum ui_playermodelLimits_e {
	MIN_SCROLLHEIGHT = 8,
	MAX_MENULISTITEMS = 12,
	MAX_PLAYERNAMELENGTH = 21,
	MAX_PLAYERSKINS = 32,
	MAX_RACES = 64,
	MAX_GENDERS = 64,
	MAX_PLAYERMODELS = 64,
	MAX_SCROLLTOP = 108,
	MAX_SCROLLRANGE = 236,
	MAX_PLAYERCHARS = 256,
	LOW_MEMORY = (5 * 1024 * 1024)
};

static void PlayerModel_BuildList(void);
static void PlayerModel_SetMenuItems(void);
static void PlayerModel_MenuInit(int32_t menuFrom);

enum ui_playermodelIDs_e {
	ID_MENUCHAR0,
	ID_MENUCHAR1,
	ID_MENUCHAR2,
	ID_MENUCHAR3,
	ID_MENUCHAR4,
	ID_MENUCHAR5,
	ID_MENUCHAR6,
	ID_MENUCHAR7,
	ID_MENUCHAR8,
	ID_MENUCHAR9,
	ID_MENUCHAR10,
	ID_MENUCHAR11,

	ID_SETTINGS = 20,

	ID_UPARROW = 100,
	ID_DNARROW,
	ID_BACK,
	ID_MAINMENU,
	ID_INGAMEMENU,
	ID_APPLY,
	ID_MODELSET,
	ID_SKINSET,
	ID_RACEFILTER,
	ID_GENDERFILTER,
	ID_SCROLLBAR
};

typedef struct
{
	int32_t		orgChar;
	int32_t		orgModel;
	int32_t		orgSkin;
} storedData_t;

typedef struct
{
	char	charName[36];
	int32_t		race;
	int32_t		gender;
	int32_t		index;
} charData_t;

typedef struct
{
	qboolean		mouseDown;
	qboolean		doubleStep;

	int32_t				yStart;
} scrollData_t;

typedef struct
{
	char	filterName[32];
	int32_t		filterIndex;
} filterData_t;

typedef struct
{
	menuframework_s	menu;
	int32_t				prevMenu;
	menubitmap_s	mainmenu;
	menubitmap_s	back;
	menubitmap_s	player;

	menubitmap_s	charMenu[MAX_MENULISTITEMS];

	menubitmap_s	upArrow;
	menubitmap_s	dnArrow;

	menulist_s		charModel;
	menulist_s		charSkin;

	menulist_s		raceFilter;
	menulist_s		genderFilter;

	menubitmap_s	apply;

	menubitmap_s	data;
	menubitmap_s	model;

	menuaction_s	scrollBar;

	qhandle_t		corner_ll_4_4;
	qhandle_t		corner_ll_4_18;
	qhandle_t		corner_lr_4_18;
	qhandle_t		corner_lr_18_4;
	qhandle_t		corner_ur_18_18;

	qhandle_t		playerIcon;

	//menutext_s		modelname;
	//menutext_s		skinname;
	//menutext_s		skinnameviewed;
	menutext_s		playername;
	playerInfo_t	playerinfo;

	int32_t				numChars;
	charData_t		charNames[MAX_PLAYERCHARS]/*[128]*/;
	char			modelNames[MAX_PLAYERMODELS][32];
	char			skinNames[MAX_PLAYERMODELS][MAX_PLAYERSKINS][32];
	//To save loading time, and the fact that loading data from txt files on the fly per spin cycle, 
	//can sometimes overload EF if the user gets button happy, We'll make it all data is loaded and consolidated on character choosing

	//TiM - I think this is wasteful as all heck, but it can't be helped
	//The model functions need the data stored as uniform char arrays,
	//and the spin controls need them stored as char pointers. O_o
	//But not only that, but since these are memory pointers, applying strUpr
	//To them destroys the case in the arrays above.
	//Hacky this may be, but there's no way around it without some totally Carmack-coding
	//Raven had to do this too, so it's an inherent flaw in Q3 coding
	char			charNamesUpr[MAX_PLAYERCHARS][64];
	char			modelNamesUpr[MAX_PLAYERMODELS][64];
	char			skinNamesUpr[MAX_PLAYERMODELS][MAX_PLAYERSKINS][64];

	char*			modelList[MAX_PLAYERMODELS];
	char*			skinList[MAX_PLAYERSKINS];

	char			modelData[64];
	char			modelName[32];

	//char			raceNames[MAX_RACES][32];
	filterData_t	raceList[MAX_RACES];
	char*			raceNames[MAX_RACES];
	int32_t				numRaces;

	filterData_t	genderList[MAX_GENDERS];
	char*			genderNames[MAX_GENDERS];
	int32_t				numGenders;

	int32_t				selectedChar;
	int32_t				scrollOffset;

	storedData_t	storedData;		//Original Skin Data

	scrollData_t	scrollData;
} playermodel_t;

static playermodel_t s_playermodel;

static int32_t QDECL FilterList_Compare(const void *ptr1, const void *ptr2);
static int32_t QDECL CharMediaList_Compare(const void *ptr1, const void *ptr2);
static void PlayerModel_DrawLoading(void);

/*
=================
PlayerModel_CheckInArray
TiM: When building an array of races etc,
run it through this list to prevent multiple redundancy
Mental Note: Study Pointer Arithmetic more....
=================
*/

static int32_t PlayerModel_CheckInFilter(char* string, filterData_t *filter, int32_t width, int32_t *num) {
	int32_t		i = 0;

	UI_LogFuncBegin();

	while (filter[i].filterName[0] && i < width) {
		if (!Q_stricmp(filter[i].filterName, string)) {
			//UI_Logger( LL_ERROR, "String %s already in cell %i\n", array[i], i );
			UI_LogFuncEnd();
			return i;
		}

		i++;
	}

	if (!filter[i].filterName[0] && i < width)
	{
		Q_strncpyz(filter[i].filterName, string, 32);
		filter[i].filterIndex = i;

		*num = *num + 1; //aah I see. You can't call *num++ since that increments the address, not the value XD
	}

	//UI_Logger( LL_DEBUG, "Added %s to cell %i", array[i], i );

	UI_LogFuncEnd();
	return i;
}

/*
=================
PlayerModel_LoadAvailableModels
TiM: Loads a list of all the .model
files there are in a character's directory.

NB: In Spin Control menu types, the number
of elements is calced on init only.  Each
time we refresh this, we'll need to update ourselves.
=================
*/
static int32_t PlayerModel_LoadAvailableModels(void) {
	int32_t		i;
	int32_t		j;
	int32_t		numFiles;
	char	fileList[4096]; //Hopefully, this will never be exceeded ROFL
	char*	filePtr;
	int32_t		fileLen;
	int32_t		strLen;
	char*	temp;
	char	fileRoute[MAX_QPATH];

	UI_LogFuncBegin();

	if (s_playermodel.selectedChar == -1){
		UI_LogFuncEnd();
		return -1;
	}

	Com_sprintf(fileRoute, MAX_QPATH,
		"models/players_rpgx/%s",
		s_playermodel.charNames[s_playermodel.selectedChar].charName);

	//Get our num files
	memset(&fileList, 0, sizeof (fileList));
	numFiles = trap_FS_GetFileList(fileRoute, ".model", fileList, sizeof(fileList));

	if (numFiles <= 0){
		UI_LogFuncEnd();
		return -1;
	}

	//Convert to ptr for easier manip
	filePtr = fileList;

	memset(&s_playermodel.modelNames, 0, sizeof(s_playermodel.modelNames));
	memset(&s_playermodel.modelNamesUpr, 0, sizeof(s_playermodel.modelNamesUpr));
	memset(s_playermodel.modelList, 0, sizeof(s_playermodel.modelList));

	//iterate thru all the null terminations in this thing
	/**
	* RPG-X | Phenix | 27/03/2007
	* Made for loop in reverse to correctly order the models. (Task#39)
	*/
	j = 0;
	for (i = 0; j < MAX_PLAYERMODELS && i < numFiles; i++, filePtr += fileLen + 1) {
		fileLen = strlen(filePtr);

		if (!fileLen || !filePtr)
			break;

		//TiM - this shouldn't be possible
		if (strchr(filePtr, '/') || strchr(filePtr, '\\'))
		{
			continue;
		}

		temp = filePtr;

		//TiM: Strip extension
		strLen = strlen(temp);
		if (strLen > 6 && !Q_stricmp(temp + strLen - 6, ".model")) {
			temp[strLen - 6] = '\0';
		}

		Q_strncpyz(s_playermodel.modelNames[j], temp, sizeof (s_playermodel.modelNames[j]));
		j++;
	}

	//sort the models, then feed them to the rest of the variables
	qsort(s_playermodel.modelNames, j, sizeof(s_playermodel.modelNames[0]), CharMediaList_Compare);

	for (i = 0; i < j; i++)
	{
		//TiM: A bit of a hacky ovveride here.
		//Make it so 'main' is set by default
		if (!Q_stricmp(s_playermodel.modelNames[i], DEFAULT_MODEL)) {
			s_playermodel.charModel.curvalue = i;
		}

		Q_strncpyz(s_playermodel.modelNamesUpr[i], s_playermodel.modelNames[i], sizeof(s_playermodel.modelNamesUpr[i]));
		s_playermodel.modelList[i] = Q_strupr(s_playermodel.modelNamesUpr[i]);
	}

	UI_LogFuncEnd();
	return j;
}

/*
================
PlayerModel_LoadAvailableSkins
TiM: Access our selected .model file, get the skin name framework and
fill the skin array with all the skins we found.

Hoi... this could get complicated... O_o
================
*/
static void PlayerModel_LoadAvailableSkins(void) {
	int32_t				j = 0;
	int32_t				i = 0;
	int32_t				fileLen;
	char			fileBuffer[20000];
	int32_t				numFiles;
	char			fileListBuffer[10000];
	char*			filePtr;
	fileHandle_t	f;
	char			filePath[MAX_QPATH];
	char*			token;
	char			skinSetFrame[MAX_QPATH];
	int32_t				strLen;
	char*			star;
	int32_t				numSkins = 0;
	int32_t				starFlags;
	char			skins[MAX_PLAYERSKINS][64];

	UI_LogFuncBegin();

	if (s_playermodel.selectedChar == -1){
		UI_LogFuncEnd();
		return;
	}

	/*memset( s_playermodel.skinNames,	0, sizeof( s_playermodel.skinNames ) );
	memset( s_playermodel.skinNamesUpr,	0, sizeof( s_playermodel.skinNamesUpr ) );
	*/

	//TiM: Ugh, this is the only way to ensure completely flushing out the skins list on each model
	for (i = 0; i < MAX_PLAYERMODELS; i++)
	{
		for (j = 0; j < MAX_PLAYERSKINS; j++)
		{
			s_playermodel.skinNames[i][j][0] = '\0';
			s_playermodel.skinNamesUpr[i][j][0] = '\0';
		}
	}

	for (i = 0; i < MAX_PLAYERSKINS; i++)
	{
		s_playermodel.skinList[i] = NULL;
		skins[i][0] = '\0';
	}

	Com_sprintf(filePath, MAX_QPATH, "models/players_rpgx/%s", s_playermodel.charNames[s_playermodel.selectedChar].charName);

	//first load the list of .skinset files we have
	numFiles = trap_FS_GetFileList(filePath,
		".skinset", fileListBuffer, sizeof(fileListBuffer));

	if (numFiles <= 0)
	{
		UI_Logger(LL_ERROR, "No Skinset files found!\n");
		UI_LogFuncEnd();
		return;
	}

	filePtr = fileListBuffer;

	j = 0;
	for (i = 0; i < MAX_PLAYERSKINS && i < numFiles; i++, filePtr += strLen + 1)
	{
		strLen = strlen(filePtr);

		if (!strLen)
			break;

		//TiM - this shouldn't be possible
		if (strchr(filePtr, '/') || strchr(filePtr, '\\'))
		{
			continue;
		}

		token = filePtr;

		if (strLen > 8 && !Q_stricmp(token + (strLen - 8), ".skinset"))
		{
			token[strLen - 8] = '\0';
		}

		Q_strncpyz(skins[j], token, sizeof(skins[j]));

		//UI_Logger( LL_DEBUG, "%s\n", skins[j] );
		j++;
	}
	//UI_Logger( LL_DEBUG, "Model Break\n", skins[j] );

	//UI_Logger( LL_DEBUG, "%s\n", s_playermodel.modelNames[2] );
	j = 0;
	while (j < MAX_PLAYERMODELS)
	{
		if (!s_playermodel.modelNames[j][0]){
			UI_LogFuncEnd();
			break;
		}

		Com_sprintf(filePath, sizeof(filePath),
			"models/players_rpgx/%s/%s.model",
			s_playermodel.charNames[s_playermodel.selectedChar].charName,
			s_playermodel.modelNames[j]);

		//load the .model data into our active buffer
		fileLen = trap_FS_FOpenFile(filePath, &f, FS_READ);

		if (fileLen <= 0) {
			UI_Logger(LL_ERROR, "File not found: %s\n", filePath);
			UI_LogFuncEnd();
			return;
		}

		if (fileLen > 20000) {
			UI_Logger(LL_ERROR, "File exceeded maximum size: %s\n", filePath);
			UI_LogFuncEnd();
			return;
		}

		memset(&fileBuffer, 0, sizeof(fileBuffer));
		trap_FS_Read(fileBuffer, sizeof(fileBuffer), f);

		trap_FS_FCloseFile(f);

		if (!fileBuffer[0]){
			UI_LogFuncEnd();
			return;
		}

		filePtr = fileBuffer;

		COM_BeginParseSession();

		memset(skinSetFrame, 0, sizeof(skinSetFrame));
		while (1) {

			token = COM_Parse(&filePtr);
			if (!token || !filePtr)
				break;

			if (!Q_stricmp(token, "skinSet")) {
				if (COM_ParseString(&filePtr, &token)) {
					continue;
				}

				Q_strncpyz(skinSetFrame, token, sizeof(skinSetFrame));
				break;
			}
		}

		//set a default one if nothing found
		if (!skinSetFrame[0])
		{
			Com_sprintf(skinSetFrame, sizeof(skinSetFrame), "%s_*", s_playermodel.modelNames[j]);
		}

		if (!strchr(skinSetFrame, '*'))
		{
			UI_Logger(LL_ERROR, "No '*' in skinset define for character: %s/%s\n", s_playermodel.charNames[s_playermodel.selectedChar].charName, s_playermodel.modelNames[j]);
			continue;
		}

		//okay... this is tricky.
		//basically, we're starting off with a possible
		//"main_*, "*_main", or "ma_*_in" set up, where we have to see
		//if a file name like "red_main" is valid, and if it is, isolate the "red" from it
		star = strstr(skinSetFrame, "*");

		////star is at the end
		if ((int32_t)(star - skinSetFrame) + 1 == (int32_t)strlen(skinSetFrame))
		{
			Q_strncpyz(filePath, skinSetFrame, sizeof(filePath));
			filePath[strlen(filePath) - 1] = '\0';

			starFlags = 1;
		}
		//star is at the front
		else if ((int32_t)(star - skinSetFrame) == 0)
		{
			star++; //QVMNOTE
			Q_strncpyz(filePath, star, sizeof(filePath));
			starFlags = 2;
		}
		else
			starFlags = 0;

		for (i = 0; i < MAX_PLAYERSKINS; i++)
		{
			if (!skins[i] || !skins[i][0])
				break;

			////star is at the end
			if (starFlags == 1)
			{
				if (strstr(skins[i], filePath) != NULL)
				{
					Q_strncpyz(s_playermodel.skinNames[j][numSkins], skins[i] + strlen(filePath), sizeof(s_playermodel.skinNames[j][numSkins]));
					//Q_strncpyz( s_playermodel.skinNamesUpr[j][numSkins], skins[i] + strlen(filePath), sizeof( s_playermodel.skinNamesUpr[j][numSkins] ) );
					numSkins++;
				}
			}
			//star is at the front
			else if (starFlags == 2)
			{
				if ((token = strstr(skins[i], filePath)) != NULL)
				{
					Q_strncpyz(s_playermodel.skinNames[j][numSkins], skins[i], (int32_t)(strlen(skins[i]) - strlen(token)) + 1);
					//Q_strncpyz( s_playermodel.skinNamesUpr[j][numSkins], skins[i], (int32_t)(strlen(skins[i]) - strlen(token))+1 );
					numSkins++;
				}
			}
			//else
			//TiM | Come back to this later...
			//For now, document it as the star must be at the front or back
		}

		qsort(s_playermodel.skinNames[j], numSkins, sizeof(s_playermodel.skinNames[j][0]), CharMediaList_Compare);

		for (i = 0; i < numSkins; i++)
		{
			Q_strncpyz(s_playermodel.skinNamesUpr[j][i], s_playermodel.skinNames[j][i], sizeof(s_playermodel.skinNamesUpr[j][i]));
			//UI_Logger( LL_DEBUG, "%s\n", s_playermodel.skinNamesUpr[j][i] );
		}

		numSkins = 0;
		j++;
	}
	UI_LogFuncEnd();
}

static int32_t QDECL CharMediaList_Compare(const void *ptr1, const void *ptr2)
{
	const char *str1, *str2;
	char chr1, chr2;

	UI_LogFuncBegin();

	str1 = (const char *)ptr1;
	str2 = (const char *)ptr2;

	//UI_Logger( LL_DEBUG, "STR1: %s STR2: %s\n", str1, str2 );

	chr1 = *str1;
	chr2 = *str2;

	//double check they're lower-case. case matters
	if (chr1 >= 'A' && chr1 <= 'Z') chr1 += 32;
	if (chr2 >= 'A' && chr2 <= 'Z') chr2 += 32;

	//cascade the alphabetical list
	while (chr1 == chr2 && chr1 && chr2)
	{
		chr1 = *(str1++);
		chr2 = *(str2++);
	}

	//based off of their ASCII order.
	UI_LogFuncEnd();
	return ((int32_t)chr1 - (int32_t)chr2);
}

static int32_t QDECL FilterList_Compare(const void *ptr1, const void *ptr2)
{
	const char *str1, *str2;
	char chr1, chr2;

	UI_LogFuncBegin();

	str1 = ((filterData_t *)ptr1)->filterName;
	str2 = ((filterData_t *)ptr2)->filterName;

	//hacky override. Make sure 'All' comes first.
	if (!Q_stricmp(str1, "ALL")){
		UI_LogFuncEnd();
		return -1;
	}

	chr1 = *str1;
	chr2 = *str2;

	//double check they're lower-case. case matters
	if (chr1 >= 'A' && chr1 <= 'Z') chr1 += 32;
	if (chr2 >= 'A' && chr2 <= 'Z') chr2 += 32;

	//cascade the alphabetical list
	while (chr1 == chr2 && chr1 && chr2)
	{
		if (*(str1 + 1) == '\0' || *(str2 + 1) == '\0')
			break;

		chr1 = *(str1++);
		chr2 = *(str2++);
	}

	//based off of their ASCII order.
	UI_LogFuncEnd();
	return ((int32_t)chr1 - (int32_t)chr2);
}

/*
=================
PlayerModel_PopulateSkinsList
=================
*/
static int32_t PlayerModel_PopulateSkinsList(void) {
	int32_t i;
	int32_t j;

	UI_LogFuncBegin();

	if (!s_playermodel.skinNames[s_playermodel.charModel.curvalue][0][0]) {
		UI_Logger(LL_ERROR, "No valid skins found.\n");
		UI_LogFuncEnd();
		return -1;
	}

	//just to ensure complete erasure of previous entries
	for (i = 0; i < MAX_PLAYERSKINS; i++)
		s_playermodel.skinList[i] = NULL;

	/**
	* RPG-X | Phenix | 27/03/2007
	* For loop reversed to output list in alphabetical order. (Task #39)
	* RPG-X | TiM | 30/4/2007
	* Reverted as it was causing an error with skin names not matching
	*/
	j = 0;
	//for ( i = (MAX_PLAYERSKINS - 1); i >= 0; i-- )
	for (i = 0; i < MAX_PLAYERSKINS; i++)
	{
		if (s_playermodel.skinNames[s_playermodel.charModel.curvalue][i][0]) //!
		{
			if (!Q_stricmp(s_playermodel.skinNames[s_playermodel.charModel.curvalue][i], DEFAULT_SKIN))
				s_playermodel.charSkin.curvalue = j; //j;

			s_playermodel.skinList[j] = s_playermodel.skinNamesUpr[s_playermodel.charModel.curvalue][i];

			Q_strupr(s_playermodel.skinList[j]);

			j++;
		}
	}

	UI_LogFuncEnd();
	return j;
}

/*
=================
PlayerModel_OffsetCharList

TiM: Called whenever we scroll the model list.
So it'll cycle the value of each one up and down.
The parameter is a pointer so the updated
value will be passed through the scope to the calling
code area. :)
==================
*/

static void PlayerModel_OffsetCharList(int32_t* offset) {
	char*	buffer; //intermediate value so performing strupr won't pwn our case sensitive data
	int32_t		i;

	UI_LogFuncBegin();

	if (*offset < 0) {
		*offset = 0;
	}

	if ((s_playermodel.numChars > MAX_MENULISTITEMS) && (*offset > (s_playermodel.numChars - MAX_MENULISTITEMS))) {
		*offset = (s_playermodel.numChars - MAX_MENULISTITEMS);
	}

	for (i = 0; i < MAX_MENULISTITEMS; i++) {
		buffer = s_playermodel.charNamesUpr[i + *offset];
		//Q_strncpyz( buffer, s_playermodel.charNames[i + *offset], sizeof ( buffer ) );

		//UI_Logger( LL_DEBUG, "Buffer - %s, %s\n", buffer, s_playermodel.charNames[1] );

		if (!buffer[0]) {
			if (s_playermodel.charMenu[i].generic.flags & (QMF_INACTIVE | QMF_HIDDEN)) {
				break;
			}
			else {
				s_playermodel.charMenu[i].generic.flags = (QMF_INACTIVE | QMF_HIDDEN);
				continue;
			}
		}

		s_playermodel.charMenu[i].generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playermodel.charMenu[i].textPtr = buffer;

		Q_strupr(s_playermodel.charMenu[i].textPtr);
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_RebuildCharMenu

TiM: Called when we scroll the races/gender button.
Rebuild the main list based on the new parameters
=================
*/
static void PlayerModel_RebuildCharMenu(void) {
	int32_t i;
	qboolean	raceValid = qfalse;
	qboolean	genderValid = qfalse;

	UI_LogFuncBegin();

	s_playermodel.numChars = 0;
	memset(&s_playermodel.charNamesUpr, 0, sizeof(s_playermodel.charNamesUpr));

	i = 0;
	while (i < MAX_PLAYERCHARS) {
		if (!s_playermodel.charNames[i].charName[0])
			break;

		raceValid = static_cast<qboolean>(s_playermodel.raceFilter.curvalue == 0 ||
			(s_playermodel.raceList[s_playermodel.raceFilter.curvalue].filterIndex == s_playermodel.charNames[i].race));

		genderValid = static_cast<qboolean>(s_playermodel.genderFilter.curvalue == 0 ||
			(s_playermodel.genderList[s_playermodel.genderFilter.curvalue].filterIndex == s_playermodel.charNames[i].gender));

		//UI_Logger( LL_DEBUG, "Char %s valid: %i %i\n", s_playermodel.charNames[i].charName, raceValid, genderValid );

		if (raceValid && genderValid)
		{
			Q_strncpyz(s_playermodel.charNamesUpr[s_playermodel.numChars], s_playermodel.charNames[i].charName, sizeof(s_playermodel.charNamesUpr[s_playermodel.numChars]));

			if (s_playermodel.raceFilter.curvalue == 0 && s_playermodel.genderFilter.curvalue == 0)
				s_playermodel.charNames[s_playermodel.numChars].index = -1;
			else
				s_playermodel.charNames[s_playermodel.numChars].index = i;

			s_playermodel.numChars++;

		}

		i++;
	}

	if (s_playermodel.numChars < MAX_MENULISTITEMS) {
		s_playermodel.upArrow.generic.flags = (QMF_INACTIVE | QMF_GRAYED);
		s_playermodel.dnArrow.generic.flags = (QMF_INACTIVE | QMF_GRAYED);
	}
	else
	{
		s_playermodel.upArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playermodel.dnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	s_playermodel.scrollOffset = 0;
	PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SpinPlayer
=================
*/
static void PlayerModel_SpinPlayer(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event == QM_ACTIVATED)
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_UpdateModel
=================
*/
static void PlayerModel_UpdateModel(void)
{
	UI_LogFuncBegin();

	vec3_t	viewangles;
	vec3_t	moveangles;

	memset(&s_playermodel.playerinfo, 0, sizeof(playerInfo_t));

	viewangles[YAW] = uis.lastYaw;
	viewangles[PITCH] = 0;
	viewangles[ROLL] = 0;
	VectorClear(moveangles);

	UI_PlayerInfo_SetModel(&s_playermodel.playerinfo, s_playermodel.modelData);
	UI_PlayerInfo_SetInfo(&s_playermodel.playerinfo, BOTH_WALK1, BOTH_WALK1, viewangles, moveangles, WP_0, trap_Cvar_VariableValue("height"), trap_Cvar_VariableValue("weight"), qfalse);
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SaveChanges
=================
*/
static void PlayerModel_SaveChanges(void)
{
	UI_LogFuncBegin();

	trap_Cvar_Set("model", va("%s/%s/%s",
		s_playermodel.charNames[s_playermodel.selectedChar].charName,
		s_playermodel.modelNames[s_playermodel.charModel.curvalue],
		s_playermodel.skinNames[s_playermodel.charModel.curvalue][s_playermodel.charSkin.curvalue]));

	//Set backup data
	s_playermodel.storedData.orgChar = s_playermodel.selectedChar;
	s_playermodel.storedData.orgModel = s_playermodel.charModel.curvalue;
	s_playermodel.storedData.orgSkin = s_playermodel.charSkin.curvalue;

	Q_strncpyz(s_playermodel.modelData, UI_Cvar_VariableString("model"), sizeof (s_playermodel.modelData));
	PlayerModel_UpdateModel();
	UI_LogFuncEnd();
}

/*
==================
PlayerModel_CheckForChange
==================
*/
static void PlayerModel_CheckForChange(void) {
	UI_LogFuncBegin();

	qboolean enableSaveButton = qfalse;

	if (s_playermodel.storedData.orgChar != s_playermodel.selectedChar)
		enableSaveButton = qtrue;

	if (s_playermodel.storedData.orgModel != s_playermodel.charModel.curvalue)
		enableSaveButton = qtrue;

	if (s_playermodel.storedData.orgSkin != s_playermodel.charSkin.curvalue)
		enableSaveButton = qtrue;

	if (s_playermodel.charSkin.numitems == 0)
		enableSaveButton = qfalse;

	if (s_playermodel.charModel.numitems == 0)
		enableSaveButton = qfalse;

	if (enableSaveButton) {
		s_playermodel.apply.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}
	else {
		s_playermodel.apply.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SetupScrollBar
=================
*/
static void PlayerModel_SetupScrollBar(menuaction_s *bar)
{
	UI_LogFuncBegin();

	int32_t height;

	//first make sure it's worth enabling this at all
	if (s_playermodel.numChars <= MAX_MENULISTITEMS)
	{
		bar->generic.flags = QMF_INACTIVE | QMF_HIDDEN;
		UI_LogFuncEnd();
		return;
	}

	//show the bar
	bar->generic.flags &= ~(QMF_INACTIVE | QMF_HIDDEN);

	//calculate the necessary height of the bar
	//by default, assume 1 pixel per offset
	height = (MAX_SCROLLRANGE)-(s_playermodel.numChars - MAX_MENULISTITEMS);

	//ensure box doesn't get too small
	if (height < MIN_SCROLLHEIGHT)
	{
		//double the step in that case
		//a bit hacky, but no need for 3 since the limit isn't that high
		height = (MAX_SCROLLRANGE)-(s_playermodel.numChars * 0.5 - MAX_MENULISTITEMS);
		s_playermodel.scrollData.doubleStep = qtrue;
	}
	else
	{
		s_playermodel.scrollData.doubleStep = qfalse;
	}

	//reset to top
	bar->generic.y = bar->generic.top = MAX_SCROLLTOP;

	bar->height = height;
	bar->generic.bottom = bar->generic.y + height;
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_UpdateScrollBar
=================
*/
static void PlayerModel_UpdateScrollBar(menuaction_s *bar)
{
	UI_LogFuncBegin();
	bar->generic.y = MAX_SCROLLTOP + s_playermodel.scrollOffset*(s_playermodel.scrollData.doubleStep ? 0.5 : 1);
	bar->generic.top = bar->generic.y;
	bar->generic.bottom = bar->generic.top + bar->height;
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_MenuEvent
=================
*/
static void PlayerModel_MenuEvent(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
	case ID_BACK:
		//PlayerModel_SaveChanges();
		UI_PopMenu();
		break;

	case ID_MAINMENU:
		//PlayerModel_SaveChanges();
		UI_MainMenu();
		break;

	case ID_INGAMEMENU:
		//PlayerModel_SaveChanges();
		UI_InGameMenu();
		break;

	case ID_SETTINGS:
		UI_PopMenu();
		//PlayerModel_SaveChanges();
		UI_PlayerSettingsMenu(s_playermodel.prevMenu);
		break;

	case ID_DNARROW:
		s_playermodel.scrollOffset++;
		PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);

		if (!(s_playermodel.scrollBar.generic.flags & QMF_INACTIVE))
			PlayerModel_UpdateScrollBar(&s_playermodel.scrollBar);
		break;

	case ID_UPARROW:
		s_playermodel.scrollOffset--;
		PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);

		if (!(s_playermodel.scrollBar.generic.flags & QMF_INACTIVE))
			PlayerModel_UpdateScrollBar(&s_playermodel.scrollBar);
		break;

	case ID_MODELSET:
		s_playermodel.charSkin.numitems = PlayerModel_PopulateSkinsList();

		if (s_playermodel.charSkin.numitems != -1)
		{
			s_playermodel.charSkin.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		}
		else
		{
			s_playermodel.skinList[0] = "NONE";
			s_playermodel.charSkin.generic.flags = QMF_INACTIVE | QMF_GRAYED;
			s_playermodel.charSkin.numitems = 0;
		}
		break;

	case ID_APPLY:
		PlayerModel_SaveChanges();
		break;

	case ID_GENDERFILTER:
		PlayerModel_RebuildCharMenu();
		PlayerModel_SetupScrollBar(&s_playermodel.scrollBar);
		break;

	case ID_RACEFILTER:
		PlayerModel_RebuildCharMenu();
		PlayerModel_SetupScrollBar(&s_playermodel.scrollBar);
		break;

	case ID_MENUCHAR0:
	case ID_MENUCHAR1:
	case ID_MENUCHAR2:
	case ID_MENUCHAR3:
	case ID_MENUCHAR4:
	case ID_MENUCHAR5:
	case ID_MENUCHAR6:
	case ID_MENUCHAR7:
	case ID_MENUCHAR8:
	case ID_MENUCHAR9:
	case ID_MENUCHAR10:
	case ID_MENUCHAR11:
	{
						  int32_t		temp;
						  int32_t		oldChar = s_playermodel.selectedChar;

						  s_playermodel.selectedChar = (((menucommon_s*)ptr)->id - ID_MENUCHAR0) + s_playermodel.scrollOffset;
						  temp = s_playermodel.selectedChar;

						  if (s_playermodel.charNames[s_playermodel.selectedChar].index >= 0) {
							  s_playermodel.selectedChar = s_playermodel.charNames[s_playermodel.selectedChar].index;
						  }

						  //UI_Logger( LL_DEBUG, "%i %i\n", s_playermodel.selectedChar, s_playermodel.charNames[ s_playermodel.selectedChar ].index );

						  //safety net. Without this, if a player held down 'enter', the game would overflow trying to load the same file over and over really fast. bad, really bad lol
						  if (oldChar == s_playermodel.selectedChar && s_playermodel.selectedChar != -1)
							  break;

						  //Reset the spin controls to 0.
						  //Without this, if we load a char with less models+skins than our last one, the game would crash
						  //since it'd be trying to parse NULL string data
						  s_playermodel.charModel.curvalue = 0;
						  s_playermodel.charSkin.curvalue = 0;

						  s_playermodel.playerIcon = trap_R_RegisterShaderNoMip(va("models/players_rpgx/%s/model_icon.jpg", s_playermodel.charNames[s_playermodel.selectedChar].charName));

						  Q_strncpyz(s_playermodel.modelName, s_playermodel.charNamesUpr[temp], sizeof(s_playermodel.modelName));

						  s_playermodel.charModel.numitems = PlayerModel_LoadAvailableModels();
						  if (s_playermodel.charModel.numitems != -1)
						  {
							  s_playermodel.charModel.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

							  PlayerModel_LoadAvailableSkins();
							  s_playermodel.charSkin.numitems = PlayerModel_PopulateSkinsList();

							  if (s_playermodel.charSkin.numitems != -1)
							  {
								  s_playermodel.charSkin.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
							  }
							  else
							  {
								  s_playermodel.skinList[0] = "NONE";
								  s_playermodel.charSkin.generic.flags = QMF_INACTIVE | QMF_GRAYED;
								  s_playermodel.charSkin.numitems = 0;
							  }
						  }
						  //reset the spin control
						  else
						  {
							  s_playermodel.modelList[0] = "NONE";
							  s_playermodel.charModel.generic.flags = QMF_INACTIVE | QMF_GRAYED;
							  s_playermodel.charModel.numitems = 0;
						  }
	}
		break;
	}
	PlayerModel_CheckForChange();
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_MenuKey
=================
*/
static sfxHandle_t PlayerModel_MenuKey(int32_t key)
{
	UI_LogFuncBegin();
	switch (key)
	{
	case K_MOUSE1:
		if (Menu_ItemAtCursor(&s_playermodel.menu) == &s_playermodel.scrollBar)
		{
			uis.activemenu->m_NoNewSelecting = qtrue;
			s_playermodel.scrollData.mouseDown = qtrue;
			s_playermodel.scrollData.yStart = uis.cursory;
		}
		break;
	}

	UI_LogFuncEnd();
	return (Menu_DefaultKey(&s_playermodel.menu, key));
}

/*
=================
PlayerModel_DrawPlayer
=================
*/
static void PlayerModel_DrawPlayer(void *self)
{
	UI_LogFuncBegin();
	menubitmap_s*	b;
	vec3_t			origin = { -40, 2.5, -4 }; //-3.8

	b = (menubitmap_s*)self;

	if (trap_MemoryRemaining() <= LOW_MEMORY) {
		UI_DrawProportionalString(b->generic.x, b->generic.y + b->height / 2, "LOW MEMORY", UI_LEFT, color_red);
		UI_LogFuncEnd();
		return;
	}

	UI_DrawPlayer((float)b->generic.x, (float)b->generic.y, (float)b->width, (float)b->height, origin, &s_playermodel.playerinfo, (int32_t)(uis.realtime / 1.5));
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_BuildList

Heavily modifed by TiM
All we'll take into account now
is a valid directory name,
and that it contains a .model file
We'll work the rest out later
=================
*/

static int32_t QDECL PlayerListOrder_Compare(const void *ptr1, const void *ptr2);

static void PlayerModel_BuildList(void)
{
	UI_LogFuncBegin();
	int32_t			numdirs;
	int32_t			numfiles;
	char		dirlist[8192];
	char		filelist[128]; //2048
	char*		dirptr;
	int32_t			i, j;
	int32_t			dirlen;
	charData_t	*tempBuff;
	//int32_t			offset;
	int32_t			temp;

	s_playermodel.selectedChar = -1;
	s_playermodel.numChars = 0;

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players_rpgx", "/", dirlist, sizeof(dirlist));
	dirptr = dirlist;

	///UI_Logger( LL_DEBUG, "%i folders found\n", numdirs );

	for (i = 0; i < numdirs && s_playermodel.numChars < MAX_PLAYERCHARS; i++, dirptr += dirlen + 1)
	{
		dirlen = strlen(dirptr);

		if (!dirptr) {
			break;
		}

		if (dirlen && dirptr[dirlen - 1] == '/')
			dirptr[dirlen - 1] = '\0';

		//I'm guessing this is for non-PK3'd files
		if (!strcmp(dirptr, ".") || !strcmp(dirptr, ".."))
			continue;

		// TiM : Check for .model files.  That's all we need
		numfiles = 0; //Just to be sure
		numfiles = trap_FS_GetFileList(va("models/players_rpgx/%s", dirptr), ".model", filelist, 128);

		temp = qfalse;
		if (numfiles > 0 && dirptr[0])
		{
			//TiM - Don't do duplicate chars (Since it caches PK3 and non PK3 ones as separate instances )
			for (j = 0; j < s_playermodel.numChars; j++)
			{
				tempBuff = &s_playermodel.charNames[j];

				if (!Q_stricmp(tempBuff->charName, dirptr)){
					temp = qtrue;
					break;
				}
			}

			if (temp)
				continue;

			tempBuff = &s_playermodel.charNames[s_playermodel.numChars];
			Q_strncpyz(tempBuff->charName, dirptr, sizeof(tempBuff[s_playermodel.numChars].charName));
			tempBuff->index = -1;

			//TiM - Load data on the races if at all possible
			if (trap_FS_GetFileList(va("models/players_rpgx/%s", dirptr), "race.cfg", filelist, 128) > 0)
			{
				fileHandle_t	f;
				char			buffer[1024];
				char*			filePtr;
				int32_t				fileLength;
				char*			token;
				char			filePath[MAX_QPATH];

				Com_sprintf(filePath, sizeof(filePath), "models/players_rpgx/%s/race.cfg", dirptr);
				fileLength = trap_FS_FOpenFile(filePath, &f, FS_READ);

				//UI_Logger( LL_DEBUG, "File %s loaded.\n", dirptr );

				if (fileLength <= 0) {
					continue;
				}
				//UI_Logger( LL_DEBUG, "We have length.\n" );

				if (fileLength > sizeof(buffer)-1) {
					continue;
				}
				//UI_Logger( LL_DEBUG, "Good size.\n" );

				memset(&buffer, 0, sizeof(buffer));
				trap_FS_Read(buffer, fileLength, f);
				//UI_Logger( LL_DEBUG, "Loaded data...\n" );

				trap_FS_FCloseFile(f);

				if (!buffer[0]) {
					continue;
				}

				//Format is 'Race Gender'. Race must precede Gender
				filePtr = buffer;

				COM_BeginParseSession();

				token = COM_Parse(&filePtr);
				if (!token)
					continue;

				//UI_Logger( LL_DEBUG, "Race %s Loaded\n", token );

				tempBuff->race = PlayerModel_CheckInFilter(token, s_playermodel.raceList, MAX_RACES, &s_playermodel.numRaces);
				//UI_Logger( LL_DEBUG, "Number of races now: %i\n",  s_playermodel.numRaces );

				token = COM_Parse(&filePtr);
				if (!token)
					continue;
				//UI_Logger( LL_DEBUG, "Gender %s loaded\n", token );

				//tempBuff->gender = PlayerModel_CheckInArray( token, s_playermodel.genderNames, MAX_GENDERS );
				tempBuff->gender = PlayerModel_CheckInFilter(token, s_playermodel.genderList, MAX_GENDERS, &s_playermodel.numGenders);

				//UI_Logger( LL_DEBUG, "%i\n", tempBuff[i].gender );
			}

			s_playermodel.numChars++;
			tempBuff = &s_playermodel.charNames[s_playermodel.numChars];
		}
	}

	//RPG-X | TiM | 30-4-2007
	//This loop obviously isn't working well enough.
	//Bringing out the big guns. Using the Windows/Q3
	//Binary data sorting function, qsort.

	//sorting function located below
	qsort(s_playermodel.charNames, s_playermodel.numChars, sizeof(charData_t), PlayerListOrder_Compare);

	//copy to the upper case list for rendering to the menu
	for (i = 0; i < s_playermodel.numChars; i++)
		Q_strncpyz(s_playermodel.charNamesUpr[i], s_playermodel.charNames[i].charName, sizeof(s_playermodel.charNamesUpr[i]));
	UI_LogFuncEnd();
}

/*
=================
PlayerListOrder_Compare

TiM - 30-4-2007
Called in the above
qsort function.
Re-orders the player
list based on alphabetical
name.
=================
*/
static int32_t QDECL PlayerListOrder_Compare(const void *ptr1, const void *ptr2)
{
	UI_LogFuncBegin();
	char			*chr1, *chr2;
	int32_t				delta;

	//extract the first characters of the name from each entry
	chr1 = ((charData_t *)ptr1)->charName;
	chr2 = ((charData_t *)ptr2)->charName;

	//double check they're lower-case. case matters
	if (*chr1 >= 'A' && *chr1 <= 'Z') *chr1 += 32;
	if (*chr2 >= 'A' && *chr2 <= 'Z') *chr2 += 32;

	//based off of their ASCII order.
	delta = (int32_t)*chr1 - (int32_t)*chr2;

	//if characters weren't the same
	if (delta != 0){
		UI_LogFuncEnd();
		return delta;
	}

	//else loop through the rest
	while (chr1 && chr2 && delta == 0)
	{
		delta = (int32_t)*chr1 - (int32_t)*chr2;

		chr1++;
		chr2++;
	}

	UI_LogFuncEnd();
	return delta;
}

/*
=================
PlayerModel_SetMenuItems
=================
*/
static void PlayerModel_SetMenuItems(void)
{
	UI_LogFuncBegin();
	int32_t				i;
	char*			temp;
	//char			model[64];
	char			model[32];
	char			skin[32];

	// name
	trap_Cvar_VariableStringBuffer("name", s_playermodel.playername.string, MAX_PLAYERNAMELENGTH);

	// model
	trap_Cvar_VariableStringBuffer("model", s_playermodel.modelData, 64);

	if ((temp = strchr(s_playermodel.modelData, '/')) == NULL)
	{
		Q_strncpyz(s_playermodel.modelName, s_playermodel.modelData, sizeof(s_playermodel.modelName));
		Q_strncpyz(model, DEFAULT_MODEL, 32);
		Q_strncpyz(skin, DEFAULT_SKIN, 32);
	}
	else
	{
		int32_t len;
		char*	tempSkin;
		//
		len = strlen(temp);
		Q_strncpyz(s_playermodel.modelName, s_playermodel.modelData, (strlen(s_playermodel.modelData) - strlen(temp)) + 1);

		//*temp++;
		temp++;
		if ((tempSkin = strchr(temp, '/')) == NULL) {
			if (!temp || !temp[1])
				Q_strncpyz(model, DEFAULT_MODEL, 32);
			else
				Q_strncpyz(model, temp, 32);

			Q_strncpyz(skin, DEFAULT_SKIN, 32);
		}
		else {
			len = strlen(tempSkin);
			//*tempSkin++;
			tempSkin++;

			Q_strncpyz(model, temp, (strlen(temp) - len) + 1);

			if (!tempSkin || !tempSkin[1])
				Q_strncpyz(skin, DEFAULT_SKIN, 32);
			else
				Q_strncpyz(skin, tempSkin, 32);
		}
	}

	//UI_Logger( LL_DEBUG, "Load model is %s %s %s\n", s_playermodel.modelName, model, skin );

	// find model in our list
	for (i = 0; i < s_playermodel.numChars; i++)
	{
		if (!Q_stricmp(s_playermodel.modelName, s_playermodel.charNames[i].charName)) {
			s_playermodel.selectedChar = i;
			break;
		}
	}

	//try to register the current shader icon
	if (s_playermodel.selectedChar != -1) {
		s_playermodel.playerIcon = trap_R_RegisterShaderNoMip(va("models/players_rpgx/%s/model_icon", s_playermodel.modelName));

		//If we're exceeded the list, update it so we're at the top
		if (s_playermodel.selectedChar > MAX_MENULISTITEMS) {
			s_playermodel.scrollOffset = (s_playermodel.selectedChar - MAX_MENULISTITEMS) + 1;
			PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);
		}

		s_playermodel.charModel.numitems = PlayerModel_LoadAvailableModels();

		if (s_playermodel.charModel.numitems != -1)
		{
			s_playermodel.charModel.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		}
		else
		{
			s_playermodel.charModel.numitems = 0;
			s_playermodel.charModel.generic.flags = QMF_GRAYED | QMF_INACTIVE;
			s_playermodel.modelList[0] = "NONE";
		}

		//TiM - Switch the spin controls to the right value;
		for (i = 0; i < s_playermodel.charModel.numitems; i++) {
			if (!Q_stricmp(s_playermodel.modelList[i], model)) {
				s_playermodel.charModel.curvalue = i;
				break;
			}
		}

		PlayerModel_LoadAvailableSkins();
		s_playermodel.charSkin.numitems = PlayerModel_PopulateSkinsList();

		if (s_playermodel.charSkin.numitems != -1)
		{
			s_playermodel.charSkin.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		}
		else
		{
			s_playermodel.charSkin.numitems = 0;
			s_playermodel.charSkin.generic.flags = QMF_GRAYED | QMF_INACTIVE;
			s_playermodel.skinList[0] = "NONE";
		}
	}



	for (i = 0; i < s_playermodel.charSkin.numitems; i++) {
		if (!Q_stricmp(s_playermodel.skinList[i], skin)) {
			s_playermodel.charSkin.curvalue = i;
			break;
		}
	}

	//Set backup data
	s_playermodel.storedData.orgChar = s_playermodel.selectedChar;
	s_playermodel.storedData.orgModel = s_playermodel.charModel.curvalue;
	s_playermodel.storedData.orgSkin = s_playermodel.charSkin.curvalue;
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_DrawScrollBar
=================
*/
static void PlayerModel_DrawScrollBar(void *self)
{
	UI_LogFuncBegin();
	qboolean		focus;
	menuaction_s	*bar;
	int32_t				*y;
	int32_t				color;
	int32_t				newY;
	int32_t				dif;

	bar = (menuaction_s *)self;

	focus = static_cast<qboolean>(Menu_ItemAtCursor(bar->generic.parent) == bar);

	if (focus)
		color = bar->color2;
	else
		color = bar->color;

	trap_R_SetColor(colorTable[color]);
	UI_DrawHandlePic(bar->generic.x, bar->generic.y, bar->width, bar->height, uis.whiteShader);
	trap_R_SetColor(NULL);

	if (!s_playermodel.scrollData.mouseDown){
		UI_LogFuncEnd();
		return;
	}

	if (!trap_Key_IsDown(K_MOUSE1))
	{
		s_playermodel.scrollData.mouseDown = qfalse;
		uis.activemenu->m_NoNewSelecting = qfalse;
		UI_LogFuncEnd();
		return;
	}

	if (uis.cursory == s_playermodel.scrollData.yStart){
		UI_LogFuncEnd();
		return;
	}

	y = &bar->generic.y;

	newY = *y + (uis.cursory - s_playermodel.scrollData.yStart);

	if (newY + bar->height > MAX_SCROLLTOP + MAX_SCROLLRANGE)
		newY = (MAX_SCROLLTOP + MAX_SCROLLRANGE) - bar->height;

	if (newY < MAX_SCROLLTOP)
		newY = MAX_SCROLLTOP;

	dif = newY - *y;

	s_playermodel.scrollOffset += dif * (s_playermodel.scrollData.doubleStep ? 2 : 1);
	PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);

	*y = newY;
	bar->generic.top = *y;
	bar->generic.bottom = *y + bar->height;

	s_playermodel.scrollData.yStart = uis.cursory;
	UI_LogFuncEnd();
}


/*
=================
PlayerSettingsMenu_Graphics
=================
*/
void PlayerModelMenu_Graphics(void)
{
	UI_LogFuncBegin();
	// Draw the basic screen layout
	UI_MenuFrame2(&s_playermodel.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Middle left line

	trap_R_SetColor(colorTable[CT_LTORANGE]);

	//TiM - Frame around the models selection list
	UI_DrawHandlePic(96, 50, 8, -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(96, 369, 8, 8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(238, 62, 32, 32, s_playermodel.corner_ur_18_18);	// UR Corner
	UI_DrawHandlePic(240, 368, 32, 8, s_playermodel.corner_lr_18_4);	// LR Corner
	UI_DrawHandlePic(96, 81, 4, 290, uis.whiteShader);	// Left side
	UI_DrawHandlePic(242, 87, 18, 18, uis.whiteShader); //Right Side Up Arrow Button
	if (s_playermodel.scrollBar.generic.flags & QMF_HIDDEN)
	{
		UI_DrawHandlePic(242, 108, 18, 236, uis.whiteShader);	// Right side
	}
	else
	{
		if (s_playermodel.scrollBar.generic.y > MAX_SCROLLTOP + 4)
			UI_DrawHandlePic(242, 108, 18, s_playermodel.scrollBar.generic.y - 108 - 3, uis.whiteShader);
		if (s_playermodel.scrollBar.generic.bottom + 3 < 343)
			UI_DrawHandlePic(242, s_playermodel.scrollBar.generic.bottom + 3, 18, 343 - 3 - s_playermodel.scrollBar.generic.bottom, uis.whiteShader);
	}
	UI_DrawHandlePic(242, 347, 18, 18, uis.whiteShader);		//Right Side Down Button
	UI_DrawHandlePic(100, 62, 141, 18, uis.whiteShader);	// Top
	UI_DrawHandlePic(101, 371, 140, 4, uis.whiteShader);	// Bottom

	//TiM - Frame around the model specific data window
	UI_DrawHandlePic(265, 50, 8, -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(265, 369, 8, 8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(422, 50, -8, -32, s_playermodel.corner_ll_4_18);	// UR Corner
	UI_DrawHandlePic(422, 369, -8, 8, s_playermodel.corner_ll_4_4);	// LR Corner
	UI_DrawHandlePic(265, 81, 4, 290, uis.whiteShader);	// Left side
	UI_DrawHandlePic(426, 81, 4, 290, uis.whiteShader);	// Right side
	UI_DrawHandlePic(269, 62, 157, 18, uis.whiteShader);	// Top
	UI_DrawHandlePic(269, 371, 157, 4, uis.whiteShader);		// Bottom

	//TiM - Draw the stunningly awesome icon of the character
	UI_DrawHandlePic(306, 114, 82, 82, uis.whiteShader);
	if (!s_playermodel.playerIcon) {
		trap_R_SetColor(colorTable[CT_BLACK]);
		UI_DrawHandlePic(307, 115, 80, 80, uis.whiteShader);
		UI_DrawProportionalString(347, 145, "?", UI_BIGFONT | UI_CENTER, colorTable[CT_LTORANGE]);
	}
	else {
		trap_R_SetColor(colorTable[CT_WHITE]);
		UI_DrawHandlePic(307, 115, 80, 80, s_playermodel.playerIcon);
	}

	//UI_DrawProportionalString(  220,  362, va("%s %d %s %d",menu_normal_text[MNT_SCREEN],(s_playermodel.modelpage + 1),menu_normal_text[MNT_OF],s_playermodel.numpages),UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(108, 64, menu_normal_text[MNT_CHARS], UI_SMALLFONT, colorTable[CT_BLACK]);	// Top

	UI_DrawProportionalString(275, 64, menu_normal_text[MNT_CHARDATA], UI_SMALLFONT, colorTable[CT_BLACK]);

	trap_R_SetColor(colorTable[CT_DKGREY2]);
	UI_DrawHandlePic(439, 79, 151, 295, uis.whiteShader);	// Background

	// Frame around player model
	trap_R_SetColor(colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(435, 50, 8, -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(435, 369, 8, 8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(440, 62, 150, 18, uis.whiteShader);	// Top
	UI_DrawHandlePic(435, 79, 4, 295, uis.whiteShader);	// Left side
	UI_DrawHandlePic(440, 371, 150, 4, uis.whiteShader);	// Bottom

	// Left rounded ends for buttons
	trap_R_SetColor(colorTable[s_playermodel.mainmenu.color]);
	UI_DrawHandlePic(s_playermodel.mainmenu.generic.x - 14, s_playermodel.mainmenu.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playermodel.back.color]);
	UI_DrawHandlePic(s_playermodel.back.generic.x - 14, s_playermodel.back.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playermodel.data.color]);
	UI_DrawHandlePic(s_playermodel.data.generic.x - 14, s_playermodel.data.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playermodel.model.color]);
	UI_DrawHandlePic(s_playermodel.model.generic.x - 14, s_playermodel.model.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	//Model Name along the top
	if (s_playermodel.modelName[0])
	{
		char* buf = s_playermodel.modelName;

		UI_DrawProportionalString(347, 89, Q_strupr(buf), UI_SMALLFONT | UI_CENTER, colorTable[CT_DKPURPLE1]);
	}

	UI_DrawProportionalString(74, 28, "881", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 150, "2445", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "600", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "3-44", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	//paint the selected model white
	{
		int32_t i;
		for (i = 0; i < MAX_MENULISTITEMS; i++)
		{
			if (s_playermodel.charMenu[i].textcolor == CT_WHITE && s_playermodel.charMenu[i].textcolor2 == CT_WHITE)
			{
				s_playermodel.charMenu[i].textcolor = CT_DKGOLD1;
				s_playermodel.charMenu[i].textcolor2 = CT_LTGOLD1;
			}

			//override between straight list, and filters
			if ((s_playermodel.charNames[i + s_playermodel.scrollOffset].index == -1 && i + s_playermodel.scrollOffset == s_playermodel.selectedChar)
				||
				(s_playermodel.charNames[i + s_playermodel.scrollOffset].index >= 0 && s_playermodel.charNames[i + s_playermodel.scrollOffset].index == s_playermodel.selectedChar)
				)
			{
				//UI_Logger( LL_DEBUG, "%i %i %i\n", i, s_playermodel.selectedChar, s_playermodel.charNames[i + s_playermodel.scrollOffset].index );
				s_playermodel.charMenu[i].textcolor = CT_WHITE;
				s_playermodel.charMenu[i].textcolor2 = CT_WHITE;
			}


		}
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerSettings_MenuDraw
=================
*/
static void PlayerModel_MenuDraw(void)
{
	UI_LogFuncBegin();
	PlayerModelMenu_Graphics();

	Menu_Draw(&s_playermodel.menu);
	UI_LogFuncEnd();
}
/*
=================
PlayerModel_MenuInit
=================
*/
static void PlayerModel_MenuInit(int32_t menuFrom)
{
	UI_LogFuncBegin();
	int32_t			i;
	//int32_t			j;
	//int32_t			k;
	int32_t			x;
	int32_t			y;
	static char	playername[32];
	//static char	modelname[32];
	//static char	skinname[32];
	//static char	skinnameviewed[32];
	qboolean	races = qfalse;
	qboolean	genders = qfalse;

	// zero set all our globals
	memset(&s_playermodel, 0, sizeof(playermodel_t));

	s_playermodel.prevMenu = menuFrom;

	//TiM : Model Spin view
	uis.spinView = qfalse;
	uis.lastYaw = 150;

	PlayerModel_Cache();

	//Set up some false data to feed our spin controls for the time being
	//Spin controls NEED data passed to them on init, eitherwise they immediately terminate.  which sucks lol
	s_playermodel.modelList[0] = "NONE";
	s_playermodel.skinList[0] = "NONE";

	Q_strncpyz(s_playermodel.genderList[0].filterName, "All", 32);
	Q_strncpyz(s_playermodel.raceList[0].filterName, "All", 32);

	// set initial states
	PlayerModel_BuildList();

	//sort the race list alphabetically
	//+1 for number to account for the 'All' value
	qsort((void *)s_playermodel.raceList, s_playermodel.numRaces + 1, sizeof(filterData_t), FilterList_Compare);
	qsort((void *)s_playermodel.genderList, s_playermodel.numGenders + 1, sizeof(filterData_t), FilterList_Compare);

	//Populate the spin control pointers
	for (i = 0; i < 128; i++) {
		if (!s_playermodel.genderList[i].filterName[0] && !s_playermodel.raceList[i].filterName[0])
			break;

		if (s_playermodel.genderList[i].filterName[0]) {
			s_playermodel.genderNames[i] = s_playermodel.genderList[i].filterName;
			genders = qtrue;
		}

		if (s_playermodel.raceList[i].filterName[0]) {
			s_playermodel.raceNames[i] = s_playermodel.raceList[i].filterName;
			races = qtrue;
		}
	}

	s_playermodel.menu.OnKey = PlayerModel_MenuKey;
	s_playermodel.menu.m_WrapAround = true;
	s_playermodel.menu.m_Fullscreen = qtrue;
	s_playermodel.menu.OnDraw = PlayerModel_MenuDraw;
	s_playermodel.menu.m_DescriptionPosition = { MENU_DESC_X, MENU_DESC_Y };
	s_playermodel.menu.m_Title = { { MENU_TITLE_X, MENU_TITLE_Y }, MNT_CHANGEPLAYER_TITLE };
	s_playermodel.menu.m_FootNote = MNT_CHANGEPLAYER;

	s_playermodel.mainmenu.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.mainmenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.mainmenu.generic.x = 110;
	s_playermodel.mainmenu.generic.y = 391;
	s_playermodel.mainmenu.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.mainmenu.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.mainmenu.width = MENU_BUTTON_MED_WIDTH;
	s_playermodel.mainmenu.height = MENU_BUTTON_MED_HEIGHT;
	s_playermodel.mainmenu.color = CT_DKPURPLE1;
	s_playermodel.mainmenu.color2 = CT_LTPURPLE1;

	if (!ingameFlag)
	{
		s_playermodel.mainmenu.textEnum = MBT_MAINMENU;
		s_playermodel.mainmenu.generic.id = ID_MAINMENU;
	}
	else
	{
		s_playermodel.mainmenu.textEnum = MBT_INGAMEMENU;
		s_playermodel.mainmenu.generic.id = ID_INGAMEMENU;
	}

	s_playermodel.mainmenu.textX = MENU_BUTTON_TEXT_X;
	s_playermodel.mainmenu.textY = MENU_BUTTON_TEXT_Y;
	s_playermodel.mainmenu.textcolor = CT_BLACK;
	s_playermodel.mainmenu.textcolor2 = CT_WHITE;

	s_playermodel.back.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.back.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.back.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.back.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.back.generic.id = ID_BACK;
	s_playermodel.back.generic.x = 110;
	s_playermodel.back.generic.y = 415;
	s_playermodel.back.width = MENU_BUTTON_MED_WIDTH;
	s_playermodel.back.height = MENU_BUTTON_MED_HEIGHT;
	s_playermodel.back.color = CT_DKPURPLE1;
	s_playermodel.back.color2 = CT_LTPURPLE1;
	s_playermodel.back.textX = MENU_BUTTON_TEXT_X;
	s_playermodel.back.textY = MENU_BUTTON_TEXT_Y;
	s_playermodel.back.textEnum = MBT_BACK;
	s_playermodel.back.generic.id = ID_BACK;
	s_playermodel.back.textcolor = CT_BLACK;
	s_playermodel.back.textcolor2 = CT_WHITE;

	s_playermodel.data.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.data.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.data.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.data.generic.id = ID_SETTINGS;
	s_playermodel.data.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.data.generic.x = 482;
	s_playermodel.data.generic.y = 391;
	s_playermodel.data.width = MENU_BUTTON_MED_WIDTH;
	s_playermodel.data.height = MENU_BUTTON_MED_HEIGHT;
	s_playermodel.data.color = CT_DKPURPLE1;
	s_playermodel.data.color2 = CT_LTPURPLE1;
	s_playermodel.data.textX = 5;
	s_playermodel.data.textY = 2;
	s_playermodel.data.textEnum = MBT_PLAYERDATA;
	s_playermodel.data.textcolor = CT_BLACK;
	s_playermodel.data.textcolor2 = CT_WHITE;

	s_playermodel.model.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.model.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.model.generic.flags = QMF_GRAYED;
	s_playermodel.model.generic.x = 482;
	s_playermodel.model.generic.y = 415;
	s_playermodel.model.width = MENU_BUTTON_MED_WIDTH;
	s_playermodel.model.height = MENU_BUTTON_MED_HEIGHT;
	s_playermodel.model.color = CT_DKPURPLE1;
	s_playermodel.model.color2 = CT_LTPURPLE1;
	s_playermodel.model.textX = 5;
	s_playermodel.model.textY = 2;
	s_playermodel.model.textEnum = MBT_CHANGEMODEL;
	s_playermodel.model.textcolor = CT_BLACK;
	s_playermodel.model.textcolor2 = CT_WHITE;

	//y =	88;
	x = 107;
	y = 85;

	for (i = 0; i < MAX_MENULISTITEMS; i++) {
		s_playermodel.charMenu[i].generic.m_Type = EMenuItemType::Bitmap;
		s_playermodel.charMenu[i].generic.flags = QMF_INACTIVE | QMF_HIDDEN;
		s_playermodel.charMenu[i].generic.x = x;
		s_playermodel.charMenu[i].generic.y = y;
		s_playermodel.charMenu[i].generic.callback = PlayerModel_MenuEvent;
		s_playermodel.charMenu[i].generic.id = ID_MENUCHAR0 + i;
		s_playermodel.charMenu[i].width = 129;
		s_playermodel.charMenu[i].height = 16;
		s_playermodel.charMenu[i].color = CT_DKPURPLE1;
		s_playermodel.charMenu[i].color2 = CT_LTPURPLE1;
		s_playermodel.charMenu[i].textPtr = NULL;
		s_playermodel.charMenu[i].textX = 4;
		s_playermodel.charMenu[i].textY = 1;
		s_playermodel.charMenu[i].textcolor = CT_DKGOLD1;
		s_playermodel.charMenu[i].textcolor2 = CT_LTGOLD1;
		s_playermodel.charMenu[i].textStyle = UI_SMALLFONT;

		y += 24;
	}

	s_playermodel.playername.generic.m_Type = EMenuItemType::PText;
	s_playermodel.playername.generic.flags = QMF_INACTIVE;
	s_playermodel.playername.generic.x = 444;
	s_playermodel.playername.generic.y = 63;
	s_playermodel.playername.string = playername;
	s_playermodel.playername.style = UI_SMALLFONT;
	s_playermodel.playername.color = colorTable[CT_BLACK];

	/*s_playermodel.modelname.generic.type			= EMenuItemType::PText;
	s_playermodel.modelname.generic.flags			= QMF_INACTIVE;
	s_playermodel.modelname.generic.x				= 121;
	s_playermodel.modelname.generic.y				= 338;
	s_playermodel.modelname.string					= modelname;
	s_playermodel.modelname.style					= UI_LEFT;
	s_playermodel.modelname.color					= colorTable[CT_LTBLUE1];*/

	/*s_playermodel.skinname.generic.type				= EMenuItemType::PText;
	s_playermodel.skinname.generic.flags			= QMF_INACTIVE;
	s_playermodel.skinname.generic.x				= 323;
	s_playermodel.skinname.generic.y				= 338;
	s_playermodel.skinname.string					= skinname;
	s_playermodel.skinname.style					= UI_RIGHT;
	s_playermodel.skinname.color					= colorTable[CT_LTBLUE1];*/

	s_playermodel.player.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.player.generic.flags = QMF_SILENT;
	s_playermodel.player.generic.ownerdraw = PlayerModel_DrawPlayer;
	s_playermodel.player.generic.callback = PlayerModel_SpinPlayer;
	s_playermodel.player.generic.x = 439; //400
	s_playermodel.player.generic.y = 80; //20
	s_playermodel.player.width = 151; //32*7.3
	s_playermodel.player.height = 291; //56*7.3

	s_playermodel.upArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.upArrow.generic.name = PIC_ARROW_UP;
	s_playermodel.upArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.upArrow.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.upArrow.generic.id = ID_UPARROW;
	s_playermodel.upArrow.generic.x = 243;
	s_playermodel.upArrow.generic.y = 89;
	s_playermodel.upArrow.width = 16;
	s_playermodel.upArrow.height = 16;
	s_playermodel.upArrow.color = CT_DKPURPLE1;
	s_playermodel.upArrow.color2 = CT_LTPURPLE1;
	s_playermodel.upArrow.textX = MENU_BUTTON_TEXT_X;
	s_playermodel.upArrow.textY = MENU_BUTTON_TEXT_Y;
	//s_playermodel.upArrow.textEnum					= MBT_PREVPAGE;
	s_playermodel.upArrow.textcolor = CT_BLACK;
	s_playermodel.upArrow.textcolor2 = CT_WHITE;

	s_playermodel.dnArrow.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.dnArrow.generic.name = PIC_ARROW_DOWN;
	s_playermodel.dnArrow.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.dnArrow.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.dnArrow.generic.id = ID_DNARROW;
	s_playermodel.dnArrow.generic.x = 243;
	s_playermodel.dnArrow.generic.y = 349;
	s_playermodel.dnArrow.width = 16;
	s_playermodel.dnArrow.height = 16;
	s_playermodel.dnArrow.color = CT_DKPURPLE1;
	s_playermodel.dnArrow.color2 = CT_LTPURPLE1;
	s_playermodel.dnArrow.textX = MENU_BUTTON_TEXT_X;
	s_playermodel.dnArrow.textY = MENU_BUTTON_TEXT_Y;
	//s_playermodel.dnArrow.textEnum					= MBT_NEXTPAGE;
	s_playermodel.dnArrow.textcolor = CT_BLACK;
	s_playermodel.dnArrow.textcolor2 = CT_WHITE;

	s_playermodel.charModel.generic.m_Type = EMenuItemType::SpinControl;
	s_playermodel.charModel.generic.flags = QMF_INACTIVE | QMF_GRAYED; //QMF_HIGHLIGHT_IF_FOCUS
	s_playermodel.charModel.generic.id = ID_MODELSET;
	s_playermodel.charModel.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.charModel.generic.x = 291;//134;
	s_playermodel.charModel.generic.y = 209;//207;
	s_playermodel.charModel.textEnum = MBT_CHARMODEL;
	s_playermodel.charModel.textcolor = CT_BLACK;
	s_playermodel.charModel.textcolor2 = CT_WHITE;
	s_playermodel.charModel.color = CT_DKPURPLE1;
	s_playermodel.charModel.color2 = CT_LTPURPLE1;
	s_playermodel.charModel.width = 92;//80;
	s_playermodel.charModel.textX = 56;
	s_playermodel.charModel.textY = 2;
	s_playermodel.charModel.textFlags = UI_CENTER;
	s_playermodel.charModel.listX = 56;
	s_playermodel.charModel.listY = 24;
	s_playermodel.charModel.listFlags = UI_CENTER;
	s_playermodel.charModel.focusWidth = 110;
	s_playermodel.charModel.focusHeight = 40;
	s_playermodel.charModel.itemnames = (const char **)s_playermodel.modelList;

	s_playermodel.charSkin.generic.m_Type = EMenuItemType::SpinControl;
	s_playermodel.charSkin.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	s_playermodel.charSkin.generic.id = ID_SKINSET;
	s_playermodel.charSkin.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.charSkin.generic.x = 291;//134;
	s_playermodel.charSkin.generic.y = 266;//207;
	s_playermodel.charSkin.textEnum = MBT_CHARSKIN;
	s_playermodel.charSkin.textcolor = CT_BLACK;
	s_playermodel.charSkin.textcolor2 = CT_WHITE;
	s_playermodel.charSkin.color = CT_DKPURPLE1;
	s_playermodel.charSkin.color2 = CT_LTPURPLE1;
	s_playermodel.charSkin.width = 90;//80;
	s_playermodel.charSkin.textX = 56;
	s_playermodel.charSkin.textY = 2;
	s_playermodel.charSkin.textFlags = UI_CENTER;
	s_playermodel.charSkin.listX = 56;
	s_playermodel.charSkin.listY = 24;
	s_playermodel.charSkin.listFlags = UI_CENTER;
	s_playermodel.charSkin.focusWidth = 110;
	s_playermodel.charSkin.focusHeight = 40;
	s_playermodel.charSkin.itemnames = (const char**)s_playermodel.skinList;

	s_playermodel.raceFilter.generic.m_Type = EMenuItemType::SpinControl;
	if (!races) {
		s_playermodel.raceFilter.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	}
	else {
		s_playermodel.raceFilter.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}
	s_playermodel.raceFilter.generic.id = ID_RACEFILTER;
	s_playermodel.raceFilter.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.raceFilter.generic.x = 270;
	s_playermodel.raceFilter.generic.y = 391;
	s_playermodel.raceFilter.textEnum = MBT_RACEFILTER;
	s_playermodel.raceFilter.textcolor = CT_BLACK;
	s_playermodel.raceFilter.textcolor2 = CT_WHITE;
	s_playermodel.raceFilter.color = CT_DKPURPLE1;
	s_playermodel.raceFilter.color2 = CT_LTPURPLE1;
	s_playermodel.raceFilter.textX = 10;
	s_playermodel.raceFilter.textY = 1;
	s_playermodel.raceFilter.width = 65;
	s_playermodel.raceFilter.itemnames = (const char**)s_playermodel.raceNames;

	s_playermodel.genderFilter.generic.m_Type = EMenuItemType::SpinControl;
	if (!genders) {
		s_playermodel.genderFilter.generic.flags = QMF_INACTIVE | QMF_GRAYED;
	}
	else {
		s_playermodel.genderFilter.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}
	s_playermodel.genderFilter.generic.id = ID_GENDERFILTER;
	s_playermodel.genderFilter.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.genderFilter.generic.x = 270;
	s_playermodel.genderFilter.generic.y = 414;
	s_playermodel.genderFilter.textEnum = MBT_GENDERFILTER;
	s_playermodel.genderFilter.textcolor = CT_BLACK;
	s_playermodel.genderFilter.textcolor2 = CT_WHITE;
	s_playermodel.genderFilter.color = CT_DKPURPLE1;
	s_playermodel.genderFilter.color2 = CT_LTPURPLE1;
	s_playermodel.genderFilter.textX = 10;
	s_playermodel.genderFilter.textY = 1;
	s_playermodel.genderFilter.width = 65;
	s_playermodel.genderFilter.itemnames = (const char**)s_playermodel.genderNames;

	s_playermodel.apply.generic.m_Type = EMenuItemType::Bitmap;
	s_playermodel.apply.generic.name = GRAPHIC_SQUARE;
	s_playermodel.apply.generic.flags = QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.apply.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.apply.generic.id = ID_APPLY;
	s_playermodel.apply.generic.x = 281;
	s_playermodel.apply.generic.y = 321;
	s_playermodel.apply.width = 132;
	s_playermodel.apply.height = 39;
	s_playermodel.apply.color = CT_DKPURPLE1;
	s_playermodel.apply.color2 = CT_LTPURPLE1;
	s_playermodel.apply.textX = MENU_BUTTON_TEXT_X;
	s_playermodel.apply.textY = MENU_BUTTON_TEXT_Y;
	s_playermodel.apply.textEnum = MBT_VIDEOAPPLYCHANGES;
	s_playermodel.apply.textcolor = CT_BLACK;
	s_playermodel.apply.textcolor2 = CT_WHITE;

	s_playermodel.scrollBar.generic.m_Type = EMenuItemType::Action;
	s_playermodel.scrollBar.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
	s_playermodel.scrollBar.generic.x = 242;
	s_playermodel.scrollBar.generic.y = 108;
	s_playermodel.scrollBar.generic.id = ID_SCROLLBAR;
	s_playermodel.scrollBar.generic.ownerdraw = PlayerModel_DrawScrollBar;
	s_playermodel.scrollBar.width = 18;
	s_playermodel.scrollBar.height = MIN_SCROLLHEIGHT;
	s_playermodel.scrollBar.color = CT_DKPURPLE1;
	s_playermodel.scrollBar.color2 = CT_LTPURPLE1;

	Menu_AddItem(&s_playermodel.menu, &s_playermodel.model);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.data);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.player);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.playername);

	Menu_AddItem(&s_playermodel.menu, &s_playermodel.upArrow);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.scrollBar);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.dnArrow);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.charModel);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.charSkin);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.apply);

	for (i = 0; i < MAX_MENULISTITEMS; i++) {
		Menu_AddItem(&s_playermodel.menu, &s_playermodel.charMenu[i]);
	}

	Menu_AddItem(&s_playermodel.menu, &s_playermodel.raceFilter);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.genderFilter);

	Menu_AddItem(&s_playermodel.menu, &s_playermodel.back);
	Menu_AddItem(&s_playermodel.menu, &s_playermodel.mainmenu);

	if (s_playermodel.numChars >= MAX_MENULISTITEMS) {
		s_playermodel.upArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playermodel.dnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	PlayerModel_SetMenuItems();

	PlayerModel_OffsetCharList(&s_playermodel.scrollOffset);

	PlayerModel_SetupScrollBar(&s_playermodel.scrollBar);
	PlayerModel_UpdateScrollBar(&s_playermodel.scrollBar);

	// update user interface
	PlayerModel_UpdateModel();
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_Cache
=================
*/
void PlayerModel_Cache(void)
{
	UI_LogFuncBegin();
	//int32_t	i;

	s_playermodel.corner_ll_4_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_playermodel.corner_ll_4_4 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	s_playermodel.corner_lr_4_18 = trap_R_RegisterShaderNoMip("menu/common/corner_lr_4_18");
	s_playermodel.corner_lr_18_4 = trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_4");
	s_playermodel.corner_ur_18_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18");

	trap_R_RegisterShaderNoMip(PIC_ARROW_UP);
	trap_R_RegisterShaderNoMip(PIC_ARROW_DOWN);
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_DrawLoading
=================
*/
static void PlayerModel_DrawLoading(void)
{
	UI_LogFuncBegin();
	//register the corners now
	qhandle_t cornerPic = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
	int32_t	y = 50;

	trap_R_SetColor(colorTable[CT_DKPURPLE2]);

	UI_DrawHandlePic(132, y + 42, 128, -64, cornerPic);	// Top Left corner
	UI_DrawHandlePic(132, y + 252, 128, 64, cornerPic);	// Bottom Left corner

	UI_DrawHandlePic(429, y + 42, -128, -64, cornerPic);	// Top Right corner
	UI_DrawHandlePic(429, y + 252, -128, 64, cornerPic);	// Bottom Right corner

	UI_DrawHandlePic(145, y + 75, 395, 18, uis.whiteShader);		// Top
	UI_DrawHandlePic(132, y + 93, 47, 175, uis.whiteShader);		// Left side
	UI_DrawHandlePic(510, y + 93, 47, 175, uis.whiteShader);		// Right side
	UI_DrawHandlePic(147, y + 265, 65, 18, uis.whiteShader);		// Bottom Left
	UI_DrawHandlePic(477, y + 265, 65, 18, uis.whiteShader);		// Bottom Right
	UI_DrawHandlePic(214, y + 265, 261, 18, uis.whiteShader);		// Bottom

	UI_DrawProportionalString(345, y + 159, menu_normal_text[MNT_LOADING], UI_SMALLFONT | UI_CENTER, colorTable[CT_LTGOLD1]);
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_Cache
=================
*/
void UI_PlayerModelMenu(int32_t menuFrom)
{
	UI_LogFuncBegin();
	//TiM - Spawn a quick "loading" box
	//Sometimes this gives me the eerie creeps the game froze
	PlayerModel_DrawLoading();

	PlayerModel_MenuInit(menuFrom);

	UI_PushMenu(&s_playermodel.menu);

	//Menu_SetCursorToItem( &s_playermodel.menu, &s_playermodel.pics[s_playermodel.selectedmodel % MAX_MODELSPERPAGE] );
	UI_LogFuncEnd();
}


