#ifndef GAME_SERVER_ENTITIES_BALL_H
#define GAME_SERVER_ENTITIES_BALL_H

#include "game/server/entity.h"

class CBall : public CEntity
{
public:
	CBall(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, int Span);

	vec2 GetPos(float Time);
	void FillInfo(CNetObj_Projectile *pProj);

	virtual void Reset();
	virtual void Tick();
	virtual void TickPaused();
	virtual void Snap(int SnappingClient);

private:
	vec2 m_Direction;
	int m_Owner;
  int m_LifeSpan;
	int m_StartTick;
	int m_pickupAgain;

	vec2 client_Pos; //"fake" Position sent to client
	int client_StartTick; //"fake" StartTick sent to client
	
};

#endif
