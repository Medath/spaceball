#include <string.h>
#include <engine/e_server_interface.h>
#include <engine/e_config.h>
#include <game/server/entities/character.hpp>
#include <game/server/player.hpp>
#include <game/server/gamecontext.hpp>
#include <game/mapitems.hpp>
#include "../entities/pickup.hpp"
#include "ball.hpp"

GAMECONTROLLER_BALL::GAMECONTROLLER_BALL()
:GAMECONTROLLER_MOD()
{
	gametype = "BALL";
	game_flags = GAMEFLAG_TEAMS;
	goalkeeper[0] = 0;
	goalkeeper[1] = 0;
	config.sv_ball_mod = 1;
	config.sv_ko_mod = 0;
	projectile_pickup = 0;
	action_bumper_speedup = (config.sv_bumper_speedup ? -1 : 0);
	action_speedup = (config.sv_speedup ? -1 : 0);
	action_speedup_up = (config.sv_speedup_up ? -1 : 0);
	action_speedup_right = (config.sv_speedup_right ? -1 : 0);
	action_speedup_down = (config.sv_speedup_down ? -1 : 0);
	action_speedup_left = (config.sv_speedup_left ? -1 : 0);
	action_bumper_slowdown = (config.sv_bumper_slowdown ? -1 : 0);
	action_slowdown = (config.sv_slowdown ? -1 : 0);
	action_bumper_random = (config.sv_bumper_random ? -1 : 0);
	action_random = (config.sv_random ? -1 : 0);
	action_ball_reset = (config.sv_ball_reset ? -1 : 0);
}

void GAMECONTROLLER_BALL::startround()
{
	GAMECONTROLLER_MOD::startround();
	spawning = config.sv_ball_respawn*server_tickspeed() + server_tick();
	projectile_pickup = 0;
	action_bumper_speedup = (config.sv_bumper_speedup ? -1 : 0);
	action_speedup = (config.sv_speedup ? -1 : 0);
	action_speedup_up = (config.sv_speedup_up ? -1 : 0);
	action_speedup_right = (config.sv_speedup_right ? -1 : 0);
	action_speedup_down = (config.sv_speedup_down ? -1 : 0);
	action_speedup_left = (config.sv_speedup_left ? -1 : 0);
	action_bumper_slowdown = (config.sv_bumper_slowdown ? -1 : 0);
	action_slowdown = (config.sv_slowdown ? -1 : 0);
	action_bumper_random = (config.sv_bumper_random ? -1 : 0);
	action_random = (config.sv_random ? -1 : 0);
	action_ball_reset = (config.sv_ball_reset ? -1 : 0);
}

void GAMECONTROLLER_BALL::action_parser(int tile, int action, const char *cmd)
{
	switch(tile)
	{
		case 0:
			action_bumper_speedup = action;
			break;
		case 1:
			action_speedup = action;
			break;
		case 2:
			action_speedup_up = action;
			break;
		case 3:
			action_speedup_right = action;
			break;
		case 4:
			action_speedup_down = action;
			break;
		case 5:
			action_speedup_left = action;
			break;
		case 6:
			action_bumper_slowdown = action;
			break;
		case 7:
			action_slowdown = action;
			break;
		case 8:
			action_bumper_random = action;
			break;
		case 9:
			action_random = action;
			break;
		case 10:
			action_ball_reset = action;
			break;
	}
	if(cmd[0] != 0)
		console_execute_line(cmd);
}

void GAMECONTROLLER_BALL::on_character_spawn(class CHARACTER *chr)
{
	if(config.sv_ball_mod)
	{
		if(config.sv_real_foot)
			config.sv_start_hammer = 0;
		else
			config.sv_start_hammer = 1;
		config.sv_start_pistol = 0;
	}
	if(config.sv_start_grenade)
	{
		chr->fire_ball_tick = server_tick() + config.sv_player_keeptime*server_tickspeed();
	}
	GAMECONTROLLER_MOD::on_character_spawn(chr);
}

int GAMECONTROLLER_BALL::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weaponid)
{
	GAMECONTROLLER_MOD::on_character_death(victim, killer, weaponid);
	if(killer && killer == victim->player)
	{
		if(victim->partly_dead)
			killer->respawn_tick = victim->partly_dead + (config.sv_spawn_delay*server_tickspeed())/1000;
		else
			killer->respawn_tick = server_tick() + (config.sv_spawn_delay*server_tickspeed())/1000;
	}
	return 0;
}

