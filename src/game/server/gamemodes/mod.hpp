#ifndef GAMECONTROLLER_MODS
#define GAMECONTROLLER_MODS

#include <game/server/gamecontroller.hpp>

class GAMECONTROLLER_MOD : public GAMECONTROLLER
{
public:
	GAMECONTROLLER_MOD();
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	virtual void on_character_spawn(class CHARACTER *chr);
	virtual void do_team_score_wincheck();
	void endround();
};

#endif
