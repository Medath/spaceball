#include "dmmod.hpp"

GAMECONTROLLER_DMMOD::GAMECONTROLLER_DMMOD()
:GAMECONTROLLER_MOD()
{
	gametype = "mDM";
}

void GAMECONTROLLER_DMMOD::tick()
{
	do_player_score_wincheck();
	GAMECONTROLLER_MOD::tick();
}
