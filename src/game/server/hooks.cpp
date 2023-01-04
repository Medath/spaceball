/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <base/math.hpp>

#include <engine/e_config.h>
#include <engine/e_server_interface.h>
extern "C"
{
	#include <engine/e_memheap.h>
}
#include <game/version.hpp>
#include <game/collision.hpp>
#include <game/layers.hpp>

#include <game/gamecore.hpp>

#include "gamecontext.hpp"
#include "gamemodes/dm.hpp"
#include "gamemodes/tdm.hpp"
#include "gamemodes/ctf.hpp"
#include "gamemodes/ball.hpp"
#include "gamemodes/dmmod.hpp"
#include "gamemodes/tdmmod.hpp"
#include "gamemodes/ctfmod.hpp"
#include "gamemodes/ko.hpp"

TUNING_PARAMS tuning;

static void check_pure_tuning()
{
	// might not be created yet during start up
	if(!game.controller)
		return;
	
	if(	strcmp(game.controller->gametype, "DM")==0 ||
		strcmp(game.controller->gametype, "TDM")==0 ||
		strcmp(game.controller->gametype, "CTF")==0)
	{
		TUNING_PARAMS p;
		if(memcmp(&p, &tuning, sizeof(TUNING_PARAMS)) != 0)
		{
			dbg_msg("server", "resetting tuning due to pure server");
			tuning = p;
		}
	}	
}

struct VOTEOPTION
{
	VOTEOPTION *next;
	VOTEOPTION *prev;
	char command[1];
};

static HEAP *voteoption_heap = 0;
static VOTEOPTION *voteoption_first = 0;
static VOTEOPTION *voteoption_last = 0;

void send_tuning_params(int cid)
{
	check_pure_tuning();
	
	msg_pack_start(NETMSGTYPE_SV_TUNEPARAMS, MSGFLAG_VITAL);
	int *params = (int *)&tuning;
	for(unsigned i = 0; i < sizeof(tuning)/sizeof(int); i++)
		msg_pack_int(params[i]);
	msg_pack_end();
	server_send_msg(cid);
}

// Server hooks
void mods_client_direct_input(int client_id, void *input)
{
	if(!game.world.paused)
		game.players[client_id]->on_direct_input((NETOBJ_PLAYER_INPUT *)input);
}

void mods_client_predicted_input(int client_id, void *input)
{
	if(!game.world.paused)
		game.players[client_id]->on_predicted_input((NETOBJ_PLAYER_INPUT *)input);
}

// Server hooks
void mods_tick()
{
	check_pure_tuning();
	
	game.tick();

#ifdef CONF_DEBUG
	if(config.dbg_dummies)
	{
		for(int i = 0; i < config.dbg_dummies ; i++)
		{
			NETOBJ_PLAYER_INPUT input = {0};
			input.direction = (i&1)?-1:1;
			game.players[MAX_CLIENTS-i-1]->on_predicted_input(&input);
		}
	}
#endif
	for(int i = 0; i < MAX_CLIENTS && server_tick()%(config.sv_msg_frame_time * server_tickspeed()) == 0; i++)
		if(game.players[i] && --game.players[i]->messages < 0)
			game.players[i]->messages = 0;
	if(strlen(config.sv_min_msg) > 0 && config.sv_msg_intervall && server_tick()%(config.sv_msg_intervall * 60 * server_tickspeed()) == 0)
		game.send_chat(-1,-2,config.sv_min_msg);
	if(server_tick()%100 || !(config.sv_kick_idle || config.sv_max_idle))
		return;
	int players_online = 0;
	int players_ingame = 0;
	int idler = -1;
	for(int i = 0; i < config.sv_max_clients; i++)
	{
		if(game.players[i] && game.players[i]->client_id >= 0)
		{
			players_online++;
			if(game.players[i]->team >= 0)
				players_ingame++;
			if(config.sv_max_idle && server_tick() - game.players[i]->last_input >= config.sv_max_idle * server_tickspeed())
			{
				if(idler == -1 && ((config.sv_ko_mod && game.players[i]->queue == -1) || !config.sv_ko_mod))
					idler = i;
				if(game.players[i]->team != -1)
				{
					game.players[i]->set_team(-1);
					(void) game.controller->check_team_balance();
					if(config.sv_ko_mod)
					{
						game.players[i]->queue = -2;
						game.controller->endround();
					}
				}
			}
			if(game.players[i]->joined && game.players[i]->joined + server_tickspeed()*5 <= server_tick())
			{
				game.send_chat_target(i, "Please use .info to get help and information");
				game.send_broadcast("Please use .info to get help and information", i);
				game.players[i]->joined = 0;
			}
		}
	}
	if(config.sv_kick_idle && players_ingame < config.sv_max_clients - config.sv_spectator_slots && players_online >= config.sv_max_clients - config.sv_reserved_slots && idler != -1)
	{
		game.players[idler]->last_input = server_tick();
		server_kick(idler, "You were away and the server was full... so you were automatically kicked");
	}
}

