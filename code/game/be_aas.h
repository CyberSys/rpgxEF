// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		be_aas.h
 *
 * desc:		Area Awareness System, stuff exported to the AI
 *
 * $Archive: /StarTrek/Code-DM/game/be_aas.h $
 * $Author: Jmonroe $
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef BE_AAS_H_
#define BE_AAS_H_

#ifndef MAX_STRINGFIELD
#define MAX_STRINGFIELD				80
#endif

//travel flags
typedef enum {
	TFL_INVALID = 0x0000001,		//!<traveling temporary not possible
	TFL_WALK = 0x0000002,			//!<walking
	TFL_CROUCH = 0x0000004,			//!<crouching
	TFL_BARRIERJUMP = 0x0000008,	//!<jumping onto a barrier
	TFL_JUMP = 0x0000010,			//!<jumping
	TFL_LADDER = 0x0000020,			//!<climbing a ladder
	TFL_WALKOFFLEDGE = 0x0000080,	//!<walking of a ledge
	TFL_SWIM = 0x0000100,			//!<swimming
	TFL_WATERJUMP = 0x0000200,		//!<jumping out of the water
	TFL_TELEPORT = 0x0000400,		//!<teleporting
	TFL_ELEVATOR = 0x0000800,		//!<elevator
	TFL_ROCKETJUMP = 0x0001000,		//!<rocket jumping
	TFL_BFGJUMP = 0x0002000,		//!<bfg jumping
	TFL_GRAPPLEHOOK = 0x0004000,	//!<grappling hook
	TFL_DOUBLEJUMP = 0x0008000,		//!<double jump
	TFL_RAMPJUMP = 0x0010000,		//!<ramp jump
	TFL_STRAFEJUMP = 0x0020000,		//!<strafe jump
	TFL_JUMPPAD = 0x0040000,		//!<jump pad
	TFL_AIR = 0x0080000,			//!<travel through air
	TFL_WATER = 0x0100000,			//!<travel through water
	TFL_SLIME = 0x0200000,			//!<travel through slime
	TFL_LAVA = 0x0400000,			//!<travel through lava
	TFL_DONOTENTER = 0x0800000,		//!<travel through donotenter area
	TFL_FUNCBOB = 0x1000000,		//!<func bobbing
	TFL_FLIGHT = 0x2000000			//!<flight
} aas_travelFlags;

//default travel flags
static const uint32_t TFL_DEFAULT = TFL_WALK | TFL_CROUCH | TFL_BARRIERJUMP | \
TFL_JUMP | TFL_LADDER | \
TFL_WALKOFFLEDGE | TFL_SWIM | TFL_WATERJUMP | \
TFL_TELEPORT | TFL_ELEVATOR | \
TFL_AIR | TFL_WATER | TFL_JUMPPAD | TFL_FUNCBOB;

typedef enum {
	SOLID_NOT,			//!< no interaction with other objects
	SOLID_TRIGGER,		//!< only touch when inside, after moving
	SOLID_BBOX,			//!< touch on edge
	SOLID_BSP			//!< bsp clip, touch on edge
} solid_t;

//!a trace is returned when a box is swept through the AAS world
typedef struct aas_trace_s {
	qboolean	startsolid;	//!< if true, the initial point was in a solid area
	double		fraction;	//!< time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		//!< final position
	int32_t		ent;		//!< entity blocking the trace
	int32_t		lastarea;	//!< last area the trace was in (zero if none)
	int32_t		area;		//!< area blocking the trace (zero if none)
	int32_t		planenum;	//!< number of the plane that was hit
} aas_trace_t;

/* Defined in botlib.h

//bsp_trace_t hit surface
typedef struct bsp_surface_s
{
char name[16];
int flags;
int value;
} bsp_surface_t;

//a trace is returned when a box is swept through the BSP world
typedef struct bsp_trace_s
{
qboolean		allsolid;	// if true, plane is not valid
qboolean		startsolid;	// if true, the initial point was in a solid area
float			fraction;	// time completed, 1.0 = didn't hit anything
vec3_t			endpos;		// final position
cplane_t		plane;		// surface normal at impact
float			exp_dist;	// expanded plane distance
int				sidenum;	// number of the brush side hit
bsp_surface_t	surface;	// hit surface
int				contents;	// contents on other side of surface hit
int				ent;		// number of entity hit
} bsp_trace_t;
*/

//!entity info
typedef struct aas_entityinfo_s {
	int32_t	valid;			//!< true if updated this frame
	int32_t	type;			//!< entity type
	int32_t	flags;			//!< entity flags
	double	ltime;			//!< local time
	double	update_time;	//!< time between last and current update
	int32_t	number;			//!< number of the entity
	vec3_t	origin;			//!< origin of the entity
	vec3_t	angles;			//!< angles of the model
	vec3_t	old_origin;		//!< for lerping
	vec3_t	lastvisorigin;	//!< last visible origin
	vec3_t	mins;			//!< bounding box minimums
	vec3_t	maxs;			//!< bounding box maximums
	int32_t	groundent;		//!< ground entity
	int32_t	solid;			//!< solid type
	int32_t	modelindex;		//!< model used
	int32_t	modelindex2;	//!< weapons, CTF flags, etc
	int32_t	frame;			//!< model frame number
	int32_t	event;			//!< impulse events -- muzzle flashes, footsteps, etc
	int32_t	eventParm;		//!< even parameter
	int32_t	powerups;		//!< bit flags
	int32_t	weapon;			//!< determines weapon and flash model, etc
	int32_t	legsAnim;		//!< mask off ANIM_TOGGLEBIT
	int32_t	torsoAnim;		//!< mask off ANIM_TOGGLEBIT
} aas_entityinfo_t;


//client movement prediction stop events, stop as soon as:
typedef enum {
	SE_NONE = 0,
	SE_HITGROUND = 1,		//!< the ground is hit
	SE_LEAVEGROUND = 2,		//!< there's no ground
	SE_ENTERWATER = 4,		//!< water is entered
	SE_ENTERSLIME = 8,		//!< slime is entered
	SE_ENTERLAVA = 16,		//!< lava is entered
	SE_HITGROUNDDAMAGE = 32,		//!< the ground is hit with damage
	SE_GAP = 64,	//!< there's a gap
	SE_TOUCHJUMPPAD = 128,		//!< touching a jump pad area
	SE_TOUCHTELEPORTER = 256,		//!< touching teleporter
	SE_ENTERAREA = 512,		//!< the given stoparea is entered
	SE_HITGROUNDAREA = 1024,	//!< a ground face in the area is hit
	SE_HITBOUNDINGBOX = 2048	//!< hit the specified bounding box
} aas_stopEvents;

typedef struct aas_clientmove_s {
	vec3_t	endpos;			//!<position at the end of movement prediction
	int32_t endarea;		//!<area at end of movement prediction
	vec3_t	velocity;		//!<velocity at the end of movement prediction
	aas_trace_t trace;		//!<last trace
	int32_t presencetype;	//!<presence type at end of movement prediction
	int32_t stopevent;		//!<event that made the prediction stop
	double endcontents;		//!<contents at the end of movement prediction
	double time;			//!<time predicted ahead
	int32_t frames;			//!<number of frames predicted ahead
} aas_clientmove_t;

typedef struct aas_altroutegoal_s {
	vec3_t origin;
	int32_t areanum;
	uint16_t starttraveltime;
	uint16_t goaltraveltime;
	uint16_t extratraveltime;
} aas_altroutegoal_t;

#endif /* BE_AAS_H_ */

