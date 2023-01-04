/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <base/system.h>
#include <base/math.hpp>
#include <base/vmath.hpp>

#include <math.h>
#include <engine/e_common_interface.h>
#include <game/mapitems.hpp>
#include <game/layers.hpp>
#include <game/collision.hpp>

static TILE *tiles;
static int width = 0;
static int height = 0;

int col_width() { return width; }
int col_height() { return height; }

int col_init()
{
	width = layers_game_layer()->width;
	height = layers_game_layer()->height;
	tiles = (TILE *)map_get_data(layers_game_layer()->data);
	
	for(int i = 0; i < width*height; i++)
	{
		int index = tiles[i].index;
		
		if(index > 128)
			continue;
		
		if(index == TILE_DEATH)
			tiles[i].index = COLFLAG_DEATH;
		else if(index == TILE_SOLID)
			tiles[i].index = COLFLAG_SOLID;
		else if(index == TILE_NOHOOK)
			tiles[i].index = COLFLAG_SOLID|COLFLAG_NOHOOK;
		else if(index == TILE_FOOL_RED)
			tiles[i].index = TILE_FOOL_RED;
		else if(index == TILE_FOOL_BLUE)
			tiles[i].index = TILE_FOOL_BLUE;
		else if(index == TILE_GOAL_RED)
			tiles[i].index = TILE_GOAL_RED;
		else if(index == TILE_GOAL_BLUE)
			tiles[i].index = TILE_GOAL_BLUE;
		else if(index == TILE_GOAL_LIMIT_RED)
			tiles[i].index = TILE_GOAL_LIMIT_RED;
		else if(index == TILE_GOAL_LIMIT_BLUE)
			tiles[i].index = TILE_GOAL_LIMIT_BLUE;
		else if(index == ENTITY_SPAWN_KEEPER_RED)
			tiles[i].index = ENTITY_SPAWN_KEEPER_RED;
		else if(index == ENTITY_SPAWN_KEEPER_BLUE)
			tiles[i].index = ENTITY_SPAWN_KEEPER_BLUE;
		else if(index == TILE_ICE)
			tiles[i].index = TILE_ICE;
		else if(index == TILE_BUMPER_SPEEDUP)
			tiles[i].index = TILE_BUMPER_SPEEDUP;
		else if(index == TILE_SPEEDUP)
			tiles[i].index = TILE_SPEEDUP;
		else if(index == TILE_SPEEDUP_UP)
			tiles[i].index = TILE_SPEEDUP_UP;
		else if(index == TILE_SPEEDUP_RIGHT)
			tiles[i].index = TILE_SPEEDUP_RIGHT;
		else if(index == TILE_SPEEDUP_DOWN)
			tiles[i].index = TILE_SPEEDUP_DOWN;
		else if(index == TILE_SPEEDUP_LEFT)
			tiles[i].index = TILE_SPEEDUP_LEFT;
		else if(index == TILE_BUMPER_SLOWDOWN)
			tiles[i].index = TILE_BUMPER_SLOWDOWN;
		else if(index == TILE_SLOWDOWN)
			tiles[i].index = TILE_SLOWDOWN;
		else if(index == TILE_BUMPER_RANDOM)
			tiles[i].index = TILE_BUMPER_RANDOM;
		else if(index == TILE_RANDOM)
			tiles[i].index = TILE_RANDOM;
		else if(index == TILE_PLAYER_COLLISION)
			tiles[i].index = TILE_PLAYER_COLLISION;
		else if(index == TILE_RESET_BALL)
			tiles[i].index = TILE_RESET_BALL;
		else if(index == TILE_ACTION_1)
			tiles[i].index = TILE_ACTION_1;
		else if(index == TILE_ACTION_2)
			tiles[i].index = TILE_ACTION_2;
		else if(index == TILE_ACTION_3)
			tiles[i].index = TILE_ACTION_3;
		else if(index == TILE_ACTION_4)
			tiles[i].index = TILE_ACTION_4;
		else if(index == TILE_ACTION_5)
			tiles[i].index = TILE_ACTION_5;
		else if(index == TILE_ACTION_6)
			tiles[i].index = TILE_ACTION_6;
		else
			tiles[i].index = 0;
	}
				
	return 1;
}


int col_get(int x, int y)
{
	int nx = clamp(x/32, 0, width-1);
	int ny = clamp(y/32, 0, height-1);
	
	if(tiles[ny*width+nx].index > 128)
		return 0;
	if(tiles[ny*width+nx].index == COLFLAG_SOLID ||tiles[ny*width+nx].index == (COLFLAG_SOLID|COLFLAG_NOHOOK) || tiles[ny*width+nx].index == COLFLAG_DEATH)
		return tiles[ny*width+nx].index;
	else
		return 0;
}

int col_is_solid(int x, int y)
{
	return col_get(x,y)&COLFLAG_SOLID;
}

int col_is_red(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return (int)tiles[ny*width+nx].index == TILE_GOAL_RED;
}
int col_is_blue(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return (int)tiles[ny*width+nx].index == (int)TILE_GOAL_BLUE;
}
int col_is_goal_limit(int x, int y, int team)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	
	if(team == 0)
		return tiles[ny*width+nx].index == TILE_GOAL_LIMIT_RED;
	if(team == 1)
		return tiles[ny*width+nx].index == TILE_GOAL_LIMIT_BLUE;
	return 0;
}
int col_is_fool(int x, int y, int team)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	
	if(team==0)return tiles[ny*width+nx].index == TILE_FOOL_RED;
	else if(team==1)return tiles[ny*width+nx].index == TILE_FOOL_BLUE;
	else return 0;
}
int col_is_bumper_speedup(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_BUMPER_SPEEDUP;
}
int col_is_speedup(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SPEEDUP;
}
int col_is_speedup_up(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SPEEDUP_UP;
}
int col_is_speedup_right(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SPEEDUP_RIGHT;
}
int col_is_speedup_down(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SPEEDUP_DOWN;
}
int col_is_speedup_left(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SPEEDUP_LEFT;
}
int col_is_bumper_slowdown(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_BUMPER_SLOWDOWN;
}
int col_is_slowdown(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_SLOWDOWN;
}
int col_is_bumper_random(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_BUMPER_RANDOM;
}
int col_is_random(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_RANDOM;
}
int col_is_player_collision(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_PLAYER_COLLISION;
}
int col_is_ice(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_ICE;
}

int col_is_ball_reset(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	return tiles[ny*width+nx].index == TILE_RESET_BALL;
}

int col_is_action_tile(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	if(tiles[ny*width+nx].index - TILE_ACTION_1 >= 0 && tiles[ny*width+nx].index - TILE_ACTION_1 < 6)
	{
		return tiles[ny*width+nx].index - TILE_ACTION_1 + 1;
	}
	return 0;
}

// TODO: rewrite this smarter!
int col_intersect_line(vec2 pos0, vec2 pos1, vec2 *out_collision, vec2 *out_before_collision)
{
	float d = distance(pos0, pos1);
	vec2 last = pos0;
	
	for(float f = 0; f < d; f++)
	{
		float a = f/d;
		vec2 pos = mix(pos0, pos1, a);
		if(col_is_solid(round(pos.x), round(pos.y)))
		{
			if(out_collision)
				*out_collision = pos;
			if(out_before_collision)
				*out_before_collision = last;
			return col_get(round(pos.x), round(pos.y));
		}
		last = pos;
	}
	if(out_collision)
		*out_collision = pos1;
	if(out_before_collision)
		*out_before_collision = pos1;
	return 0;
}