void mods_snap(int client_id)
{
	game.snap(client_id);
}

void mods_client_enter(int client_id)
{
	//game.world.insert_entity(&game.players[client_id]);
	game.players[client_id]->respawn();
	dbg_msg("game", "join player='%d:%s'", client_id, server_clientname(client_id));


	char buf[512];
	str_format(buf, sizeof(buf), "%s entered and joined the %s", server_clientname(client_id), game.controller->get_team_name(game.players[client_id]->team));
	game.send_chat(-1, GAMECONTEXT::CHAT_ALL, buf); 

	dbg_msg("game", "team_join player='%d:%s' team=%d", client_id, server_clientname(client_id), game.players[client_id]->team);
	game.players[client_id]->messages = 0;
	game.players[client_id]->muted = 0;
	game.players[client_id]->last_input = server_tick();
	game.players[client_id]->queue = -1;
	game.players[client_id]->voted = false;
	game.players[client_id]->joined = server_tick();
}

void mods_connected(int client_id)
{
	game.players[client_id] = new(client_id) PLAYER(client_id);
	//game.players[client_id].init(client_id);
	//game.players[client_id].client_id = client_id;
	
	// Check which team the player should be on
	if(config.sv_tournament_mode)
		game.players[client_id]->team = -1;
	else
		game.players[client_id]->team = game.controller->get_auto_team(client_id);
		
	(void) game.controller->check_team_balance();

	// send motd
	NETMSG_SV_MOTD msg;
	msg.message = config.sv_motd;
	msg.pack(MSGFLAG_VITAL);
	server_send_msg(client_id);
}

void mods_client_drop(int client_id)
{
	if(config.sv_goalkeeper && game.players[client_id]->team >= 0)
	{
		if(game.players[client_id]->goalkeeper)
		{
			game.players[client_id]->goalkeeper = 0;
			game.controller->goalkeeper[game.players[client_id]->team]--;
		}
	}
	game.players[client_id]->queue = -2;
	bool endround = (game.players[client_id]->team >= 0 && config.sv_ko_mod) ? true:false;
	game.abort_vote_kick_on_disconnect(client_id);
	game.players[client_id]->on_disconnect();
	delete game.players[client_id];
	game.players[client_id] = 0;
	
	(void) game.controller->check_team_balance();
	if(endround)
		game.controller->endround();
}

/*static bool is_separator(char c) { return c == ';' || c == ' ' || c == ',' || c == '\t'; }

static const char *liststr_find(const char *str, const char *needle)
{
	int needle_len = strlen(needle);
	while(*str)
	{
		int wordlen = 0;
		while(str[wordlen] && !is_separator(str[wordlen]))
			wordlen++;
		
		if(wordlen == needle_len && strncmp(str, needle, needle_len) == 0)
			return str;
		
		str += wordlen+1;
	}
	
	return 0;
}*/

int add_spam_msg(int client_id)
{
	if(game.players[client_id]->muted > server_tick())
	{
		game.send_broadcast("You are still muted!", client_id);
		return -1;
	}
	game.players[client_id]->muted = 0;
	if(++game.players[client_id]->messages > config.sv_max_msgs)
	{
		char buf[512];
		strcpy(buf, server_clientname(client_id));
		strcat(buf, " is muted now because of spamming.");
		game.send_chat(-1,-2,buf);
		game.send_broadcast("You are muted because of spamming", client_id);
		dbg_msg("spam","muting now");
		game.players[client_id]->muted = server_tick() + server_tickspeed()*config.sv_msg_mute_time;
	}
	return 0;
}

