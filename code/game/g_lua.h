#ifndef _G_LUA_H
#define _G_LUA_H

#include "g_local.h"

#include "lua.hpp"

enum g_luaLimits_e {
	NUM_VMS = 3
};

#if defined __linux__
#define HOSTARCH	"UNIX"
#define EXTENSION	"so"
#elif defined WIN32
#define HOSTARCH	"WIN32"
#define EXTENSION	"dll"
#elif defined __APPLE__
#define HOSTARCH	"UNIX"
#define EXTENSION	"dylib"
#endif

#define Lua_RegisterGlobal(L, n, v) (lua_pushstring(L, v), lua_setglobal(L, n))
#define Lua_RegConstInteger(L, n) (lua_pushstring(L, #n), lua_pushinteger(L, n), lua_settable(L, -3))
#define Lua_RegConstString(L, n) (lua_pushstring(L, #n), lua_pushstring(L, n), lua_settable(L, -3))

typedef struct {
	int32_t			id;
	char			filename[MAX_QPATH];
	char*			code;
	int32_t			code_size;
	int32_t			error;
	lua_State*		L;
} lvm_t;

extern lvm_t* lVM[NUM_VMS];

/**
 * @brief A debug print function for Lua.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param fmt Describes the format to use.
 *
 */
void QDECL LUA_DEBUG(const char* fmt, ...);

/**
 * @brief Lua logger.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param fmt Describes the format to use.
 *
 */
void QDECL LUA_LOG(const char* fmt, ...);

/**
 * @brief Initialize Lua.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @return A qboolean indicating success of failure.
 */
qboolean G_Lua_Init(void);

/**
 * @brief A Lua function call.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] vm   If non-null, the virtual mashine.
 * @param [in] func If non-null, the function.
 * @param nargs		    Number of arguments.
 * @param nresults	    Number of results.
 *
 * @return A qboolean indicating success or failure.
 */
qboolean G_Lua_Call(lvm_t* vm, char* func, int32_t nargs, int32_t nresults);

/**
 * @brief Resume execution of a Lua vm.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] vm   If non-null, the virtual mashine.
 * @param [in] T    If non-null, the lua_State * to process.
 * @param [in] func If non-null, the function.
 * @param nargs		The nargs.
 *
 * @return A qboolean indicating success of failure.
 */
qboolean G_Lua_Resume(lvm_t* vm, lua_State* T, char* func, int32_t nargs);

/**
 * @brief Get a Lua function.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] vm   If non-null, the virtual mashine.
 * @param [in] name If non-null, the name.
 *
 * @return A qboolean.
 */
qboolean G_Lua_GetFunction(lvm_t* vm, char* name);

/**
 * @brief Start Lua VM.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] vm If non-null, the virtual mashine.
 *
 * @return A qboolean.
 */
qboolean G_Lua_StartVM(lvm_t* vm);

/**
 * @brief Stop Lua VM.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] vm If non-null, the virtual mashine.
 */
void G_Lua_StopVM(lvm_t* vm);

/**
 * @brief Shutdown Lua.
 *
 * @author GSIO01
 * @date 14.10.2013
 */
void G_Lua_Shutdown(void);

/**
 * @brief Print the lua status to a client console.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] ent If non-null, the client.
 */
void G_Lua_Status(gentity_t* ent);

/**
 * @brief Get the lua VM for a given Lua state.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] L If non-null, the lua_State * to process.
 *
 * @return null if it fails, else a lvm_t*.
 */
lvm_t* G_Lua_GetVM(lua_State* L);

// lua_entity.c
typedef struct {
	gentity_t *e;
} luaGentity_t;

/**
 * @brief Open lua entity library.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] L If non-null, the lua_State * to process.
 *
 * @return An int.
 */
int32_t	Luaopen_Entity(lua_State* L);

/**
 * @brief Push an entity onto the lua stack.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] L   If non-null, the lua_State * to process.
 * @param [in] ent If non-null, the entity to push.
 */
void Lua_PushEntity(lua_State* L, gentity_t* ent);

/**
 * @brief Get an entity from the lua stack.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] L If non-null, the lua_State * to process.
 * @param argNum	 The argument which is the entity.
 *
 * @return null if it fails, else a lent_t*.
 */
luaGentity_t* Lua_GetEntity(lua_State* L, int32_t argNum);

/**
 * @brief Open lua weapons library.
 *
 * @author GSIO01
 * @date 14.10.2013
 *
 * @param [in] L If non-null, the lua_State * to process.
 *
 * @return An int.
 */
int32_t	Luaopen_Weapons(lua_State* L);

// lua_trace.c
typedef struct {
	trace_t *tr;
} luaTrace_t;

void Lua_PushTrace(lua_State* L, trace_t* tr);
luaTrace_t *Lua_GetTrace(lua_State* L, int32_t argNum);
int Luaopen_Trace(lua_State* L);

