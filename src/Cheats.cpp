#include "Cheats.hpp"

#include "Modules/Console.hpp"

#include "Game.hpp"

Variable sar_autorecord("sar_autorecord", "0", "Enables automatic demo recording.\n");
Variable sar_save_flag("sar_save_flag", "#SAVE#", "Echo message when using sar_bind_save.\n"
                                                  "Default is \"#SAVE#\", a SourceRuns standard.\n"
                                                  "Keep this empty if no echo message should be binded.\n",
    0);
Variable sar_time_demo_dev("sar_time_demo_dev", "0", 0, "Printing mode when using sar_time_demo.\n"
                                                        "0 = default,\n"
                                                        "1 = console commands,\n"
                                                        "2 = console commands & packets.\n");
Variable sar_sum_during_session("sar_sum_during_session", "1", "Updates the summary counter automatically during a session.\n");
Variable sar_timer_always_running("sar_timer_always_running", "1", "Timer will save current value when disconnecting.\n");
Variable sar_hud_text("sar_hud_text", "", "Draws specified text when not empty.\n", 0);
Variable sar_hud_position("sar_hud_position", "0", 0, "Draws absolute position of the client.\n");
Variable sar_hud_angles("sar_hud_angles", "0", 0, "Draws absolute view angles of the client.\n"
                                                  "0 = default,\n"
                                                  "1 = xy,\n"
                                                  "2 = xyz.\n");
Variable sar_hud_velocity("sar_hud_velocity", "0", 0, "Draws velocity of the client.\n"
                                                      "0 = default,\n"
                                                      "1 = x/y/z,\n"
                                                      "2 = x/y\n");
Variable sar_hud_session("sar_hud_session", "0", "Draws current session tick.\n");
Variable sar_hud_last_session("sar_hud_last_session", "0", "Draws value of latest completed session.\n");
Variable sar_hud_sum("sar_hud_sum", "0", "Draws summary value of sessions.\n");
Variable sar_hud_timer("sar_hud_timer", "0", "Draws current value of timer.\n");
Variable sar_hud_avg("sar_hud_avg", "0", "Draws calculated average of timer.\n");
Variable sar_hud_cps("sar_hud_cps", "0", "Draws latest checkpoint of timer.\n");
Variable sar_hud_demo("sar_hud_demo", "0", "Draws name, tick and time of current demo.\n");
Variable sar_hud_jumps("sar_hud_jumps", "0", "Draws total jump count.\n");
Variable sar_hud_portals("sar_hud_portals", "0", "Draws total portal count.\n");
Variable sar_hud_steps("sar_hud_steps", "0", "Draws total step count.\n");
Variable sar_hud_jump("sar_hud_jump", "0", "Draws current jump distance.\n");
Variable sar_hud_jump_peak("sar_hud_jump_peak", "0", "Draws longest jump distance.\n");
Variable sar_hud_trace("sar_hud_trace", "0", 0, "Draws distance values of tracer. "
                                                "0 = default,\n"
                                                "1 = vec3,\n"
                                                "2 = vec2.\n");
Variable sar_hud_frame("sar_hud_frame", "0", "Draws current frame count.\n");
Variable sar_hud_last_frame("sar_hud_last_frame", "0", "Draws last saved frame value.\n");
Variable sar_hud_velocity_peak("sar_hud_velocity_peak", "0", "Draws last saved velocity peak.\n");
Variable sar_hud_default_spacing("sar_hud_default_spacing", "4", 0, "Spacing between elements of HUD.\n");
Variable sar_hud_default_padding_x("sar_hud_default_padding_x", "2", 0, "X padding of HUD.\n");
Variable sar_hud_default_padding_y("sar_hud_default_padding_y", "2", 0, "Y padding of HUD.\n");
Variable sar_hud_default_font_index("sar_hud_default_font_index", "0", 0, "Font index of HUD.\n");
Variable sar_hud_default_font_color("sar_hud_default_font_color", "255 255 255 255", "RGBA font color of HUD.\n", 0);
Variable sar_ihud("sar_ihud", "0", 0, "Draws keyboard events of client.\n"
                                      "0 = default,\n"
                                      "1 = forward;back;moveleft;moveright,\n"
                                      "2 = 1 + duck;jump;use,\n"
                                      "3 = 2 + attack;attack2,\n"
                                      "4 = 3 + speed;reload.\n");