void mods_message(int msgtype, int client_id)
{
	void *rawmsg = netmsg_secure_unpack(msgtype);
	PLAYER *p = game.players[client_id];
	
	if(!rawmsg)
	{
		dbg_msg("server", "dropped weird message '%s' (%d), failed on '%s'", netmsg_get_name(msgtype), msgtype, netmsg_failed_on());
		return;
	}
	
	if(msgtype == NETMSGTYPE_CL_SAY)
	{
		if(add_spam_msg(client_id) == -1)
			return;
		NETMSG_CL_SAY *msg = (NETMSG_CL_SAY *)rawmsg;
		int team = msg->team;
		if(team)
			team = p->team;
		else
			team = GAMECONTEXT::CHAT_ALL;
		
		if(config.sv_spamprotection && p->last_chat+time_freq() > time_get())
			return;
		
		p->last_chat = time_get();
		p->last_input = server_tick();
		if(config.sv_cwscore && strcmp("/cwscore",msg->message) == 0)
		{
			char buf[512];
			str_format(buf,sizeof(buf), "Clanwar score: Red: %i Blue: %i", game.controller->cwscore[0], game.controller->cwscore[1]);
			game.send_chat_target(p->client_id, buf);
			return;
		}
		if(config.sv_handle_mapvotes && (strcmp("/++",msg->message) == 0 || strcmp("/--", msg->message) == 0))
		{
			if(game.players[client_id]->voted)
			{
				game.send_broadcast("You already voted, ignoring vote", client_id);
			}
			else
			{
				game.players[client_id]->voted = true;
				dbg_msg("game", "map-voting %s", msg->message);
				game.send_broadcast("You voted for the map, thank you", client_id);
			}
			return;
		}
		if(config.sv_goalkeeper && strcmp(msg->message, "/goalkeeper") == 0 && p->team >= 0)
		{
			if(!p->goalkeeper && game.controller->goalkeeper[p->team] < config.sv_goalkeeper)
			{
				char buf[300];
				strcpy(buf,server_clientname(p->client_id));
				p->goalkeeper = 1;
				game.controller->goalkeeper[p->team]++;
				strcat(buf, " is goalkeeper now");
				game.send_chat(-1,-1,buf);
				game.send_chat(-1,p->team,buf);
				p->kill_character(-1);
				return;
			}
			else if(p->goalkeeper)
			{
				game.controller->goalkeeper[p->team]--;
				char buf[300];
				strcpy(buf,server_clientname(p->client_id));
				p->goalkeeper = 0;
				strcat(buf, " is not a goalkeeper anymore");
				game.send_chat(-1,-1,buf);
				game.send_chat(-1,p->team,buf);
				p->kill_character(-1);
				return;
			}
			else
			{
				game.send_broadcast("No more goalkeepers", p->client_id);
				return;
			}
		}
		if(config.sv_ball_mod && strcmp(msg->message, "/ball") == 0)
		{
			p->ballposition = !p->ballposition;
			return;
		}
		if(strcmp(msg->message, ".info") == 0)
		{
			game.send_chat_target(p->client_id, "MOD from scosu with support from Rajh. Commands:");
			game.send_chat_target(p->client_id, ".modinfo (Help for the MOD)");
			if(config.sv_ball_mod)
				game.send_chat_target(p->client_id, "/ball (Enables/Disables the displaying of the ball position)");
			if(config.sv_goalkeeper)
				game.send_chat_target(p->client_id, "/goalkeeper (You become goalkeeper for your team)");
			if(config.sv_handle_mapvotes)
			{
				game.send_chat_target(p->client_id, "/++ (You vote positive for the map)");
				game.send_chat_target(p->client_id, "/-- (You vote negative for the map)");
			}
			if(config.sv_cwscore)
				game.send_chat_target(p->client_id, "/cwscore (Shows you the current scores of both teams)");
			return;
		}
		if(strcmp(msg->message, ".modinfo") == 0)
		{
			if(config.sv_ball_mod)
			{
				game.send_chat_target(p->client_id, "In this MOD you have to take the ball (grenade) and shoot it into the goal.");
				game.send_chat_target(p->client_id, "Points:");
				game.send_chat_target(p->client_id, "Goal with a pass: Goaler: 2 Passer: 1 Team: 3");
				game.send_chat_target(p->client_id, "Goal without pass: Goaler: 2 Team: 2");
				game.send_chat_target(p->client_id, "MOD from scosu with support from Rajh. You can download it at http://modpack.scosu.de");
			}
			else if(config.sv_ko_mod)
			{
				game.send_chat_target(p->client_id, "In this MOD every player playes at least one time. In the end there is a winner through something like a knockout-tournament. To join the game, you have to join one of the teams. If you are the next one in the queue, you automatically join one of the teams and the round begins.");
				game.send_chat_target(p->client_id, "MOD from scosu with support from Rajh. You can download it at http://modpack.scosu.de");
			}
			else if(game.controller->mod)
			{
				game.send_chat_target(p->client_id, "It's a MOD. But no specified one. so just try out or check server info for help.");
				game.send_chat_target(p->client_id, "MOD from scosu with support from Rajh. You can download it at http://modpack.scosu.de");
			}
			else
			{
				game.send_chat_target(p->client_id, "This server is running a normal game. It doesn't use a modified gameplay only some things for a better server behaviour.");
				game.send_chat_target(p->client_id, "MOD from scosu with support from Rajh. You can download it at http://modpack.scosu.de");
			}
			return;
		}
		
		game.send_chat(client_id, team, msg->message);
	}
	else if(msgtype == NETMSGTYPE_CL_CALLVOTE)
	{
		int64 now = time_get();
		if(game.vote_closetime)
		{
			game.send_chat_target(client_id, "Wait for current vote to end before calling a new one.");
			return;
		}
		
		int64 timeleft = p->last_votecall + time_freq()*60 - now;
		if(timeleft > 0)
		{
			char chatmsg[512] = {0};
			str_format(chatmsg, sizeof(chatmsg), "You must wait %d seconds before making another vote", (timeleft/time_freq())+1);
			game.send_chat_target(client_id, chatmsg);
			return;
		}
		
		char chatmsg[512] = {0};
		char desc[512] = {0};
		char cmd[512] = {0};
		NETMSG_CL_CALLVOTE *msg = (NETMSG_CL_CALLVOTE *)rawmsg;
		if(str_comp_nocase(msg->type, "option") == 0)
		{
			VOTEOPTION *option = voteoption_first;
			while(option)
			{
				if(str_comp_nocase(msg->value, option->command) == 0)
				{
					str_format(chatmsg, sizeof(chatmsg), "%s called vote to change server option '%s'", server_clientname(client_id), option->command);
					str_format(desc, sizeof(desc), "%s", option->command);
					str_format(cmd, sizeof(cmd), "%s", option->command);
					break;
				}

				option = option->next;
			}
			
			if(!option)
			{
				str_format(chatmsg, sizeof(chatmsg), "'%s' isn't an option on this server", msg->value);
				game.send_chat_target(client_id, chatmsg);
				return;
			}
		}
		else if(str_comp_nocase(msg->type, "kick") == 0)
		{
			if(!config.sv_vote_kick)
			{
				game.send_chat_target(client_id, "Server does not allow voting to kick players");
				return;
			}
			
			int kick_id = atoi(msg->value);
			if(kick_id < 0 || kick_id >= MAX_CLIENTS || !game.players[kick_id])
			{
				game.send_chat_target(client_id, "Invalid client id to kick");
				return;
			}
			
			str_format(chatmsg, sizeof(chatmsg), "%s called a vote to kick '%s'", server_clientname(client_id), server_clientname(kick_id));
			str_format(desc, sizeof(desc), "Kick '%s'", server_clientname(kick_id));
			str_format(cmd, sizeof(cmd), "kick %d", kick_id);
			if (!config.sv_vote_kick_bantime)
				str_format(cmd, sizeof(cmd), "kick %d", kick_id);
			else
				str_format(cmd, sizeof(cmd), "ban %d %d", kick_id, config.sv_vote_kick_bantime);
		}
		
		if(cmd[0])
		{
			game.send_chat(-1, GAMECONTEXT::CHAT_ALL, chatmsg);
			game.start_vote(desc, cmd);
			p->vote = 1;
			game.vote_creator = client_id;
			p->last_votecall = now;
			game.send_vote_status(-1);
		}
	}
	else if(msgtype == NETMSGTYPE_CL_VOTE)
	{
		if(!game.vote_closetime)
			return;

		if(p->vote == 0)
		{
			NETMSG_CL_VOTE *msg = (NETMSG_CL_VOTE *)rawmsg;
			p->vote = msg->vote;
			game.send_vote_status(-1);
		}
	}
	else if (msgtype == NETMSGTYPE_CL_SETTEAM && !game.world.paused)
	{
		if(add_spam_msg(client_id) == -1)
			return;
		NETMSG_CL_SETTEAM *msg = (NETMSG_CL_SETTEAM *)rawmsg;
		
		if(p->team == msg->team || (config.sv_spamprotection && p->last_setteam+time_freq()*3 > time_get()))
			return;
		p->last_input = server_tick();
		if(config.sv_ko_mod)
		{
			if(game.players[client_id]->queue >= 0)
			{
				game.send_broadcast("You are already in waiting-queue... just wait", client_id);
				return;
			}
			if(game.players[client_id]->team >= 0 && msg->team != -1)
			{
				game.send_broadcast("You are playing, you can't change team now", client_id);
				return;
			}
			else if(game.players[client_id]->team >= 0)
			{
				p->set_team(-1);
				p->queue = -2;
				game.controller->endround();
				return;
			}
			if(game.players[client_id]->queue == -2)
			{
				game.send_broadcast("You played already. Wait for the next round.", client_id);
				return;
			}
			int player[2];
			player[0] = -1;
			player[1] = -1;
			int max_queue = -1;
			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if(game.players[i])
				{
					if(game.players[i]->team >= 0)
						player[game.players[i]->team] = i;
					if(game.players[i]->queue > max_queue)
						max_queue = game.players[i]->queue;
				}
			}
			if(max_queue != -1)
			{
				game.players[client_id]->queue = max_queue + 1;
			}
			else
			{
				game.players[client_id]->queue = -2;
				if((player[0] != -1 && player[1] == -1) || (player[1] != -1 && player[0] == -1))
				{
					if(player[0] == -1)
						game.players[client_id]->set_team(0);
					else
						game.players[client_id]->set_team(1);
					game.controller->startround();
					game.send_broadcast("The game starts now", -1);
					return;
				}
				else if(player[0] != -1 && player[1] != -1)
				{
					game.players[client_id]->queue = 0;
					game.send_broadcast("You are in the waiting-queue now", client_id);
					return;
				}
			}
		}
		// Switch team on given client and kill/respawn him
		if(game.controller->can_join_team(msg->team, client_id))
		{
			if(game.controller->can_change_team(p, msg->team))
			{
				p->last_setteam = time_get();
				p->set_team(msg->team);
				(void) game.controller->check_team_balance();
			}
			else
				game.send_broadcast("Teams must be balanced, please join other team", client_id);
		}
		else
		{
			char buf[128];
			str_format(buf, sizeof(buf), "Only %d active players are allowed", config.sv_max_clients-config.sv_spectator_slots);
			game.send_broadcast(buf, client_id);
		}
	}
	else if (msgtype == NETMSGTYPE_CL_CHANGEINFO || msgtype == NETMSGTYPE_CL_STARTINFO)
	{
		if(add_spam_msg(client_id) == -1)
			return;
		NETMSG_CL_CHANGEINFO *msg = (NETMSG_CL_CHANGEINFO *)rawmsg;
		
		if(config.sv_spamprotection && p->last_changeinfo+time_freq()*5 > time_get())
			return;
			
		p->last_changeinfo = time_get();
		
		p->use_custom_color = msg->use_custom_color;
		p->color_body = msg->color_body;
		p->color_feet = msg->color_feet;

		// check for invalid chars
		unsigned char *name = (unsigned char *)msg->name;
		while (*name)
		{
			if(*name < 32)
				*name = ' ';
			name++;
		}

		// copy old name
		char oldname[MAX_NAME_LENGTH];
		str_copy(oldname, server_clientname(client_id), MAX_NAME_LENGTH);
		
		server_setclientname(client_id, msg->name);
		if(msgtype == NETMSGTYPE_CL_CHANGEINFO && strcmp(oldname, server_clientname(client_id)) != 0)
		{
			char chattext[256];
			str_format(chattext, sizeof(chattext), "%s changed name to %s", oldname, server_clientname(client_id));
			game.send_chat(-1, GAMECONTEXT::CHAT_ALL, chattext);
		}
		
		// set skin
		str_copy(p->skin_name, msg->skin, sizeof(p->skin_name));
		
		game.controller->on_player_info_change(p);
		
		if(msgtype == NETMSGTYPE_CL_STARTINFO)
		{
			// send vote options
			NETMSG_SV_VOTE_CLEAROPTIONS clearmsg;
			clearmsg.pack(MSGFLAG_VITAL);
			server_send_msg(client_id);
			VOTEOPTION *current = voteoption_first;
			while(current)
			{
				NETMSG_SV_VOTE_OPTION optionmsg;
				optionmsg.command = current->command;
				optionmsg.pack(MSGFLAG_VITAL);
				server_send_msg(client_id);
				current = current->next;
			}
			
			// send tuning parameters to client
			send_tuning_params(client_id);

			//
			NETMSG_SV_READYTOENTER m;
			m.pack(MSGFLAG_VITAL|MSGFLAG_FLUSH);
			server_send_msg(client_id);
		}
	}
	else if (msgtype == NETMSGTYPE_CL_EMOTICON && !game.world.paused)
	{
		if(add_spam_msg(client_id) == -1)
			return;
		NETMSG_CL_EMOTICON *msg = (NETMSG_CL_EMOTICON *)rawmsg;
		
		if(config.sv_spamprotection && p->last_emote+time_freq()*3 > time_get())
			return;
			
		p->last_emote = time_get();
		
		game.send_emoticon(client_id, msg->emoticon);
	}
	else if (msgtype == NETMSGTYPE_CL_KILL && !game.world.paused)
	{
		p->last_input = server_tick();
		if(p->last_kill+time_freq()*3 > time_get() || p->get_character() == NULL)
			return;
		
		p->last_kill = time_get();
		p->respawn_tick = server_tick()+server_tickspeed()*3;
		p->kill_character(WEAPON_SELF);
	}
	for(int i = 0; i < MAX_CLIENTS && server_tick()%(config.sv_msg_frame_time * server_tickspeed()) == 0; i++)
		if(game.players[i] && --game.players[i]->messages < 0)
			game.players[i]->messages = 0;
	if(strlen(config.sv_min_msg) > 0 && config.sv_msg_intervall && server_tick()%(config.sv_msg_intervall * 60 * server_tickspeed()) == 0)
		game.send_chat(-1,-2,config.sv_min_msg);
	if(server_tick()%100 || !(config.sv_kick_idle || config.sv_max_idle))
		return;
	int players_online = 0;
	int players_ingame = 0;
	int idler = -1;
	for(int i = 0; i < config.sv_max_clients; i++)
	{
		if(game.players[i] && game.players[i]->client_id >= 0)
		{
			players_online++;
			if(game.players[i]->team >= 0)
				players_ingame++;
			if(config.sv_max_idle && server_tick() - game.players[i]->last_input >= config.sv_max_idle * server_tickspeed())
			{
				if(idler == -1 && ((config.sv_ko_mod && game.players[i]->queue == -1) || !config.sv_ko_mod))
					idler = i;
				if(game.players[i]->team != -1)
				{
					game.players[i]->set_team(-1);
					(void) game.controller->check_team_balance();
					if(config.sv_ko_mod)
					{
						game.players[i]->queue = -2;
						game.controller->endround();
					}
				}
			}
			if(game.players[i]->joined && game.players[i]->joined + server_tickspeed()*5 <= server_tick())
			{
				game.send_chat_target(i, "Please use .info to get help and information");
				game.send_broadcast("Please use .info to get help and information", i);
				game.players[i]->joined = 0;
			}
		}
	}
	if(config.sv_kick_idle && players_ingame < config.sv_max_clients - config.sv_spectator_slots && players_online >= config.sv_max_clients - config.sv_reserved_slots && idler != -1)
	{
		game.players[idler]->last_input = server_tick();
		server_kick(idler, "You were away and the server was full... so you were automatically kicked");
	}
}

