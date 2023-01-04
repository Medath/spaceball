#include <game/server/gamemodes/mod.hpp>

class GAMECONTROLLER_BALL : public GAMECONTROLLER_MOD
{
public:
	GAMECONTROLLER_BALL();
	virtual void on_player_goal(class PLAYER *goaler,int goalteam);
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	virtual void on_character_spawn(class CHARACTER *chr);
	virtual bool can_spawn(PLAYER *player, vec2 *out_pos);
	void startround();
	virtual void tick();
	virtual void action_parser(int tile, int action, const char* cmd);
};

