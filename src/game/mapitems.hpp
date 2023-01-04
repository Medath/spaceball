/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#ifndef GAME_MAPITEMS_H
#define GAME_MAPITEMS_H

// layer types
enum
{
	LAYERTYPE_INVALID=0,
	LAYERTYPE_GAME, // not used
	LAYERTYPE_TILES,
	LAYERTYPE_QUADS,
	
	MAPITEMTYPE_VERSION=0,
	MAPITEMTYPE_INFO,
	MAPITEMTYPE_IMAGE,
	MAPITEMTYPE_ENVELOPE,
	MAPITEMTYPE_GROUP,
	MAPITEMTYPE_LAYER,
	MAPITEMTYPE_ENVPOINTS,
	

	CURVETYPE_STEP=0,
	CURVETYPE_LINEAR,
	CURVETYPE_SLOW,
	CURVETYPE_FAST,
	CURVETYPE_SMOOTH,
	NUM_CURVETYPES,
	
	// game layer tiles
	ENTITY_NULL=0,
	ENTITY_SPAWN,
	ENTITY_SPAWN_RED,
	ENTITY_SPAWN_BLUE,
	ENTITY_FLAGSTAND_RED,
	ENTITY_FLAGSTAND_BLUE,
	ENTITY_ARMOR_1,
	ENTITY_HEALTH_1,
	ENTITY_WEAPON_SHOTGUN,
	ENTITY_WEAPON_GRENADE,
	ENTITY_POWERUP_NINJA,
	ENTITY_WEAPON_RIFLE,
	NUM_ENTITIES,
	
	TILE_AIR=0,
	TILE_SOLID,
	TILE_DEATH,
	TILE_NOHOOK,

	TILE_NO_USE_1,
	TILE_NO_USE_2,
	TILE_NO_USE_3,
	TILE_NO_USE_4,
	TILE_NO_USE_5,
	TILE_NO_USE_6,
	TILE_NO_USE_7,
	TILE_NO_USE_8,
	TILE_NO_USE_9,
	TILE_NO_USE_a,
	TILE_NO_USE_b,
	TILE_NO_USE_c,
	
	TILE_NO_USE_10,
	TILE_NO_USE_11,
	TILE_NO_USE_12,
	TILE_NO_USE_13,
	TILE_NO_USE_14,
	TILE_NO_USE_15,
	TILE_NO_USE_16,
	TILE_NO_USE_17,
	TILE_NO_USE_18,
	TILE_NO_USE_19,
	TILE_NO_USE_1a,
	TILE_NO_USE_1b,
	TILE_NO_USE_1c,
	TILE_NO_USE_1d,
	TILE_NO_USE_1e,
	TILE_NO_USE_1f,
	
	TILE_NO_USE_20,
	TILE_NO_USE_21,
	TILE_NO_USE_22,
	TILE_NO_USE_23,
	TILE_NO_USE_24,
	TILE_NO_USE_25,
	TILE_NO_USE_26,
	TILE_NO_USE_27,
	TILE_NO_USE_28,
	TILE_NO_USE_29,
	TILE_NO_USE_2a,
	TILE_NO_USE_2b,
	TILE_NO_USE_2c,
	TILE_NO_USE_2d,
	TILE_NO_USE_2e,
	TILE_NO_USE_2f,
	
	TILE_NO_USE_31,
	TILE_NO_USE_32,
	TILE_NO_USE_33,
	TILE_NO_USE_34,
	TILE_NO_USE_35,
	
	TILE_FOOL_RED,
	TILE_FOOL_BLUE,
	TILE_GOAL_RED,
	TILE_GOAL_BLUE,
	TILE_GOAL_LIMIT_RED,
	TILE_GOAL_LIMIT_BLUE,
	ENTITY_SPAWN_KEEPER_RED,
	ENTITY_SPAWN_KEEPER_BLUE,
	TILE_BUMPER_SPEEDUP,
	TILE_SPEEDUP,
	TILE_SPEEDUP_UP,
	TILE_SPEEDUP_RIGHT,
	TILE_SPEEDUP_DOWN,
	TILE_SPEEDUP_LEFT,
	TILE_BUMPER_SLOWDOWN,
	TILE_SLOWDOWN,
	TILE_BUMPER_RANDOM,
	TILE_RANDOM,
	TILE_PLAYER_COLLISION,
	TILE_RESET_BALL,
	TILE_ACTION_1,
	TILE_ACTION_2,
	TILE_ACTION_3,
	TILE_ACTION_4,
	TILE_ACTION_5,
	TILE_ACTION_6,
	
	TILE_ICE,
	
	TILEFLAG_VFLIP=1,
	TILEFLAG_HFLIP=2,
	TILEFLAG_OPAQUE=4,
	
	LAYERFLAG_DETAIL=1,
	
	ENTITY_OFFSET=255-16*4,
};

typedef struct
{
	int x, y; // 22.10 fixed point
} POINT;

typedef struct
{
	int r, g, b, a;
} COLOR;

typedef struct
{
	POINT points[5];
	COLOR colors[4];
	POINT texcoords[4];
	
	int pos_env;
	int pos_env_offset;
	
	int color_env;
	int color_env_offset;
} QUAD;

typedef struct
{
	unsigned char index;
	unsigned char flags;
	unsigned char skip;
	unsigned char reserved;
} TILE;

typedef struct 
{
	int version;
	int width;
	int height;
	int external;
	int image_name;
	int image_data;
} MAPITEM_IMAGE;

struct MAPITEM_GROUP_v1
{
	int version;
	int offset_x;
	int offset_y;
	int parallax_x;
	int parallax_y;

	int start_layer;
	int num_layers;
} ;


struct MAPITEM_GROUP : public MAPITEM_GROUP_v1
{
	enum { CURRENT_VERSION=2 };
	
	int use_clipping;
	int clip_x;
	int clip_y;
	int clip_w;
	int clip_h;
} ;

typedef struct
{
	int version;
	int type;
	int flags;
} MAPITEM_LAYER;

typedef struct
{
	MAPITEM_LAYER layer;
	int version;
	
	int width;
	int height;
	int flags;
	
	COLOR color;
	int color_env;
	int color_env_offset;
	
	int image;
	int data;
} MAPITEM_LAYER_TILEMAP;

typedef struct
{
	MAPITEM_LAYER layer;
	int version;
	
	int num_quads;
	int data;
	int image;
} MAPITEM_LAYER_QUADS;

typedef struct
{
	int version;
} MAPITEM_VERSION;

typedef struct
{
	int time; // in ms
	int curvetype;
	int values[4]; // 1-4 depending on envelope (22.10 fixed point)
} ENVPOINT;

typedef struct
{
	int version;
	int channels;
	int start_point;
	int num_points;
	int name;
} MAPITEM_ENVELOPE;

#endif