static void con_tune_param(void *result, void *user_data)
{
	const char *param_name = console_arg_string(result, 0);
	float new_value = console_arg_float(result, 1);

	if(tuning.set(param_name, new_value))
	{
		dbg_msg("tuning", "%s changed to %.2f", param_name, new_value);
		send_tuning_params(-1);
	}
	else
		console_print("No such tuning parameter");
}

static void con_tune_reset(void *result, void *user_data)
{
	TUNING_PARAMS p;
	tuning = p;
	send_tuning_params(-1);
	console_print("tuning reset");
}

static void con_tune_dump(void *result, void *user_data)
{
	for(int i = 0; i < tuning.num(); i++)
	{
		float v;
		tuning.get(i, &v);
		dbg_msg("tuning", "%s %.2f", tuning.names[i], v);
	}
}


static void con_change_map(void *result, void *user_data)
{
	game.controller->change_map(console_arg_string(result, 0));
}

static void con_restart(void *result, void *user_data)
{
	if(console_arg_num(result))
		game.controller->do_warmup(console_arg_int(result, 0));
	else
		game.controller->startround();
}

static void con_broadcast(void *result, void *user_data)
{
	game.send_broadcast(console_arg_string(result, 0), -1);
}

static void con_say(void *result, void *user_data)
{
	game.send_chat(-1, GAMECONTEXT::CHAT_ALL, console_arg_string(result, 0));
}

