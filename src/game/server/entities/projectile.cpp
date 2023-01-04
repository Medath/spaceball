#include <engine/e_server_interface.h>
#include <engine/e_config.h>
#include <game/generated/g_protocol.hpp>
#include <game/server/gamecontext.hpp>
#include "projectile.hpp"


//////////////////////////////////////////////////
// projectile
//////////////////////////////////////////////////
PROJECTILE::PROJECTILE(int type, int owner, vec2 pos, vec2 dir, int span,
	int damage, int flags, float force, int sound_impact, int weapon)
: ENTITY(NETOBJTYPE_PROJECTILE)
{
	this->type = type;
	this->pos = pos;
	this->direction = dir;
	this->lifespan = span;
	this->owner = owner;
	this->flags = flags;
	this->force = force;
	this->damage = damage;
	this->sound_impact = sound_impact;
	this->weapon = weapon;
	this->bounce = 0;
	this->start_tick = server_tick();
	if((dir.x < 0?-dir.x:dir.x) > (dir.y < 0?-dir.y:dir.y))
		this->pick_up_again = abs(dir.x * (float)server_tickspeed() / tuning.grenade_speed * 70.0);
	else
		this->pick_up_again = abs(dir.y * (float)server_tickspeed() / tuning.grenade_speed * 70.0);
	this->randomized = false;
	game.world.insert_entity(this);
}

void PROJECTILE::reset()
{
	game.world.destroy_entity(this);
}

vec2 PROJECTILE::get_pos(float time)
{
	float curvature = 0;
	float speed = 0;
	if(type == WEAPON_GRENADE)
	{
		curvature = tuning.grenade_curvature;
		speed = tuning.grenade_speed;
	}
	else if(type == WEAPON_SHOTGUN)
	{
		curvature = tuning.shotgun_curvature;
		speed = tuning.shotgun_speed;
	}
	else if(type == WEAPON_GUN)
	{
		curvature = tuning.gun_curvature;
		speed = tuning.gun_speed;
	}
	
	return calc_pos(pos, direction, curvature, speed, time);
}


