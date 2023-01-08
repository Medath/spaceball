#include <game/server/gamecontext.h>
#include <game/server/gamecontroller.h>

#include "character.h"
#include "ball.h"

CBall::CBall(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, int Span) 
  : CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE, Pos)
{
  m_Direction = Dir;
	m_StartTick = Server()->Tick();
  m_Owner = Owner;
  m_LifeSpan = Span;

  float largestAbs = abs(m_Direction.x) > abs(m_Direction.y) ? m_Direction.x : m_Direction.y;
  m_pickupAgain = abs(largestAbs * (float)Server()->TickSpeed() / GameServer()->Tuning()->m_GrenadeSpeed * 70.0);

  GameWorld()->InsertEntity(this);
}

//MAGIC copied from the original ball mod. Don't question it. It works
#define VERTEX_DOWN_RIGHT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)ColPos.x+(OFFSET_X))/32))*32 - FreePos.x + VertexRadius;\
float ny = ((int)(((int)ColPos.y+(OFFSET_Y))/32))*32 - FreePos.y + VertexRadius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny < 0)\
{\
	m_Direction.y = Velocity.y;\
	m_Direction.x = -Velocity.x;\
}\
else if(nx < 0)\
{\
	m_Direction.x = Velocity.x;\
	m_Direction.y = -Velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (Velocity.x * nx + Velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	m_Direction.x = Velocity.x - 2*nx;\
	m_Direction.y = Velocity.y - 2*ny;\
}\
CurPos.x -= 1;\
CurPos.y -= 1;

#define VERTEX_UP_RIGHT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)ColPos.x+(OFFSET_X))/32))*32 - FreePos.x + VertexRadius;\
float ny = ((int)(((int)ColPos.y+(OFFSET_Y))/32))*32 - FreePos.y+31 - VertexRadius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny > 0)\
{\
	m_Direction.y = Velocity.y;\
	m_Direction.x = -Velocity.x;\
}\
else if(nx < 0)\
{\
	m_Direction.x = Velocity.x;\
	m_Direction.y = -Velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (Velocity.x * nx + Velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	m_Direction.x = Velocity.x - 2*nx;\
	m_Direction.y = Velocity.y - 2*ny;\
}\
CurPos.x -= 1;\
CurPos.y += 1;

#define VERTEX_DOWN_LEFT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)ColPos.x+(OFFSET_X))/32))*32 - FreePos.x+31 - VertexRadius;\
float ny = ((int)(((int)ColPos.y+(OFFSET_Y))/32))*32 - FreePos.y + VertexRadius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny < 0)\
{\
	m_Direction.y = Velocity.y;\
	m_Direction.x = -Velocity.x;\
}\
else if(nx > 0)\
{\
	m_Direction.x = Velocity.x;\
	m_Direction.y = -Velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (Velocity.x * nx + Velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	m_Direction.x = Velocity.x - 2*nx;\
	m_Direction.y = Velocity.y - 2*ny;\
}\
CurPos.x += 1;\
CurPos.y -= 1;

#define VERTEX_UP_LEFT(OFFSET_X,OFFSET_Y) \
float nx = ((int)(((int)ColPos.x+(OFFSET_X))/32))*32 - FreePos.x+31 - VertexRadius;\
float ny = ((int)(((int)ColPos.y+(OFFSET_Y))/32))*32 - FreePos.y+31 - VertexRadius;\
float nnorm = sqrt(nx*nx + ny*ny);\
if(ny > 0)\
{\
	m_Direction.y = Velocity.y;\
	m_Direction.x = -Velocity.x;\
}\
else if(nx > 0)\
{\
	m_Direction.x = Velocity.x;\
	m_Direction.y = -Velocity.y;\
}\
else\
{\
	nx = nx / nnorm;\
	ny = ny / nnorm;\
	float vel = (Velocity.x * nx + Velocity.y *ny);\
	nx = vel*nx;\
	ny = vel*ny;\
	m_Direction.x = Velocity.x - 2*nx;\
	m_Direction.y = Velocity.y - 2*ny;\
}\
CurPos.x += 1;\
CurPos.y += 1;

void CBall::Reset() {
  GameWorld()->DestroyEntity(this);
}

vec2 CBall::GetPos(float Time) {
  return CalcPos(m_Pos, m_Direction,
    GameServer()->Tuning()->m_GrenadeCurvature,
    GameServer()->Tuning()->m_GrenadeSpeed,
    Time
  );
}

void CBall::Tick() {

  //Constants that used to be config options in the original
  const int CalcSteps = 100;
  //The default BounceLoss in the original code was 50, but the sample config uses 4
  //Also it was split up in x and y components
  const int BounceLoss = 4; 
  const int VertexRadius = 0; //config definition allowed up to 31

  //Variables used in the original collision magic
  float Steps = 1.0/CalcSteps;
  float FreeTime = -1.0f;
	vec2 FreePos = vec2(0,0);
	vec2 ColPos = vec2(0,0);
  
  float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
  float Nt = (Server()->Tick()-m_StartTick+1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);

  vec2 CollisionPos;
  vec2 BeforeCollisionPos;
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
  CCharacter *TargetChr;
  if (m_pickupAgain == 0) {
    TargetChr = GameWorld()->IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, NULL);
  } else {
    m_pickupAgain--;
    TargetChr = GameWorld()->IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, OwnerChar);
  }

	m_LifeSpan--;

	if(TargetChr) {
    //Character got the ball
    GameServer()->m_pController->OnBallPickup(TargetChr);
    GameWorld()->DestroyEntity(this);
  } if (m_LifeSpan < 0) {
    //Ball was flying around for too long. Destroy it
    GameServer()->m_pController->OnBallTimeout();
    GameWorld()->DestroyEntity(this);
  } if (GameLayerClipped(CurPos)) {
    //Ball is somewhere out of bounds. Destroy it
    GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "ballent", "Ball was out of bounds. Deleted it");
    GameWorld()->DestroyEntity(this);
  }

  //Count how many steps are left until the ball hits something?
  for (float i = Ct+Steps; i <= Nt; i += Steps) {
    vec2 TmpPos = GetPos(i);
    if (GameServer()->Collision()->TestBox(TmpPos, vec2(10, 10), GameServer()->Collision()->COLFLAG_SOLID)) {
      break;
    }
    FreeTime = i;
  }

  if (FreeTime == -1.0f) {
    //Ball hit something immediately
    m_pickupAgain = 0;
    bool shouldBeDestroyed = true;
    //Step backwards to the previous tick to find the collision
    for (float St = Ct; St > Ct - 1.0f; St -= Steps) {
      vec2 SearchPos = GetPos(St);
      if (!GameServer()->Collision()->TestBox(SearchPos, vec2(10, 10), GameServer()->Collision()->COLFLAG_SOLID)) {
        FreeTime = St;
        ColPos = GetPos(St+Steps);
        FreePos = GetPos(St);
        shouldBeDestroyed = false;
        break;
      }
    }
    if (shouldBeDestroyed) {
      //The ball was already colliding in the entire previous tick.
      //Something must be wrong so it will just be destroyed
      GameWorld()->DestroyEntity(this);
    }
  } else {
    FreeTime += Ct;
    ColPos = GetPos(FreeTime + Steps);
    FreePos = GetPos(FreeTime);
  }

  if (FreeTime < Nt - Steps) {
    bool coll[4] = {false};
    coll[0] = GameServer()->Collision()->CheckPoint(ColPos + vec2(5, 5));
    coll[1] = GameServer()->Collision()->CheckPoint(ColPos + vec2(5, -5));
    coll[2] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-5, 5));
    coll[3] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-5, -5));

    short CollidingPoints = 0;
    for (int i = 0; i < 4; i++) {
      if (coll[i]) { CollidingPoints++; }
    }

    vec2 Velocity = vec2(m_Direction.x / (BounceLoss+100)*100,
      (m_Direction.y
      + 2 * GameServer()->Tuning()->m_GrenadeCurvature / 10000 * GameServer()->Tuning()->m_GrenadeSpeed * (Server()->Tick()-m_StartTick) / (float)Server()->TickSpeed())
      / (BounceLoss+100)*100);

    switch(CollidingPoints) {
      case 1:
  			if(coll[0])	{
  				VERTEX_DOWN_RIGHT(5,5)
  			}
  			else if(coll[1]) {
  				VERTEX_UP_RIGHT(5,-5)
  			}
  			else if(coll[2]) {
  				VERTEX_DOWN_LEFT(-5,5)
  			}
  			else if(coll[3]) {
  				VERTEX_UP_LEFT(-5,-5)
  			}
      break;
      case 2: {
        bool collNeighbours[2] = { false };
        if (coll[0] && coll[1]) {
          collNeighbours[0] = GameServer()->Collision()->CheckPoint(ColPos + vec2(+5, -32));
          collNeighbours[1] = GameServer()->Collision()->CheckPoint(ColPos + vec2(+5, +32));
          if (!collNeighbours[0] == !collNeighbours[1]) {
            m_Direction = vec2(-Velocity.x, Velocity.y);
          } else if (collNeighbours[0]) {
            VERTEX_UP_RIGHT(5, 0)
          } else {
            VERTEX_DOWN_RIGHT(5, 0)
          }
        } else if (coll[1] && coll[3]) {
          collNeighbours[0] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-32, -5));
          collNeighbours[1] = GameServer()->Collision()->CheckPoint(ColPos + vec2(+32, -5));
          if (!collNeighbours[0] == !collNeighbours[1]) {
            m_Direction = vec2(Velocity.x, -Velocity.y);
          } else if (collNeighbours[0]) {
            VERTEX_UP_LEFT(0, -5)
          } else {
            VERTEX_UP_RIGHT(0, -5)
          }
        } else if (coll[3] && coll[2]) {
          collNeighbours[0] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-5, -32));
          collNeighbours[1] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-5, +32));
          if (!collNeighbours[0] == !collNeighbours[1]) {
            m_Direction = vec2(-Velocity.x, Velocity.y);
          } else if (collNeighbours[0]) {
            VERTEX_UP_LEFT(-5, 0)
          } else {
            VERTEX_DOWN_LEFT(-5, 0)
          }
        } else if (coll[2] && coll[0]) {
          collNeighbours[0] = GameServer()->Collision()->CheckPoint(ColPos + vec2(-32, +5));
          collNeighbours[1] = GameServer()->Collision()->CheckPoint(ColPos + vec2(+32, +5));
          if (!collNeighbours[0] == !collNeighbours[1]) {
            m_Direction = vec2(Velocity.x, -Velocity.y);
          } else if (collNeighbours[0]) {
            VERTEX_DOWN_LEFT(0, 5)
          } else {
            VERTEX_DOWN_RIGHT(0, 5)
          }
        }
      }
      break;
      case 3:
        m_Direction = -Velocity;
      break;
      default:
        GameServer()->SendBroadcast("This message should never appear", -1);
        GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "ballent", "CollidingPoints not in 1,2,3");
        if (GameServer()->Collision()->CheckPoint(ColPos.x, FreePos.y)) {
          Velocity.x = -Velocity.x;
        } else if (GameServer()->Collision()->CheckPoint(FreePos.x, ColPos.y)) {
          Velocity.y = -Velocity.y;
        } else {
          Velocity = -Velocity;
        }
        m_Direction = Velocity;
    }

    m_Pos = vec2(FreePos.x - m_Direction.x * (Ct - FreeTime),
                FreePos.y - (m_Direction.y * (Ct - FreeTime) * 2 * GameServer()->Tuning()->m_GrenadeCurvature / 10000 * GameServer()->Tuning()->m_GrenadeSpeed));

    //The start tick has to be reset after every bounce,
    //because otherwise GetPos() will return wrong coordinates
    m_StartTick = Server()->Tick();

    m_pickupAgain = 0;
  }

  //The goal check uses some bit shifting magic. Look at the enums in collision.h and mapitems.h and their binary representations
  if (GameServer()->Collision()->TestBox(m_Pos, vec2(10, 10), TILE_GOAL_BLUE & TILE_GOAL_RED)) {
    //GOAL! But which one?
    int Team = GameServer()->Collision()->TestBox(m_Pos, vec2(10, 10), 1 << 3) ? TEAM_RED : TEAM_BLUE;
    GameServer()->m_pController->OnGoal(GameServer()->m_apPlayers[m_Owner], Team);
    GameWorld()->DestroyEntity(this);
  }
}

void CBall::TickPaused() { ++m_StartTick; }

void CBall::FillInfo(CNetObj_Projectile *pProj) {
  pProj->m_X = (int)m_Pos.x;
  pProj->m_Y = (int)m_Pos.y;
  pProj->m_VelX = (int)(m_Direction.x*100.0f);
  pProj->m_VelY = (int)(m_Direction.y*100.0f);
  pProj->m_StartTick = m_StartTick;
  pProj->m_Type = WEAPON_GRENADE;
}

void CBall::Snap(int SnappingClient) {
  float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();

  if (NetworkClipped(SnappingClient, GetPos(Ct))) {
    return;
  }

  CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, GetID(), sizeof(CNetObj_Projectile)));
  if(pProj) {
    FillInfo(pProj);
  }
}