static void con_set_team(void *result, void *user_data)
{
	int client_id = clamp(console_arg_int(result, 0), 0, (int)MAX_CLIENTS - 1);
	int team = clamp(console_arg_int(result, 1), -1, 1);
	
	dbg_msg("", "%d %d", client_id, team);
	
	if(!game.players[client_id])
		return;
	
	game.players[client_id]->set_team(team);
	(void) game.controller->check_team_balance();
}

static void con_addvote(void *result, void *user_data)
{
	int len = strlen(console_arg_string(result, 0));
	
	if(!voteoption_heap)
		voteoption_heap = memheap_create();
	
	VOTEOPTION *option = (VOTEOPTION *)memheap_allocate(voteoption_heap, sizeof(VOTEOPTION) + len);
	option->next = 0;
	option->prev = voteoption_last;
	if(option->prev)
		option->prev->next = option;
	voteoption_last = option;
	if(!voteoption_first)
		voteoption_first = option;
	
	mem_copy(option->command, console_arg_string(result, 0), len+1);
	dbg_msg("server", "added option '%s'", option->command);
}

static void con_vote(void *result, void *user_data)
{
	if(str_comp_nocase(console_arg_string(result, 0), "yes") == 0)
		game.vote_enforce = GAMECONTEXT::VOTE_ENFORCE_YES;
	else if(str_comp_nocase(console_arg_string(result, 0), "no") == 0)
		game.vote_enforce = GAMECONTEXT::VOTE_ENFORCE_NO;
	dbg_msg("server", "forcing vote %s", console_arg_string(result, 0));
}

