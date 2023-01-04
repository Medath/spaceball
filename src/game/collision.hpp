/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#ifndef GAME_MAPRES_COL_H
#define GAME_MAPRES_COL_H

#include <base/vmath.hpp>

enum
{
	COLFLAG_SOLID=1,
	COLFLAG_DEATH=2,
	COLFLAG_NOHOOK=4,
};

int col_init();
int col_is_solid(int x, int y);
int col_is_red(int x, int y);
int col_is_blue(int x, int y);
int col_is_fool(int x, int y,int team);
int col_is_goal_limit(int x, int y, int team);
int col_is_bumper_speedup(int x, int y);
int col_is_speedup(int x, int y);
int col_is_speedup_up(int x, int y);
int col_is_speedup_right(int x, int y);
int col_is_speedup_down(int x, int y);
int col_is_speedup_left(int x, int y);
int col_is_bumper_slowdown(int x, int y);
int col_is_slowdown(int x, int y);
int col_is_bumper_random(int x, int y);
int col_is_random(int x, int y);
int col_is_player_collision(int x, int y);
int col_is_ice(int x, int y);
int col_is_ball_reset(int x, int y);
int col_is_action_tile(int x, int y);
int col_get(int x, int y);
int col_width();
int col_height();
int col_intersect_line(vec2 pos0, vec2 pos1, vec2 *out_collision, vec2 *out_before_collision);

#endif