void GAMECONTROLLER_BALL::on_player_goal(class PLAYER *goaler,int goalteam)
{
	if(config.sv_silent_mode != 1 && config.sv_goal_timer)
	{
		char tmp[100];
		str_format(tmp, sizeof(tmp), "Goal shot after %.2f seconds", (float)(server_tick() - projectile_pickup)/(float)server_tickspeed());
		game.send_chat(-1, GAMECONTEXT::CHAT_ALL, tmp);
	}
	projectile_pickup = 0;
	PLAYER *passer_chr;
	if(passer == -1)
		passer_chr = NULL;
	else
		passer_chr = game.players[passer];
	
	char out_msg[512] = "";
	if(!goaler)
		goto reset_everything;
	else
	{
		if(config.sv_passes_only == 1 && goaler->team != goalteam && (!passer_chr || passer_chr == goaler || goaler->team != passer_chr->team))
		{
			game.send_broadcast("Pass Goals only !\n( No Point! )", -1);
			goto reset_everything;
		}
		else if(config.sv_passes_only == 0 || goaler->team == goalteam || (passer_chr && passer_chr != goaler && goaler->team == passer_chr->team))
		{
			strcpy(out_msg, server_clientname(goaler->client_id));
			if(goalteam == 0)
				strcat(out_msg, " scored for the Blue team");
			else
				strcat(out_msg, " scored for the Red team");
			if(goaler->team == goalteam)
				goaler->score-=config.sv_own_goal; // teamkill
			else
				goaler->score+=config.sv_goaler_score; // normal kill
			teamscore[!goalteam]+=config.sv_team_score;
		}
	}
	if(passer_chr && passer_chr != goaler && goaler->team != goalteam)
	{
		if(passer_chr->team != goalteam)
		{
			if(config.sv_new_pass_system == 0)
			{
				passer_chr->score+=config.sv_passer_score;
				teamscore[!goalteam]+=config.sv_team_pass_score;
				strcat(out_msg, "\nwith a pass from ");
				strcat(out_msg, server_clientname(passer_chr->client_id));
				dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, passer_chr->client_id);
			}
			else
			{
				float pass_usability_time = (goaler->get_ball_time - passer_chr->last_shot_time)/(float)server_tickspeed();
				float shot_usability_time = (server_tick() - goaler->last_shot_time)/(float)server_tickspeed();


				if(shot_usability_time >= config.sv_shot_usability)
				{
					// passes only system
					// punkte werden wieder abgezogen, message ausgegeben und zu reset_everything springen
					if(config.sv_passes_only == 1)
					{
						goaler->score-=config.sv_goaler_score;
						teamscore[!goalteam]-=config.sv_team_score;
						game.send_broadcast("Pass Goals only !\n( No Point! )", -1);
						goto reset_everything;
					}
					strcat(out_msg, "Goal was out of time!");

					if(config.sv_silent_mode != 1)
					{
						char tmp[100];
						str_format(tmp, sizeof(tmp), "Luckgoal?! no points for pass (%.2f sec too late)", (float)(config.sv_shot_usability-shot_usability_time));
						game.send_chat(-1, GAMECONTEXT::CHAT_ALL, tmp);
					}

					dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, -1);
				}
				else if(pass_usability_time <= config.sv_pass_usability)
				{
					passer_chr->score+=config.sv_passer_score;
					teamscore[!goalteam]+=config.sv_team_pass_score;

					strcat(out_msg, "\nwith a pass from ");
					strcat(out_msg, server_clientname(passer_chr->client_id));

					if(config.sv_silent_mode != 1)
					{
						char tmp[100];
						str_format(tmp, sizeof(tmp), "The pass lasts %.2f seconds.", pass_usability_time);
						game.send_chat(-1, GAMECONTEXT::CHAT_ALL, tmp);
					}

					dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, passer_chr->client_id);
				}
				else
				{
					// passes only system
					// punkte werden wieder abgezogen, message ausgegeben und zu reset_everything springen
					if(config.sv_passes_only == 1)
					{
						goaler->score-=config.sv_goaler_score;
						teamscore[!goalteam]-=config.sv_team_score;
						game.send_broadcast("Pass Goals only !\n( No Point! )", -1);
						goto reset_everything;
					}
					// otherwise the passer doesn't get any points
					strcat(out_msg, "\npass was out of time ...");

					if(config.sv_silent_mode != 1)
					{
						char tmp[100];
						str_format(tmp, sizeof(tmp), "The pass was %.2f seconds too long.", (float)(pass_usability_time-config.sv_pass_usability));
						game.send_chat(-1, GAMECONTEXT::CHAT_ALL, tmp);
					}

					dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, -1);
				}
			}
		}
		else
			dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, -1);
	}
	else
		dbg_msg("ball","goal=%d goaler=%d passer=%d", goalteam, goaler->client_id, -1);
	
	game.send_broadcast(out_msg, -1);
	