static void con_respawn_ball(void *result, void *user_data)
{
	game.world.reset_requested = true;
}

static void con_mute(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) >= MAX_CLIENTS)
		return;
	char buf[512];
	strcpy(buf, server_clientname(console_arg_int(result, 0)));
	strcat(buf, " is muted now.");
	game.send_chat(-1,-2,buf);
	game.send_broadcast("You are muted", console_arg_int(result, 0));
	dbg_msg("spam","muting now");
	game.players[console_arg_int(result, 0)]->muted = server_tick() + server_tickspeed()*console_arg_int(result, 1);
}

static void con_unmute(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) >= MAX_CLIENTS)
		return;
	game.send_broadcast("You can speak again", console_arg_int(result, 0));
	dbg_msg("spam","unmuting now");
	game.players[console_arg_int(result, 0)]->muted = 0;
	game.players[console_arg_int(result, 0)]->messages = 0;
}

static void con_gen_pw(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) >= MAX_CLIENTS)
		return;
	char pw[9];
	server_generate_pw(console_arg_int(result, 0), pw);
	game.send_chat_target(console_arg_int(result, 0), pw);
}

static void con_set_score(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) >= MAX_CLIENTS || !game.players[console_arg_int(result, 0)])
		return;
	game.players[console_arg_int(result, 0)]->score = console_arg_int(result, 1);
	char buf[512];
	sprintf(buf, "The admin set your score to %d", console_arg_int(result, 1));
	game.send_broadcast(buf, console_arg_int(result, 0));
}