#define VERTEX_DOWN_RIGHT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)col_pos.x+(OFFSET_X))/32))*32 - free_pos.x + config.sv_vertex_radius;\
float ny = ((int)(((int)col_pos.y+(OFFSET_Y))/32))*32 - free_pos.y + config.sv_vertex_radius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny < 0)\
{\
	direction.y = velocity.y;\
	direction.x = -velocity.x;\
}\
else if(nx < 0)\
{\
	direction.x = velocity.x;\
	direction.y = -velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (velocity.x * nx + velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	direction.x = velocity.x - 2*nx;\
	direction.y = velocity.y - 2*ny;\
}\
curpos.x -= 1;\
curpos.y -= 1;

#define VERTEX_UP_RIGHT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)col_pos.x+(OFFSET_X))/32))*32 - free_pos.x + config.sv_vertex_radius;\
float ny = ((int)(((int)col_pos.y+(OFFSET_Y))/32))*32 - free_pos.y+31 - config.sv_vertex_radius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny > 0)\
{\
	direction.y = velocity.y;\
	direction.x = -velocity.x;\
}\
else if(nx < 0)\
{\
	direction.x = velocity.x;\
	direction.y = -velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (velocity.x * nx + velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	direction.x = velocity.x - 2*nx;\
	direction.y = velocity.y - 2*ny;\
}\
curpos.x -= 1;\
curpos.y += 1;

#define VERTEX_DOWN_LEFT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)col_pos.x+(OFFSET_X))/32))*32 - free_pos.x+31 - config.sv_vertex_radius;\
float ny = ((int)(((int)col_pos.y+(OFFSET_Y))/32))*32 - free_pos.y + config.sv_vertex_radius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny < 0)\
{\
	direction.y = velocity.y;\
	direction.x = -velocity.x;\
}\
else if(nx > 0)\
{\
	direction.x = velocity.x;\
	direction.y = -velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (velocity.x * nx + velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	direction.x = velocity.x - 2*nx;\
	direction.y = velocity.y - 2*ny;\
}\
curpos.x += 1;\
curpos.y -= 1;

#define VERTEX_UP_LEFT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)col_pos.x+(OFFSET_X))/32))*32 - free_pos.x+31 - config.sv_vertex_radius;\
float ny = ((int)(((int)col_pos.y+(OFFSET_Y))/32))*32 - free_pos.y+31 - config.sv_vertex_radius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny > 0)\
{\
	direction.y = velocity.y;\
	direction.x = -velocity.x;\
}\
else if(nx > 0)\
{\
	direction.x = velocity.x;\
	direction.y = -velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (velocity.x * nx + velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	direction.x = velocity.x - 2*nx;\
	direction.y = velocity.y - 2*ny;\
}\
curpos.x += 1;\
curpos.y += 1;

void PROJECTILE::tick()
{
	if(config.sv_ball_mod)
	{
		bool explode = false;
		float nt = (server_tick()-start_tick+1)/(float)server_tickspeed();
		float ct = (server_tick()-start_tick)/(float)server_tickspeed();
		float pt = (server_tick()-start_tick-1)/(float)server_tickspeed();
		vec2 curpos = get_pos(ct);
		vec2 prevpos = get_pos(pt);
		vec2 free_pos = vec2(0,0);
		vec2 col_pos = vec2(0,0);
		int action_tile = col_is_action_tile(curpos.x, curpos.y);
		float free_time = -1.0f;
		
		float steps = 1.0/config.sv_calc_steps;
		if(col_is_ball_reset(curpos.x, curpos.y) && game.controller->action_ball_reset != 0)
		{
			explode = true;
			goto ball_explode;
		}
		if (col_is_speedup(curpos.x, curpos.y) && game.controller->action_speedup != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			float vec_length = sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
			vec2 normalized = vec2(velocity.x/vec_length, velocity.y/vec_length);
			direction.x += normalized.x*(float)config.sv_accelerate/100000.0;
			direction.y = velocity.y + normalized.y*(float)config.sv_accelerate/100000.0;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if (col_is_speedup_up(curpos.x, curpos.y) && game.controller->action_speedup_up != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			direction.y = velocity.y - (float)config.sv_accelerate_up/100000.0;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if (col_is_speedup_right(curpos.x, curpos.y) && game.controller->action_speedup_right != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			direction.x += (float)config.sv_accelerate_right/100000.0;
			direction.y = velocity.y;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if (col_is_speedup_down(curpos.x, curpos.y) && game.controller->action_speedup_down != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			direction.y = velocity.y + (float)config.sv_accelerate_down/100000.0;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if (col_is_speedup_left(curpos.x, curpos.y) && game.controller->action_speedup_left != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			direction.x -= (float)config.sv_accelerate_left/100000.0;
			direction.y = velocity.y;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if (col_is_slowdown(curpos.x, curpos.y) && game.controller->action_slowdown != 0)
		{
			vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
			direction.x = velocity.x * (100000-config.sv_brake)/100000.0;
			direction.y = velocity.y * (100000-config.sv_brake)/100000.0;
			pos = curpos;
			start_tick = server_tick();
			nt = 1/(float)server_tickspeed();
			ct = 0;
			pt = -nt;
		}
		else if(action_tile)
		{
			switch(action_tile)
			{
				case 1:
					if(config.sv_action_1_cause == 0)
					{
						game.controller->action_parser(config.sv_action_1_tile, config.sv_action_1_type, config.sv_action_1_cmd);
					}
					break;
				case 2:
					if(config.sv_action_2_cause == 0)
					{
						game.controller->action_parser(config.sv_action_2_tile, config.sv_action_2_type, config.sv_action_2_cmd);
					}
					break;
				case 3:
					if(config.sv_action_3_cause == 0)
					{
						game.controller->action_parser(config.sv_action_3_tile, config.sv_action_3_type, config.sv_action_3_cmd);
					}
					break;
				case 4:
					if(config.sv_action_4_cause == 0)
					{
						game.controller->action_parser(config.sv_action_4_tile, config.sv_action_4_type, config.sv_action_4_cmd);
					}
					break;
				case 5:
					if(config.sv_action_5_cause == 0)
					{
						game.controller->action_parser(config.sv_action_5_tile, config.sv_action_5_type, config.sv_action_5_cmd);
					}
					break;
				case 6:
					if(config.sv_action_6_cause == 0)
					{
						game.controller->action_parser(config.sv_action_6_tile, config.sv_action_6_type, config.sv_action_6_cmd);
					}
					break;
			}
		}
		if (col_is_random(curpos.x, curpos.y) && game.controller->action_random != 0)
		{
			if(!randomized)
			{
				vec2 velocity = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
				float random = rand()/10000.0;
				float vec_length = sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
				direction = vec2(sin(random)*vec_length, cos(random)*vec_length);
				randomized = true;
				pos = curpos;
				start_tick = server_tick();
				nt = 1/(float)server_tickspeed();
				ct = 0;
				pt = -nt;
			}
		}
		else
		{
			randomized = false;
		}
		
		
		
		float i;
		for(i = ct+steps; i <= nt; i += steps)
		{
			vec2 tmp_pos = get_pos(i);
			if(col_is_solid((int)tmp_pos.x+5, (int)tmp_pos.y+5))
			{
				break;
			}
			else if(col_is_solid((int)tmp_pos.x+5, (int)tmp_pos.y-5))
			{
				break;
			}
			else if(col_is_solid((int)tmp_pos.x-5, (int)tmp_pos.y+5))
			{
				break;
			}
			else if(col_is_solid((int)tmp_pos.x-5, (int)tmp_pos.y-5))
			{
				break;
			}
			free_time = i;
		}
		
		
		
		if(free_time == -1.0f)
		{
			pick_up_again = 0;
			explode = true;
			for(float st = ct; st > ct-1.0f; st -= steps)			// i changed from ct-1.0f
			{
				vec2 search_pos = get_pos(st);
				if(!(col_is_solid((int)search_pos.x+5, (int)search_pos.y+5) || col_is_solid((int)search_pos.x+5, (int)search_pos.y-5) || col_is_solid((int)search_pos.x-5, (int)search_pos.y+5) || col_is_solid((int)search_pos.x-5, (int)search_pos.y-5)))
				{
					free_time = st;
					col_pos = get_pos(st+steps);
					free_pos = get_pos(st);
					explode = false;
					break;
				}
			}
		}
		else
		{
			free_time += ct;
			col_pos = get_pos(free_time+steps);
			free_pos = get_pos(free_time);
		}
		if(free_time < nt-steps)
		{
			bool coll[4] = {false,false,false,false};
			coll[0] = col_is_solid((int)col_pos.x+5, (int)col_pos.y+5);
			coll[1] = col_is_solid((int)col_pos.x+5, (int)col_pos.y-5);
			coll[2] = col_is_solid((int)col_pos.x-5, (int)col_pos.y+5);
			coll[3] = col_is_solid((int)col_pos.x-5, (int)col_pos.y-5);
			
			short int colliding_points = 0;
			if(coll[0])
			{
				colliding_points++;
			}
			if(coll[1])
			{
				colliding_points++;
			}
			if(coll[2])
			{
				colliding_points++;
			}
			if(coll[3])
			{
				colliding_points++;
			}
			
			vec2 velocity;
			if(col_is_bumper_random(free_pos.x, free_pos.y) && !randomized && game.controller->action_bumper_random != 0)
			{
				vec2 velocity_tmp = vec2(direction.x, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*ct));
				float random = rand()/10000.0;
				float vec_length = sqrt(velocity_tmp.x*velocity_tmp.x + velocity_tmp.y*velocity_tmp.y);
				velocity = vec2(sin(random)*vec_length, cos(random)*vec_length);
			}
			else
			{
				velocity = vec2(direction.x/(config.sv_bounce_loss_x+100)*100, (direction.y + 2*tuning.grenade_curvature/10000*tuning.grenade_speed*(server_tick()-start_tick)/(float)server_tickspeed())/(config.sv_bounce_loss_y+100)*100);
			}
			if(colliding_points == 1)
			{
				if(config.sv_conservative_collision)
				{
					if(coll[0])
					{
						if(velocity.x > velocity.y)
						{
							direction.x = -velocity.x;
							direction.y = velocity.y;
						}
						else
						{
							direction.x = velocity.x;
							direction.y = -velocity.y;
						}
					}
					else if(coll[1])
					{
						if(velocity.x > -velocity.y)
						{
							direction.x = -velocity.x;
							direction.y = velocity.y;
						}
						else
						{
							direction.x = velocity.x;
							direction.y = -velocity.y;
						}
					}
					else if(coll[2])
					{
						if(-velocity.x > velocity.y)
						{
							direction.x = -velocity.x;
							direction.y = velocity.y;
						}
						else
						{
							direction.x = velocity.x;
							direction.y = -velocity.y;
						}
					}
					else if(coll[3])
					{
						if(-velocity.x > -velocity.y)
						{
							direction.x = -velocity.x;
							direction.y = velocity.y;
						}
						else
						{
							direction.x = velocity.x;
							direction.y = -velocity.y;
						}
					}
				}
				else if(coll[0])
				{
					VERTEX_DOWN_RIGHT(5,5)
				}
				else if(coll[1])
				{
					VERTEX_UP_RIGHT(5,-5)
				}
				else if(coll[2])
				{
					VERTEX_DOWN_LEFT(-5,5)
				}
				else if(coll[3])
				{
					VERTEX_UP_LEFT(-5,-5)
				}
			}
			else if(colliding_points == 3)
			{
				direction.x = -velocity.x;
				direction.y = -velocity.y;
			}
			else if(colliding_points == 2)
			{
				int coll_neighb[2];
				coll_neighb[0] = 0;
				coll_neighb[1] = 0;
				if(coll[0] && coll[1])
				{
					coll_neighb[0] = col_is_solid((int)col_pos.x+5, (int)col_pos.y-32);
					coll_neighb[1] = col_is_solid((int)col_pos.x+5, (int)col_pos.y+32);
					if(coll_neighb[0] && coll_neighb[1])
					{
						direction.x = -velocity.x;
						direction.y = velocity.y;
					}
					else if(coll_neighb[0])
					{
						VERTEX_UP_RIGHT(5,0)
					}
					else if(coll_neighb[1])
					{
						VERTEX_DOWN_RIGHT(5,0)
					}
					else
					{
						direction.x = -velocity.x;
						direction.y = velocity.y;
					}
				}
				else if(coll[1] && coll[3])
				{
					coll_neighb[0] = col_is_solid((int)col_pos.x-32, (int)col_pos.y-5);
					coll_neighb[1] = col_is_solid((int)col_pos.x+32, (int)col_pos.y-5);
					if(coll_neighb[0] && coll_neighb[1])
					{
						direction.x = velocity.x;
						direction.y = -velocity.y;
					}
					else if(coll_neighb[0])
					{
						VERTEX_UP_LEFT(0,-5)
					}
					else if(coll_neighb[1])
					{
						VERTEX_UP_RIGHT(0,-5)
					}
					else
					{
						direction.x = velocity.x;
						direction.y = -velocity.y;
					}
				}
				else if(coll[3] && coll[2])
				{
					coll_neighb[0] = col_is_solid((int)col_pos.x-5, (int)col_pos.y-32);
					coll_neighb[1] = col_is_solid((int)col_pos.x-5, (int)col_pos.y+32);
					if(coll_neighb[0] && coll_neighb[1])
					{
						direction.x = -velocity.x;
						direction.y = velocity.y;
					}
					else if(coll_neighb[0])
					{
						VERTEX_UP_LEFT(-5,0)
					}
					else if(coll_neighb[1])
					{
						VERTEX_DOWN_LEFT(-5,0)
					}
					else
					{
						direction.x = -velocity.x;
						direction.y = velocity.y;
					}
				}
				else if(coll[2] && coll[0])
				{
					coll_neighb[0] = col_is_solid((int)col_pos.x-32, (int)col_pos.y+5);
					coll_neighb[1] = col_is_solid((int)col_pos.x+32, (int)col_pos.y+5);
					if(coll_neighb[0] && coll_neighb[1])
					{
						direction.x = velocity.x;
						direction.y = -velocity.y;
					}
					else if(coll_neighb[0])
					{
						VERTEX_DOWN_LEFT(0,5)
					}
					else if(coll_neighb[1])
					{
						VERTEX_DOWN_RIGHT(0,5)
					}
					else
					{
						direction.x = velocity.x;
						direction.y = -velocity.y;
					}
				}
			}
			else
			{
				if(col_is_solid((int)col_pos.x, (int)free_pos.y))
				{
					direction.x = -velocity.x;
					direction.y = velocity.y;
				}
				else if(col_is_solid((int)free_pos.x, (int)col_pos.y))
				{
					direction.x = velocity.x;
					direction.y = -velocity.y;
				}
				else
				{
					direction.x = -velocity.x;
					direction.y = -velocity.y;
				}
			}
			pos.x = free_pos.x - direction.x*(ct-free_time);
			pos.y = free_pos.y - (direction.y + (ct-free_time)*2*tuning.grenade_curvature/10000*tuning.grenade_speed);
			if(col_is_bumper_speedup(free_pos.x, free_pos.y) && game.controller->action_bumper_speedup != 0)
			{

				float vec_length = sqrt(direction.x*direction.x + direction.y*direction.y);
				vec2 normalized = vec2(direction.x/vec_length, direction.y/vec_length);
				direction.x += normalized.x*(float)config.sv_bumper_accelerate/1000.0;
				direction.y += normalized.y*(float)config.sv_bumper_accelerate/1000.0;
			}
			else if(col_is_bumper_slowdown(free_pos.x, free_pos.y) && game.controller->action_bumper_slowdown != 0)
			{
				direction.x = velocity.x * (config.sv_bumper_brake - 100000)/100000.0;
				direction.y = velocity.y * (config.sv_bumper_brake - 100000)/100000.0;
			}
			start_tick = server_tick();
			pick_up_again = 0;
		}
ball_explode:
		CHARACTER *targetchr;
		if(pick_up_again == 0)
		{
			targetchr = game.world.intersect_character(prevpos, curpos, 6.0f, curpos, NULL);
		}
		else
		{
			pick_up_again--;
			targetchr = game.world.intersect_character(prevpos, curpos, 6.0f, curpos, game.get_player_char(owner));
		}
		game.controller->ball = curpos;
		if((targetchr && (targetchr->armor > 0 || config.sv_pickup_with_no_armor)) || --lifespan < 0 || explode)
		{
			if(explode || lifespan < 0)
			{
				game.controller->spawning = server_tick();
			}
			if(config.sv_explosions && (lifespan >= 0 || weapon == WEAPON_GRENADE))
				game.create_sound(curpos, sound_impact);

			if(flags & PROJECTILE_FLAGS_EXPLODE && config.sv_explosions)
				game.create_explosion(curpos, owner, weapon, false);
			else if(targetchr)
			{
				// matthis
				// catch the ball
				targetchr->player->get_ball_time = server_tick();

				game.controller->passer = owner;
				targetchr->weapons[WEAPON_GRENADE].got = true;
				if(targetchr->weapons[WEAPON_GRENADE].ammo < 10)
					targetchr->weapons[WEAPON_GRENADE].ammo++;
				targetchr->active_weapon = WEAPON_GRENADE;
				targetchr->last_weapon = WEAPON_GRENADE;
				if(targetchr->player->goalkeeper)
					targetchr->fire_ball_tick = server_tick()+server_tickspeed()*config.sv_goal_keeptime;
				else
					targetchr->fire_ball_tick = server_tick()+server_tickspeed()*config.sv_player_keeptime;
				targetchr->reload_timer = config.sv_ball_reloader;
			}

			game.world.destroy_entity(this);
		}
		else if (col_is_red((int)curpos.x,(int)curpos.y) && owner > -1 && game.players[owner] && game.players[owner]->team != -1)
		{
			game.world.destroy_entity(this);
			game.controller->on_player_goal(game.players[owner], 0);
		}
		else if (col_is_blue((int)curpos.x,(int)curpos.y) && owner > -1 && game.players[owner] && game.players[owner]->team != -1)
		{
			game.world.destroy_entity(this);
			game.controller->on_player_goal(game.players[owner], 1);
		}
	}
	else
	{
		float pt = (server_tick()-start_tick-1)/(float)server_tickspeed();
		float ct = (server_tick()-start_tick)/(float)server_tickspeed();
		vec2 prevpos = get_pos(pt);
		vec2 curpos = get_pos(ct);

		lifespan--;
	
		int collide = col_intersect_line(prevpos, curpos, &curpos, 0);
		//int collide = col_check_point((int)curpos.x, (int)curpos.y);
		CHARACTER *ownerchar = game.get_player_char(owner);
		CHARACTER *targetchr = game.world.intersect_character(prevpos, curpos, 6.0f, curpos, ownerchar);
		if(targetchr || collide || lifespan < 0)
		{
			if(lifespan >= 0 || weapon == WEAPON_GRENADE)
				game.create_sound(curpos, sound_impact);

			if(flags & PROJECTILE_FLAGS_EXPLODE)
				game.create_explosion(curpos, owner, weapon, false);
			else if(targetchr)
				targetchr->take_damage(direction * max(0.001f, force), damage, owner, weapon);

			game.world.destroy_entity(this);
		}
	}
}

void PROJECTILE::fill_info(NETOBJ_PROJECTILE *proj)
{
	proj->x = (int)pos.x;
	proj->y = (int)pos.y;
	proj->vx = (int)(direction.x*100.0f);
	proj->vy = (int)(direction.y*100.0f);
	proj->start_tick = start_tick;
	proj->type = type;
}

void PROJECTILE::snap(int snapping_client)
{
	float ct = (server_tick()-start_tick)/(float)server_tickspeed();
	
	if(networkclipped(snapping_client, get_pos(ct)))
		return;

	NETOBJ_PROJECTILE *proj = (NETOBJ_PROJECTILE *)snap_new_item(NETOBJTYPE_PROJECTILE, id, sizeof(NETOBJ_PROJECTILE));
	fill_info(proj);
}
