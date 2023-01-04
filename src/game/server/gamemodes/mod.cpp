#include <string.h>
#include <engine/e_server_interface.h>
#include <engine/e_config.h>
#include <game/server/entities/character.hpp>
#include <game/server/gamecontext.hpp>
#include <game/server/player.hpp>
#include "mod.hpp"

GAMECONTROLLER_MOD::GAMECONTROLLER_MOD()
{
	mod = 1;
}

void GAMECONTROLLER_MOD::on_character_spawn(class CHARACTER *chr)
{
	if(config.sv_start_hammer)
	{
		chr->active_weapon = WEAPON_HAMMER;
		chr->last_weapon = WEAPON_HAMMER;
	}
	else if(config.sv_start_pistol)
	{
		chr->active_weapon = WEAPON_GUN;
		chr->last_weapon = WEAPON_GUN;
	}
	else if(config.sv_start_shotgun)
	{
		chr->active_weapon = WEAPON_SHOTGUN;
		chr->last_weapon = WEAPON_SHOTGUN;
	}
	else if(config.sv_start_grenade)
	{
		chr->active_weapon = WEAPON_GRENADE;
		chr->last_weapon = WEAPON_GRENADE;
	}
	else if(config.sv_start_rifle)
	{
		chr->active_weapon = WEAPON_RIFLE;
		chr->last_weapon = WEAPON_RIFLE;
	}
	else if(config.sv_start_ninja)
	{
		chr->active_weapon = WEAPON_NINJA;
		chr->last_weapon = WEAPON_NINJA;
	}
	else
	{
		chr->active_weapon = WEAPON_HAMMER;
		chr->last_weapon = WEAPON_HAMMER;
	}
	
	chr->weapons[WEAPON_HAMMER].got = config.sv_start_hammer ? 1 : 0;
	chr->weapons[WEAPON_HAMMER].ammo = config.sv_start_hammer ? -1 : 0;
	chr->weapons[WEAPON_GUN].got = config.sv_start_pistol ? 1 : 0;
	chr->weapons[WEAPON_GUN].ammo = config.sv_start_pistol;
	data->weapons.id[WEAPON_GUN].ammoregentime = config.sv_pistol_regen;
	chr->weapons[WEAPON_SHOTGUN].got = config.sv_start_shotgun ? 1 : 0;
	chr->weapons[WEAPON_SHOTGUN].ammo = config.sv_start_shotgun;
	data->weapons.id[WEAPON_SHOTGUN].ammoregentime = config.sv_shotgun_regen;
	chr->weapons[WEAPON_GRENADE].got = config.sv_start_grenade ? 1 : 0;
	chr->weapons[WEAPON_GRENADE].ammo = config.sv_start_grenade;
	data->weapons.id[WEAPON_GRENADE].ammoregentime = config.sv_grenade_regen;
	chr->weapons[WEAPON_RIFLE].got = config.sv_start_rifle ? 1 : 0;
	chr->weapons[WEAPON_RIFLE].ammo = config.sv_start_rifle;
	data->weapons.id[WEAPON_RIFLE].ammoregentime = config.sv_rifle_regen;
	if(config.sv_start_ninja)
	{
		chr->active_weapon = WEAPON_NINJA;
		chr->last_weapon = WEAPON_NINJA;
		chr->weapons[WEAPON_NINJA].got = 1;
		chr->weapons[WEAPON_NINJA].ammo = -1;
		chr->ninja.activationtick = server_tick();
	}
	else
	{
		chr->weapons[WEAPON_NINJA].got = 0;
		chr->weapons[WEAPON_NINJA].ammo = 0;
	}
	chr->health = config.sv_start_health;
	chr->armor = config.sv_start_armor;
	chr->fire_ball_tick = 0;
	chr->partly_dead = 0;
	chr->health_regen = 0;
	chr->armor_regen = 0;
}

int GAMECONTROLLER_MOD::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weaponid)
{
	if(victim)
		victim->player->respawn_tick = server_tick() + (config.sv_spawn_delay*server_tickspeed())/1000;
	// do scoreing
	if(!killer)
		return 0;
	if(killer == victim->player)
	{
		if(!config.sv_suicide_score)
			return 0;
		victim->player->score--; // suicide
	}
	else
	{
		if(is_teamplay() && victim->team == killer->team)
			killer->score--; // teamkill
		else if(config.sv_kill_score)
			killer->score++; // normal kill
	}
	return 0;
}

void GAMECONTROLLER_MOD::endround()
{
	if(config.sv_generate_pro_pw)
	{
		int players_team[2];
		players_team[0] = 0;
		players_team[1] = 0;
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(!game.players[i] || game.players[i]->team < 0)
				continue;
			players_team[game.players[i]->team]++;
		}
		if(players_team[0] && players_team[1] &&
			teamscore[0] > 4 && teamscore[1] > 4 &&
			abs(players_team[0] - players_team[1]) < 2 &&
			(float)teamscore[0]/(float)teamscore[1] > 0.7 && (float)teamscore[1]/(float)teamscore[0] > 0.7)
		{
			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if(!game.players[i] || game.players[i]->team == -1 || game.players[i]->score < 4)
					continue;
				if(game.players[i] &&
					(float)game.players[i]->score/((float)teamscore[game.players[i]->team]/(float)players_team[game.players[i]->team]) > 1.0)
				{
					char pw[9];
					server_generate_pw(i, pw);
					game.send_chat_target(i, config.sv_pre_password_msg);
					game.send_chat_target(i, pw);
				}
			}
		}
	}
	GAMECONTROLLER::endround();
}


void GAMECONTROLLER_MOD::do_team_score_wincheck()
{
	if(game_over_tick == -1 && !warmup)
	{
		// check score win condition
		if((config.sv_scorelimit > 0 && (teamscore[0] >= config.sv_scorelimit || teamscore[1] >= config.sv_scorelimit)) ||
			(config.sv_timelimit > 0 && (server_tick()-round_start_tick) >= config.sv_timelimit*server_tickspeed()*60))
		{
			if(teamscore[0] > teamscore[1]+config.sv_diff_score || teamscore[0]+config.sv_diff_score < teamscore[1])
				endround();
			else
				sudden_death = 1;
		}
	}
}