static void con_set_teamscore(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) > 1)
		return;
	game.controller->teamscore[console_arg_int(result, 0)] = console_arg_int(result, 1);
	char buf[512];
	sprintf(buf, "The admin set score of %s team to %d", (console_arg_int(result, 0)?"blue":"red"), console_arg_int(result, 1));
	game.send_broadcast(buf, console_arg_int(result, 0));
}

static void con_set_cwscore(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) > 1)
		return;
	game.controller->cwscore[console_arg_int(result, 0)] = console_arg_int(result, 1);
	char buf[512];
	sprintf(buf, "The admin set cwscore of %s team to %d", (console_arg_int(result, 0)?"blue":"red"), console_arg_int(result, 1));
	game.send_broadcast(buf, console_arg_int(result, 0));
}

static void con_reset_cwscore(void *result, void *user_data)
{
	game.controller->cwscore[0] = 0;
	game.controller->cwscore[1] = 0;
	char buf[512];
	sprintf(buf, "reset of cwscore");
	game.send_broadcast(buf, console_arg_int(result, 0));
}

static void con_kill(void *result, void *user_data)
{
	if(console_arg_int(result, 0) < 0 || console_arg_int(result, 0) >= MAX_CLIENTS || !game.players[console_arg_int(result, 0)])
		return;
	game.players[console_arg_int(result, 0)]->last_kill = time_get();
	game.players[console_arg_int(result, 0)]->kill_character(-1); //(client_id, -1);
	game.players[console_arg_int(result, 0)]->respawn_tick = server_tick()+server_tickspeed()*console_arg_int(result, 1);
	char buf[512];
	sprintf(buf, "The admin killed you for %d seconds", console_arg_int(result, 1));
	game.send_broadcast(buf, console_arg_int(result, 0));
}

static void con_set_all_spec(void *result, void *user_data)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!game.players[i] || game.players[i]->team < 0)
			continue;
		game.players[i]->set_team(-1);
	}
}

static void con_set_all_kick(void *result, void *user_data)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!game.players[i])
			continue;
		server_kick(i, console_arg_string(result, 0));
	}
}

void mods_console_init()
{
	MACRO_REGISTER_COMMAND("tune", "si", CFGFLAG_SERVER, con_tune_param, 0, "");
	MACRO_REGISTER_COMMAND("tune_reset", "", CFGFLAG_SERVER, con_tune_reset, 0, "");
	MACRO_REGISTER_COMMAND("tune_dump", "", CFGFLAG_SERVER, con_tune_dump, 0, "");

	MACRO_REGISTER_COMMAND("change_map", "r", CFGFLAG_SERVER, con_change_map, 0, "");
	MACRO_REGISTER_COMMAND("restart", "?i", CFGFLAG_SERVER, con_restart, 0, "");
	MACRO_REGISTER_COMMAND("broadcast", "r", CFGFLAG_SERVER, con_broadcast, 0, "");
	MACRO_REGISTER_COMMAND("say", "r", CFGFLAG_SERVER, con_say, 0, "");
	MACRO_REGISTER_COMMAND("set_team", "ii", CFGFLAG_SERVER, con_set_team, 0, "");

	MACRO_REGISTER_COMMAND("addvote", "r", CFGFLAG_SERVER, con_addvote, 0, "");
	MACRO_REGISTER_COMMAND("vote", "r", CFGFLAG_SERVER, con_vote, 0, "");
	
	
	
	MACRO_REGISTER_COMMAND("reset", "", CFGFLAG_SERVER, con_respawn_ball, 0, "");
 	MACRO_REGISTER_COMMAND("mute", "ii", CFGFLAG_SERVER, con_mute, 0, "");
 	MACRO_REGISTER_COMMAND("unmute", "i", CFGFLAG_SERVER, con_unmute, 0, "");
 	MACRO_REGISTER_COMMAND("gen_pro_pw", "i", CFGFLAG_SERVER, con_gen_pw, 0, "");
 	MACRO_REGISTER_COMMAND("set_teamscore", "ii", CFGFLAG_SERVER, con_set_teamscore, 0, "");
 	MACRO_REGISTER_COMMAND("set_cwscore", "ii", CFGFLAG_SERVER, con_set_cwscore, 0, "");
 	MACRO_REGISTER_COMMAND("set_playerscore", "ii", CFGFLAG_SERVER, con_set_score, 0, "");
 	MACRO_REGISTER_COMMAND("kill", "ii", CFGFLAG_SERVER, con_kill, 0, "");
 	MACRO_REGISTER_COMMAND("all_spec", "", CFGFLAG_SERVER, con_set_all_spec, 0, "");
	MACRO_REGISTER_COMMAND("all_kick", "", CFGFLAG_SERVER, con_set_all_kick, 0, "");
	MACRO_REGISTER_COMMAND("reset_cwscore", "", CFGFLAG_SERVER, con_reset_cwscore, 0, "");
}

