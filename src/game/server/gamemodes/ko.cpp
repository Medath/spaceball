#include <string.h>
#include <engine/e_server_interface.h>
#include <game/server/gamecontext.hpp>
#include <engine/e_config.h>
#include "ko.hpp"

GAMECONTROLLER_KO::GAMECONTROLLER_KO()
:GAMECONTROLLER_MOD()
{
	gametype = "K.O.";
	game_flags = GAMEFLAG_TEAMS;
	config.sv_ko_mod = 1;
	config.sv_ball_mod = 0;
	config.sv_tournament_mode = 1;
}

void GAMECONTROLLER_KO::tick()
{
	if(game_over_tick != -1)
	{
		// game over.. wait for restart
		if(server_tick() > game_over_tick+server_tickspeed()*10)
		{
			int next_player = -1;
			int player[2];
			player[0] = -1;
			player[1] = -1;
			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if(game.players[i])
				{
					if(game.players[i]->queue >= 0)
					{
						if(next_player == -1)
						{
							next_player = i;
						}
						else if(game.players[i]->queue < game.players[next_player]->queue)
						{
							next_player = i;
						}
					}
					if(game.players[i]->team >= 0)
					{
						player[game.players[i]->team] = i;
					}
				}
			}
			if(next_player != -1)
			{
				if((teamscore[0] > teamscore[1] && player[0] != -1) || player[1] == -1)
				{
					if(player[1] != -1)
						game.players[player[1]]->set_team(-1);
					game.players[next_player]->set_team(1);
					game.players[next_player]->queue = -2;
				}
				else
				{
					if(player[0] != -1)
						game.players[player[0]]->set_team(-1);
					game.players[next_player]->set_team(0);
					game.players[next_player]->queue = -2;
				}
				startround();
				return;
			}
			else if(player[0] != -1 || player[1] != -1)
			{
				char buf[512];
				if((teamscore[0] > teamscore[1] && player[0] != -1) || (player[0] != -1 && player[1] == -1))
					strcpy(buf, server_clientname(player[0]));
				else if(player[1] != -1)
					strcpy(buf, server_clientname(player[1]));
				strcat(buf, " is the best player on this server.");
				if(player[0] != -1)
					game.players[player[0]]->set_team(-1);
				if(player[1] != -1)
					game.players[player[1]]->set_team(-1);
				for(int i = 0; i < MAX_CLIENTS; i++)
				{
					if(game.players[i])
					{
						game.players[i]->queue = -1;
						game.players[i]->last_input = server_tick();
					}
				}
				game.send_broadcast(buf, -1);
				game.send_chat(-1, -2, buf);
			}
			cyclemap();
			startround();
		}
	}
	do_team_score_wincheck();
	GAMECONTROLLER_MOD::tick();
}

int GAMECONTROLLER_KO::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon)
{
	GAMECONTROLLER_MOD::on_character_death(victim, killer, weapon);
	
	if(weapon >= 0)
	{
		// do team scoring
		if((killer == victim->player && config.sv_suicide_score) || killer->team == victim->player->team)
			teamscore[killer->team&1]--; // klant arschel
		else
			teamscore[killer->team&1]++; // good shit
	}
	return 0;
}