Variable sar_ihud_x("sar_ihud_x", "0", 0, "X offset of input HUD.\n");
Variable sar_ihud_y("sar_ihud_y", "0", 0, "Y offset of input HUD.\n");
Variable sar_ihud_button_padding("sar_ihud_button_padding", "2", 0, "Padding between buttons of input HUD.\n");
Variable sar_ihud_button_size("sar_ihud_button_size", "60", 0, "Button size of input HUD.\n");
Variable sar_ihud_button_color("sar_ihud_button_color", "0 0 0 233", "RGBA button color of input HUD.\n", 0);
Variable sar_ihud_font_color("sar_ihud_font_color", "255 255 255 255", "RGBA font color of input HUD.\n", 0);
Variable sar_ihud_font_index("sar_ihud_font_index", "1", 0, "Font index of input HUD.\n");
Variable sar_ihud_layout("sar_ihud_layout", "WASDCSELRSR", "Layout of input HUD.\n"
                                                           "Characters are in this order:\n"
                                                           "forward,\n"
                                                           "back,\n"
                                                           "moveleft,\n"
                                                           "moveright,\n"
                                                           "duck,\n"
                                                           "jump,\n"
                                                           "use,\n"
                                                           "attack,\n"
                                                           "attack2,\n"
                                                           "speed,\n"
                                                           "reload.\n"
                                                           "Keep it empty to disable drawing characters.\n",
    0);
Variable sar_ihud_shadow("sar_ihud_shadow", "1", "Draws button shadows of input HUD.\n");
Variable sar_ihud_shadow_color("sar_ihud_shadow_color", "0 0 0 32", "RGBA button shadow color of input HUD.\n", 0);
Variable sar_ihud_shadow_font_color("sar_ihud_shadow_font_color", "255 255 255 32", "RGBA button shadow font color of input HUD.\n", 0);
Variable sar_sr_hud("sar_sr_hud", "0", 0, "Draws speedrun timer.\n");
Variable sar_sr_hud_x("sar_sr_hud_x", "0", 0, "X offset of speedrun timer HUD.\n");
Variable sar_sr_hud_y("sar_sr_hud_y", "100", 0, "Y offset of speedrun timer HUD.\n");
Variable sar_sr_hud_font_color("sar_sr_hud_font_color", "255 255 255 255", "RGBA font color of speedrun timer HUD.\n", 0);
Variable sar_sr_hud_font_index("sar_sr_hud_font_index", "70", 0, "Font index of speedrun timer HUD.\n");
Variable sar_stats_jumps_xy("sar_stats_jumps_xy", "0", "Saves jump distance as 2D vector.\n");
Variable sar_stats_velocity_peak_xy("sar_stats_velocity_peak_xy", "0", "Saves velocity peak as 2D vector.\n");
Variable sar_stats_auto_reset("sar_stats_auto_reset", "0", 0, "Resets all stats automatically.\n"
                                                              "0 = default,\n"
                                                              "1 = restart or disconnect only,\n"
                                                              "2 = any load & sar_timer_start.\n"
                                                              "Note: Portal counter is not part of the \"stats\" feature.\n");
Variable sar_autojump("sar_autojump", "0", "Enables automatic jumping on the server.\n");
Variable sar_jumpboost("sar_jumpboost", "0", 0, "Enables special game movement on the server.\n"
                                                "0 = Default,\n"
                                                "1 = Orange Box Engine,\n"
                                                "2 = Pre-OBE\n");
Variable sar_aircontrol("sar_aircontrol", "0",
#ifdef _WIN32
    0,
#endif
    "Enables more air-control on the server.\n");
Variable sar_disable_challenge_stats_hud("sar_disable_challenge_stats_hud", "0", "Disables opening the challenge mode stats HUD.\n");
Variable sar_tas_autostart("sar_tas_autostart", "1", "Starts queued commands automatically on first frame after a load.\n");
Variable sar_tas_autorecord("sar_tas_autorecord", "0", "Starts recording inputs on first frame after a load.\n");
Variable sar_tas_autoplay("sar_tas_autoplay", "1", "Starts playing back recorded inputs on first frame after a load.\n");
Variable sar_debug_event_queue("sar_debug_event_queue", "0", "Prints entitity events when they are fired, similar to developer.\n");
Variable sar_debug_game_events("sar_debug_game_events", "0", "Prints game event data, similar to net_showevents.\n");
Variable sar_speedrun_autostart("sar_speedrun_autostart", "0", "Starts speedrun timer automatically on first frame after a load.\n");
Variable sar_speedrun_autostop("sar_speedrun_autostop", "0", "Stops speedrun timer automatically when going into the menu.\n");

