#pragma once
#include <stdarg.h>

#include "Console.hpp"
#include "DemoPlayer.hpp"
#include "DemoRecorder.hpp"
#include "Tier1.hpp"

#include "Features/Cvars.hpp"
#include "Features/Session.hpp"
#include "Features/Speedrun/SpeedrunTimer.hpp"
#include "Features/Stats/Stats.hpp"
#include "Features/StepCounter.hpp"
#include "Features/Summary.hpp"
#include "Features/Tas/CommandQueuer.hpp"
#include "Features/Tas/ReplaySystem.hpp"
#include "Features/Timer/Timer.hpp"

#include "Cheats.hpp"
#include "Game.hpp"
#include "Interface.hpp"
#include "SAR.hpp"
#include "Utils.hpp"

#if _WIN32
#define IServerMessageHandler_VMT_Offset 8
#endif

namespace Engine {

Interface* engine;
Interface* cl;
Interface* s_GameEventManager;
Interface* eng;
Interface* debugoverlay;

using _GetScreenSize = int(__stdcall*)(int& width, int& height);
using _ClientCmd = int(__func*)(void* thisptr, const char* szCmdString);
using _GetLocalPlayer = int(__func*)(void* thisptr);
using _GetViewAngles = int(__func*)(void* thisptr, QAngle& va);
using _SetViewAngles = int(__func*)(void* thisptr, QAngle& va);
using _GetMaxClients = int (*)();
using _AddListener = bool(__func*)(void* thisptr, IGameEventListener2* listener, const char* name, bool serverside);
using _RemoveListener = bool(__func*)(void* thisptr, IGameEventListener2* listener);
using _Cbuf_AddText = void(__cdecl*)(int slot, const char* pText, int nTickDelay);
using _AddText = void(__func*)(void* thisptr, const char* pText, int nTickDelay);
#ifdef _WIN32
using _GetActiveSplitScreenPlayerSlot = int (*)();
using _ScreenPosition = int(__stdcall*)(const Vector& point, Vector& screen);
using _ConPrintEvent = int(__stdcall*)(IGameEvent* ev);
#else
using _GetActiveSplitScreenPlayerSlot = int (*)(void* thisptr);
using _ScreenPosition = int(__stdcall*)(void* thisptr, const Vector& point, Vector& screen);
using _ConPrintEvent = int(__cdecl*)(void* thisptr, IGameEvent* ev);
#endif

_GetScreenSize GetScreenSize;
_ClientCmd ClientCmd;
_GetLocalPlayer GetLocalPlayer;
_GetViewAngles GetViewAngles;
_SetViewAngles SetViewAngles;
_GetMaxClients GetMaxClients;
_GetActiveSplitScreenPlayerSlot GetActiveSplitScreenPlayerSlot;
_AddListener AddListener;
_RemoveListener RemoveListener;
_Cbuf_AddText Cbuf_AddText;
_AddText AddText;
_ScreenPosition ScreenPosition;
_ConPrintEvent ConPrintEvent;

int* tickcount;
float* interval_per_tick;
char* m_szLevelName;
CHostState* hoststate;
void* s_CommandBuffer;
bool* m_bWaitEnabled;

void ExecuteCommand(const char* cmd)
{
    ClientCmd(engine->ThisPtr(), cmd);
}
void ClientCommand(const char* fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    char data[1024];
    vsnprintf(data, sizeof(data), fmt, argptr);
    va_end(argptr);
    ClientCmd(engine->ThisPtr(), data);
}
int GetSessionTick()
{
    int result = *tickcount - session->baseTick;
    return (result >= 0) ? result : 0;
}
float ToTime(int tick)
{
    return tick * *interval_per_tick;
}
int GetLocalPlayerIndex()
{
    return GetLocalPlayer(engine->ThisPtr());
}
QAngle GetAngles()
{
    auto va = QAngle();
    GetViewAngles(engine->ThisPtr(), va);
    return va;
}
void SetAngles(QAngle va)
{
    SetViewAngles(engine->ThisPtr(), va);
}
void SendToCommandBuffer(const char* text, int delay)
{
    if (Game::IsPortal2Engine()) {
#ifdef _WIN32
        auto slot = GetActiveSplitScreenPlayerSlot();
#else
        auto slot = GetActiveSplitScreenPlayerSlot(nullptr);
#endif
        Cbuf_AddText(slot, text, delay);
    } else if (Game::IsHalfLife2Engine()) {
        AddText(s_CommandBuffer, text, delay);
    }
}
int PointToScreen(const Vector& point, Vector& screen)
{
#ifdef _WIN32
    return ScreenPosition(point, screen);
#else
    return ScreenPosition(nullptr, point, screen);
#endif
}

bool isInSession = false;
unsigned currentFrame = 0;
unsigned lastFrame = 0;
HOSTSTATES prevState;

void SessionStarted(bool menu = false)
{
    if (isInSession) {
        return;
    }

    if (menu) {
        console->Print("Session started! (menu)\n");
        session->Rebase(*tickcount);

        if (sar_speedrun_autostop.GetBool()) {
            speedrun->Stop(false);
        } else {
            speedrun->Unpause(tickcount);
        }
    } else {
        if (GetMaxClients() <= 1) {
            console->Print("Session Started!\n");
            session->Rebase(*tickcount);
            timer->Rebase(*tickcount);

            speedrun->Unpause(tickcount);
        }

        if (rebinder->isSaveBinding || rebinder->isReloadBinding) {
            if (DemoRecorder::IsRecordingDemo) {
                rebinder->UpdateIndex(*DemoRecorder::m_nDemoNumber);
            } else {
                rebinder->UpdateIndex(rebinder->lastIndexNumber + 1);
            }

            rebinder->RebindSave();
            rebinder->RebindReload();
        }

        if (sar_tas_autostart.GetBool()) {
            tasQueuer->Start();
        }
        if (sar_tas_autorecord.GetBool()) {
            tasReplaySystem->StartRecording();
        }
        if (sar_tas_autoplay.GetBool()) {
            tasReplaySystem->StartPlaying();
        }
        if (sar_speedrun_autostart.GetBool() && !speedrun->IsActive()) {
            speedrun->Start(tickcount);
        }

        stepCounter->ResetTimer();
        currentFrame = 0;
    }

    isInSession = true;
}
void SessionEnded()
{
    if (!isInSession) {
        return;
    }

    int tick = GetSessionTick();

    if (tick != 0) {
        console->Print("Session: %i (%.3f)\n", tick, Engine::ToTime(tick));
        session->lastSession = tick;
    }

    if (summary->isRunning) {
        summary->Add(tick, Engine::ToTime(tick), m_szLevelName);
        console->Print("Total: %i (%.3f)\n", summary->totalTicks, Engine::ToTime(summary->totalTicks));
    }

    if (timer->isRunning) {
        if (sar_timer_always_running.GetBool()) {
            timer->Save(*tickcount);
            console->Print("Timer paused: %i (%.3f)!\n", timer->totalTicks, Engine::ToTime(timer->totalTicks));
        } else {
            timer->Stop(*tickcount);
            console->Print("Timer stopped!\n");
        }
    }

    auto reset = sar_stats_auto_reset.GetInt();
    if ((reset == 1 && !*m_bLoadgame) || reset >= 2) {
        stats->ResetAll();
    }

    DemoRecorder::CurrentDemo = "";
    lastFrame = currentFrame;
    currentFrame = 0;

    tasQueuer->Stop();
    tasReplaySystem->Stop();
    speedrun->Pause();

    isInSession = false;
}
void SessionChanged(int state)
{
    console->DevMsg("state = %i\n", state);

    // Demo recorder starts syncing from this tick
    if (state == SignonState::Full) {
        SessionStarted();
    } else {
        SessionEnded();
    }
}
void SessionChanged()
{
    console->DevMsg("m_currentState = %i\n", hoststate->m_currentState);

    if (hoststate->m_currentState == HOSTSTATES::HS_CHANGE_LEVEL_SP
        || hoststate->m_currentState == HOSTSTATES::HS_CHANGE_LEVEL_MP
        || hoststate->m_currentState == HOSTSTATES::HS_GAME_SHUTDOWN) {
        SessionEnded();
    } else if (hoststate->m_currentState == HOSTSTATES::HS_RUN
        && !hoststate->m_activeGame) {
        SessionStarted(true);
    }
}
void SafeUnload(const char* postCommand = nullptr)
{
    // The exit command will handle everything
    ExecuteCommand("sar_exit");

    if (postCommand) {
        SendToCommandBuffer(postCommand, SAFE_UNLOAD_TICK_DELAY);
    }
}

// CClientState::Disconnect
DETOUR(Disconnect, bool bShowMainMenu)
{
    SessionEnded();
    return Original::Disconnect(thisptr, bShowMainMenu);
}
#ifdef _WIN32
DETOUR(Disconnect2, int unk1, int unk2, int unk3)
{
    SessionEnded();
    return Original::Disconnect2(thisptr, unk1, unk2, unk3);
}
DETOUR_COMMAND(connect)
{
    SessionEnded();
    Original::connect_callback(args);
}
#else
DETOUR(Disconnect2, int unk, bool bShowMainMenu)
{
    SessionEnded();
    return Original::Disconnect2(thisptr, unk, bShowMainMenu);
}
#endif

// CClientState::SetSignonState
DETOUR(SetSignonState, int state, int count, void* unk)
{
    SessionChanged(state);
    return Original::SetSignonState(thisptr, state, count, unk);
}
DETOUR(SetSignonState2, int state, int count)
{
    SessionChanged(state);
    return Original::SetSignonState2(thisptr, state, count);
}

// CEngine::Frame
DETOUR(Frame)
{
    if (hoststate->m_currentState != prevState) {
        SessionChanged();
    }
    prevState = hoststate->m_currentState;

    if (hoststate->m_activeGame) {
        speedrun->Update(tickcount, hoststate->m_levelName);
    }

    return Original::Frame(thisptr);
}

DETOUR_COMMAND(plugin_load)
{
    // Prevent crash when trying to load SAR twice or try to find the module in
    // the plugin list if the initial search thread failed
    if (args.ArgC() >= 2) {
        auto file = std::string(args[1]);
        if (ends_with(file, std::string(MODULE("sar"))) || ends_with(file, std::string("sar"))) {
            if (sar->GetPlugin()) {
                plugin->ptr->m_bDisable = true;
                console->PrintActive("SAR: Plugin fully loaded!\n");
            }
            return;
        }
    }

    Original::plugin_load_callback(args);
}
DETOUR_COMMAND(plugin_unload)
{
    if (args.ArgC() >= 2 && sar->GetPlugin() && atoi(args[1]) == plugin->index) {
        SafeUnload();
    } else {
        Original::plugin_unload_callback(args);
    }
}
DETOUR_COMMAND(exit)
{
    SafeUnload("exit");
}
DETOUR_COMMAND(quit)
{
    SafeUnload("quit");
}
DETOUR_COMMAND(help)
{
    cvars->PrintHelp(args);
}

void Init()
{
    engine = Interface::Create(MODULE("engine"), "VEngineClient0", false);
    if (engine) {
        GetScreenSize = engine->Original<_GetScreenSize>(Offsets::GetScreenSize);
        ClientCmd = engine->Original<_ClientCmd>(Offsets::ClientCmd);
        GetLocalPlayer = engine->Original<_GetLocalPlayer>(Offsets::GetLocalPlayer);
        GetViewAngles = engine->Original<_GetViewAngles>(Offsets::GetViewAngles);
        SetViewAngles = engine->Original<_SetViewAngles>(Offsets::SetViewAngles);
        GetMaxClients = engine->Original<_GetMaxClients>(Offsets::GetMaxClients);
        GetGameDirectory = engine->Original<_GetGameDirectory>(Offsets::GetGameDirectory);

        Memory::Read<_Cbuf_AddText>((uintptr_t)ClientCmd + Offsets::Cbuf_AddText, &Cbuf_AddText);
        Memory::Deref<void*>((uintptr_t)Cbuf_AddText + Offsets::s_CommandBuffer, &s_CommandBuffer);
        if (game->version == SourceGame::Portal2) {
            m_bWaitEnabled = reinterpret_cast<bool*>((uintptr_t)s_CommandBuffer + Offsets::m_bWaitEnabled);
        }

        void* clPtr = nullptr;
        if (Game::IsPortal2Engine()) {
            typedef void* (*_GetClientState)();
            auto GetClientState = Memory::Read<_GetClientState>((uintptr_t)ClientCmd + Offsets::GetClientStateFunction);
            clPtr = GetClientState();

            GetActiveSplitScreenPlayerSlot = engine->Original<_GetActiveSplitScreenPlayerSlot>(Offsets::GetActiveSplitScreenPlayerSlot);
        } else if (Game::IsHalfLife2Engine()) {
            auto ServerCmdKeyValues = engine->Original(Offsets::ServerCmdKeyValues);
            clPtr = Memory::Deref<void*>(ServerCmdKeyValues + Offsets::cl);

            Memory::Read<_AddText>((uintptr_t)Cbuf_AddText + Offsets::AddText, &AddText);
        }

        if (cl = Interface::Create(clPtr)) {
            if (Game::IsPortal2Engine()) {
                cl->Hook(Detour::SetSignonState, Original::SetSignonState, Offsets::Disconnect - 1);
                cl->Hook(Detour::Disconnect, Original::Disconnect, Offsets::Disconnect);
            } else {
                cl->Hook(Detour::SetSignonState2, Original::SetSignonState2, Offsets::Disconnect - 1);
#ifdef _WIN32
                HOOK_COMMAND(connect);
#else
                cl->Hook(Detour::Disconnect2, Original::Disconnect2, Offsets::Disconnect);
#endif
            }

            auto disconnect = cl->Original(Offsets::Disconnect);
            DemoPlayer::Init(Memory::DerefDeref<void*>(disconnect + Offsets::demoplayer));
            DemoRecorder::Init(Memory::DerefDeref<void*>(disconnect + Offsets::demorecorder));

#if _WIN32
            auto IServerMessageHandler_VMT = Memory::Deref<uintptr_t>((uintptr_t)cl->ThisPtr() + IServerMessageHandler_VMT_Offset);
            auto ProcessTick = Memory::Deref<uintptr_t>(IServerMessageHandler_VMT + sizeof(uintptr_t) * Offsets::ProcessTick);
#else
            auto ProcessTick = cl->Original(Offsets::ProcessTick);
#endif
            tickcount = Memory::Deref<int*>(ProcessTick + Offsets::tickcount);
            interval_per_tick = Memory::Deref<float*>(ProcessTick + Offsets::interval_per_tick);
            speedrun->SetIntervalPerTick(interval_per_tick);

            auto SetSignonState = cl->Original(Offsets::Disconnect - 1);
            auto HostState_OnClientConnected = Memory::Read(SetSignonState + Offsets::HostState_OnClientConnected);
            Memory::Deref<CHostState*>(HostState_OnClientConnected + Offsets::hoststate, &hoststate);

            if (auto s_EngineAPI = Interface::Create(MODULE("engine"), "VENGINE_LAUNCHER_API_VERSION0", false)) {
                auto IsRunningSimulation = s_EngineAPI->Original(Offsets::IsRunningSimulation);
                auto engAddr = Memory::DerefDeref<void*>(IsRunningSimulation + Offsets::eng);

                if (eng = Interface::Create(engAddr)) {
                    eng->Hook(Detour::Frame, Original::Frame, Offsets::Frame);
                }
            }
        }
    }

    if (auto tool = Interface::Create(MODULE("engine"), "VENGINETOOL0", false)) {
        auto GetCurrentMap = tool->Original(Offsets::GetCurrentMap);
        m_szLevelName = Memory::Deref<char*>(GetCurrentMap + Offsets::m_szLevelName);
        m_bLoadgame = reinterpret_cast<bool*>((uintptr_t)m_szLevelName + Offsets::m_bLoadGame);
    }

    if (game->version == SourceGame::Portal2) {
        s_GameEventManager = Interface::Create(MODULE("engine"), "GAMEEVENTSMANAGER002", false);
        if (s_GameEventManager) {
            AddListener = s_GameEventManager->Original<_AddListener>(Offsets::AddListener);
            RemoveListener = s_GameEventManager->Original<_RemoveListener>(Offsets::RemoveListener);

            auto FireEventClientSide = s_GameEventManager->Original(Offsets::FireEventClientSide);
            auto FireEventIntern = Memory::Read(FireEventClientSide + Offsets::FireEventIntern);
            Memory::Read<_ConPrintEvent>(FireEventIntern + Offsets::ConPrintEvent, &ConPrintEvent);
        }
    }

    /*debugoverlay = Interface::Create(MODULE("engine"), "VDebugOverlay0", false);
    if (debugoverlay) {
        ScreenPosition = debugoverlay->Original<_ScreenPosition>(Offsets::ScreenPosition);
    }*/

    HOOK_COMMAND(plugin_load);
    HOOK_COMMAND(plugin_unload);
    HOOK_COMMAND(exit);
    HOOK_COMMAND(quit);
    HOOK_COMMAND(help);
}
void Shutdown()
{
    Interface::Delete(engine);
    Interface::Delete(cl);
    Interface::Delete(eng);
    Interface::Delete(s_GameEventManager);

#ifdef _WIN32
    UNHOOK_COMMAND(connect);
#endif
    UNHOOK_COMMAND(plugin_load);
    UNHOOK_COMMAND(plugin_unload);
    UNHOOK_COMMAND(exit);
    UNHOOK_COMMAND(quit);
    UNHOOK_COMMAND(help);

    DemoPlayer::Shutdown();
    DemoRecorder::Shutdown();
}
}