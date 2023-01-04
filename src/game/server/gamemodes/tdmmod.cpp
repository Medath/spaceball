#include <engine/e_server_interface.h>
#include <game/server/entities/character.hpp>
#include <game/server/player.hpp>
#include <engine/e_config.h>
#include "tdmmod.hpp"

GAMECONTROLLER_TDMMOD::GAMECONTROLLER_TDMMOD()
:GAMECONTROLLER_MOD()
{
	gametype = "mTDM";
	game_flags = GAMEFLAG_TEAMS;
}

int GAMECONTROLLER_TDMMOD::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon)
{
	GAMECONTROLLER_MOD::on_character_death(victim, killer, weapon);
	
	if(weapon >= 0)
	{
		// do team scoring
		if(killer == victim->player || killer->team == victim->player->team)
			teamscore[killer->team&1]--; // klant arschel
		else if(config.sv_kill_score)
			teamscore[killer->team&1]++; // good shit
	}
	return 0;
}

void GAMECONTROLLER_TDMMOD::tick()
{
	do_team_score_wincheck();
	GAMECONTROLLER_MOD::tick();
}
