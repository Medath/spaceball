/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */


/* client */
MACRO_CONFIG_INT(cl_predict, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Predict client movements")
MACRO_CONFIG_INT(cl_nameplates, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show nameplates")
MACRO_CONFIG_INT(cl_nameplates_always, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Always show nameplats disregarding of distance")
MACRO_CONFIG_INT(cl_autoswitch_weapons, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Auto switch weapon on pickup")

MACRO_CONFIG_INT(cl_showfps, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame FPS counter")

MACRO_CONFIG_INT(cl_airjumpindicator, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(cl_threadsoundloading, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(cl_warning_teambalance, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Warn about team balance")

MACRO_CONFIG_INT(cl_mouse_deadzone, 300, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(cl_mouse_followfactor, 60, 0, 200, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(cl_mouse_max_distance, 800, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ed_showkeys, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(cl_flow, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(cl_show_welcome, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(cl_motd_time, 10, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "How long to show the server message of the day")

MACRO_CONFIG_STR(cl_version_server, 100, "version.teeworlds.com", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Server to use to check for new versions")

MACRO_CONFIG_INT(player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toggles usage of custom colors")
MACRO_CONFIG_INT(player_color_body, 65408, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player body color")
MACRO_CONFIG_INT(player_color_feet, 65408, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player feet color")
MACRO_CONFIG_STR(player_skin, 64, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player skin")

MACRO_CONFIG_INT(ui_page, 5, 0, 9, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface page")
MACRO_CONFIG_INT(ui_toolbox_page, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toolbox page")
MACRO_CONFIG_STR(ui_server_address, 128, "localhost:8303", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface server address")
MACRO_CONFIG_INT(ui_scale, 100, 1, 100000, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface scale")

MACRO_CONFIG_INT(ui_color_hue, 160, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color hue")
MACRO_CONFIG_INT(ui_color_sat, 70, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color saturation")
MACRO_CONFIG_INT(ui_color_lht, 175, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color lightness")
MACRO_CONFIG_INT(ui_color_alpha, 228, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface alpha")

MACRO_CONFIG_INT(gfx_noclip, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disable clipping")

/* server */
MACRO_CONFIG_INT(sv_warmup, 0, 0, 0, CFGFLAG_SERVER, "Number of seconds to do warpup before round starts")
MACRO_CONFIG_STR(sv_motd, 900, "", CFGFLAG_SERVER, "Message of the day to display for the clients")
MACRO_CONFIG_INT(sv_teamdamage, 0, 0, 1, CFGFLAG_SERVER, "Team damage")
MACRO_CONFIG_STR(sv_maprotation, 768, "", CFGFLAG_SERVER, "Maps to rotate between")
MACRO_CONFIG_INT(sv_rounds_per_map, 1, 1, 100, CFGFLAG_SERVER, "Number of rounds on each map before rotating")
MACRO_CONFIG_INT(sv_powerups, 1, 0, 1, CFGFLAG_SERVER, "Allow powerups like ninja")
MACRO_CONFIG_INT(sv_scorelimit, 20, 0, 1000, CFGFLAG_SERVER, "Score limit (0 disables)")
MACRO_CONFIG_INT(sv_timelimit, 0, 0, 1000, CFGFLAG_SERVER, "Time limit in minutes (0 disables)")
MACRO_CONFIG_STR(sv_gametype, 32, "dm", CFGFLAG_SERVER, "Game type (dm, tdm, ctf)")
MACRO_CONFIG_INT(sv_tournament_mode, 0, 0, 1, CFGFLAG_SERVER, "Tournament mode. When enabled, players joins the server as spectator")
MACRO_CONFIG_INT(sv_spamprotection, 1, 0, 1, CFGFLAG_SERVER, "Spam protection")

MACRO_CONFIG_INT(sv_spectator_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of slots to reserve for spectators")
MACRO_CONFIG_INT(sv_teambalance_time, 1, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before autobalancing teams")

MACRO_CONFIG_INT(sv_vote_kick, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to kick players")
MACRO_CONFIG_INT(sv_vote_kick_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time to ban a player if kicked by vote. 0 makes it just use kick")
MACRO_CONFIG_INT(sv_vote_scorelimit, 0, 0, 1, CFGFLAG_SERVER, "Allow voting to change score limit")
MACRO_CONFIG_INT(sv_vote_timelimit, 0, 0, 1, CFGFLAG_SERVER, "Allow voting to change time limit")

/* debug */
#ifdef CONF_DEBUG /* this one can crash the server if not used correctly */
	MACRO_CONFIG_INT(dbg_dummies, 0, 0, 15, CFGFLAG_SERVER, "")
#endif

MACRO_CONFIG_INT(dbg_focus, 0, 0, 1, CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(dbg_tuning, 0, 0, 1, CFGFLAG_CLIENT, "")





MACRO_CONFIG_INT(sv_ball_mod, 0, 0, 1, CFGFLAG_SERVER, "Is enabled if you choose the ball gametype")
MACRO_CONFIG_INT(sv_ninja_mod, 0, 0, 1, CFGFLAG_SERVER, "If you have the ninja, you can use it forever")
MACRO_CONFIG_INT(sv_ko_mod, 0, 0, 1, CFGFLAG_SERVER, "Is enabled if you choose the ko gametype")



MACRO_CONFIG_INT(sv_hammer_team_att_loss, 0, 0, 10, CFGFLAG_SERVER, "The attacker looses this health, if he attacks a teammate")
MACRO_CONFIG_INT(sv_hammer_att_loss, 0, 0, 10, CFGFLAG_SERVER, "The attacker looses this health")
MACRO_CONFIG_INT(sv_hammer_def_loss, 0, 0, 10, CFGFLAG_SERVER, "The victim player looses this armor")
MACRO_CONFIG_INT(sv_big_hammer_team_att_loss, 0, 0, 10, CFGFLAG_SERVER, "The attacker looses this health, if he attacks a teammate")
MACRO_CONFIG_INT(sv_big_hammer_att_loss, 0, 0, 10, CFGFLAG_SERVER, "The attacker looses this health")
MACRO_CONFIG_INT(sv_big_hammer_def_loss, 0, 0, 10, CFGFLAG_SERVER, "The victim player looses this armor")
MACRO_CONFIG_INT(sv_partly_dead, 0, 0, 10000, CFGFLAG_SERVER, "If the player only has 1hp, the player is partly dead for this value seconds. In this time there is no health/armor regeneration and the player can't hook other players or take the ball")
MACRO_CONFIG_INT(sv_big_hammer, 0, 0, 10, CFGFLAG_SERVER, "Enables a big hammer. This hammer has a higher range than the normal one and only decreases the armor of the victim")
MACRO_CONFIG_INT(sv_hook_team_att_decr, 0, 0, 1000, CFGFLAG_SERVER, "The hooker's health is decreased by this value in 10 seconds, when hooking a teammate")
MACRO_CONFIG_INT(sv_hook_att_decr, 0, 0, 1000, CFGFLAG_SERVER, "The hooker's health is decreased by this value in 10 seconds")
MACRO_CONFIG_INT(sv_hook_def_decr, 0, 0, 1000, CFGFLAG_SERVER, "The hooked player's armor is decreased by this value in 10 seconds")
MACRO_CONFIG_INT(sv_hook_def_health_decr, 0, 0, 1000, CFGFLAG_SERVER, "The hooked player's health is decreased by this value in 10 seconds")
MACRO_CONFIG_INT(sv_health_regen, 0, 0, 1000, CFGFLAG_SERVER, "Health regeneration (this value is the added health in 10 seconds)")
MACRO_CONFIG_INT(sv_armor_regen, 0, 0, 1000, CFGFLAG_SERVER, "Armor regeneration (this value is the added armor in 10 seconds)")


MACRO_CONFIG_INT(sv_hook_teammates, 1, 0, 1, CFGFLAG_SERVER, "Enable the ability to hook teammates")

MACRO_CONFIG_INT(sv_grenade_startspeed, 0, 0, 1000000, CFGFLAG_SERVER, "startspeed of the grenade (the sum of player-speed and normal grenade-speed)")

MACRO_CONFIG_INT(sv_ice_friction, 4, 0, 1000000, CFGFLAG_SERVER, "")
MACRO_CONFIG_INT(sv_ice_max_speed, 10000, 0, 1000000, CFGFLAG_SERVER, "")
MACRO_CONFIG_INT(sv_ice_accel, 180, 0, 1000000, CFGFLAG_SERVER, "")

MACRO_CONFIG_INT(sv_spawn_delay, 2000, 0, 100000, CFGFLAG_SERVER, "Spawn delay for players after a kill")

MACRO_CONFIG_INT(sv_msg_intervall, 4, 0, 10000, CFGFLAG_SERVER, "After what time the message sv_min_msg is displayed (minutes).")
MACRO_CONFIG_STR(sv_min_msg, 512, "", CFGFLAG_SERVER, "A message displayed every sv_msg_intervall minutes in the chat.")

MACRO_CONFIG_INT(sv_msg_frame_time, 4, 0, 10000, CFGFLAG_SERVER, "After this time the server decreases the number of messages a player sent by one. (So it's kind of messages per minute here)")
MACRO_CONFIG_INT(sv_max_msgs, 5, 0, 10000, CFGFLAG_SERVER, "Maximal messages before muting the player. (So that's something like toleranz to the sv_msg_frame_time value)")
MACRO_CONFIG_INT(sv_msg_mute_time, 90, 0, 10000, CFGFLAG_SERVER, "How long are people muted")
MACRO_CONFIG_INT(sv_max_idle, 60, 0, 10000, CFGFLAG_SERVER, "After this time afk-players join the spectators")
MACRO_CONFIG_INT(sv_kick_idle, 1, 0, 1, CFGFLAG_SERVER, "Kick afk-player if the server is full")

MACRO_CONFIG_INT(sv_start_health, 10, 0, 10, CFGFLAG_SERVER, "The player has this health after respawn.")
MACRO_CONFIG_INT(sv_start_armor, 0, 0, 10, CFGFLAG_SERVER, "The player has this armor after respawn.")
MACRO_CONFIG_INT(sv_start_hammer, 1, 0, 1, CFGFLAG_SERVER, "The player has a hammer after respawn.")
MACRO_CONFIG_INT(sv_start_pistol, 10, 0, 10, CFGFLAG_SERVER, "The player has a pistol after respawn with this ammo.")
MACRO_CONFIG_INT(sv_start_shotgun, 0, 0, 10, CFGFLAG_SERVER, "The player has a shotgun after respawn with this ammo.")
MACRO_CONFIG_INT(sv_start_grenade, 0, 0, 10, CFGFLAG_SERVER, "The player has a grenadelauncher after respawn with this ammo.")
MACRO_CONFIG_INT(sv_start_ninja, 0, 0, 1, CFGFLAG_SERVER, "The player has a ninja after respawn.")
MACRO_CONFIG_INT(sv_start_rifle, 0, 0, 10, CFGFLAG_SERVER, "The player has a rifle after respawn with this ammo.")

MACRO_CONFIG_INT(sv_pistol_regen, 500, 0, 100000, CFGFLAG_SERVER, "Ammo regeneration")
MACRO_CONFIG_INT(sv_shotgun_regen, 0, 0, 100000, CFGFLAG_SERVER, "Ammo regeneration")
MACRO_CONFIG_INT(sv_grenade_regen, 0, 0, 100000, CFGFLAG_SERVER, "Ammo regeneration")
MACRO_CONFIG_INT(sv_rifle_regen, 0, 0, 100000, CFGFLAG_SERVER, "Ammo regeneration")
MACRO_CONFIG_INT(sv_immediate_ammo_regen, 0, 0, 1, CFGFLAG_SERVER, "Not waiting the reload time before regenerating the ammo.")

MACRO_CONFIG_STR(sv_pre_password_msg, 512, "Here is your password for the professional server. It's the correct one for this day and your nick, noone else.", CFGFLAG_SERVER, "Message the server gives the player who receives a password for pro-server")
MACRO_CONFIG_INT(sv_generate_pro_pw, 0, 0, 1, CFGFLAG_SERVER, "Gives good players a password at the end of a round.")
MACRO_CONFIG_INT(sv_use_pro_pw, 0, 0, 1, CFGFLAG_SERVER, "The player have to type in the custom password for his name or the general password.")
MACRO_CONFIG_STR(sv_pro_password, 32, "", CFGFLAG_SERVER, "The general password for the professional server (is needed to generate and check passwords)")

MACRO_CONFIG_INT(sv_reserved_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of reserved slots")
MACRO_CONFIG_STR(sv_reserved_slot_pass, 32, "", CFGFLAG_SERVER, "Password for a reserved slot")

MACRO_CONFIG_INT(sv_handle_mapvotes, 1, 0, 1, CFGFLAG_SERVER, "Generates a special debug message and gives the player the feeling of voting for the map")



MACRO_CONFIG_INT(sv_goaler_score, 1, 0, 1000, CFGFLAG_SERVER, "Score for the goaler")
MACRO_CONFIG_INT(sv_passer_score, 1, 0, 1000, CFGFLAG_SERVER, "Score for the passer")
MACRO_CONFIG_INT(sv_team_score, 1, 0, 1000, CFGFLAG_SERVER, "Score for the team")
MACRO_CONFIG_INT(sv_team_pass_score, 1, 0, 1000, CFGFLAG_SERVER, "Score for the team through a goal with pass")
MACRO_CONFIG_INT(sv_own_goal, 1, 0, 1000, CFGFLAG_SERVER, "Negative score for a wrong goal")

MACRO_CONFIG_INT(sv_second_weapon, 0, 0, 6, CFGFLAG_SERVER, "The normal weapon if the player don't have the ball")
MACRO_CONFIG_INT(sv_ball_att_decr, 0, 0, 1000, CFGFLAG_SERVER, "The ball owner's health is decreased by this in 10 seconds")
MACRO_CONFIG_INT(sv_ball_def_decr, 0, 0, 1000, CFGFLAG_SERVER, "The ball owner's armor is decreased by this in 10 seconds")
MACRO_CONFIG_INT(sv_bounce_loss_x, 50, 0, 100000, CFGFLAG_SERVER, "The ball looses that much x-speed after a bounce")
MACRO_CONFIG_INT(sv_bounce_loss_y, 50, 0, 100000, CFGFLAG_SERVER, "The ball looses that much y-speed after a bounce")
MACRO_CONFIG_INT(sv_explosions, 0, 0, 1, CFGFLAG_SERVER, "Should the grenades explode")
MACRO_CONFIG_INT(sv_goal_keeptime, 5, 0, 100000, CFGFLAG_SERVER, "The goalkeeper fires the ball automatically after this time (0 immediately)")
MACRO_CONFIG_INT(sv_player_keeptime, 3, 0, 100000, CFGFLAG_SERVER, "The player fires the ball automatically after this time (0 immediately)")
MACRO_CONFIG_INT(sv_real_foot, 0, 0, 1, CFGFLAG_SERVER, "Disables the hammer")
MACRO_CONFIG_INT(sv_suicide_score, 1, 0, 1, CFGFLAG_SERVER, "Count selfkills as negative score")
MACRO_CONFIG_INT(sv_ball_respawn, 6, 0, 100000, CFGFLAG_SERVER, "Respawn time of the ball")
MACRO_CONFIG_INT(sv_goalkeeper, 0, 0, 16, CFGFLAG_SERVER, "Enables goalkeeper")
MACRO_CONFIG_INT(sv_goalkeeper_jumping, 0, 0, 1, CFGFLAG_SERVER, "Endless jumping of the goalkeeper")
MACRO_CONFIG_INT(sv_hook_goalkeeper, 1, 0, 1, CFGFLAG_SERVER, "Enable the ability to hook goalkeeper")

MACRO_CONFIG_INT(sv_pickup_with_no_armor, 1, 0, 1, CFGFLAG_SERVER, "Pickup the ball without any armor?")

MACRO_CONFIG_INT(sv_diff_score, 0, 0, 1000, CFGFLAG_SERVER, "Difference between the team-scores before a team can win")
MACRO_CONFIG_INT(sv_kill_score, 1, 0, 1, CFGFLAG_SERVER, "Are there points for a kill")

MACRO_CONFIG_INT(sv_cwscore, 0, 0, 1, CFGFLAG_SERVER, "Should cw_score be displayed")

MACRO_CONFIG_INT(sv_silent_mode, 0, 0, 1, CFGFLAG_SERVER, "No servermessages in the chat")
MACRO_CONFIG_INT(sv_total_silence, 0, 0, 1, CFGFLAG_SERVER, "Disables the chat")
MACRO_CONFIG_INT(sv_spec_silence, 0, 0, 1, CFGFLAG_SERVER, "The spectators can't speak global only under the spectators")
MACRO_CONFIG_INT(sv_respawn_powerups, 1, 0, 1, CFGFLAG_SERVER, "Should the powerups respawn")
MACRO_CONFIG_INT(sv_ball_reloader, 10, 0, 1000, CFGFLAG_SERVER, "Should the powerups respawn")

MACRO_CONFIG_STR(sv_version_string, 32, "0.5 b67d1f1a1eea234e", CFGFLAG_SERVER, "Version string")


MACRO_CONFIG_INT(sv_bumper_accelerate, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a bumper-tile divided by 1000")
MACRO_CONFIG_INT(sv_bumper_brake, 500, 0, 100000, CFGFLAG_SERVER, "The braking getting from a bumper-tile divided by 1000")
MACRO_CONFIG_INT(sv_brake, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile divided by 100000")
MACRO_CONFIG_INT(sv_accelerate, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile divided by 100000")
MACRO_CONFIG_INT(sv_accelerate_up, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile in one direction divided by 100000")
MACRO_CONFIG_INT(sv_accelerate_right, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile in one direction divided by 100000")
MACRO_CONFIG_INT(sv_accelerate_down, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile in one direction divided by 100000")
MACRO_CONFIG_INT(sv_accelerate_left, 500, 0, 10000000, CFGFLAG_SERVER, "The acceleration getting from a tile in one direction divided by 100000")

MACRO_CONFIG_INT(sv_goal_timer, 0, 0, 1, CFGFLAG_SERVER, "Activate a goal timer")
MACRO_CONFIG_INT(sv_calc_steps, 100, 0, 10000000, CFGFLAG_SERVER, "Claculation steps per tick for the collision detection")
MACRO_CONFIG_INT(sv_action_1_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_1_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_1_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_1_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")
MACRO_CONFIG_INT(sv_action_2_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_2_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_2_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_2_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")
MACRO_CONFIG_INT(sv_action_3_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_3_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_3_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_3_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")
MACRO_CONFIG_INT(sv_action_4_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_4_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_4_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_4_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")
MACRO_CONFIG_INT(sv_action_5_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_5_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_5_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_5_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")
MACRO_CONFIG_INT(sv_action_6_cause, 0, 0, 3, CFGFLAG_SERVER, "0 = ball, 1 = tee, 2 = red tee, 3 = blue tee")
MACRO_CONFIG_INT(sv_action_6_tile, 0, 0, 10, CFGFLAG_SERVER, "0 = BUMPER_SPEEDUP, 1 = SPEEDUP, 2 = SPEEDUP_UP, 3 = SPEEDUP_RIGHT, 4 = SPEEDUP_DOWN, 5 = SPEEDUP_LEFT, 6 = BUMPER_SLOWDOWN, 7 = SLOWDOWN, 8 = BUMPER_RANDOM, 9 = RANDOM, 10 = BALL_RESET")
MACRO_CONFIG_INT(sv_action_6_type, 0, -1, 10000000, CFGFLAG_SERVER, "-1 = activate, 0 = deactivate, X = activate for X seconds")
MACRO_CONFIG_STR(sv_action_6_cmd, 512, "", CFGFLAG_SERVER, "Command that should be executed")


MACRO_CONFIG_INT(sv_bumper_speedup, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_speedup, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_speedup_up, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_speedup_right, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_speedup_down, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_speedup_left, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_bumper_slowdown, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_slowdown, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_bumper_random, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_random, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_ball_reset, 1, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")

MACRO_CONFIG_INT(sv_vertex_radius, 0, 0, 31, CFGFLAG_SERVER, "is this tile activated at the beginning?")
MACRO_CONFIG_INT(sv_conservative_collision, 0, 0, 1, CFGFLAG_SERVER, "is this tile activated at the beginning?")



// matthis
MACRO_CONFIG_INT(sv_new_pass_system, 0, 0, 1,    CFGFLAG_SERVER, "1 = Activate the new pass system, 2 = Deactivate it.")
MACRO_CONFIG_INT(sv_pass_usability,  4, 0, 1000, CFGFLAG_SERVER, "In this time ( seconds ) you can exploit a pass.")
MACRO_CONFIG_INT(sv_shot_usability,  4, 0, 1000, CFGFLAG_SERVER, "If the time between shoot and goal is bigger than it, there wont be a passpoint!")
MACRO_CONFIG_INT(sv_passes_only,     0, 0, 1,    CFGFLAG_SERVER, "1 = You Can only score with Pass Goals")
MACRO_CONFIG_INT(sv_owngoal_respawn, 0, 0, 1,    CFGFLAG_SERVER, "If sv_passes_only == 1 and someone does an own goal, then the players will respawn if sv_owngoal_respawn is 1, otherwise not.")


