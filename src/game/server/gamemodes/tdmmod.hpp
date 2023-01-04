#include <game/server/gamemodes/mod.hpp>

class GAMECONTROLLER_TDMMOD : public GAMECONTROLLER_MOD
{
public:
	GAMECONTROLLER_TDMMOD();
	
	int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	virtual void tick();
};