void mods_init()
{
	//if(!data) /* only load once */
		//data = load_data_from_memory(internal_data);
		
	for(int i = 0; i < NUM_NETOBJTYPES; i++)
		snap_set_staticsize(i, netobj_get_size(i));

	layers_init();
	col_init();

	// reset everything here
	//world = new GAMEWORLD;
	//players = new PLAYER[MAX_CLIENTS];

	// select gametype
	if(strcmp(config.sv_gametype, "mod") == 0)
		game.controller = new GAMECONTROLLER_MOD;
	else if(strcmp(config.sv_gametype, "ctf") == 0)
		game.controller = new GAMECONTROLLER_CTF;
	else if(strcmp(config.sv_gametype, "tdm") == 0)
		game.controller = new GAMECONTROLLER_TDM;
	else if(strcmp(config.sv_gametype, "ball") == 0)
		game.controller = new GAMECONTROLLER_BALL;
	else if(strcmp(config.sv_gametype, "dm-mod") == 0)
		game.controller = new GAMECONTROLLER_DMMOD;
	else if(strcmp(config.sv_gametype, "tdm-mod") == 0)
		game.controller = new GAMECONTROLLER_TDMMOD;
	else if(strcmp(config.sv_gametype, "ctf-mod") == 0)
		game.controller = new GAMECONTROLLER_CTFMOD;
	else if(strcmp(config.sv_gametype, "ko") == 0)
		game.controller = new GAMECONTROLLER_KO;
	else
		game.controller = new GAMECONTROLLER_DM;
	game.controller->cwscore[0] = 0;
	game.controller->cwscore[1] = 0;
	// setup core world
	//for(int i = 0; i < MAX_CLIENTS; i++)
	//	game.players[i].core.world = &game.world.core;

	// create all entities from the game layer
	MAPITEM_LAYER_TILEMAP *tmap = layers_game_layer();
	TILE *tiles = (TILE *)map_get_data(tmap->data);
	
	/*
	num_spawn_points[0] = 0;
	num_spawn_points[1] = 0;
	num_spawn_points[2] = 0;
	*/
	
	for(int y = 0; y < tmap->height; y++)
	{
		for(int x = 0; x < tmap->width; x++)
		{
			int index = tiles[y*tmap->width+x].index;
			
			if(index >= ENTITY_OFFSET)
			{
				vec2 pos(x*32.0f+16.0f, y*32.0f+16.0f);
				game.controller->on_entity(index-ENTITY_OFFSET, pos);
			}
			else if(index == ENTITY_SPAWN_KEEPER_RED || index == ENTITY_SPAWN_KEEPER_BLUE)
			{
				vec2 pos(x*32.0f+16.0f, y*32.0f+16.0f);
				game.controller->on_entity(index, pos);
			}
		}
	}

	//game.world.insert_entity(game.controller);

#ifdef CONF_DEBUG
	if(config.dbg_dummies)
	{
		for(int i = 0; i < config.dbg_dummies ; i++)
		{
			mods_connected(MAX_CLIENTS-i-1);
			mods_client_enter(MAX_CLIENTS-i-1);
			if(game.controller->is_teamplay())
				game.players[MAX_CLIENTS-i-1]->team = i&1;
		}
	}
#endif
}

void mods_shutdown()
{
	delete game.controller;
	game.controller = 0;
	game.clear();
}

void mods_presnap() {}
void mods_postsnap()
{
	game.events.clear();
}

extern "C" const char *mods_net_version() { return GAME_NETVERSION; }
extern "C" const char *mods_version() { return GAME_VERSION; }
