#include <game/server/gamemodes/mod.hpp>
#include <game/server/entity.hpp>

class GAMECONTROLLER_CTFMOD : public GAMECONTROLLER_MOD
{
public:
	class FLAG *flags[2];
	
	GAMECONTROLLER_CTFMOD();
	virtual void tick();
	
	virtual bool on_entity(int index, vec2 pos);
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
};
