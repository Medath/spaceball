#ifndef GAME_SERVER_GAMEMODES_BALL_H
#define GAME_SERVER_GAMEMODES_BALL_H
#include <game/server/gamecontroller.h>

class CGameControllerBALL : public IGameController
{
private:
	int m_PickupRespawnTick = -1;
	bool m_PickupSpawned = false;
	int m_PickedupTick = 0;

public:
	CGameControllerBALL(class CGameContext *pGameServer);
	virtual void Tick();
	virtual void OnCharacterSpawn(CCharacter *pChr);
	virtual void OnGoal(CPlayer *Scorer, int team);
	virtual void OnBallSpawn();
	virtual void OnBallPickup(CCharacter *Char);
	virtual int GetBallPickupTick();
	virtual void OnBallTimeout();
	virtual int GetPickupSpawnTick(int originalTick);
	virtual int OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon);
};
#endif