reset_everything:
	passer = -1;
	if(config.sv_passes_only == 0 ||
	(config.sv_owngoal_respawn == 1 && goaler->team == goalteam) ||
	(passer_chr != NULL && passer_chr != goaler && goaler->team == passer_chr->team))
	{
		CHARACTER *tmp;
		for (int i=0;i < MAX_CLIENTS ;i++)
		{
			if(game.players[i])
			{
				if((tmp = game.players[i]->get_character()) != NULL)
				{
					tmp->player->respawn_tick = server_tick() + (config.sv_spawn_delay*server_tickspeed())/1000;
					tmp->reset();
				}
				else if(game.players[i]->respawn_tick < server_tick() + (config.sv_spawn_delay*server_tickspeed())/1000)
				{
					game.players[i]->respawn_tick = server_tick() + (config.sv_spawn_delay*server_tickspeed())/1000;
				}
			}
		}
	}
	game.create_sound_global(SOUND_CTF_CAPTURE);
	spawning = server_tick() + config.sv_ball_respawn*server_tickspeed();
	game.world.remove_entities();
	game.world.remove_projectiles();
	action_bumper_speedup = (config.sv_bumper_speedup ? -1 : 0);
	action_speedup = (config.sv_speedup ? -1 : 0);
	action_speedup_up = (config.sv_speedup_up ? -1 : 0);
	action_speedup_right = (config.sv_speedup_right ? -1 : 0);
	action_speedup_down = (config.sv_speedup_down ? -1 : 0);
	action_speedup_left = (config.sv_speedup_left ? -1 : 0);
	action_bumper_slowdown = (config.sv_bumper_slowdown ? -1 : 0);
	action_slowdown = (config.sv_slowdown ? -1 : 0);
	action_bumper_random = (config.sv_bumper_random ? -1 : 0);
	action_random = (config.sv_random ? -1 : 0);
	action_ball_reset = (config.sv_ball_reset ? -1 : 0);
}

bool GAMECONTROLLER_BALL::can_spawn(PLAYER *player, vec2 *out_pos)
{
	if(player->goalkeeper)
	{
		SPAWNEVAL eval;
	
		// spectators can't spawn
		if(player->team == -1)
			return false;
		evaluate_spawn_type(&eval, 3+(player->team&1));

		*out_pos = eval.pos;
		return eval.got;
	}
	else
	{
		return GAMECONTROLLER::can_spawn(player, out_pos);
	}
}

void GAMECONTROLLER_BALL::tick()
{
	if(server_tick()%server_tickspeed() == 0)
	{
		if(action_bumper_speedup > 0)
			action_bumper_speedup--;
		if(action_speedup > 0)
			action_speedup--;
		if(action_speedup_up > 0)
			action_speedup_up--;
		if(action_speedup_right > 0)
			action_speedup_right--;
		if(action_speedup_down > 0)
			action_speedup_down--;
		if(action_speedup_left > 0)
			action_speedup_left--;
		if(action_bumper_slowdown > 0)
			action_bumper_slowdown--;
		if(action_slowdown > 0)
			action_slowdown--;
		if(action_bumper_random > 0)
			action_bumper_random--;
		if(action_random > 0)
			action_random--;
		if(action_ball_reset > 0)
			action_ball_reset--;
	}
	do_team_score_wincheck();
	GAMECONTROLLER_MOD::tick();
}