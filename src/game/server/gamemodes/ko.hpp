#include <game/server/gamemodes/mod.hpp>

class GAMECONTROLLER_KO : public GAMECONTROLLER_MOD
{
public:
	GAMECONTROLLER_KO();
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	void tick();
};

