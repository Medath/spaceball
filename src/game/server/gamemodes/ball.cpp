#include "ball.h"
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

void CGameControllerBALL::Tick()
{
	// this is the main part of the gamemode, this function is run every tick
	IGameController::Tick();
}

void CGameControllerBALL::OnCharacterSpawn(CCharacter *pChr) {
	IGameController::OnCharacterSpawn(pChr);

	pChr->IncreaseArmor(10);
	pChr->SetWeapon(WEAPON_HAMMER);
}

void CGameControllerBALL::OnGoal(CPlayer *Scorer, int Team) {
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "goal");

	m_PickupSpawned = false;
	m_PickedupTick = 0;
	//Respawn in 5 seconds
	m_PickupRespawnTick = Server()->Tick() + Server()->TickSpeed() * 5;
	m_aTeamscore[Team]++;

	int ScorerID;
	if (Scorer) {
		//own goal?
		Scorer->m_Score += Team == Scorer->GetTeam() ? 1 : -1;
		ScorerID = Scorer->GetCID();
	} else {
		ScorerID = MAX_PLAYERS - 1;
	}

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
}

void CGameControllerBALL::OnBallSpawn() {
	m_PickupSpawned = true;
	GameServer()->SendChat(-1, CHAT_ALL, -1, "Ball respawned");
}

void CGameControllerBALL::OnBallPickup(CCharacter *Char) {
  Char->GiveWeapon(WEAPON_GRENADE, 1);
  Char->SetWeapon(WEAPON_GRENADE);
	m_PickedupTick = Server()->Tick();
}

int CGameControllerBALL::GetBallPickupTick() {
	return m_PickedupTick;
}

void CGameControllerBALL::OnBallTimeout() {
	m_PickupRespawnTick = Server()->Tick();
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
