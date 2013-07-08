#ifndef _G_CLIENT_H
#define _G_CLIENT_H

#include "g_local.h"

/**
 * Select a spawnpoint.
 *
 * \param avoidPoint Point to avoid.
 * \param origin Origin.
 * \param angles Angles.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Client_SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );

/**
 * Set the clients view angle.
 *
 * \param ent Entity for which to set view angle.
 * \param angle New view angle.
 */
void G_Client_SetViewAngle( gentity_t* ent, vec3_t angle );

/**
 * Respawn client.
 *
 * \param ent Client to respawn.
 */
void G_Client_Respawn(gentity_t *ent);

/**
 * Init the body que.
 */
void G_Client_InitBodyQue(void);

//TiM - Delayed Transport Beam
void G_InitTransport( int32_t clientNum, vec3_t origin, vec3_t angles );

/**
 * Pick a random team.
 *
 * \param ignoreClientNum Client to ignore.
 * \return Random team.
 */
team_t G_Client_PickTeam( int32_t ignoreClientNum );

/**
 * Clients user info changed.
 *
 * \param clientNum Client num.
 */
void G_Client_UserinfoChanged( int32_t clientNum );

/**
 * Client connect.
 *
 * \param clientNum Client num.
 * \param firstTime Indicate whether this was the first connect.
 * \param isBot Indicate if bot or player.
 */
/*@shared@*/ char* G_Client_Connect( int32_t clientNum, qboolean firstTime, qboolean isBot );

/**
 * Initialize client.
 *
 * \param clientNum Client num.
 * \param careAboutWarmup Indicate whether to care about warm up.
 * \param isBot Bot or client?
 * \param first First time?
 */
void G_Client_Begin( int32_t clientNum, qboolean careAboutWarmup, qboolean isBot, qboolean first );

/**
 * Store the clients initial status.
 *
 * \param ent The client.
 */
void G_Client_StoreClientInitialStatus( gentity_t* ent );

/**
 * Spawn client.
 *
 * \param ent Client to spawn
 * \param rpgx_spawn rpgx_spawn
 * \param fromDeath Is this a spawn from death?
 */
void G_Client_Spawn( gentity_t* ent, int32_t rpgx_spawn, qboolean fromDeath );

/**
 * Disconnect client.
 *
 * \param clientNum Client num.
 */
void G_Client_Disconnect( int32_t clientNum );

/**
 *  Check if player is an admin.
 *
 * \param ent the player
 * \return Whether player is admin.
 */
qboolean IsAdmin( gentity_t* ent );

/**
 *	Get location message for a client.
 * 
 * \param ent The client.
 * \param loc Location char.
 * \param loclen Length of location char.
 * \return Indicates success or fail.
 */
qboolean G_Client_GetLocationMsg(gentity_t* ent, char* loc, int32_t loclen);

/**
 * Check client statuses.
 */
void G_Client_CheckClientStatus(void);

/**
 * Client command.
 *
 * \param clientNum Client num.
 */
void G_Client_Command( int clientNum );

/**
 * Begin intermission.
 */
void G_Client_BeginIntermission(void);

/**
 * Let the client die.
 *
 * \param self Client.
 * \param inflictor Entity causing death.
 * \param attacker Entity that made inflicotr cause death.
 * \param damage ammount of demage
 * \param mod means of death
 */
void G_Client_Die (gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

/**
 * Add score for the client.
 *
 * \param ent The client.
 * \param score Ammount of score to add.
 */
void G_Client_AddScore( gentity_t* ent, int score );

#endif /* _G_CLIENT_H */