// lua_game.c
int32_t Luaopen_Game(lua_State* L);

// lua_qmath.c
int32_t Luaopen_Qmath(lua_State* L);

// lua_vector.c
int32_t Luaopen_Vector(lua_State* L);
void Lua_PushVector(lua_State* L, vec3_t v);
vec_t* Lua_GetVector(lua_State* L, int32_t argNum);
int32_t Lua_IsVector(lua_State* L, int32_t index);
vec3_t* Lua_GetVectorMisc(lua_State* L, int32_t* index);

// lua_mover.c
int32_t Luaopen_Mover(lua_State* L);

// lua_cinematic.c
int32_t Luaopen_Cinematic(lua_State* L);

// lua_sound.c
int32_t Luaopen_Sound(lua_State* L);

/**
 *	Lua hook for InitGame event.
 *
 *	\param leveltime level time the event occured
 *	\param radomseed a random seed
 *	\param restart is this a map restart?
 */
void LuaHook_G_InitGame(int32_t leveltime, uint32_t randomseed, int32_t restart);

/**
 * Lua hook for weapon firing.
 *
 * \param ent The player.
 * \param muzzle The muzzle point.
 * \param forwar The forward vector.
 * \param alt_fire Indicator whether the shot is alternative fire mode.
 * \param weapon Weapon number.
 */
void LuaHook_G_FireWeapon(int32_t ent, vec3_t muzzle, vec3_t forward, int32_t alt_fire, int32_t weapon);

/**
 *	Lua hook for Shutdown event.
 *
 *	\param restart is this a map restart?
 */
void LuaHook_G_Shutdown(int32_t restart);

/**
 *	Lua hook for RunFrame event.
 *
 *	\param leveltime the level time
 */
void LuaHook_G_RunFrame(int32_t leveltime);

/**
 * Lua hook for G_Print function.
 *
 *	\param text text to be printed
 */
void LuaHook_G_Print(char* text);
/**
 * Lua hook for G_ClientPrint function.
 *
 *	\param text text to be printed
 *	\param entnum entity index for client the text gets send to
 */
void LuaHook_G_ClientPrint(char* text, int32_t entnum);

/**
 * Lua hook for entity think function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the think function was called on
 *	\return success or fail
 */
void LuaHook_G_EntityThink(char* function, int32_t entnum);

/**
 * Lua hook for entity touch function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the touch function was called on
 *	\param othernum entiy index of touching entity
 *	\return success or fail
 */
void LuaHook_G_EntityTouch(char* function, int32_t entnum, int32_t othernum);

/**
 * Lua hook for entity use function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the use function was called on
 * \param othernum entity index of other entity
 * \param activatornum entity index of activating entity
 */
void LuaHook_G_EntityUse(char* function, int32_t entnum, int32_t othernum, int32_t activatornum);

/**
 * Lua hook for entity hurt function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the hurt function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 */
void LuaHook_G_EntityHurt(char* function, int32_t entnum, int32_t inflictornum, int32_t attackernum);

/**
 * Lua hook for entity die function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the die function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 * \param dmg ammount of damage
 * \param mod means of death
 */
void LuaHook_G_EntityDie(char* function, int32_t entnum, int32_t inflictornum, int32_t attackernum, int32_t dmg, int32_t mod);

/**
 * Lua hook for entity free function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the free function was called on
 */
void LuaHook_G_EntityFree(char* function, int32_t entnum);

/**
 * Lua hook for entity trigger function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the trigger function was called on
 * \param othernum entity index of triggering entity
 */
void LuaHook_G_EntityTrigger(char* function, int32_t entnum, int32_t othernum);

/**
 * Lua hook for entity spawn function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the spawn function was called on
 */
void LuaHook_G_EntitySpawn(char* function, int32_t entnum);

/**
 * Lua hook for entity reached function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached function was called on
 */
void LuaHook_G_EntityReached(char* function, int32_t entnum);

/**
 * Lua hook for entity reached angular function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached angular function was called on
 */
void LuaHook_G_EntityReachedAngular(char* function, int32_t entnum);

/**
 * \fn unsigned G_Lua_NumThreads(void);
 *
 * \brief Get information about lua threads.
 * 		  
 * Prints status information about lua threads and returns the number of active lua threads.
 *
 * \author GSIO01
 * \date 14.10.2013
 *
 * \return An unsigned.
 */

void LuaHook_G_CmdScriptCall(char* function);

unsigned G_Lua_NumThreads(void);

/**
 * \fn void G_Lua_CollectGarbage(void);
 *
 * \brief Garbage collection of inactive lua threads.
 *
 * \author GSIO01
 * \date 14.10.2013
 */
void G_Lua_CollectGarbage(void);

#endif
