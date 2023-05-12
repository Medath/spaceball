#include <string>
#include "ball.h"
#include "engine/shared/config.h"
#include "game/server/entities/character.h"
#include "game/server/gamecontext.h"
#include "game/server/player.h"

CGameControllerBALL::CGameControllerBALL(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_pGameType = "BALL";

	m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode

	pGameServer->Tuning()->m_Gravity = 0.25f;
	pGameServer->Tuning()->m_GrenadeCurvature = 0;
	pGameServer->Tuning()->m_GrenadeLifetime = 40;
	pGameServer->Tuning()->m_GrenadeSpeed = 530;
	pGameServer->Tuning()->m_HookLength = 440;
}

bool CGameControllerBALL::DoWincheckMatch() {
	// check score win condition
	if((m_GameInfo.m_ScoreLimit > 0 && (m_aTeamscore[TEAM_RED] >= m_GameInfo.m_ScoreLimit || m_aTeamscore[TEAM_BLUE] >= m_GameInfo.m_ScoreLimit)) ||
		(m_GameInfo.m_TimeLimit > 0 && (Server()->Tick()-m_GameStartTick) >= m_GameInfo.m_TimeLimit*Server()->TickSpeed()*60))
	{
		if(abs(m_aTeamscore[TEAM_RED] - m_aTeamscore[TEAM_BLUE]) >= Config()->m_SvRequiredScoreDifference)
		{
			EndMatch();
			return true;
		}
		else
			m_SuddenDeath = 1;
	}
	return false;
}

void CGameControllerBALL::Tick()
{
	// this is the main part of the gamemode, this function is run every tick
	IGameController::Tick();
	if (!m_WasGameRunning != !IsGameRunning()) {
		//Gamestate just changed (started, or stopped, for example by switching to warmup),
		//reset everything
		m_PickupSpawned = false;
		m_PickedupTick = 0;
		//Respawn Ball
		m_PickupRespawnTick = Server()->Tick() + Server()->TickSpeed() * m_BallRespawnSeconds;
	}

	m_WasGameRunning = IsGameRunning();
}

void CGameControllerBALL::OnCharacterSpawn(CCharacter *pChr) {
	pChr->IncreaseHealth(10);
	pChr->IncreaseArmor(10);
	pChr->GiveWeapon(WEAPON_HAMMER, -1);
}

void CGameControllerBALL::OnGoal(CPlayer *Scorer, int Team) {
	std::string BroadcastMsg = "";

	m_aTeamscore[Team]++;

	int ScorerID;
	if (Scorer) {
		ScorerID = Scorer->GetCID();
		BroadcastMsg += Server()->ClientName(Scorer->GetCID());
		Scorer->m_Score += Team == Scorer->GetTeam() ? +1 : -1;
	} else {
		BroadcastMsg += "u̵̮̬͇̲̣͑̉͊̆͌̂̑̔͝ṉ̵̢̜̦͈̹̼̖̱́̊̄̕͠k̵̮̤̳͕̪̈́̓̉͐̎͒n̶̠̤͕̩̊̈̾̅̈́͂͛̔͢ͅo̫̻͍̗͂͆̓͐͘͜͞ͅw̢̪̤̯͚̜͕̱̲͑̒́̔̉͗̌̂ñ̛̘̦̪̱̤̦̅̒̂̈́͂̍́͝";
		ScorerID = MAX_PLAYERS - 1;
	}

	BroadcastMsg += " scored for the ";
	BroadcastMsg += Team == TEAM_BLUE ? "blue" : "red";
	BroadcastMsg += " team";
	
	if (m_BallPasser != -1) {
		//goal with pass
		CPlayer *Passer = GameServer()->m_apPlayers[m_BallPasser];
		BroadcastMsg += (std::string) " with a pass from ";
		if (Passer) {
			BroadcastMsg += Server()->ClientName(Passer->GetCID());
			Passer->m_Score++;
		} else {
			BroadcastMsg += "u̵̮̬͇̲̣͑̉͊̆͌̂̑̔͝ṉ̵̢̜̦͈̹̼̖̱́̊̄̕͠k̵̮̤̳͕̪̈́̓̉͐̎͒n̶̠̤͕̩̊̈̾̅̈́͂͛̔͢ͅo̫̻͍̗͂͆̓͐͘͜͞ͅw̢̪̤̯͚̜͕̱̲͑̒́̔̉͗̌̂ñ̛̘̦̪̱̤̦̅̒̂̈́͂̍́͝";
		}
		m_aTeamscore[Team]++;
	}

	BroadcastMsg += "!";
	GameServer()->SendBroadcast(BroadcastMsg.c_str(), -1);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "ball", BroadcastMsg.c_str());

	//Why was CreateSoundGlobal removed??
	GameServer()->SendGameMsg(
		GAMEMSG_CTF_CAPTURE,
		Team,
		ScorerID,
		Server()->TickSpeed() * 61,
		-1
	);

	//Kill all players
	for (int i = 0; i != MAX_CLIENTS; ++i) {
		CPlayer *p = GameServer()->m_apPlayers[i];
		if (p) {
			p->KillCharacter();
		}
	}

	//reset for new round
	m_BallPasser = -1;
	m_PickupSpawned = false;
	m_PickedupTick = 0;
	//Respawn Ball
	m_PickupRespawnTick = Server()->Tick() + Server()->TickSpeed() * m_BallRespawnSeconds;
}

void CGameControllerBALL::OnBallSpawn() {
	m_PickupSpawned = true;
	GameServer()->SendChat(-1, CHAT_ALL, -1, "Ball respawned");
}

void CGameControllerBALL::OnBallPickup(CPlayer *PreviousOwner, CCharacter *Char) {
  Char->GiveWeapon(WEAPON_GRENADE, 1);
  Char->SetWeapon(WEAPON_GRENADE);
	m_PickedupTick = Server()->Tick();

	//reset the ball passer if previous owner is null,
	//or if it was a self pass,
	//or if the pass came from another team
	if (!PreviousOwner
			|| Char->GetPlayer()->GetTeam() != PreviousOwner->GetTeam()
			|| PreviousOwner->GetCID() == Char->GetPlayer()->GetCID()) {
		m_BallPasser = -1;
	} else {
		m_BallPasser = PreviousOwner->GetCID();
	}
}

int CGameControllerBALL::GetBallPickupTick() {
	return m_PickedupTick;
}

void CGameControllerBALL::OnBallTimeout() {
	m_PickupSpawned = false;
	m_PickedupTick = 0;
	m_PickupRespawnTick = Server()->Tick();
	m_BallPasser = -1;
}

int CGameControllerBALL::GetPickupSpawnTick(int originalTick) {
	if (m_PickupSpawned && !m_PickedupTick) {
		return -1; //This should prevent the CPickup from respawning
	} else if (m_PickupSpawned && m_PickedupTick) {
		return Server()->Tick() + 1;
	} else if (m_PickupRespawnTick == -1) {
		return originalTick;
	}
	return m_PickupRespawnTick;
}

//IGameController::OnCharacterDeath messes with the player scores and respawn time when they die.
//This stub is here to avoid that
int CGameControllerBALL::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon) {
	return 0;
}
