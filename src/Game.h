#pragma once

#include <thread>
#ifndef __ANDROID__
#include "audio/SoundSystem.h"
#include "audio/OggStream.h"
#else
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#include "audio/AndroidSoundSystem.h"
#endif
#include "Vectors.h"
#include "TouchData.h"
#include "SysConfig.h"
#include "ShaderProgram.h"
#include "CClient.h"
#include "CServer.h"
#include "map.h"


const int GameKeyCount = 10;

static CMap mapas;
static CClient clientas;
static CServer serveris;
static SystemConfig sys;

enum Actions
{
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_OPEN,
    ACTION_BACK,
    ACTION_FIRE,
    ACTION_NEXT_WEAPON,
    ACTION_MAP

};

enum GameState
{
    GAMESTATE_TITLE,
    GAMESTATE_INTRO,
    GAMESTATE_HELP,
    GAMESTATE_GAME,
    GAMESTATE_ENDING
};



class Game
{

    float   OrthoMatrix[16];
    float   ViewMatrix[16];

    ShaderProgram        defaultShader;
    ShaderProgram        colorShader;


public:

    GameState state;

    char EditText[255];

    TouchData   touches;

    int DebugMode;
    Vector3D gamepad;
    Vector3D OldGamepad;
    float MouseX;
    float MouseY;

    float OldMouseX;
    float OldMouseY;

    float RelativeMouseX;
    float RelativeMouseY;
    unsigned char globalKEY;
    unsigned char globalKeyUp;
    unsigned char OldKeys[GameKeyCount]; //zaidimo mygtukai
    unsigned char Keys[GameKeyCount]; //zaidimo mygtukai
    char DocumentPath[255];
    bool Works;
    bool windowed;
    unsigned ScreenWidth;
    unsigned ScreenHeight;
    float TimeTicks;
    float DeltaTime;
    float DT;
    float Accumulator;

public:

    Game();

    void init();
    void render();
    void logic();
    void destroy();
    void loadConfig();
    static void Threadproc();

private:
    void DrawTitleScreen();
    void DrawEndScreen();
    void DrawGameplay();
    void MoveDude();
    void SendWarpMessage();
    void ItemPickup();
    void SendItemCRemove(int itemIndex);
    void GoToLevel(int level, int otherplayer);
    void SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked);
    //mapo pavadinimas, klientu skaicius
    static void SendMapInfo(int clientIndex, CMap& map);
    //monster races amd item positions
    static void SendMapData(int clientIndex, CMap& map);
    void PutExit();
    void InitServer();
    void StopServer();
    bool JoinServer(const char* ip, unsigned port);
    void QuitServer();
    void SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe);
    void DoorsInteraction();
    void SlimeReaction(int index);
    void SendClientAtackImpulse(int victimID, int hp);
    void SendClientCoords();
    void SendClientDoorState(int doorx,int doory, unsigned char doorframe);
    void AddaptMapView();
    void DrawMissionObjectives();
    void MonsterAI(int index);
    void BeatEnemy(int aID, int damage);
    void SendAtackImpulse(unsigned int clientIndex, int victim, int hp);
    void SendBulletImpulse(int monsterindex, int ammo, int clientIndex, bool isMine);
    void findpskxy();
    void LoadFirstMap();
    void TitleMenuLogic();
    void HandleBullets();
    void AnimateSlime();
    void ResetVolume();
    bool InitAudio();
    void GetData();
    void GetClientAtackImpulse(const char* buf, int * index, int ClientIndex);
    void QuitApp();
    void SendData();
    bool OnHit(Bullet& bul);
    void DrawStats();
    void SendPlayerInfoToClient(int clientindex);
    void GetMapInfo(const char* bufer, int bufersize, int* index);
    void KillPlayer(int index);
    void GetAtackImpulse(const char* buf,int* index);
    void SendItemCreation(float x, float y, int value, unsigned int clientIndex);
    void GetNewItemInfo(char* bufer, int* index);
    void GetMapData(const char* bufer, int* index);
    void DrawMap(float r,float g, float b);
    void GetDoorInfo(const char* bufer,int * index, int* dx, int* dy, unsigned char* frame);
    void KillEnemy(int ID);
    int FPS();
    void DrawSomeText();
    void GetClientCoords(const char* bufer, int * buferindex, unsigned int clientIndex);
    void GetCharData(const char* bufer, int bufersize, int* index);
    void DrawMiniMap(int x, int y);
    void LoadMap(const char* mapname, int otherplayers);
    void HelpScreenLogic();
    int PlayerCount();
    void LoadShader(ShaderProgram* shader, const char* name);
    void IntroScreenLogic();

    void CoreGameLogic();
};


static std::thread serverThread(&Game::Threadproc);