Variable cl_showpos;
Variable sv_cheats;
Variable sv_footsteps;
Variable sv_alternateticks;
Variable sv_bonus_challenge;
Variable sv_accelerate;
Variable sv_airaccelerate;
Variable sv_friction;
Variable sv_maxspeed;
Variable sv_stopspeed;
Variable sv_maxvelocity;
Variable sv_transition_fade_time;
Variable sv_laser_cube_autoaim;
Variable ui_loadingscreen_transition_time;
Variable hide_gun_when_holding;

void Cheats::Init()
{
    cl_showpos = Variable("cl_showpos");
    sv_cheats = Variable("sv_cheats");
    sv_footsteps = Variable("sv_footsteps");
    sv_alternateticks = Variable("sv_alternateticks");
    sv_bonus_challenge = Variable("sv_bonus_challenge");
    sv_accelerate = Variable("sv_accelerate");
    sv_airaccelerate = Variable("sv_airaccelerate");
    sv_friction = Variable("sv_friction");
    sv_maxspeed = Variable("sv_maxspeed");
    sv_stopspeed = Variable("sv_stopspeed");
    sv_maxvelocity = Variable("sv_maxvelocity");

    sv_accelerate.Unlock();
    sv_airaccelerate.Unlock();
    sv_friction.Unlock();
    sv_maxspeed.Unlock();
    sv_stopspeed.Unlock();
    sv_maxvelocity.Unlock();
    sv_footsteps.Unlock();

    if (game->version == SourceGame::Portal2) {
        sv_transition_fade_time = Variable("sv_transition_fade_time");
        sv_laser_cube_autoaim = Variable("sv_laser_cube_autoaim");
        ui_loadingscreen_transition_time = Variable("ui_loadingscreen_transition_time");
        hide_gun_when_holding = Variable("hide_gun_when_holding");

        // Don't find a way to abuse this, ok?
        sv_bonus_challenge.Unlock(false);
        sv_transition_fade_time.Unlock();
        sv_laser_cube_autoaim.Unlock();
        ui_loadingscreen_transition_time.Unlock();
        // Not a real cheat, right?
        hide_gun_when_holding.Unlock(false);
    } else if (game->version == SourceGame::HalfLife2) {
        auto sv_portal_debug_touch = Variable("sv_portal_debug_touch");
        if (!!sv_portal_debug_touch) {
            game->version = SourceGame::Portal;
            console->DevMsg("SAR: Detected Portal version!\n");
        }
    }

    sar_jumpboost.UniqueFor(Game::IsPortal2Engine);
    sar_aircontrol.UniqueFor(Game::IsPortal2Engine);
    sar_hud_portals.UniqueFor(Game::IsPortalGame);
    sar_disable_challenge_stats_hud.UniqueFor(Game::HasChallengeMode);
    sar_debug_game_events.UniqueFor(Game::HasChallengeMode);
    sar_sr_hud.UniqueFor(Game::HasChallengeMode);
    sar_sr_hud_x.UniqueFor(Game::HasChallengeMode);
    sar_sr_hud_y.UniqueFor(Game::HasChallengeMode);
    sar_sr_hud_font_color.UniqueFor(Game::HasChallengeMode);
    sar_sr_hud_font_index.UniqueFor(Game::HasChallengeMode);

    Variable::RegisterAll();
}
void Cheats::Shutdown()
{
    sv_accelerate.Lock();
    sv_airaccelerate.Lock();
    sv_friction.Lock();
    sv_maxspeed.Lock();
    sv_stopspeed.Lock();
    sv_maxvelocity.Lock();
    sv_footsteps.Lock();

    if (game->version == SourceGame::Portal2) {
        sv_bonus_challenge.Lock();
        sv_transition_fade_time.Lock();
        sv_laser_cube_autoaim.Lock();
        ui_loadingscreen_transition_time.Lock();
        hide_gun_when_holding.Lock();
    }

    Variable::UnregisterAll();
}

Cheats* cheats;